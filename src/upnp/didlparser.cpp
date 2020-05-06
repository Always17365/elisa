/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "didlparser.h"

#include "upnpcontrolcontentdirectory.h"
#include "upnpcontrolabstractservicereply.h"
#include "upnpservicedescription.h"
#include "upnpdevicedescription.h"

#include <QVector>
#include <QString>

#include <QDomDocument>
#include <QDomNode>

class DidlParserPrivate
{
public:

    QString mBrowseFlag;

    QString mFilter;

    QString mSortCriteria;

    QString mSearchCriteria;

    QString mParentId;

    UpnpControlContentDirectory *mContentDirectory = nullptr;

    bool mIsDataValid = false;

    QVector<QString> mNewAlbumIds;

    QHash<QString, MusicAlbum> mNewAlbums;

    QVector<QString> mNewMusicTrackIds;

    QHash<QString, MusicAudioTrack> mNewMusicTracks;

    QHash<QString, QVector<MusicAudioTrack>> mNewTracksByAlbums;

    QList<MusicAudioTrack> mNewTracksList;

    QHash<QString, QUrl> mCovers;

};

DidlParser::DidlParser(QObject *parent) : QObject(parent), d(new DidlParserPrivate)
{
}

DidlParser::~DidlParser()
= default;

const QString &DidlParser::browseFlag() const
{
    return d->mBrowseFlag;
}

const QString &DidlParser::filter() const
{
    return d->mFilter;
}

const QString &DidlParser::sortCriteria() const
{
    return d->mSortCriteria;
}

const QString &DidlParser::searchCriteria() const
{
    return d->mSearchCriteria;
}

UpnpControlContentDirectory *DidlParser::contentDirectory() const
{
    return d->mContentDirectory;
}

bool DidlParser::isDataValid() const
{
    return d->mIsDataValid;
}

void DidlParser::setBrowseFlag(const QString &flag)
{
    d->mBrowseFlag = flag;
    Q_EMIT browseFlagChanged();
}

void DidlParser::setFilter(const QString &flag)
{
    d->mFilter = flag;
    Q_EMIT filterChanged();
}

void DidlParser::setSortCriteria(const QString &criteria)
{
    d->mSortCriteria = criteria;
    Q_EMIT sortCriteriaChanged();
}

void DidlParser::setSearchCriteria(const QString &criteria)
{
    d->mSearchCriteria = criteria;
    Q_EMIT searchCriteriaChanged();
}

void DidlParser::setContentDirectory(UpnpControlContentDirectory *directory)
{
    d->mContentDirectory = directory;

    if (!d->mContentDirectory) {
        Q_EMIT contentDirectoryChanged();
        return;
    }

    Q_EMIT contentDirectoryChanged();
}

void DidlParser::setParentId(QString parentId)
{
    if (d->mParentId == parentId)
        return;

    d->mParentId = parentId;
    emit parentIdChanged();
}

void DidlParser::systemUpdateIDChanged()
{
    search();
}

void DidlParser::browse(int startIndex, int maximumNmberOfResults)
{
    auto upnpAnswer = d->mContentDirectory->browse(d->mParentId, d->mBrowseFlag, d->mFilter, startIndex, maximumNmberOfResults, d->mSortCriteria);

    if (startIndex == 0) {
        d->mNewAlbumIds.clear();
        d->mNewAlbums.clear();
        d->mNewMusicTracks.clear();
        d->mNewMusicTrackIds.clear();
        d->mCovers.clear();
    }

    connect(upnpAnswer, &UpnpControlAbstractServiceReply::finished, this, &DidlParser::browseFinished);
}

void DidlParser::search(int startIndex, int maximumNumberOfResults)
{
    if (!d->mContentDirectory) {
        return;
    }

    if (startIndex == 0) {
        d->mNewAlbumIds.clear();
        d->mNewAlbums.clear();
        d->mNewMusicTracks.clear();
        d->mNewMusicTrackIds.clear();
        d->mCovers.clear();
    }

    auto upnpAnswer = d->mContentDirectory->search(d->mParentId, d->mSearchCriteria, d->mFilter, startIndex, maximumNumberOfResults, d->mSortCriteria);

    connect(upnpAnswer, &UpnpControlAbstractServiceReply::finished, this, &DidlParser::searchFinished);
}

QString DidlParser::parentId() const
{
    return d->mParentId;
}

const QVector<QString> &DidlParser::newAlbumIds() const
{
    return d->mNewAlbumIds;
}

const QHash<QString, MusicAlbum> &DidlParser::newAlbums() const
{
    return d->mNewAlbums;
}

const QVector<QString> &DidlParser::newMusicTrackIds() const
{
    return d->mNewMusicTrackIds;
}

const QList<MusicAudioTrack> &DidlParser::newMusicTracks() const
{
    return d->mNewTracksList;
}

const QHash<QString, QUrl> &DidlParser::covers() const
{
    return d->mCovers;
}

void DidlParser::browseFinished(UpnpControlAbstractServiceReply *self)
{
    const auto &resultData = self->result();

    bool success = self->success();

    if (!success) {
        d->mIsDataValid = false;
        Q_EMIT isDataValidChanged(d->mContentDirectory->description()->deviceDescription()->UDN().mid(5), d->mParentId);

        return;
    }

    QString result = resultData[QStringLiteral("Result")].toString();

    bool intConvert;
    auto numberReturned = resultData[QStringLiteral("NumberReturned")].toInt(&intConvert);

    if (!intConvert) {
        d->mIsDataValid = false;
        Q_EMIT isDataValidChanged(d->mContentDirectory->description()->deviceDescription()->UDN().mid(5), d->mParentId);

        return;
    }

    auto totalMatches = resultData[QStringLiteral("TotalMatches")].toInt(&intConvert);

    if (!intConvert) {
        d->mIsDataValid = false;
        Q_EMIT isDataValidChanged(d->mContentDirectory->description()->deviceDescription()->UDN().mid(5), d->mParentId);

        return;
    }

    if (totalMatches > numberReturned) {
        browse(d->mNewMusicTracks.size() + numberReturned);
    }

    QDomDocument browseDescription;
    browseDescription.setContent(result);

    browseDescription.documentElement();

    auto containerList = browseDescription.elementsByTagName(QStringLiteral("container"));
    for (int containerIndex = 0; containerIndex < containerList.length(); ++containerIndex) {
        const QDomNode &containerNode(containerList.at(containerIndex));
        if (!containerNode.isNull()) {
            decodeContainerNode(containerNode, d->mNewAlbums, d->mNewAlbumIds);
        }
    }

    auto itemList = browseDescription.elementsByTagName(QStringLiteral("item"));
    for (int itemIndex = 0; itemIndex < itemList.length(); ++itemIndex) {
        const QDomNode &itemNode(itemList.at(itemIndex));
        if (!itemNode.isNull()) {
            decodeAudioTrackNode(itemNode, d->mNewMusicTracks, d->mNewMusicTrackIds);
        }
    }

    groupNewTracksByAlbums();
    d->mIsDataValid = true;
    Q_EMIT isDataValidChanged(d->mContentDirectory->description()->deviceDescription()->UDN().mid(5), d->mParentId);
}

void DidlParser::groupNewTracksByAlbums()
{
    d->mNewTracksByAlbums.clear();
    for(const auto &newTrack : qAsConst(d->mNewMusicTracks)) {
        d->mNewTracksByAlbums[newTrack.albumName()].push_back(newTrack);
    }
}

void DidlParser::searchFinished(UpnpControlAbstractServiceReply *self)
{
    const auto &resultData = self->result();

    bool success = self->success();

    if (!success) {
        d->mIsDataValid = false;
        Q_EMIT isDataValidChanged(d->mContentDirectory->description()->deviceDescription()->UDN().mid(5), d->mParentId);

        return;
    }

    QString result = resultData[QStringLiteral("Result")].toString();

    bool intConvert;
    auto numberReturned = resultData[QStringLiteral("NumberReturned")].toInt(&intConvert);

    if (!intConvert) {
        d->mIsDataValid = false;
        Q_EMIT isDataValidChanged(d->mContentDirectory->description()->deviceDescription()->UDN().mid(5), d->mParentId);

        return;
    }

    auto totalMatches = resultData[QStringLiteral("TotalMatches")].toInt(&intConvert);

    if (!intConvert) {
        d->mIsDataValid = false;
        Q_EMIT isDataValidChanged(d->mContentDirectory->description()->deviceDescription()->UDN().mid(5), d->mParentId);

        return;
    }

    if (totalMatches > numberReturned) {
        search(d->mNewMusicTracks.size() + numberReturned, numberReturned);
    }

    QDomDocument browseDescription;
    browseDescription.setContent(result);

    browseDescription.documentElement();

    auto containerList = browseDescription.elementsByTagName(QStringLiteral("container"));
    for (int containerIndex = 0; containerIndex < containerList.length(); ++containerIndex) {
        const QDomNode &containerNode(containerList.at(containerIndex));
        if (!containerNode.isNull()) {
            decodeContainerNode(containerNode, d->mNewAlbums, d->mNewAlbumIds);
        }
    }

    auto itemList = browseDescription.elementsByTagName(QStringLiteral("item"));
    for (int itemIndex = 0; itemIndex < itemList.length(); ++itemIndex) {
        const QDomNode &itemNode(itemList.at(itemIndex));
        if (!itemNode.isNull()) {
            decodeAudioTrackNode(itemNode, d->mNewMusicTracks, d->mNewMusicTrackIds);
        }
    }

    groupNewTracksByAlbums();
    d->mIsDataValid = true;
    Q_EMIT isDataValidChanged(d->mContentDirectory->description()->deviceDescription()->UDN().mid(5), d->mParentId);
}

void DidlParser::decodeContainerNode(const QDomNode &containerNode, QHash<QString, MusicAlbum> &newData,
                                     QVector<QString> &newDataIds)
{
    auto parentID = containerNode.toElement().attribute(QStringLiteral("parentID"));
    const auto &id = containerNode.toElement().attribute(QStringLiteral("id"));

    newDataIds.push_back(id);
    auto &chilData = newData[id];

    chilData.setParentId(parentID);
    chilData.setId(id);

    const QString &childCount = containerNode.toElement().attribute(QStringLiteral("childCount"));
    chilData.setTracksCount(childCount.toInt());

    const QDomNode &titleNode = containerNode.firstChildElement(QStringLiteral("dc:title"));
    if (!titleNode.isNull()) {
        chilData.setTitle(titleNode.toElement().text());
    }

    const QDomNode &authorNode = containerNode.firstChildElement(QStringLiteral("upnp:artist"));
    if (!authorNode.isNull()) {
        chilData.setArtist(authorNode.toElement().text());
    }

    const QDomNode &resourceNode = containerNode.firstChildElement(QStringLiteral("res"));
    if (!resourceNode.isNull()) {
        chilData.setResourceURI(QUrl::fromUserInput(resourceNode.toElement().text()));
    }

#if 0
    const QDomNode &classNode = containerNode.firstChildElement(QStringLiteral("upnp:class"));
    if (classNode.toElement().text().startsWith(QLatin1String("object.container.album.musicAlbum"))) {
        chilData[ColumnsRoles::ItemClassRole] = DidlParser::Album;
    } else if (classNode.toElement().text().startsWith(QLatin1String("object.container.person.musicArtist"))) {
        chilData[ColumnsRoles::ItemClassRole] = DidlParser::Artist;
    } else if (classNode.toElement().text().startsWith(QLatin1String("object.container"))) {
        chilData[ColumnsRoles::ItemClassRole] = DidlParser::Container;
    }
#endif

    const QDomNode &albumArtNode = containerNode.firstChildElement(QStringLiteral("upnp:albumArtURI"));
    if (!albumArtNode.isNull()) {
        chilData.setAlbumArtURI(QUrl::fromUserInput(albumArtNode.toElement().text()));
    }
}

void DidlParser::decodeAudioTrackNode(const QDomNode &itemNode, QHash<QString, MusicAudioTrack> &newData,
                                      QVector<QString> &newDataIds)
{
    const QString &parentID = itemNode.toElement().attribute(QStringLiteral("parentID"));
    const QString &id = itemNode.toElement().attribute(QStringLiteral("id"));

    newDataIds.push_back(id);
    auto &chilData = newData[id];

    chilData.setParentId(parentID);
    chilData.setId(id);

    const QDomNode &titleNode = itemNode.firstChildElement(QStringLiteral("dc:title"));
    if (!titleNode.isNull()) {
        chilData.setTitle(titleNode.toElement().text());
    }

    const QDomNode &authorNode = itemNode.firstChildElement(QStringLiteral("dc:creator"));
    if (!authorNode.isNull()) {
        chilData.setArtist(authorNode.toElement().text());
    }

    const QDomNode &albumAuthorNode = itemNode.firstChildElement(QStringLiteral("upnp:artist"));
    if (!albumAuthorNode.isNull()) {
        chilData.setAlbumArtist(albumAuthorNode.toElement().text());
    }

    if (chilData.albumArtist().isEmpty()) {
        chilData.setAlbumArtist(chilData.artist());
    }

    if (chilData.artist().isEmpty()) {
        chilData.setArtist(chilData.albumArtist());
    }

    const QDomNode &albumNode = itemNode.firstChildElement(QStringLiteral("upnp:album"));
    if (!albumNode.isNull()) {
        chilData.setAlbumName(albumNode.toElement().text());
    }

    const QDomNode &albumArtNode = itemNode.firstChildElement(QStringLiteral("upnp:albumArtURI"));
    if (!albumArtNode.isNull()) {
        d->mCovers[chilData.albumName()] = QUrl::fromUserInput(albumArtNode.toElement().text());
    }

    const QDomNode &resourceNode = itemNode.firstChildElement(QStringLiteral("res"));
    if (!resourceNode.isNull()) {
        chilData.setResourceURI(QUrl::fromUserInput(resourceNode.toElement().text()));
        if (resourceNode.attributes().contains(QLatin1String("duration"))) {
            const QDomNode &durationNode = resourceNode.attributes().namedItem(QStringLiteral("duration"));
            QString durationValue = durationNode.nodeValue();
            if (durationValue.startsWith(QLatin1String("0:"))) {
                durationValue.remove(0, 2);
            }
            if (durationValue.contains(uint('.'))) {
                durationValue = durationValue.split(QLatin1Char('.')).first();
            }

            chilData.setDuration(QTime::fromString(durationValue, QStringLiteral("mm:ss")));
            if (!chilData.duration().isValid()) {
                chilData.setDuration(QTime::fromString(durationValue, QStringLiteral("hh:mm:ss")));
                if (!chilData.duration().isValid()) {
                    chilData.setDuration(QTime::fromString(durationValue, QStringLiteral("hh:mm:ss.z")));
                }
            }
        }

        const QDomNode &trackNumberNode = itemNode.firstChildElement(QStringLiteral("upnp:originalTrackNumber"));
        if (!trackNumberNode.isNull()) {
            chilData.setTrackNumber(trackNumberNode.toElement().text().toInt());
        }

#if 0
        if (resourceNode.attributes().contains(QLatin1String("artist"))) {
            const QDomNode &artistNode = resourceNode.attributes().namedItem(QStringLiteral("artist"));
            //chilData[ColumnsRoles::ArtistRole] = artistNode.nodeValue();
        }
#endif
    }
}


#include "moc_didlparser.cpp"

