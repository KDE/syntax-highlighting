/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KSYNTAXHIGHLIGHTING_FOLDINGREGION_H
#define KSYNTAXHIGHLIGHTING_FOLDINGREGION_H

#include "ksyntaxhighlighting_export.h"

#include <QTypeInfo>

namespace KSyntaxHighlighting
{
/*!
 * \class KSyntaxHighlighting::FoldingRegion
 * \inheaderfile KSyntaxHighlighting/FoldingRegion
 * \inmodule KSyntaxHighlighting
 *
 * \brief Represents a begin or end of a folding region.
 * \since 5.28
 */
class KSYNTAXHIGHLIGHTING_EXPORT FoldingRegion
{
public:
    /*!
     * Defines whether a FoldingRegion starts or ends a folding region.
     *
     * \value None
     * \value Begin
     * \value End
     */
    enum Type {
        None,
        Begin,
        End
    };

    /*!
     * Constructs an invalid folding region, meaning that isValid() returns \c false.
     * To obtain valid instances, see AbstractHighlighter::applyFolding().
     */
    FoldingRegion();

    /*! Compares two FoldingRegion instances for equality. */
    bool operator==(const FoldingRegion &other) const;

    /*!
     * Returns \c true if this is a valid folding region.
     * A valid FoldingRegion is defined by a type() other than Type::None.
     *
     * \note The FoldingRegion%s passed in AbstractHighlighter::applyFolding()
     *       are always valid.
     */
    bool isValid() const;

    /*!
     * Returns a unique identifier for this folding region.
     *
     * As example, the C/C++ highlighter starts and ends a folding region for
     * scopes, e.g.:
     * \code
     * void foo() {     // '{' starts a folding region
     *     if (bar()) { // '{' starts a (nested) folding region
     *     }            // '}' ends the (nested) folding region
     * }                // '}' ends the outer folding region
     * \endcode
     * In this example, all braces '{' and '}' have the same id(), meaning that
     * if you want to find the matching closing region for the first opening
     * brace, you need to do kind of a reference counting to find the correct
     * closing brace.
     */
    int id() const;

    /*!
     * Returns whether this is the begin or end of a region.
     *
     * \note The FoldingRegion%s passed in AbstractHighlighter::applyFolding()
     *       are always valid, i.e. either Type::Begin or Type::End.
     */
    Type type() const;

    /*!
     * Returns the matching start or end region.
     *
     * \note Will return invalid region for an invalid region.
     *
     * \since 6.0
     */
    FoldingRegion sibling() const;

private:
    friend class Rule;
    KSYNTAXHIGHLIGHTING_NO_EXPORT FoldingRegion(Type type, int id);

    // 0 is invalid, positive begin, negative end
    int m_idWithType = 0;
};

}

QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(KSyntaxHighlighting::FoldingRegion, Q_PRIMITIVE_TYPE);
QT_END_NAMESPACE

#endif
