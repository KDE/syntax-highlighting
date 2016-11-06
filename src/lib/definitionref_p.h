/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KSYNTAXHIGHLIGHTING_DEFINITIONREF_P_H
#define KSYNTAXHIGHLIGHTING_DEFINITIONREF_P_H

#include <memory>

namespace KSyntaxHighlighting {

class Definition;
class DefinitionData;
class DefinitionPrivate;

/** Weak reference for Definition instances.
 *
 * This must be used when holding Definition instances
 * in objects hold directly or indirectly by Definition
 * to avoid reference count loops and thus memory leaks.
 *
 * @internal
 */
class DefinitionRef
{
public:
    DefinitionRef();
    explicit DefinitionRef(const Definition &def);
    ~DefinitionRef();
    DefinitionRef& operator=(const Definition &def);

    Definition definition() const;

private:
    friend class DefinitionData;
    std::weak_ptr<DefinitionData> d;
};

}

#endif

