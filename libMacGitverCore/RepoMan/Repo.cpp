/*
 * MacGitver
 * Copyright (C) 2012-2013 Sascha Cunz <sascha@babbelbox.org>
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License (Version 2) as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QDebug>
#include <QTimer>

#include "libGitWrap/Reference.hpp"
#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Submodule.hpp"

#include "libMacGitverCore/App/MacGitver.hpp"

#include "RepoMan/Repo.hpp"
#include "RepoMan/RepoMan.hpp"

namespace RM
{

    Repo::Repo()
    {
    }

    Repo::Repo( const Git::Repository& repo )
    {
        mRepo = repo;
        mPath = repo.basePath();
        mIsLoaded = mRepo.isValid();
        mIsActive = false;
        mIsDisabled = false;
        mIsBare = mRepo.isValid() && mRepo.isBare();
        mIsSubModule = false;
        mDisplayAlias = QString();
        mParent = NULL;
        mUnloadTimer = NULL;

        if( mPath.endsWith( L'/' ) )
        {
            mPath = mPath.left( mPath.length() - 1 );
        }

        findAlias();

        if( mDisplayAlias.isEmpty() )
        {
            if (mPath.isEmpty()) {
                mDisplayAlias = trUtf8("Unknown Repository");
            }
            else {
                QStringList sl = mPath.split( QChar( L'/' ), QString::SkipEmptyParts );
                mDisplayAlias = sl.last();
            }
        }
    }

    Repo::~Repo()
    {
        qDeleteAll( mChildren );

        if( mIsLoaded )
        {
            unload();
        }

        if( mParent )
        {
            mParent->removeChild( this );
        }

        MacGitver::repoMan().internalClosedRepo(this);
    }

    Git::Repository Repo::gitRepo()
    {
        if( ensureIsLoaded() )
        {
            return mRepo;
        }

        return Git::Repository();
    }

    QString Repo::path() const
    {
        return mPath;
    }

    bool Repo::isSubModule() const
    {
        return mIsSubModule;
    }

    bool Repo::isBare() const
    {
        return mIsBare;
    }

    bool Repo::isLoaded() const
    {
        return mIsLoaded;
    }

    bool Repo::isDisabled() const
    {
        return mIsDisabled;
    }

    bool Repo::isActive() const
    {
        return mIsActive;
    }

    Repo* Repo::parentRepository()
    {
        return mParent;
    }

    Repo::List Repo::children() const
    {
        return mChildren;
    }

    void Repo::setActive( bool active )
    {
        if( active == mIsActive )
        {
            return;
        }

        if( mIsActive )
        {
            Q_ASSERT( !mUnloadTimer );
            mUnloadTimer = new QTimer( this );
            connect( mUnloadTimer, SIGNAL(timeout()), this, SLOT(unloadTimer()) );
            mUnloadTimer->setInterval( 15 * 60 * 1000 ); // quarter of an hour
            mUnloadTimer->start();

            mIsActive = false;

            MacGitver::repoMan().internalActivate( NULL );
        }
        else
        {
            if( mUnloadTimer )
            {
                mUnloadTimer->stop();
                mUnloadTimer->deleteLater();
                mUnloadTimer = NULL;
            }
            mIsActive = true;

            MacGitver::repoMan().internalActivate( this );
        }
    }

    QString Repo::displayAlias() const
    {
        return mDisplayAlias;
    }

    void Repo::setDisplayAlias( const QString& alias )
    {
        if( mDisplayAlias != alias )
        {
            mDisplayAlias = alias;
            emit aliasChanged( alias );
        }
    }

    void Repo::load()
    {
        Q_ASSERT( !mIsLoaded );
    }

    void Repo::unload()
    {
        if( mIsActive )
        {
            qDebug() << "Unloading active RepoInfo. Will deactivate it first.";
            setActive( false );
        }
        Q_ASSERT( !mIsActive );

        if( mUnloadTimer )
        {
            mUnloadTimer->stop();
            mUnloadTimer->deleteLater();
            mUnloadTimer = NULL;
        }

        emit aboutToUnload( this );

        mIsLoaded = false;
        mRepo = Git::Repository();

        emit unloaded( this );
    }

    bool Repo::ensureIsLoaded()
    {
        if( mIsLoaded )
        {
            return true;
        }

        if( mIsDisabled )
        {
            return false;
        }

        load();
        return mIsLoaded;
    }

    void Repo::unloadTimer()
    {
        Q_ASSERT( mUnloadTimer );

        mUnloadTimer->stop();
        mUnloadTimer->deleteLater();
        mUnloadTimer = NULL;

        unload();
    }

    void Repo::removeChild( Repo* child )
    {
        for( int i = 0; i < mChildren.count(); i++ )
        {
            if( mChildren.at( i ) == child )
            {
                mChildren.removeAt( i );
                emit childRemoved( this, child );
                return;
            }
        }
    }

    void Repo::close()
    {
        emit aboutToClose( this );

        foreach( Repo* child, mChildren )
        {
            child->close();
        }

        delete this;
    }

    Repo* Repo::repoByPath( const QString& basePath, bool searchSubmodules )
    {
        foreach( Repo* info, mChildren )
        {
            if( info->path() == basePath )
            {
                return info;
            }

            if( searchSubmodules )
            {
                if( Repo* sub = info->repoByPath( basePath, true ) )
                {
                    return sub;
                }
            }
        }

        return NULL;
    }

    void Repo::scanSubmodules()
    {
        if( !ensureIsLoaded() )
        {
            return;
        }

        Git::Result r;
        Git::Submodule::List subs = mRepo.submodules( r );
        if( !r )
        {
            return;
        }

        List oldChildren = mChildren;

        foreach (Git::Submodule sub, subs) {
            Git::Result child;
            Git::Repository subRepo = sub.subRepository(child);
            if (!child) {
                continue;
            }

            Repo* subInfo = NULL;
            QString path = subRepo.basePath();

            subInfo = repoByPath( path, true );
            if( !subInfo )
            {
                subInfo = new Repo( subRepo );
                subInfo->mIsSubModule = true;
                subInfo->setDisplayAlias( subRepo.name() );
                subInfo->mParent = this;
                mChildren.append( subInfo );

                emit childAdded( this, subInfo );

                if( !subInfo->isBare() )
                {
                    subInfo->scanSubmodules();
                }
            }
            else
            {
                oldChildren.removeOne( subInfo );
            }
        }

        foreach( Repo* info, oldChildren )
        {
            removeChild( info );
        }
    }

    QString Repo::branchDisplay() const
    {
        if( mIsLoaded )
        {
            Git::Result r;
            Git::Reference HEAD = mRepo.HEAD( r );

            if( HEAD.isValid() )
            {
                if( HEAD.name() != QLatin1String( "HEAD" ) )
                {
                    return trUtf8( "<b style=\"background-color: #FFB54F;"
                                   "\">%1</b>" ).arg( HEAD.name().mid( 11 ) );
                }
                else
                {
                    return trUtf8( "detached at <b>%1</b>" ).arg(HEAD.objectId(r).toString());
                }
            }
            else
            {
                return trUtf8( "<b style=\"color: red;\">Branch yet to be born</b>" );
            }
        }
        return tr( "&lt;unknown&gt;" );
    }

    void Repo::findAlias()
    {
    }

}