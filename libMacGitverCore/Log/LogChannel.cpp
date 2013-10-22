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

#include <QString>

#include "libHeaven/Icons/IconRef.hpp"

#include "libMacGitverCore/App/MacGitver.hpp"

#include "libMacGitverCore/Log/LogChannel.hpp"
#include "libMacGitverCore/Log/LogEvent.hpp"
#include "libMacGitverCore/Log/LogManager.hpp"
#include "libMacGitverCore/Log/LogTemplate.hpp"

namespace Log
{

    /**
     * @class       Channel
     * @brief       A logging channel
     *
     * A logging channel is a separate entity to which can be logged. By default, the channels as
     * defined in the Log::Type enumeration exist.
     *
     */

    class Channel::Data : public QSharedData
    {
    public:
        Data();

    public:
        Template        defaultTemplate;
        QString         name;
        QString         displayName;
        Heaven::IconRef icon;
        Event::List     events;
    };

    /**
     * @brief       Copy constructor
     *
     * @param[in]   other   The channel to create a copy of
     *
     */
    Channel::Channel(const Channel& other)
        : d(other.d)
    {
    }

    /**
     * @brief       Default constructor
     *
     * Creates an invalid Channel object
     *
     */
    Channel::Channel()
    {
    }

    /**
     * @internal
     * @brief       Creation constructor
     *
     * @param[in]   _d  Channel data
     *
     */
    Channel::Channel(Channel::Data* _d)
        : d(_d)
    {
    }

    /**
     * @brief       Create a new channel object
     *
     * @param[in]   name    The name for the new channel. This name must be unique accross all
     *                      logging channels in the system.
     *
     * @return      The created channel.
     *
     * The new channel must be registered with the Log::Manager.
     */
    Channel Channel::create(const QString& name)
    {
        Data* d = new Data;
        d->name = name;
        return d;
    }

    /**
     * @brief       Destructor
     *
     */
    Channel::~Channel()
    {
    }

    /**
     * @brief       Assignment operator
     *
     * @param[in]   other   Channel to be assigned to this
     *
     * @return      A reference to this.
     *
     */
    Channel& Channel::operator=(const Channel& other)
    {
        d = other.d;
        return *this;
    }

    /**
     * @brief       Check for validity
     *
     * @return      `true`, if this is a valid Channel object, `false` otherwise.
     *
     */
    bool Channel::isValid() const
    {
        return d;
    }

    /**
     * @brief       Get this channel's name
     *
     * @return      The name of this channel
     *
     * The name can be used to identify the channel. Names must be unique among all channels.
     *
     */
    QString Channel::name() const
    {
        return d ? d->name : QString();
    }

    /**
     * @brief       Get an icon for this channel
     *
     * @return      A icon ref to be used to associate with this channel.
     *
     */
    Heaven::IconRef Channel::icon() const
    {
        return d ? d->icon : Heaven::IconRef();
    }

    /**
     * @brief       Add an event to this channel
     *
     * @param[in]   event   The event to add
     *
     * Adding an event to a channel means that it becomes visible to the user. The consumer is
     * informed about the new event.
     *
     */
    void Channel::addEvent(Event event)
    {
        Q_ASSERT(d);
        if (d) {
            event.setChannel(d.data());
            d->events.append(event);

            MacGitver::log().eventAdded(event);
        }
    }

    /**
     * @brief       Set a user visible display name for this channel
     *
     * @param[in]   name    The name to set as display name
     *
     */
    void Channel::setDisplayName(const QString& name)
    {
        Q_ASSERT(d);
        if (d) {
            d->displayName = name;
        }
    }

    /**
     * @brief       Get a name that can be displayed to the user
     *
     * @return      Display name of this channel
     *
     */
    QString Channel::displayName() const
    {
        return d ? d->displayName : QString();
    }

    /**
     * @brief       Get the default template
     *
     * @return      Default template of this Channel
     *
     */
    Template Channel::defaultTemplate() const
    {
        return d ? d->defaultTemplate : Template();
    }

    /**
     * @brief       Set the default template
     *
     * @param[in]   t   The template to set as default template.
     *
     */
    void Channel::setDefaultTemplate(Template t)
    {
        Q_ASSERT(d);
        if (d) {
            d->defaultTemplate = t;
        }
    }

    //-- Channel::Data -------------------------------------------------------------------------- >8

    Channel::Data::Data()
    {
    }

}
