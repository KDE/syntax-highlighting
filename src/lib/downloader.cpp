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

#include "downloader.h"
#include "syntaxdefinition.h"
#include "repository.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QTimer>
#include <QXmlStreamReader>

using namespace SyntaxHighlighting;

Downloader::Downloader(Repository *repo, QObject *parent) :
    QObject(parent),
    m_repo(repo),
    m_nam(new QNetworkAccessManager(this)),
    m_pendingDownloads(0)
{
    Q_ASSERT(repo);

    m_downloadLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/KateSyntax");
    QDir().mkpath(m_downloadLocation);
    Q_ASSERT(QFile::exists(m_downloadLocation));
}

Downloader::~Downloader()
{
}

void Downloader::start()
{
    QNetworkRequest req(QUrl(QStringLiteral("https://www.kate-editor.org/syntax/update-5.24.xml")));
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    auto reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        definitionListDownloadFinished(reply);
    });
}

void Downloader::definitionListDownloadFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << reply->error();
        emit done(); // TODO return error
        return;
    }

    QXmlStreamReader parser(reply);
    while (!parser.atEnd()) {
        switch (parser.readNext()) {
            case QXmlStreamReader::StartElement:
                if (parser.name() == QLatin1String("Definition"))
                    updateDefinition(parser);
                break;
            default:
                break;
        }
    }

    if (m_pendingDownloads == 0)
        emit informationMessage(tr("All syntax definitions are up-to-date."));
    checkDone();
}

void Downloader::updateDefinition(QXmlStreamReader &parser)
{
    const auto name = parser.attributes().value(QLatin1String("name"));
    if (name.isEmpty())
        return;

    auto localDef = m_repo->definitionForName(name.toString());
    if (!localDef.isValid()) {
        emit informationMessage(tr("Downloading new syntax definition for '%1'...").arg(name.toString()));
        downloadDefinition(QUrl(parser.attributes().value(QLatin1String("url")).toString()));
        return;
    }

    const auto version = parser.attributes().value(QLatin1String("version"));
    if (localDef.version() < version.toFloat()) {
        emit informationMessage(tr("Updating syntax definition for '%1' to version %2...").arg(name.toString(), version.toString()));
        downloadDefinition(QUrl(parser.attributes().value(QLatin1String("url")).toString()));
    }
}

void Downloader::downloadDefinition(const QUrl& downloadUrl)
{
    if (!downloadUrl.isValid())
        return;
    auto url = downloadUrl;
    if (url.scheme() == QLatin1String("http"))
        url.setScheme(QStringLiteral("https"));

    QNetworkRequest req(url);
    auto reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        downloadDefinitionFinished(reply);
    });
    ++m_pendingDownloads;
}

void Downloader::downloadDefinitionFinished(QNetworkReply *reply)
{
    --m_pendingDownloads;
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Failed to download definition file" << reply->url() << reply->error();
        checkDone();
        return;
    }

    // handle redirects
    // needs to be done manually, download server redirects to unsafe http links
    const auto redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!redirectUrl.isEmpty()) {
        downloadDefinition(reply->url().resolved(redirectUrl));
        checkDone();
        return;
    }

    QFile file(m_downloadLocation + QLatin1Char('/') + reply->url().fileName());
    if (!file.open(QFile::WriteOnly)) {
        qWarning() << "Failed to open" << file.fileName() << file.error();
    } else {
        file.write(reply->readAll());
    }
    checkDone();
}

void Downloader::checkDone()
{
    if (m_pendingDownloads == 0)
        emit QTimer::singleShot(0, this, &Downloader::done);
}
