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

#include "libRepoMan/RepoMan.hpp"

#include "libRepoMan/Private/Dumper.hpp"

#include "libRepoMan/Backend/ServiceRunner.hpp"

#include "libRepoMan/Services/OpenRepository.hpp"
#include "libRepoMan/Services/RefreshRepository.hpp"

#include "libRepoMan/Data/RepoMan.hpp"
#include "libRepoMan/Data/Repo.hpp"

#include "libLogger/Manager.hpp"

#include "libBlueSky/Application.hpp"

#include <QDebug>
#include <QPointer>

namespace RM
{

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
    {
        connect(BlueSky::Application::instance(),
                &BlueSky::Application::activeModeChanged,
                this,
                &RepoMan::reactivateWorkaround);
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
        Backend::perform(new Services::OpenRepository(path));
    }

    /**
     * @brief       Close all currently open repositories
     *
     */
    void RepoMan::closeAll()
    {
        #if 0 // ###DEAD
        RM_D(RepoMan);

        foreach(Repo* repo, d->repos) {
            repo->close();
        }
        #endif
    }

    void RepoMan::reactivateWorkaround()
    {
        #if 0 // ###DEAD
        RM_D(RepoMan);
        if (d->activeRepo) {
            Events::self()->repositoryDeactivated(d->activeRepo);
            Events::self()->repositoryActivated(d->activeRepo);
        }
        #endif
    }

    void RepoMan::activate(const Frontend::Repo& repository)
    {
        #if 0 // ###DEAD
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
        #endif
    }

    #if 0 // ###DEAD
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
    #endif

    Frontend::Repo RepoMan::activeRepository()
    {
        #if 0 // ###DEAD
        RM_D(RepoMan);
        return d->activeRepo;
        #else
        return Frontend::Repo();
        #endif
    }

    Frontend::Repo::List RepoMan::repositories() const
    {
        #if 0 // ###DEAD
        RM_CD(RepoMan);
        return d->repos;
        #else
        return Frontend::Repo::List();
        #endif
    }

    void RepoMan::refresh(RefreshTypes what)
    {
        Log::Manager().addMessage(trUtf8("Refreshing git repositories..."));
        Backend::perform(new Services::RefreshRepository(activeRepository()));
    }

    #if 0 // ###DEAD
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
        default:                    return nullptr;
        }
    }
    #endif

}