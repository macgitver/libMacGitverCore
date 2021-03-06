/*
 * MacGitver
 * Copyright (C) 2012-2015 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@cunz-rad.com>
 * (C) Nils Fenner <nilsfenner@web.de>
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

#include "libMacGitverCore/MacGitverApi.hpp"

#include <QString>
#include <QHash>

class MGV_CORE_API ShellExpand
{
private:
    class State;

public:
    typedef QHash<QString, QString> Macros;

    ShellExpand();
    ShellExpand(const Macros& macros);

public:
    bool overwriteMacroDeclarations() const;
    void setOverwriteMacroDeclarations(bool enabled);

    bool addMacro(const QString &name, const QString &value);
    bool addMacros(const Macros& macros);

    const Macros& macros() const;

    QString expandText(const QString& input);
    QString expandFile(const QString& fileName);

private:
    QString replacementLogic(const QString& parameter, const QString& command = QString(), const QString& arg = QString());
    inline bool processExternal(QString &value, const QString &command, const QString &arg);
    bool replaceExpandedValue(QString& value, bool cond, const QString &arg);

private:
    Macros  mMacros;
    bool    mOverwriteMacroDecls;
};
