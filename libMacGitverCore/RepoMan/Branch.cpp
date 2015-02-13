/*
 * MacGitver
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@macgitver.org>
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

#include "Branch.hpp"
#include "Repo.hpp"
#include "Events.hpp"

#include "RepoMan/Private/Dumper.hpp"
#include "RepoMan/Private/BranchPrivate.hpp"

namespace RM
{

    using namespace Internal;

    Branch::Branch(Base* parent, const Git::Reference &ref)
        : Ref( *new BranchPrivate(this, ref) )
    {
        RM_D(Branch);
        d->linkToParent( parent );
    }

    QString Branch::upstreamRefName() const
    {
        RM_CD(Branch);
        return d->mUpstreamRefName;
    }

    Ref* Branch::upstream()
    {
        return repository()->findReference(upstreamRefName());
    }

    bool Branch::hasUpstream() const
    {
        RM_CD(Branch);
        return d->mHasUpstream;
    }

    int Branch::aheadCount() const
    {
        RM_CD(Branch);
        return d->mAheadCount;
    }

    int Branch::behindCount() const
    {
        RM_CD(Branch);
        return d->mBehindCount;
    }

    //-- BranchPrivate -----------------------------------------------------------------------------

    BranchPrivate::BranchPrivate(Branch* pub, const Git::Reference& ref)
        : RefPrivate(pub, BranchType, ref)
        , mHasUpstream(false)
        , mAheadCount(0)
        , mBehindCount(0)
    {
    }

    ObjTypes BranchPrivate::objType() const
    {
        return BranchObject;
    }

    void BranchPrivate::postCreation()
    {
        Repo* repo = repository();
        if ( !repoEventsBlocked( repo ) ) {
            Events::self()->branchCreated( repo, pub<Branch>() );
        }

        RefPrivate::postCreation();
    }

    void BranchPrivate::preTerminate()
    {
        Repo* repo = repository();
        if ( !repoEventsBlocked( repo ) ) {
            Events::self()->branchAboutToBeDeleted( repo, pub<Branch>() );
        }

        RefPrivate::preTerminate();
    }

    void BranchPrivate::dumpSelf(Internal::Dumper& dumper) const
    {
        dumper.addLine(QString(QLatin1String("Branch 0x%1 - %2"))
                       .arg(quintptr(mPub),0,16)
                       .arg(mName));
    }

    bool BranchPrivate::refreshSelf()
    {
        // TODO: Update "upstream" and "divergence"
        return RefPrivate::refreshSelf();
    }

    QString BranchPrivate::objectTypeName() const
    {
        return QLatin1String("Branch");
    }

    bool BranchPrivate::refreshDetails(const Git::Reference& ref)
    {

        return RefPrivate::refreshDetails(ref);
    }

    bool BranchPrivate::inherits(ObjTypes type) const
    {
        return type == BranchObject || RefPrivate::inherits(type);
    }

}
