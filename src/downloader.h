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

#ifndef KATESYNTAX_DOWNLOADER_H
#define KATESYNTAX_DOWNLOADER_H

#include "kf5syntaxhighlighting_export.h"

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QUrl;
class QXmlStreamReader;

namespace KateSyntax {

class SyntaxRepository;

class KF5SYNTAXHIGHLIGHTING_EXPORT Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(SyntaxRepository *repo, QObject *parent = nullptr);
    ~Downloader();

    void start();

Q_SIGNALS:
    void informationMessage(const QString &msg);
    void done();

private:
    void definitionListDownloadFinished(QNetworkReply *reply);
    void updateDefinition(QXmlStreamReader &parser);
    void downloadDefinition(const QUrl &url);
    void downloadDefinitionFinished(QNetworkReply *reply);
    void checkDone();

    SyntaxRepository *m_repo;
    QNetworkAccessManager *m_nam;
    QString m_downloadLocation;
    int m_pendingDownloads;
};
}

#endif // KATESYNTAX_DOWNLOADER_H
