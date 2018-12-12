/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ALBUMMODEL_H
#define ALBUMMODEL_H

#include "elisaLib_export.h"

#include "databaseinterface.h"
#include "musicalbum.h"

#include <QAbstractItemModel>
#include <QVector>
#include <QHash>
#include <QString>

#include <memory>

class MusicAudioTrack;
class DatabaseInterface;
class AlbumModelPrivate;
class QMutex;

class ELISALIB_EXPORT AlbumModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(MusicAlbum albumData
               READ albumData
               WRITE setAlbumData
               NOTIFY albumDataChanged)

    Q_PROPERTY(QString title
               READ title
               NOTIFY titleChanged)

    Q_PROPERTY(QString author
               READ author
               NOTIFY authorChanged)

    Q_PROPERTY(int tracksCount
               READ tracksCount
               NOTIFY tracksCountChanged)

public:

    enum ItemClass {
        Container = 0,
        Album = 1,
        Artist = 2,
        AudioTrack = 3,
    };

    enum ColumnsRoles {
        TitleRole = Qt::UserRole + 1,
        DurationRole,
        StringDurationRole,
        MilliSecondsDurationRole,
        ArtistRole,
        AlbumRole,
        AlbumArtistRole,
        TrackNumberRole,
        DiscNumberRole,
        RatingRole,
        GenreRole,
        LyricistRole,
        ComposerRole,
        CommentRole,
        YearRole,
        ChannelsRole,
        BitRateRole,
        SampleRateRole,
        ImageRole,
        ResourceRole,
        IdRole,
        DatabaseIdRole,
        DiscFirstTrackRole,
        IsSingleDiscAlbumRole,
        ContainerDataRole,
        SecondaryTextRole,
        ImageUrlRole,
        ShadowForImageRole,
    };

    Q_ENUM(ColumnsRoles)

    using ListTrackDataType = DatabaseInterface::ListTrackDataType;

    using TrackDataType = DatabaseInterface::TrackDataType;

    using ListAlbumDataType = DatabaseInterface::ListAlbumDataType;

    using AlbumDataType = DatabaseInterface::AlbumDataType;

    explicit AlbumModel(QObject *parent = nullptr);

    ~AlbumModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    MusicAlbum albumData() const;

    QString title() const;

    QString author() const;

    int tracksCount() const;

    Q_INVOKABLE void loadAlbumData(qulonglong id);

Q_SIGNALS:

    void albumDataChanged();

    void titleChanged();

    void authorChanged();

    void tracksCountChanged();

    void requestAlbumData(qulonglong id);

public Q_SLOTS:

    void setAlbumData(const MusicAlbum &album);

    void albumModified(const AlbumDataType &modifiedAlbum);

    void albumRemoved(qulonglong modifiedAlbumId);

private:

    void trackAdded(const MusicAudioTrack &newTrack);

    void trackModified(const MusicAudioTrack &modifiedTrack);

    void trackRemoved(const MusicAudioTrack &removedTrack);

    QVariant internalDataTrack(const MusicAudioTrack &track, int role, int rowIndex) const;

    std::unique_ptr<AlbumModelPrivate> d;

};

Q_DECLARE_METATYPE(AlbumModel::ItemClass)

#endif // ALBUMMODEL_H
