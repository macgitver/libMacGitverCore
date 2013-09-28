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

#ifndef MGV_REPO_MANAGER_HPP
#define MGV_REPO_MANAGER_HPP

#include <QObject>

#include "libGitWrap/Repository.hpp"

#include "libMacGitverCore/RepoMan/Repo.hpp"

namespace RM
{

    class MGV_CORE_API RepoMan : public QObject
    {
        Q_OBJECT
    public:
        RepoMan();
        ~RepoMan();

    public:
        Repo* open(const QString& path);
        Repo* open(const Git::Repository& repo);

        void closeAll();

        Repo* activeRepository();
        void activate( Repo* repository );

        Repo::List repositories() const;

        Repo* repoByPath( const QString& basePath, bool searchSubmodules );

        void internalClosedRepo(Repo* repository);
        void internalActivate(Repo* repository);

    signals:
        void firstRepositoryOpened();
        void lastRepositoryClosed();
        void repositoryOpened(RM::Repo* repo);
        void repositoryClosed();
        void repositoryActivated(RM::Repo* repo);
        void repositoryDeactivated(RM::Repo* repo);
        void hasActiveRepositoryChanged(bool hasActiveRepo);

    private:
        Repo::List    mRepos;
        Repo*         mActiveRepo;
    };

}

#endif
