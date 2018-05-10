/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "alltracksmodel.h"

#include <algorithm>

#include <QDebug>

class AllTracksModelPrivate
{
public:

    QHash<qulonglong, MusicAudioTrack> mAllTracks;

    QList<qulonglong> mIds;

};

AllTracksModel::AllTracksModel(QObject *parent) : QAbstractItemModel(parent), d(std::make_unique<AllTracksModelPrivate>())
{
}

AllTracksModel::~AllTracksModel()
= default;

int AllTracksModel::rowCount(const QModelIndex &parent) const
{
    auto tracksCount = 0;

    if (parent.isValid()) {
        return tracksCount;
    }

    tracksCount = d->mAllTracks.size();

    return tracksCount;
}

QHash<int, QByteArray> AllTracksModel::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();

    roles[static_cast<int>(ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(ColumnsRoles::DurationRole)] = "duration";
    roles[static_cast<int>(ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(ColumnsRoles::AlbumArtistRole)] = "albumArtist";
    roles[static_cast<int>(ColumnsRoles::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(ColumnsRoles::DiscNumberRole)] = "discNumber";
    roles[static_cast<int>(ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(ColumnsRoles::GenreRole)] = "genre";
    roles[static_cast<int>(ColumnsRoles::LyricistRole)] = "lyricist";
    roles[static_cast<int>(ColumnsRoles::ComposerRole)] = "composer";
    roles[static_cast<int>(ColumnsRoles::CommentRole)] = "comment";
    roles[static_cast<int>(ColumnsRoles::YearRole)] = "year";
    roles[static_cast<int>(ColumnsRoles::ChannelsRole)] = "channels";
    roles[static_cast<int>(ColumnsRoles::BitRateRole)] = "bitRate";
    roles[static_cast<int>(ColumnsRoles::SampleRateRole)] = "sampleRate";
    roles[static_cast<int>(ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ColumnsRoles::DatabaseIdRole)] = "databaseId";
    roles[static_cast<int>(ColumnsRoles::IsSingleDiscAlbumRole)] = "isSingleDiscAlbum";
    roles[static_cast<int>(ColumnsRoles::ContainerDataRole)] = "containerData";
    roles[static_cast<int>(ColumnsRoles::ResourceRole)] = "trackResource";
    roles[static_cast<int>(ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(ColumnsRoles::ShadowForImageRole)] = "shadowForImage";

    return roles;
}

Qt::ItemFlags AllTracksModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant AllTracksModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    const auto tracksCount = d->mAllTracks.size();

    if (!index.isValid()) {
        return result;
    }

    if (index.column() != 0) {
        return result;
    }

    if (index.row() < 0) {
        return result;
    }

    if (index.parent().isValid()) {
        return result;
    }

    if (index.internalId() != 0) {
        return result;
    }

    if (index.row() < 0 || index.row() >= tracksCount) {
        return result;
    }

    const auto &track = d->mAllTracks[d->mIds[index.row()]];

    switch(role)
    {
    case ColumnsRoles::TitleRole:
        if (d->mAllTracks[d->mIds[index.row()]].title().isEmpty()) {
            result = d->mAllTracks[d->mIds[index.row()]].resourceURI().fileName();
        } else {
            result = d->mAllTracks[d->mIds[index.row()]].title();
        }
        break;
    case ColumnsRoles::MilliSecondsDurationRole:
        result = d->mAllTracks[d->mIds[index.row()]].duration().msecsSinceStartOfDay();
        break;
    case ColumnsRoles::DurationRole:
    {
        QTime trackDuration = d->mAllTracks[d->mIds[index.row()]].duration();
        if (trackDuration.hour() == 0) {
            result = trackDuration.toString(QStringLiteral("mm:ss"));
        } else {
            result = trackDuration.toString();
        }
        break;
    }
    case ColumnsRoles::ArtistRole:
        result = d->mAllTracks[d->mIds[index.row()]].artist();
        break;
    case ColumnsRoles::AlbumRole:
        result = d->mAllTracks[d->mIds[index.row()]].albumName();
        break;
    case ColumnsRoles::AlbumArtistRole:
        result = d->mAllTracks[d->mIds[index.row()]].albumArtist();
        break;
    case ColumnsRoles::TrackNumberRole:
        result = d->mAllTracks[d->mIds[index.row()]].trackNumber();
        break;
    case ColumnsRoles::DiscNumberRole:
        result = d->mAllTracks[d->mIds[index.row()]].discNumber();
        break;
    case ColumnsRoles::IsSingleDiscAlbumRole:
        result = d->mAllTracks[d->mIds[index.row()]].isSingleDiscAlbum();
        break;
    case ColumnsRoles::RatingRole:
        result = d->mAllTracks[d->mIds[index.row()]].rating();
        break;
    case ColumnsRoles::GenreRole:
        result = d->mAllTracks[d->mIds[index.row()]].genre();
        break;
    case ColumnsRoles::LyricistRole:
        result = d->mAllTracks[d->mIds[index.row()]].lyricist();
        break;
    case ColumnsRoles::ComposerRole:
        result = d->mAllTracks[d->mIds[index.row()]].composer();
        break;
    case ColumnsRoles::CommentRole:
        result = d->mAllTracks[d->mIds[index.row()]].comment();
        break;
    case ColumnsRoles::YearRole:
        result = d->mAllTracks[d->mIds[index.row()]].year();
        break;
    case ColumnsRoles::ChannelsRole:
        result = d->mAllTracks[d->mIds[index.row()]].channels();
        break;
    case ColumnsRoles::BitRateRole:
        result = d->mAllTracks[d->mIds[index.row()]].bitRate();
        break;
    case ColumnsRoles::SampleRateRole:
        result = d->mAllTracks[d->mIds[index.row()]].sampleRate();
        break;
    case ColumnsRoles::ImageRole:
    {
        const auto &imageUrl = d->mAllTracks[d->mIds[index.row()]].albumCover();
        if (imageUrl.isValid()) {
            result = imageUrl;
        }
        break;
    }
    case ColumnsRoles::ResourceRole:
        result = d->mAllTracks[d->mIds[index.row()]].resourceURI();
        break;
    case ColumnsRoles::IdRole:
        result = d->mAllTracks[d->mIds[index.row()]].title();
        break;
    case ColumnsRoles::DatabaseIdRole:
        result = d->mAllTracks[d->mIds[index.row()]].databaseId();
        break;
    case ColumnsRoles::ContainerDataRole:
        result = QVariant::fromValue(d->mAllTracks[d->mIds[index.row()]]);
        break;
    case Qt::DisplayRole:
        result = track.title();
        break;
    case ColumnsRoles::SecondaryTextRole:
    {
        auto secondaryText = QString();
        secondaryText = QStringLiteral("<b>%1 - %2</b>%3");

        secondaryText = secondaryText.arg(track.trackNumber());
        secondaryText = secondaryText.arg(track.title());

        if (track.artist() == track.albumArtist()) {
            secondaryText = secondaryText.arg(QString());
        } else {
            auto artistText = QString();
            artistText = QStringLiteral(" - <i>%1</i>");
            artistText = artistText.arg(track.artist());
            secondaryText = secondaryText.arg(artistText);
        }

        result = secondaryText;
        break;
    }
    case ColumnsRoles::ImageUrlRole:
    {
        const auto &imageUrl = d->mAllTracks[d->mIds[index.row()]].albumCover();
        if (imageUrl.isValid()) {
            result = imageUrl;
        } else {
            result = QUrl(QStringLiteral("image://icon/media-optical-audio"));
        }
        break;
    }
    case ColumnsRoles::ShadowForImageRole:
        result = d->mAllTracks[d->mIds[index.row()]].albumCover().isValid();
        break;
    }

    return result;
}

QModelIndex AllTracksModel::index(int row, int column, const QModelIndex &parent) const
{
    auto result = QModelIndex();

    if (column != 0) {
        return result;
    }

    if (parent.isValid()) {
        return result;
    }

    if (row > d->mAllTracks.size() - 1) {
        return result;
    }

    result = createIndex(row, column);

    return result;
}

QModelIndex AllTracksModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    auto result = QModelIndex();

    return result;
}

int AllTracksModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

void AllTracksModel::tracksAdded(const QList<MusicAudioTrack> &allTracks)
{
    auto newAllTracks = d->mAllTracks;
    auto newTracksIds = QList<qulonglong>();

    int countNewTracks = 0;

    for (const auto &oneTrack : allTracks) {
        if (!newAllTracks.contains(oneTrack.databaseId())) {
            newAllTracks[oneTrack.databaseId()] = oneTrack;
            newTracksIds.push_back(oneTrack.databaseId());
            ++countNewTracks;
        }
    }

    if (countNewTracks > 0) {
        beginInsertRows({}, d->mAllTracks.size(), d->mAllTracks.size() + countNewTracks - 1);

        d->mAllTracks = newAllTracks;
        d->mIds.append(newTracksIds);

        endInsertRows();
    }
}

void AllTracksModel::trackRemoved(qulonglong removedTrackId)
{
    auto itTrack = std::find(d->mIds.begin(), d->mIds.end(), removedTrackId);
    if (itTrack == d->mIds.end()) {
        return;
    }

    auto position = itTrack - d->mIds.begin();

    beginRemoveRows({}, position, position);
    d->mIds.erase(itTrack);
    d->mAllTracks.remove(removedTrackId);
    endRemoveRows();
}

void AllTracksModel::trackModified(const MusicAudioTrack &modifiedTrack)
{
    auto trackExists = (d->mAllTracks.contains(modifiedTrack.databaseId()));
    if (!trackExists) {
        return;
    }

    auto itTrack = std::find(d->mIds.begin(), d->mIds.end(), modifiedTrack.databaseId());
    if (itTrack == d->mIds.end()) {
        return;
    }

    auto position = itTrack - d->mIds.begin();

    d->mAllTracks[modifiedTrack.databaseId()] = modifiedTrack;
    Q_EMIT dataChanged(index(position, 0), index(position, 0));
}


#include "moc_alltracksmodel.cpp"
