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

#ifndef KATESYNTAX_RULE_H
#define KATESYNTAX_RULE_H

#include "keywordlist.h"
#include "contextswitch.h"
#include "matchresult.h"
#include "syntaxdefinition.h"

#include <QRegExp>
#include <QString>
#include <QVector>

#include <memory>

class QXmlStreamReader;

namespace KateSyntax {

class Rule
{
public:
    Rule();
    virtual ~Rule();

    typedef std::shared_ptr<Rule> Ptr;

    SyntaxDefinition syntaxDefinition() const;
    void setSyntaxDefinition(const SyntaxDefinition &def);

    QString attribute() const;
    ContextSwitch context() const;
    bool isLookAhead() const;
    bool isDynamic() const;

    bool load(QXmlStreamReader &reader);
    void resolveContext();

    MatchResult match(const QString &text, int offset, const QStringList &captures);

    static Rule::Ptr create(const QStringRef &name);

protected:
    virtual bool doLoad(QXmlStreamReader &reader);
    virtual MatchResult doMatch(const QString &text, int offset, const QStringList &captures) = 0;

    bool isDelimiter(QChar c) const;

private:
    Q_DISABLE_COPY(Rule)

    SyntaxDefinition m_def;
    QString m_attribute;
    ContextSwitch m_context;
    QVector<Rule::Ptr> m_subRules;
    int m_column;
    bool m_firstNonSpace;
    bool m_lookAhead;
    bool m_dynamic;
};


class AnyChar : public Rule
{
protected:
    bool doLoad(QXmlStreamReader & reader) override;
    MatchResult doMatch(const QString & text, int offset, const QStringList&) override;

private:
    QString m_chars;
};

class DetectChar : public Rule
{
protected:
    bool doLoad(QXmlStreamReader & reader) override;
    MatchResult doMatch(const QString & text, int offset, const QStringList &captures) override;

private:
    QChar m_char;
};

class Detect2Char : public Rule
{
protected:
    bool doLoad(QXmlStreamReader & reader) override;
    MatchResult doMatch(const QString & text, int offset, const QStringList &captures) override;

private:
    QChar m_char1;
    QChar m_char2;
};

class DetectIdentifier : public Rule
{
protected:
    MatchResult doMatch(const QString & text, int offset, const QStringList&) override;
};

class DetectSpaces : public Rule
{
protected:
    MatchResult doMatch(const QString & text, int offset, const QStringList&) override;
};

class Float : public Rule
{
protected:
    MatchResult doMatch(const QString & text, int offset, const QStringList&) override;
};

class IncludeRules : public Rule
{
public:
    QString contextName() const;
    QString definitionName() const;

protected:
    bool doLoad(QXmlStreamReader & reader) override;
    MatchResult doMatch(const QString & text, int offset, const QStringList&) override;

private:
    QString m_contextName;
    QString m_defName;
    bool m_processingMarker;
};

class Int : public Rule
{
protected:
    MatchResult doMatch(const QString & text, int offset, const QStringList &captures) override;
};

class HlCChar : public Rule
{
protected:
    MatchResult doMatch(const QString & text, int offset, const QStringList&) override;
};

class HlCHex : public Rule
{
protected:
    MatchResult doMatch(const QString & text, int offset, const QStringList&) override;
};

class HlCOct : public Rule
{
protected:
    MatchResult doMatch(const QString & text, int offset, const QStringList&) override;
};

class HlCStringChar : public Rule
{
protected:
    MatchResult doMatch(const QString & text, int offset, const QStringList&) override;
};

class KeywordListRule : public Rule
{
protected:
    bool doLoad(QXmlStreamReader & reader) override;
    MatchResult doMatch(const QString & text, int offset, const QStringList&) override;

private:
    QString m_listName;
    KeywordList m_keywordList;
};

class LineContinue : public Rule
{
protected:
    bool doLoad(QXmlStreamReader & reader) override;
    MatchResult doMatch(const QString & text, int offset, const QStringList&) override;

private:
    QChar m_char;
};

class RangeDetect : public Rule
{
protected:
    bool doLoad(QXmlStreamReader & reader) override;
    MatchResult doMatch(const QString & text, int offset, const QStringList&) override;

private:
    QChar m_begin;
    QChar m_end;
};

class RegExpr : public Rule
{
protected:
    bool doLoad(QXmlStreamReader & reader) override;
    MatchResult doMatch(const QString & text, int offset, const QStringList &captures) override;

private:
    QString m_pattern;
    QRegExp m_regexp;
};

class StringDetect : public Rule
{
protected:
    bool doLoad(QXmlStreamReader & reader) override;
    MatchResult doMatch(const QString & text, int offset, const QStringList &captures) override;

private:
    QString m_string;
    Qt::CaseSensitivity m_caseSensitivity;
};

class WordDetect : public Rule
{
protected:
    bool doLoad(QXmlStreamReader & reader) override;
    MatchResult doMatch(const QString & text, int offset, const QStringList &captures) override;

private:
    QString m_word;
};

}

#endif // KATESYNTAX_RULE_H
