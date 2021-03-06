/*
 * MacGitver
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@macgitver.org>
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

#include "libBlueSky/Contexts.hpp"

#include "libMacGitverCore/MacGitver/IRepositoryContext.hpp"
#include "libMacGitverCore/RepoMan/Repo.hpp"

#include "Branches/BranchesViewData.hpp"
#include "Branches/BranchesModel.hpp"

BranchesViewData::BranchesViewData()
    : BlueSky::ViewContextData()
    , mModel( NULL )
{
}

void BranchesViewData::attachedToContext(BlueSky::ViewContext* context)
{
    IRepositoryContext* ctx = qobject_cast< IRepositoryContext* >(context);

    mModel = new BranchesModel( this );

    mModel->setRepository(ctx ? ctx->repository() : NULL);
}

void BranchesViewData::detachedFromContext()
{
    delete mModel;
    mModel = NULL;
}

RM::Repo* BranchesViewData::repository() const
{
    IRepositoryContext* ctx = qobject_cast< IRepositoryContext* >( attachedContext() );
    return ctx ? ctx->repository() : NULL;
}

