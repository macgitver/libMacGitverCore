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

#include "libRepoMan/Backend/RepoMan.hpp"

#include "libLogger/Manager.hpp"

#include <QThread>
#include <QPointer>

namespace RM
{

    namespace Backend
    {

        static RepoMan* repoMan()
        {
            static QPointer< RepoMan > sInstance;
            if (sInstance.isNull()) {
                sInstance = new RepoMan;
            }
            return sInstance;
        }

        RepoMan::RepoMan()
            : mWorkerThread(new QThread)
        {
            moveToThread(mWorkerThread);
            mWorkerThread->start();

            setupLogging();
        }

        RepoMan& RepoMan::instance()
        {
            return *repoMan();
        }

        QThread* RepoMan::workerThread()
        {
            return repoMan()->mWorkerThread;
        }

        void RepoMan::setupLogging()
        {
            mGitErrorChannel = Log::Channel::create(tr("Git Errors"));

            Log::Manager().addChannel(mGitErrorChannel);

            mGitErrorTemplate = Log::Template::create(tr("Git Errors"));
            mGitErrorTemplate.setTransformation(tr("$$ caused a git error: $gwerror$"));
            Log::Manager().addTemplate(mGitErrorTemplate);
        }

        Log::Channel RepoMan::gitErrorChannel() const
        {
            return mGitErrorChannel;
        }

        Log::Template RepoMan::gitErrorTemplate() const
        {
            return mGitErrorTemplate;
        }

    }

}