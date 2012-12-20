/*
 * MacGitver
 * Copyright (C) 2012 Sascha Cunz <sascha@babbelbox.org>
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

#ifndef MGV_REPOSITORY_CORE_H
#define MGV_REPOSITORY_CORE_H

#include "libMacGitverCore/Interfaces/IRepositoryCore.h"

#include "libGitWrap/Repository.hpp"

class RepositoryCore : public IRepository
{
public:
    RepositoryCore();
    ~RepositoryCore();

public:
    void setRepository( Git::Repository repo );

public:
    bool createBranch( const Git::ObjectId& baseOID, const QString& branchName, bool force );

private:
    Git::Repository mRepo;
};

#endif
