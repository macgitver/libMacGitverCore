/*
 * MacGitver
 * Copyright (C) 2012-2015 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@cunz-rad.com>
 * (C) Cunz RaD Ltd.
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

#include "libMacGitverCore/App/MacGitver.hpp"

#include "RepoMan/RepoMan.hpp"
#include "RepoMan/Events.hpp"

#include "RepoMan/Private/Dumper.hpp"

#include "RepoMan/Data/RepoManData.hpp"
#include "RepoMan/Data/RepoData.hpp"

#include "libBlueSky/Application.hpp"

#include <QDebug>

namespace RM
{

    using namespace Internal;

    /**
     * @class       RepoMan
     * @brief       Manages the open / active repositories
     */

    /**
     * @brief       Constructor
     *
     * Initializes the repository subsystem. Invoked from MacGitver boot up code.
     *
     */
    RepoMan::RepoMan()
        : Base(*new RepoManPrivate(this))
    {
        Events::addReceiver(this);

        connect(BlueSky::Application::instance(),
                SIGNAL(activeModeChanged(BlueSky::Mode*)),
                this,
                SLOT(reactivateWorkaround()));
    }

    /**
     * @brief       Destructor
     *
     * Close all repositories.
     *
     */
    RepoMan::~RepoMan()
    {
        closeAll();
        Events::delReceiver(this);
    }

    RepoMan& RepoMan::instance()
    {
        static QPointer<RepoMan> sSelf;
        if (!sSelf) {
            sSelf = new RepoMan;
        }
        return *sSelf;
    }

    void RepoMan::terminate()
    {
        delete this;
    }

    /**
     * @brief       Open a repository
     *
     * Opens the Repository at @a path
     *
     * @param[in]   path    The path where to look for the repository to open.
     *
     */
    void RepoMan::open(const QString& path)
    {
        // ### REPOMAN TODO
        qDebug() << "RepoMan::Open is currently not implemented.";
    }

    /**
     * @brief       Close all currently open repositories
     *
     */
    void RepoMan::closeAll()
    {
        RM_D(RepoMan);

        foreach(Repo* repo, d->repos) {
            repo->close();
        }
    }

    void RepoMan::reactivateWorkaround()
    {
        RM_D(RepoMan);
        if (d->activeRepo) {
            Events::self()->repositoryDeactivated(d->activeRepo);
            Events::self()->repositoryActivated(d->activeRepo);
        }
    }

    void RepoMan::activate(Repo* repository)
    {
        RM_D(RepoMan);

        if(repository != d->activeRepo) {
            Repo* old = d->activeRepo;

            if (d->activeRepo) {
                d->activeRepo->deactivated();
                Events::self()->repositoryDeactivated(d->activeRepo);
            }

            d->activeRepo = repository;

            if (repository) {
                repository->activated();
                Events::self()->repositoryActivated(d->activeRepo);
            }

            // Finally emit a signal that just tells about the change
            if ((d->activeRepo != NULL) != (old != NULL)) {
                emit hasActiveRepositoryChanged(d->activeRepo != NULL);
            }
        }
    }

    void RepoMan::internalClosedRepo(Repo* repository)
    {
        RM_D(RepoMan);

        // ### We can implement this by far better, now...

        // This pointer is actually useless. THIS IS THE LAST call issued by the destructor of the
        // RepositoryInfo itself. We should probably NOT give this pointer away.

        // However, we need it to find the closed repository in our list. Calling here should have
        // probably happened before the repository is actually destructing itself.

        int i = d->repos.indexOf(repository);
        if (i != -1) {
            d->repos.remove(i);
            emit repositoryClosed();

            if (d->repos.count() == 0) {
                emit lastRepositoryClosed();
            }
        }
    }

    Repo* RepoMan::activeRepository()
    {
        RM_D(RepoMan);
        return d->activeRepo;
    }

    Repo::List RepoMan::repositories() const
    {
        RM_CD(RepoMan);
        return d->repos;
    }

    //-- RepoManPrivate ----------------------------------------------------------------------------

    RepoManPrivate::RepoManPrivate(RepoMan* _pub)
        : BasePrivate(_pub)
        , activeRepo(NULL)
    {
        setupActions(_pub);
    }

    bool RepoManPrivate::refreshSelf()
    {
        return true;
    }

    ObjTypes RepoManPrivate::objType() const
    {
        return ObjTypes::RepoManager;
    }

    void RepoManPrivate::dumpSelf(Internal::Dumper& dumper) const
    {
        dumper.addLine(QStringLiteral("Repository-Manager"));
    }

    void RepoManPrivate::preTerminate()
    {
        // Do we need to do smth?
    }

    QString RepoManPrivate::displayName() const
    {
        return QStringLiteral("RepoMan");
    }

    QString RepoManPrivate::objectTypeName() const
    {
        return QStringLiteral("RepoMan");
    }

    Heaven::Menu* RepoManPrivate::contextMenuFor(Base* object)
    {
        switch (object->objType()) {

        case ObjTypes::Branch:      return menuCtxBranch;
        case ObjTypes::Tag:         return menuCtxTag;
        case ObjTypes::Repo:        return menuCtxRepo;
        case ObjTypes::Submodule:   return menuCtxSubmodule;
        case ObjTypes::Namespace:   return menuCtxNamespace;
        case ObjTypes::RefLog:      return menuCtxRefLog;
        case ObjTypes::Remote:      return menuCtxRemote;

        //case ObjTypes::Reference:                 return menuCtxRef;
        //case ObjTypes::RefTreeNode:         return menuCtxRefTreeNode;

        default:
            break;
        }

        return nullptr;
    }

}