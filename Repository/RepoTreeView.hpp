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

#ifndef MGV_REPO_TREE_VIEW_HPP
#define MGV_REPO_TREE_VIEW_HPP

#include "libHeaven/Views/GlobalView.hpp"
#include "libHeaven/Widgets/MiniSplitter.h"

class QTreeView;

class RepoTreeView : public Heaven::GlobalView
{
    Q_OBJECT
public:
    RepoTreeView();

private:
    void setupUi();

private:
    Heaven::MiniSplitter*   mSplitter;
    QTreeView*              mRepos;
};

#endif
