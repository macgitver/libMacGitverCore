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

#include "RepoMan/Data/RemoteData.hpp"

#include "RepoMan/Private/Dumper.hpp"

#include "RepoMan/Events.hpp"

namespace RM
{

    namespace Internal
    {

        RemotePrivate::RemotePrivate(Remote* _pub, const Git::Remote& _obj)
            : BasePrivate(_pub)
        {
            name = _obj.name();
        }

        ObjTypes RemotePrivate::objType() const
        {
            return ObjTypes::Remote;
        }

        void RemotePrivate::dumpSelf(Internal::Dumper& dumper) const
        {
            dumper.addLine(QString(QStringLiteral("Remote %2 0x%1"))
                           .arg(quintptr(mPub),0,16)
                           .arg(name));
        }

        void RemotePrivate::postCreation()
        {
            if (!repoEventsBlocked()) {
                Events::self()->remoteCreated(repository(), pub<Remote>());
            }

            BasePrivate::postCreation();
        }

        void RemotePrivate::preTerminate()
        {
            if (!repoEventsBlocked()) {
                Events::self()->remoteAboutToBeDeleted(repository(), pub<Remote>());
            }

            BasePrivate::preTerminate();
        }

        QString RemotePrivate::displayName() const
        {
            return name;
        }


        bool RemotePrivate::refreshSelf()
        {
            return true;
        }

        QString RemotePrivate::objectTypeName() const
        {
            return QStringLiteral("Remote");
        }

        bool RemotePrivate::inherits(ObjTypes type) const
        {
            return type == ObjTypes::Remote || BasePrivate::inherits(type);
        }

    }

}