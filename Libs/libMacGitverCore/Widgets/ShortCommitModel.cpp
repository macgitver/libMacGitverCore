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

#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Result.hpp"

#include "ShortCommitModel.h"

ShortCommitModel::ShortCommitModel( QObject* parent )
    : QAbstractTableModel( parent )
{
}

ShortCommitModel::~ShortCommitModel()
{
}

void ShortCommitModel::clear()
{
    beginResetModel();

    mCommitList.clear();

    endResetModel();
}

void ShortCommitModel::setCommitList( const Git::CommitList& list )
{
    beginResetModel();

    mCommitList = list;

    endResetModel();
}

bool ShortCommitModel::setCommitList(Git::Repository repo, const Git::ObjectIdList& list )
{
    Git::CommitList commits;
    Git::Result r;

    foreach (Git::ObjectId sha1, list) {
        Git::Commit commit = repo.lookupCommit(r, sha1);
        if (!r) {
            return false;
        }
        commits.append( commit );
    }

    setCommitList( commits );
    return true;
}

Git::Commit ShortCommitModel::indexToCommit( const QModelIndex& index ) const
{
    if(index.isValid()) {
        return mCommitList.at( index.row() );
    }
    return Git::Commit();
}

int ShortCommitModel::rowCount( const QModelIndex& parent ) const
{
    return parent.isValid() ? 0 : mCommitList.count();
}

int ShortCommitModel::columnCount( const QModelIndex& parent ) const
{
    return parent.isValid() ? 0 : 3;
}

QVariant ShortCommitModel::data( const QModelIndex& index, int role ) const
{
    QVariant value;
    Git::Result r;

    if( role == Qt::DisplayRole )
    {
        Git::Commit commit = indexToCommit( index );
        switch( index.column() )
        {
        case 0:
            value = commit.shortMessage();
            break;

        case 1:
            value = commit.author().when().toString();
            break;

        case 2:
            value = commit.id().toString().left( 7 );
            break;

        default:
            return QVariant();
        }
    }

    if( !r )
    {
        return QVariant();
    }

    return value;
}
