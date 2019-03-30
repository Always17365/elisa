/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "databaseinterface.h"

#include "databaseLogging.h"

#include "musicaudiotrack.h"

#include <KI18n/KLocalizedString>

#include <QCoreApplication>

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QDateTime>
#include <QMutex>
#include <QVariant>
#include <QAtomicInt>
#include <QElapsedTimer>
#include <QDebug>

#include <algorithm>

class DatabaseInterfacePrivate
{
public:

    DatabaseInterfacePrivate(const QSqlDatabase &tracksDatabase)
        : mTracksDatabase(tracksDatabase), mSelectAlbumQuery(mTracksDatabase),
          mSelectTrackQuery(mTracksDatabase), mSelectAlbumIdFromTitleQuery(mTracksDatabase),
          mInsertAlbumQuery(mTracksDatabase), mSelectTrackIdFromTitleAlbumIdArtistQuery(mTracksDatabase),
          mInsertTrackQuery(mTracksDatabase), mSelectTracksFromArtist(mTracksDatabase),
          mSelectTrackFromIdQuery(mTracksDatabase), mSelectCountAlbumsForArtistQuery(mTracksDatabase),
          mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery(mTracksDatabase),
          mSelectAllAlbumsFromArtistQuery(mTracksDatabase), mSelectAllArtistsQuery(mTracksDatabase),
          mInsertArtistsQuery(mTracksDatabase), mSelectArtistByNameQuery(mTracksDatabase),
          mSelectArtistQuery(mTracksDatabase), mUpdateTrackStatistics(mTracksDatabase),
          mRemoveTrackQuery(mTracksDatabase), mRemoveAlbumQuery(mTracksDatabase),
          mRemoveArtistQuery(mTracksDatabase), mSelectAllTracksQuery(mTracksDatabase),
          mInsertTrackMapping(mTracksDatabase), mUpdateTrackFirstPlayStatistics(mTracksDatabase),
          mInsertMusicSource(mTracksDatabase), mSelectMusicSource(mTracksDatabase),
          mUpdateTrackPriority(mTracksDatabase), mUpdateTrackFileModifiedTime(mTracksDatabase),
          mSelectTracksMapping(mTracksDatabase), mSelectTracksMappingPriority(mTracksDatabase),
          mUpdateAlbumArtUriFromAlbumIdQuery(mTracksDatabase), mSelectTracksMappingPriorityByTrackId(mTracksDatabase),
          mSelectAlbumIdsFromArtist(mTracksDatabase), mSelectAllTrackFilesQuery(mTracksDatabase),
          mRemoveTracksMappingFromSource(mTracksDatabase), mRemoveTracksMapping(mTracksDatabase),
          mSelectTracksWithoutMappingQuery(mTracksDatabase), mSelectAlbumIdFromTitleAndArtistQuery(mTracksDatabase),
          mSelectAlbumIdFromTitleWithoutArtistQuery(mTracksDatabase),
          mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery(mTracksDatabase), mSelectAlbumArtUriFromAlbumIdQuery(mTracksDatabase),
          mInsertComposerQuery(mTracksDatabase), mSelectComposerByNameQuery(mTracksDatabase),
          mSelectComposerQuery(mTracksDatabase), mInsertLyricistQuery(mTracksDatabase),
          mSelectLyricistByNameQuery(mTracksDatabase), mSelectLyricistQuery(mTracksDatabase),
          mInsertGenreQuery(mTracksDatabase), mSelectGenreByNameQuery(mTracksDatabase),
          mSelectGenreQuery(mTracksDatabase), mSelectAllTracksShortQuery(mTracksDatabase),
          mSelectAllAlbumsShortQuery(mTracksDatabase), mSelectAllComposersQuery(mTracksDatabase),
          mSelectAllLyricistsQuery(mTracksDatabase), mSelectCountAlbumsForComposerQuery(mTracksDatabase),
          mSelectCountAlbumsForLyricistQuery(mTracksDatabase), mSelectAllGenresQuery(mTracksDatabase),
          mSelectGenreForArtistQuery(mTracksDatabase), mSelectGenreForAlbumQuery(mTracksDatabase),
          mUpdateTrackQuery(mTracksDatabase), mUpdateAlbumArtistQuery(mTracksDatabase),
          mUpdateAlbumArtistInTracksQuery(mTracksDatabase), mQueryMaximumTrackIdQuery(mTracksDatabase),
          mQueryMaximumAlbumIdQuery(mTracksDatabase), mQueryMaximumArtistIdQuery(mTracksDatabase),
          mQueryMaximumLyricistIdQuery(mTracksDatabase), mQueryMaximumComposerIdQuery(mTracksDatabase),
          mQueryMaximumGenreIdQuery(mTracksDatabase), mSelectAllArtistsWithGenreFilterQuery(mTracksDatabase),
          mSelectAllAlbumsShortWithGenreArtistFilterQuery(mTracksDatabase), mSelectAllAlbumsShortWithArtistFilterQuery(mTracksDatabase),
          mSelectAllRecentlyPlayedTracksQuery(mTracksDatabase), mSelectAllFrequentlyPlayedTracksQuery(mTracksDatabase),
          mClearTracksTable(mTracksDatabase), mClearAlbumsTable(mTracksDatabase), mClearArtistsTable(mTracksDatabase),
          mClearComposerTable(mTracksDatabase), mClearGenreTable(mTracksDatabase), mClearLyricistTable(mTracksDatabase),
          mArtistMatchGenreQuery(mTracksDatabase), mSelectTrackIdQuery(mTracksDatabase)
    {
    }

    QSqlDatabase mTracksDatabase;

    QSqlQuery mSelectAlbumQuery;

    QSqlQuery mSelectTrackQuery;

    QSqlQuery mSelectAlbumIdFromTitleQuery;

    QSqlQuery mInsertAlbumQuery;

    QSqlQuery mSelectTrackIdFromTitleAlbumIdArtistQuery;

    QSqlQuery mInsertTrackQuery;

    QSqlQuery mSelectTracksFromArtist;

    QSqlQuery mSelectTrackFromIdQuery;

    QSqlQuery mSelectCountAlbumsForArtistQuery;

    QSqlQuery mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery;

    QSqlQuery mSelectAllAlbumsFromArtistQuery;

    QSqlQuery mSelectAllArtistsQuery;

    QSqlQuery mInsertArtistsQuery;

    QSqlQuery mSelectArtistByNameQuery;

    QSqlQuery mSelectArtistQuery;

    QSqlQuery mUpdateTrackStatistics;

    QSqlQuery mRemoveTrackQuery;

    QSqlQuery mRemoveAlbumQuery;

    QSqlQuery mRemoveArtistQuery;

    QSqlQuery mSelectAllTracksQuery;

    QSqlQuery mInsertTrackMapping;

    QSqlQuery mUpdateTrackFirstPlayStatistics;

    QSqlQuery mInsertMusicSource;

    QSqlQuery mSelectMusicSource;

    QSqlQuery mUpdateTrackPriority;

    QSqlQuery mUpdateTrackFileModifiedTime;

    QSqlQuery mSelectTracksMapping;

    QSqlQuery mSelectTracksMappingPriority;

    QSqlQuery mUpdateAlbumArtUriFromAlbumIdQuery;

    QSqlQuery mSelectTracksMappingPriorityByTrackId;

    QSqlQuery mSelectAlbumIdsFromArtist;

    QSqlQuery mSelectAllTrackFilesQuery;

    QSqlQuery mRemoveTracksMappingFromSource;

    QSqlQuery mRemoveTracksMapping;

    QSqlQuery mSelectTracksWithoutMappingQuery;

    QSqlQuery mSelectAlbumIdFromTitleAndArtistQuery;

    QSqlQuery mSelectAlbumIdFromTitleWithoutArtistQuery;

    QSqlQuery mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery;

    QSqlQuery mSelectAlbumArtUriFromAlbumIdQuery;

    QSqlQuery mInsertComposerQuery;

    QSqlQuery mSelectComposerByNameQuery;

    QSqlQuery mSelectComposerQuery;

    QSqlQuery mInsertLyricistQuery;

    QSqlQuery mSelectLyricistByNameQuery;

    QSqlQuery mSelectLyricistQuery;

    QSqlQuery mInsertGenreQuery;

    QSqlQuery mSelectGenreByNameQuery;

    QSqlQuery mSelectGenreQuery;

    QSqlQuery mSelectAllTracksShortQuery;

    QSqlQuery mSelectAllAlbumsShortQuery;

    QSqlQuery mSelectAllComposersQuery;

    QSqlQuery mSelectAllLyricistsQuery;

    QSqlQuery mSelectCountAlbumsForComposerQuery;

    QSqlQuery mSelectCountAlbumsForLyricistQuery;

    QSqlQuery mSelectAllGenresQuery;

    QSqlQuery mSelectGenreForArtistQuery;

    QSqlQuery mSelectGenreForAlbumQuery;

    QSqlQuery mUpdateTrackQuery;

    QSqlQuery mUpdateAlbumArtistQuery;

    QSqlQuery mUpdateAlbumArtistInTracksQuery;

    QSqlQuery mQueryMaximumTrackIdQuery;

    QSqlQuery mQueryMaximumAlbumIdQuery;

    QSqlQuery mQueryMaximumArtistIdQuery;

    QSqlQuery mQueryMaximumLyricistIdQuery;

    QSqlQuery mQueryMaximumComposerIdQuery;

    QSqlQuery mQueryMaximumGenreIdQuery;

    QSqlQuery mSelectAllArtistsWithGenreFilterQuery;

    QSqlQuery mSelectAllAlbumsShortWithGenreArtistFilterQuery;

    QSqlQuery mSelectAllAlbumsShortWithArtistFilterQuery;

    QSqlQuery mSelectAllRecentlyPlayedTracksQuery;

    QSqlQuery mSelectAllFrequentlyPlayedTracksQuery;

    QSqlQuery mClearTracksTable;

    QSqlQuery mClearAlbumsTable;

    QSqlQuery mClearArtistsTable;

    QSqlQuery mClearComposerTable;

    QSqlQuery mClearGenreTable;

    QSqlQuery mClearLyricistTable;

    QSqlQuery mArtistMatchGenreQuery;

    QSqlQuery mSelectTrackIdQuery;

    QSet<qulonglong> mModifiedTrackIds;

    QSet<qulonglong> mModifiedAlbumIds;

    QSet<qulonglong> mModifiedArtistIds;

    QSet<qulonglong> mInsertedTracks;

    QSet<qulonglong> mInsertedAlbums;

    QSet<qulonglong> mInsertedArtists;

    qulonglong mAlbumId = 1;

    qulonglong mArtistId = 1;

    qulonglong mComposerId = 1;

    qulonglong mLyricistId = 1;

    qulonglong mGenreId = 1;

    qulonglong mTrackId = 1;

    QAtomicInt mStopRequest = 0;

    bool mInitFinished = false;

    bool mIsInBadState = false;

};

DatabaseInterface::DatabaseInterface(QObject *parent) : QObject(parent), d(nullptr)
{
}

DatabaseInterface::~DatabaseInterface()
{
    if (d) {
        d->mTracksDatabase.close();
    }
}

void DatabaseInterface::init(const QString &dbName, const QString &databaseFileName)
{
    QSqlDatabase tracksDatabase = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), dbName);

    if (!databaseFileName.isEmpty()) {
        tracksDatabase.setDatabaseName(QStringLiteral("file:") + databaseFileName);
    } else {
        tracksDatabase.setDatabaseName(QStringLiteral("file:memdb1?mode=memory"));
    }
    tracksDatabase.setConnectOptions(QStringLiteral("foreign_keys = ON;locking_mode = EXCLUSIVE;QSQLITE_OPEN_URI;QSQLITE_BUSY_TIMEOUT=500000"));

    auto result = tracksDatabase.open();
    if (result) {
        qCDebug(orgKdeElisaDatabase) << "database open";
    } else {
        qCDebug(orgKdeElisaDatabase) << "database not open";
    }
    qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::init" << (tracksDatabase.driver()->hasFeature(QSqlDriver::Transactions) ? "yes" : "no");

    tracksDatabase.exec(QStringLiteral("PRAGMA foreign_keys = ON;"));

    d = std::make_unique<DatabaseInterfacePrivate>(tracksDatabase);

    initDatabase();
    initRequest();

    if (!databaseFileName.isEmpty()) {
        reloadExistingDatabase();
    }
}

qulonglong DatabaseInterface::albumIdFromTitleAndArtist(const QString &title, const QString &artist)
{
    auto result = qulonglong{0};

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalAlbumIdFromTitleAndArtist(title, artist);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DatabaseInterface::ListTrackDataType DatabaseInterface::allTracksData()
{
    auto result = ListTrackDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalAllTracksPartialData();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DatabaseInterface::ListTrackDataType DatabaseInterface::recentlyPlayedTracksData(int count)
{
    auto result = ListTrackDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalRecentlyPlayedTracksData(count);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DatabaseInterface::ListTrackDataType DatabaseInterface::frequentlyPlayedTracksData(int count)
{
    auto result = ListTrackDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalFrequentlyPlayedTracksData(count);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DatabaseInterface::ListAlbumDataType DatabaseInterface::allAlbumsData()
{
    auto result = ListAlbumDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalAllAlbumsPartialData(d->mSelectAllAlbumsShortQuery);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DatabaseInterface::ListAlbumDataType DatabaseInterface::allAlbumsDataByGenreAndArtist(const QString &genre, const QString &artist)
{
    auto result = ListAlbumDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    d->mSelectAllAlbumsShortWithGenreArtistFilterQuery.bindValue(QStringLiteral(":artistFilter"), artist);
    d->mSelectAllAlbumsShortWithGenreArtistFilterQuery.bindValue(QStringLiteral(":genreFilter"), genre);

    result = internalAllAlbumsPartialData(d->mSelectAllAlbumsShortWithGenreArtistFilterQuery);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DatabaseInterface::ListAlbumDataType DatabaseInterface::allAlbumsDataByArtist(const QString &artist)
{
    auto result = ListAlbumDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    d->mSelectAllAlbumsShortWithArtistFilterQuery.bindValue(QStringLiteral(":artistFilter"), artist);

    result = internalAllAlbumsPartialData(d->mSelectAllAlbumsShortWithArtistFilterQuery);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DatabaseInterface::AlbumDataType DatabaseInterface::albumDataFromDatabaseId(qulonglong id)
{
    auto result = DatabaseInterface::AlbumDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalOneAlbumPartialData(id);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DatabaseInterface::ListTrackDataType DatabaseInterface::albumData(qulonglong databaseId)
{
    auto result = ListTrackDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    d->mSelectTrackQuery.bindValue(QStringLiteral(":albumId"), databaseId);

    auto queryResult = execQuery(d->mSelectTrackQuery);

    if (!queryResult || !d->mSelectTrackQuery.isSelect() || !d->mSelectTrackQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::albumData" << d->mSelectTrackQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::albumData" << d->mSelectTrackQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::albumData" << d->mSelectTrackQuery.lastError();
    }

    while (d->mSelectTrackQuery.next()) {
        const auto &currentRecord = d->mSelectTrackQuery.record();

        result.push_back(buildTrackDataFromDatabaseRecord(currentRecord));
    }

    d->mSelectTrackQuery.finish();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DatabaseInterface::ListArtistDataType DatabaseInterface::allArtistsData()
{
    auto result = ListArtistDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalAllArtistsPartialData(d->mSelectAllArtistsQuery);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DatabaseInterface::ListArtistDataType DatabaseInterface::allArtistsDataByGenre(const QString &genre)
{
    qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::allArtistsDataByGenre" << genre;

    auto result = ListArtistDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    d->mSelectAllArtistsWithGenreFilterQuery.bindValue(QStringLiteral(":genreFilter"), genre);

    result = internalAllArtistsPartialData(d->mSelectAllArtistsWithGenreFilterQuery);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DatabaseInterface::ListGenreDataType DatabaseInterface::allGenresData()
{
    auto result = ListGenreDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalAllGenresPartialData();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

bool DatabaseInterface::internalArtistMatchGenre(qulonglong databaseId, const QString &genre)
{
    auto result = true;

    if (!d) {
        return result;
    }

    d->mArtistMatchGenreQuery.bindValue(QStringLiteral(":databaseId"), databaseId);
    d->mArtistMatchGenreQuery.bindValue(QStringLiteral(":genreFilter"), genre);

    auto queryResult = execQuery(d->mArtistMatchGenreQuery);

    if (!queryResult || !d->mArtistMatchGenreQuery.isSelect() || !d->mArtistMatchGenreQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::artistMatchGenre" << d->mArtistMatchGenreQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::artistMatchGenre" << d->mArtistMatchGenreQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::artistMatchGenre" << d->mArtistMatchGenreQuery.lastError();

        d->mArtistMatchGenreQuery.finish();

        auto transactionResult = finishTransaction();
        if (!transactionResult) {
            return result;
        }

        return result;
    }

    result = d->mArtistMatchGenreQuery.next();

    d->mArtistMatchGenreQuery.finish();

    qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalArtistMatchGenre" << databaseId << (result ? "match" : "does not match");

    return result;
}

DatabaseInterface::ListTrackDataType DatabaseInterface::tracksDataFromAuthor(const QString &ArtistName)
{
    auto allTracks = ListTrackDataType{};

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return allTracks;
    }

    allTracks = internalTracksFromAuthor(ArtistName);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return allTracks;
    }

    return allTracks;
}

DatabaseInterface::TrackDataType DatabaseInterface::trackDataFromDatabaseId(qulonglong id)
{
    auto result = TrackDataType();

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalOneTrackPartialData(id);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

qulonglong DatabaseInterface::trackIdFromTitleAlbumTrackDiscNumber(const QString &title, const QString &artist, const QString &album,
                                                                   int trackNumber, int discNumber)
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalTrackIdFromTitleAlbumTracDiscNumber(title, artist, album, trackNumber, discNumber);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

qulonglong DatabaseInterface::trackIdFromFileName(const QUrl &fileName)
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalTrackIdFromFileName(fileName);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

void DatabaseInterface::applicationAboutToQuit()
{
    d->mStopRequest = 1;
}

void DatabaseInterface::askRestoredTracks()
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    auto result = internalAllFileName();

    Q_EMIT restoredTracks(result);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }
}

void DatabaseInterface::trackHasStartedPlaying(const QUrl &fileName, const QDateTime &time)
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    updateTrackStatistics(fileName, time);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }
}

void DatabaseInterface::clearData()
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    auto queryResult = execQuery(d->mClearTracksTable);

    if (!queryResult || !d->mClearTracksTable.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearTracksTable.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearTracksTable.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearTracksTable.lastError();
    }

    d->mClearTracksTable.finish();

    queryResult = execQuery(d->mClearAlbumsTable);

    if (!queryResult || !d->mClearAlbumsTable.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearAlbumsTable.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearAlbumsTable.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearAlbumsTable.lastError();
    }

    d->mClearAlbumsTable.finish();

    queryResult = execQuery(d->mClearComposerTable);

    if (!queryResult || !d->mClearComposerTable.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearComposerTable.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearComposerTable.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearComposerTable.lastError();
    }

    d->mClearComposerTable.finish();

    queryResult = execQuery(d->mClearLyricistTable);

    if (!queryResult || !d->mClearLyricistTable.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearLyricistTable.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearLyricistTable.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearLyricistTable.lastError();
    }

    d->mClearLyricistTable.finish();

    queryResult = execQuery(d->mClearGenreTable);

    if (!queryResult || !d->mClearGenreTable.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearGenreTable.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearGenreTable.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearGenreTable.lastError();
    }

    d->mClearGenreTable.finish();

    queryResult = execQuery(d->mClearArtistsTable);

    if (!queryResult || !d->mClearArtistsTable.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearArtistsTable.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearArtistsTable.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearArtistsTable.lastError();
    }

    d->mClearArtistsTable.finish();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }

    Q_EMIT cleanedDatabase();
}

void DatabaseInterface::initChangesTrackers()
{
    d->mModifiedTrackIds.clear();
    d->mModifiedAlbumIds.clear();
    d->mModifiedArtistIds.clear();
    d->mInsertedTracks.clear();
    d->mInsertedAlbums.clear();
    d->mInsertedArtists.clear();
}

void DatabaseInterface::recordModifiedTrack(qulonglong trackId)
{
    d->mModifiedTrackIds.insert(trackId);
}

void DatabaseInterface::recordModifiedAlbum(qulonglong albumId)
{
    d->mModifiedAlbumIds.insert(albumId);
}

void DatabaseInterface::insertTracksList(const QList<MusicAudioTrack> &tracks, const QHash<QString, QUrl> &covers)
{
    qCDebug(orgKdeElisaDatabase()) << "DatabaseInterface::insertTracksList" << tracks.count();
    if (d->mStopRequest == 1) {
        Q_EMIT finishInsertingTracksList();
        return;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        Q_EMIT finishInsertingTracksList();
        return;
    }

    initChangesTrackers();

    for(const auto &oneTrack : tracks) {
        d->mSelectTracksMapping.bindValue(QStringLiteral(":fileName"), oneTrack.resourceURI());

        auto result = execQuery(d->mSelectTracksMapping);

        if (!result || !d->mSelectTracksMapping.isSelect() || !d->mSelectTracksMapping.isActive()) {
            Q_EMIT databaseError();

            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertTracksList" << d->mSelectTracksMapping.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertTracksList" << d->mSelectTracksMapping.boundValues();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertTracksList" << d->mSelectTracksMapping.lastError();

            d->mSelectTracksMapping.finish();

            rollBackTransaction();
            Q_EMIT finishInsertingTracksList();
            return;
        }

        bool isNewTrack = !d->mSelectTracksMapping.next();

        if (isNewTrack) {
            insertTrackOrigin(oneTrack.resourceURI(), oneTrack.fileModificationTime(),
                              QDateTime::currentDateTime());
        } else if (!d->mSelectTracksMapping.record().value(0).isNull() && d->mSelectTracksMapping.record().value(0).toULongLong() != 0) {
            updateTrackOrigin(oneTrack.resourceURI(), oneTrack.fileModificationTime());
        }

        d->mSelectTracksMapping.finish();

        bool isInserted = false;

        const auto insertedTrackId = internalInsertTrack(oneTrack, covers, isInserted);

        if (isInserted && insertedTrackId != 0) {
            d->mInsertedTracks.insert(insertedTrackId);
        }

        if (d->mStopRequest == 1) {
            transactionResult = finishTransaction();
            if (!transactionResult) {
                Q_EMIT finishInsertingTracksList();
                return;
            }
            Q_EMIT finishInsertingTracksList();
            return;
        }
    }

    if (!d->mInsertedArtists.isEmpty()) {
        ListArtistDataType newArtists;

        for (auto artistId : qAsConst(d->mInsertedArtists)) {
            newArtists.push_back({{DatabaseIdRole, artistId}});
        }
        qCInfo(orgKdeElisaDatabase) << "artistsAdded" << newArtists.size();
        Q_EMIT artistsAdded(newArtists);
    }

    if (!d->mInsertedAlbums.isEmpty()) {
        ListAlbumDataType newAlbums;

        for (auto albumId : qAsConst(d->mInsertedAlbums)) {
            d->mModifiedAlbumIds.remove(albumId);
            newAlbums.push_back(internalOneAlbumPartialData(albumId));
        }

        qCInfo(orgKdeElisaDatabase) << "albumsAdded" << newAlbums.size();
        Q_EMIT albumsAdded(newAlbums);
    }

    for (auto albumId : qAsConst(d->mModifiedAlbumIds)) {
        Q_EMIT albumModified({{DatabaseIdRole, albumId}}, albumId);
    }

    if (!d->mInsertedTracks.isEmpty()) {
        ListTrackDataType newTracks;

        for (auto trackId : qAsConst(d->mInsertedTracks)) {
            newTracks.push_back(internalOneTrackPartialData(trackId));
            d->mModifiedTrackIds.remove(trackId);
        }

        qCInfo(orgKdeElisaDatabase) << "tracksAdded" << newTracks.size();
        Q_EMIT tracksAdded(newTracks);
    }

    for (auto trackId : qAsConst(d->mModifiedTrackIds)) {
        Q_EMIT trackModified(internalOneTrackPartialData(trackId));
    }

    transactionResult = finishTransaction();
    if (!transactionResult) {
        Q_EMIT finishInsertingTracksList();
        return;
    }
    Q_EMIT finishInsertingTracksList();
}

void DatabaseInterface::removeTracksList(const QList<QUrl> &removedTracks)
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        Q_EMIT finishRemovingTracksList();
        return;
    }

    initChangesTrackers();

    internalRemoveTracksList(removedTracks);

    if (!d->mInsertedArtists.isEmpty()) {
        ListArtistDataType newArtists;
        for (auto artistId : qAsConst(d->mInsertedArtists)) {
            newArtists.push_back({{DatabaseIdRole, artistId}});
        }
        Q_EMIT artistsAdded(newArtists);
    }

    transactionResult = finishTransaction();
    if (!transactionResult) {
        Q_EMIT finishRemovingTracksList();
        return;
    }

    Q_EMIT finishRemovingTracksList();
}

bool DatabaseInterface::startTransaction() const
{
    auto result = false;

    auto transactionResult = d->mTracksDatabase.transaction();
    if (!transactionResult) {
        qCDebug(orgKdeElisaDatabase) << "transaction failed" << d->mTracksDatabase.lastError() << d->mTracksDatabase.lastError().driverText();

        return result;
    }

    result = true;

    return result;
}

bool DatabaseInterface::finishTransaction() const
{
    auto result = false;

    auto transactionResult = d->mTracksDatabase.commit();

    if (!transactionResult) {
        qCDebug(orgKdeElisaDatabase) << "commit failed" << d->mTracksDatabase.lastError() << d->mTracksDatabase.lastError().nativeErrorCode();

        return result;
    }

    result = true;

    return result;
}

bool DatabaseInterface::rollBackTransaction() const
{
    auto result = false;

    auto transactionResult = d->mTracksDatabase.rollback();

    if (!transactionResult) {
        qCDebug(orgKdeElisaDatabase) << "commit failed" << d->mTracksDatabase.lastError() << d->mTracksDatabase.lastError().nativeErrorCode();

        return result;
    }

    result = true;

    return result;
}

void DatabaseInterface::initDatabase()
{
    auto listTables = d->mTracksDatabase.tables();

    if (listTables.contains(QStringLiteral("DatabaseVersionV2")) ||
        listTables.contains(QStringLiteral("DatabaseVersionV3")) ||
        listTables.contains(QStringLiteral("DatabaseVersionV4")) ||
        listTables.contains(QStringLiteral("DatabaseVersionV6")) ||
        listTables.contains(QStringLiteral("DatabaseVersionV7")) ||
        listTables.contains(QStringLiteral("DatabaseVersionV8")) ||
        listTables.contains(QStringLiteral("DatabaseVersionV10"))) {

        qCDebug(orgKdeElisaDatabase()) << "Old database schema unsupported: delete and start from scratch";
        qCDebug(orgKdeElisaDatabase()) << "list of old tables" << d->mTracksDatabase.tables();

        auto oldTables = QStringList{
                QStringLiteral("DatabaseVersionV2"),
                QStringLiteral("DatabaseVersionV3"),
                QStringLiteral("DatabaseVersionV4"),
                QStringLiteral("DatabaseVersionV5"),
                QStringLiteral("DatabaseVersionV6"),
                QStringLiteral("DatabaseVersionV7"),
                QStringLiteral("DatabaseVersionV8"),
                QStringLiteral("DatabaseVersionV10"),
                QStringLiteral("AlbumsArtists"),
                QStringLiteral("TracksArtists"),
                QStringLiteral("TracksMapping"),
                QStringLiteral("Tracks"),
                QStringLiteral("Composer"),
                QStringLiteral("Genre"),
                QStringLiteral("Lyricist"),
                QStringLiteral("Albums"),
                QStringLiteral("DiscoverSource"),
                QStringLiteral("Artists"),};
        for (const auto &oneTable : oldTables) {
            if (listTables.indexOf(oneTable) == -1) {
                continue;
            }

            QSqlQuery createSchemaQuery(d->mTracksDatabase);

            auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE ") + oneTable);

            if (!result) {
                qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initDatabase" << createSchemaQuery.lastQuery();
                qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError();

                Q_EMIT databaseError();
            }
        }

        listTables = d->mTracksDatabase.tables();
    }

    checkDatabaseSchema();
    listTables = d->mTracksDatabase.tables();

    if (listTables.contains(QStringLiteral("DatabaseVersionV5")) &&
        !listTables.contains(QStringLiteral("DatabaseVersionV9"))) {
        upgradeDatabaseV9();
        upgradeDatabaseV11();
        upgradeDatabaseV12();
    } else if (listTables.contains(QStringLiteral("DatabaseVersionV9"))) {
        if (!listTables.contains(QStringLiteral("DatabaseVersionV11"))) {
            upgradeDatabaseV11();
        }
        if (!listTables.contains(QStringLiteral("DatabaseVersionV12"))) {
            upgradeDatabaseV12();
        }
    } else {
        createDatabaseV9();
        upgradeDatabaseV11();
        upgradeDatabaseV12();
    }
}

void DatabaseInterface::createDatabaseV9()
{
    qCInfo(orgKdeElisaDatabase) << "begin creation of v9 database schema";

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DatabaseVersionV9` (`Version` INTEGER PRIMARY KEY NOT NULL)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DiscoverSource` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Name` VARCHAR(55) NOT NULL, "
                                                                   "UNIQUE (`Name`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Artists` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Name` VARCHAR(55) NOT NULL, "
                                                                   "UNIQUE (`Name`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Composer` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Name` VARCHAR(55) NOT NULL, "
                                                                   "UNIQUE (`Name`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Genre` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Name` VARCHAR(85) NOT NULL, "
                                                                   "UNIQUE (`Name`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Lyricist` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Name` VARCHAR(55) NOT NULL, "
                                                                   "UNIQUE (`Name`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Albums` ("
                                                                   "`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Title` VARCHAR(55) NOT NULL, "
                                                                   "`ArtistName` VARCHAR(55), "
                                                                   "`AlbumPath` VARCHAR(255) NOT NULL, "
                                                                   "`CoverFileName` VARCHAR(255) NOT NULL, "
                                                                   "UNIQUE (`Title`, `ArtistName`, `AlbumPath`), "
                                                                   "CONSTRAINT fk_artists FOREIGN KEY (`ArtistName`) REFERENCES `Artists`(`Name`) "
                                                                   "ON DELETE CASCADE)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Tracks` ("
                                                                   "`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Title` VARCHAR(85) NOT NULL, "
                                                                   "`ArtistName` VARCHAR(55), "
                                                                   "`AlbumTitle` VARCHAR(55), "
                                                                   "`AlbumArtistName` VARCHAR(55), "
                                                                   "`AlbumPath` VARCHAR(255), "
                                                                   "`TrackNumber` INTEGER DEFAULT -1, "
                                                                   "`DiscNumber` INTEGER DEFAULT -1, "
                                                                   "`Duration` INTEGER NOT NULL, "
                                                                   "`Rating` INTEGER NOT NULL DEFAULT 0, "
                                                                   "`Genre` VARCHAR(55), "
                                                                   "`Composer` VARCHAR(55), "
                                                                   "`Lyricist` VARCHAR(55), "
                                                                   "`Comment` VARCHAR(255) DEFAULT '', "
                                                                   "`Year` INTEGER DEFAULT 0, "
                                                                   "`Channels` INTEGER DEFAULT -1, "
                                                                   "`BitRate` INTEGER DEFAULT -1, "
                                                                   "`SampleRate` INTEGER DEFAULT -1, "
                                                                   "`HasEmbeddedCover` BOOLEAN NOT NULL, "
                                                                   "`ImportDate` INTEGER NOT NULL, "
                                                                   "`FirstPlayDate` INTEGER, "
                                                                   "`LastPlayDate` INTEGER, "
                                                                   "`PlayCounter` INTEGER NOT NULL, "
                                                                   "UNIQUE ("
                                                                   "`Title`, `AlbumTitle`, `AlbumArtistName`, "
                                                                   "`AlbumPath`, `TrackNumber`, `DiscNumber`"
                                                                   "), "
                                                                   "CONSTRAINT fk_artist FOREIGN KEY (`ArtistName`) REFERENCES `Artists`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_composer FOREIGN KEY (`Composer`) REFERENCES `Composer`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_lyricist FOREIGN KEY (`Lyricist`) REFERENCES `Lyricist`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_genre FOREIGN KEY (`Genre`) REFERENCES `Genre`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_album FOREIGN KEY ("
                                                                   "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"
                                                                   "REFERENCES `Albums`(`Title`, `ArtistName`, `AlbumPath`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `TracksMapping` ("
                                                                   "`TrackID` INTEGER NULL, "
                                                                   "`DiscoverID` INTEGER NOT NULL, "
                                                                   "`FileName` VARCHAR(255) NOT NULL, "
                                                                   "`Priority` INTEGER NOT NULL, "
                                                                   "`FileModifiedTime` DATETIME NOT NULL, "
                                                                   "PRIMARY KEY (`FileName`), "
                                                                   "CONSTRAINT TracksUnique UNIQUE (`TrackID`, `Priority`), "
                                                                   "CONSTRAINT fk_tracksmapping_trackID FOREIGN KEY (`TrackID`) REFERENCES `Tracks`(`ID`) ON DELETE CASCADE, "
                                                                   "CONSTRAINT fk_tracksmapping_discoverID FOREIGN KEY (`DiscoverID`) REFERENCES `DiscoverSource`(`ID`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TitleAlbumsIndex` ON `Albums` "
                                                                  "(`Title`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`ArtistNameAlbumsIndex` ON `Albums` "
                                                                  "(`ArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksAlbumIndex` ON `Tracks` "
                                                                  "(`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`ArtistNameIndex` ON `Tracks` "
                                                                  "(`ArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`AlbumArtistNameIndex` ON `Tracks` "
                                                                  "(`AlbumArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksFileNameIndex` ON `TracksMapping` "
                                                                  "(`FileName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    qCInfo(orgKdeElisaDatabase) << "end creation of v9 database schema";
}

void DatabaseInterface::upgradeDatabaseV9()
{
    qCInfo(orgKdeElisaDatabase) << "begin update to v9 of database schema";

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DatabaseVersionV9` (`Version` INTEGER PRIMARY KEY NOT NULL)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery disableForeignKeys(d->mTracksDatabase);

        auto result = disableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=OFF"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << disableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << disableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.transaction();

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `NewAlbums` ("
                                                                   "`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Title` VARCHAR(55) NOT NULL, "
                                                                   "`ArtistName` VARCHAR(55), "
                                                                   "`AlbumPath` VARCHAR(255) NOT NULL, "
                                                                   "`CoverFileName` VARCHAR(255) NOT NULL, "
                                                                   "`AlbumInternalID` VARCHAR(55), "
                                                                   "UNIQUE (`Title`, `ArtistName`, `AlbumPath`), "
                                                                   "CONSTRAINT fk_artists FOREIGN KEY (`ArtistName`) REFERENCES `Artists`(`Name`) "
                                                                   "ON DELETE CASCADE)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();
        }
    }

    {
        QSqlQuery copyDataQuery(d->mTracksDatabase);

        auto result = copyDataQuery.exec(QStringLiteral("INSERT INTO `NewAlbums` "
                                                        "SELECT "
                                                        "album.`ID`, "
                                                        "album.`Title`, "
                                                        "artist.`Name`, "
                                                        "album.`AlbumPath`, "
                                                        "album.`CoverFileName`, "
                                                        "album.`AlbumInternalID` "
                                                        "FROM "
                                                        "`Albums` album, "
                                                        "`AlbumsArtists` albumArtistMapping, "
                                                        "`Artists` artist "
                                                        "WHERE "
                                                        "album.`ID` = albumArtistMapping.`AlbumID` AND "
                                                        "albumArtistMapping.`ArtistID` = artist.`ID`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << copyDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << copyDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `Albums`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `AlbumsArtists`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("ALTER TABLE `NewAlbums` RENAME TO `Albums`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `NewTracks` ("
                                                                   "`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Title` VARCHAR(85) NOT NULL, "
                                                                   "`ArtistName` VARCHAR(55), "
                                                                   "`AlbumTitle` VARCHAR(55), "
                                                                   "`AlbumArtistName` VARCHAR(55), "
                                                                   "`AlbumPath` VARCHAR(255), "
                                                                   "`TrackNumber` INTEGER DEFAULT -1, "
                                                                   "`DiscNumber` INTEGER DEFAULT -1, "
                                                                   "`Duration` INTEGER NOT NULL, "
                                                                   "`Rating` INTEGER NOT NULL DEFAULT 0, "
                                                                   "`Genre` VARCHAR(55), "
                                                                   "`Composer` VARCHAR(55), "
                                                                   "`Lyricist` VARCHAR(55), "
                                                                   "`Comment` VARCHAR(255) DEFAULT '', "
                                                                   "`Year` INTEGER DEFAULT 0, "
                                                                   "`Channels` INTEGER DEFAULT -1, "
                                                                   "`BitRate` INTEGER DEFAULT -1, "
                                                                   "`SampleRate` INTEGER DEFAULT -1, "
                                                                   "`HasEmbeddedCover` BOOLEAN NOT NULL, "
                                                                   "`ImportDate` INTEGER NOT NULL, "
                                                                   "`FirstPlayDate` INTEGER, "
                                                                   "`LastPlayDate` INTEGER, "
                                                                   "`PlayCounter` INTEGER NOT NULL, "
                                                                   "UNIQUE ("
                                                                   "`Title`, `AlbumTitle`, `AlbumArtistName`, "
                                                                   "`AlbumPath`, `TrackNumber`, `DiscNumber`"
                                                                   "), "
                                                                   "CONSTRAINT fk_artist FOREIGN KEY (`ArtistName`) REFERENCES `Artists`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_composer FOREIGN KEY (`Composer`) REFERENCES `Composer`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_lyricist FOREIGN KEY (`Lyricist`) REFERENCES `Lyricist`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_genre FOREIGN KEY (`Genre`) REFERENCES `Genre`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_album FOREIGN KEY ("
                                                                   "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"
                                                                   "REFERENCES `Albums`(`Title`, `ArtistName`, `AlbumPath`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery copyDataQuery(d->mTracksDatabase);

        auto result = copyDataQuery.exec(QStringLiteral("INSERT INTO `NewTracks` "
                                                        "(`ID`, `Title`, `ArtistName`, "
                                                        "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`, "
                                                        "`TrackNumber`, `DiscNumber`, `Duration`, "
                                                        "`Rating`, `Genre`, `Composer`, "
                                                        "`Lyricist`, `Comment`, `Year`, "
                                                        "`Channels`, `BitRate`, `SampleRate`, "
                                                        "`HasEmbeddedCover`, `ImportDate`, `PlayCounter`) "
                                                        "SELECT "
                                                        "track.`ID`, "
                                                        "track.`Title`, "
                                                        "artist.`Name`, "
                                                        "album.`Title`, "
                                                        "album.`ArtistName`, "
                                                        "album.`AlbumPath`, "
                                                        "track.`TrackNumber`, "
                                                        "track.`DiscNumber`, "
                                                        "track.`Duration`, "
                                                        "track.`Rating`, "
                                                        "genre.`Name`, "
                                                        "composer.`Name`, "
                                                        "lyricist.`Name`, "
                                                        "track.`Comment`, "
                                                        "track.`Year`, "
                                                        "track.`Channels`, "
                                                        "track.`BitRate`, "
                                                        "track.`SampleRate`, "
                                                        "FALSE, "
                                                        "strftime('%s', 'now'), "
                                                        "0 "
                                                        "FROM "
                                                        "`Tracks` track, "
                                                        "`TracksArtists` trackArtistMapping, "
                                                        "`Artists` artist, "
                                                        "`Albums` album "
                                                        "left join "
                                                        "`Genre` genre "
                                                        "on track.`GenreID` = genre.`ID` "
                                                        "left join "
                                                        "`Composer` composer "
                                                        "on track.`ComposerID` = composer.`ID` "
                                                        "left join "
                                                        "`Lyricist` lyricist "
                                                        "on track.`LyricistID` = lyricist.`ID` "
                                                        "WHERE "
                                                        "track.`ID` = trackArtistMapping.`TrackID` AND "
                                                        "trackArtistMapping.`ArtistID` = artist.`ID` AND "
                                                        "track.`AlbumID` = album.`ID`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << copyDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << copyDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `Tracks`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `TracksArtists`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("ALTER TABLE `NewTracks` RENAME TO `Tracks`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.commit();

    {
        QSqlQuery enableForeignKeys(d->mTracksDatabase);

        auto result = enableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=ON"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << enableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << enableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    qCInfo(orgKdeElisaDatabase) << "finished update to v9 of database schema";
}

void DatabaseInterface::upgradeDatabaseV11()
{
    qCInfo(orgKdeElisaDatabase) << "begin update to v11 of database schema";

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DatabaseVersionV11` (`Version` INTEGER PRIMARY KEY NOT NULL)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery disableForeignKeys(d->mTracksDatabase);

        auto result = disableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=OFF"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << disableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << disableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.transaction();

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `TracksData` ("
                                                                   "`DiscoverID` INTEGER NOT NULL, "
                                                                   "`FileName` VARCHAR(255) NOT NULL, "
                                                                   "`FileModifiedTime` DATETIME NOT NULL, "
                                                                   "`ImportDate` INTEGER NOT NULL, "
                                                                   "`FirstPlayDate` INTEGER, "
                                                                   "`LastPlayDate` INTEGER, "
                                                                   "`PlayCounter` INTEGER NOT NULL, "
                                                                   "PRIMARY KEY (`FileName`, `DiscoverID`), "
                                                                   "CONSTRAINT fk_tracksmapping_discoverID FOREIGN KEY (`DiscoverID`) REFERENCES `DiscoverSource`(`ID`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastError();
        }
    }

    {
        QSqlQuery copyDataQuery(d->mTracksDatabase);

        auto result = copyDataQuery.exec(QStringLiteral("INSERT INTO `TracksData` "
                                                        "SELECT "
                                                        "m.`DiscoverID`, "
                                                        "m.`FileName`, "
                                                        "m.`FileModifiedTime`, "
                                                        "t.`ImportDate`, "
                                                        "t.`FirstPlayDate`, "
                                                        "t.`LastPlayDate`, "
                                                        "t.`PlayCounter` "
                                                        "FROM "
                                                        "`Tracks` t, "
                                                        "`TracksMapping` m "
                                                        "WHERE "
                                                        "t.`ID` = m.`TrackID`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << copyDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << copyDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `NewTracks` ("
                                                            "`ID` INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                            "`DiscoverID` INTEGER NOT NULL, "
                                                            "`FileName` VARCHAR(255) NOT NULL, "
                                                            "`Priority` INTEGER NOT NULL, "
                                                            "`Title` VARCHAR(85) NOT NULL, "
                                                            "`ArtistName` VARCHAR(55), "
                                                            "`AlbumTitle` VARCHAR(55), "
                                                            "`AlbumArtistName` VARCHAR(55), "
                                                            "`AlbumPath` VARCHAR(255), "
                                                            "`TrackNumber` INTEGER, "
                                                            "`DiscNumber` INTEGER, "
                                                            "`Duration` INTEGER NOT NULL, "
                                                            "`Rating` INTEGER NOT NULL DEFAULT 0, "
                                                            "`Genre` VARCHAR(55), "
                                                            "`Composer` VARCHAR(55), "
                                                            "`Lyricist` VARCHAR(55), "
                                                            "`Comment` VARCHAR(255), "
                                                            "`Year` INTEGER, "
                                                            "`Channels` INTEGER, "
                                                            "`BitRate` INTEGER, "
                                                            "`SampleRate` INTEGER, "
                                                            "`HasEmbeddedCover` BOOLEAN NOT NULL, "
                                                            "UNIQUE ("
                                                            "`FileName`"
                                                            "), "
                                                            "UNIQUE ("
                                                            "`Priority`, `Title`, `ArtistName`, "
                                                            "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`"
                                                            "), "
                                                            "CONSTRAINT fk_fileName FOREIGN KEY (`FileName`, `DiscoverID`) "
                                                            "REFERENCES `TracksData`(`FileName`, `DiscoverID`) ON DELETE CASCADE, "
                                                            "CONSTRAINT fk_artist FOREIGN KEY (`ArtistName`) REFERENCES `Artists`(`Name`), "
                                                            "CONSTRAINT fk_tracks_composer FOREIGN KEY (`Composer`) REFERENCES `Composer`(`Name`), "
                                                            "CONSTRAINT fk_tracks_lyricist FOREIGN KEY (`Lyricist`) REFERENCES `Lyricist`(`Name`), "
                                                            "CONSTRAINT fk_tracks_genre FOREIGN KEY (`Genre`) REFERENCES `Genre`(`Name`), "
                                                            "CONSTRAINT fk_tracks_discoverID FOREIGN KEY (`DiscoverID`) REFERENCES `DiscoverSource`(`ID`)"
                                                            "CONSTRAINT fk_tracks_album FOREIGN KEY ("
                                                            "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"
                                                            "REFERENCES `Albums`(`Title`, `ArtistName`, `AlbumPath`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery copyDataQuery(d->mTracksDatabase);

        auto result = copyDataQuery.exec(QStringLiteral("INSERT OR IGNORE INTO `NewTracks` "
                                                        "("
                                                        "`DiscoverID`, "
                                                        "`FileName`, "
                                                        "`Priority`, "
                                                        "`Title`, "
                                                        "`ArtistName`, "
                                                        "`AlbumTitle`, "
                                                        "`AlbumArtistName`, "
                                                        "`AlbumPath`, "
                                                        "`TrackNumber`, "
                                                        "`DiscNumber`, "
                                                        "`Duration`, "
                                                        "`Rating`, "
                                                        "`Genre`, "
                                                        "`Composer`, "
                                                        "`Lyricist`, "
                                                        "`Comment`, "
                                                        "`Year`, "
                                                        "`Channels`, "
                                                        "`BitRate`, "
                                                        "`SampleRate`, "
                                                        "`HasEmbeddedCover`"
                                                        ") "
                                                        "SELECT "
                                                        "m.`DiscoverID`, "
                                                        "m.`FileName`, "
                                                        "m.`Priority`, "
                                                        "t.`Title`, "
                                                        "t.`ArtistName`, "
                                                        "t.`AlbumTitle`, "
                                                        "t.`AlbumArtistName`, "
                                                        "t.`AlbumPath`, "
                                                        "t.`TrackNumber`, "
                                                        "t.`DiscNumber`, "
                                                        "t.`Duration`, "
                                                        "t.`Rating`, "
                                                        "t.`Genre`, "
                                                        "t.`Composer`, "
                                                        "t.`Lyricist`, "
                                                        "t.`Comment`, "
                                                        "t.`Year`, "
                                                        "t.`Channels`, "
                                                        "t.`BitRate`, "
                                                        "t.`SampleRate`, "
                                                        "t.`HasEmbeddedCover` "
                                                        "FROM "
                                                        "`Tracks` t, "
                                                        "`TracksMapping` m "
                                                        "WHERE "
                                                        "t.`ID` = m.`TrackID`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << copyDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << copyDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery updateDataQuery(d->mTracksDatabase);

        auto result = updateDataQuery.exec(QStringLiteral("UPDATE `NewTracks` "
                                                          "SET "
                                                          "`TrackNumber` = NULL "
                                                          "WHERE "
                                                          "`TrackNumber` = -1"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery updateDataQuery(d->mTracksDatabase);

        auto result = updateDataQuery.exec(QStringLiteral("UPDATE `NewTracks` "
                                                          "SET "
                                                          "`Channels` = NULL "
                                                          "WHERE "
                                                          "`Channels` = -1"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery updateDataQuery(d->mTracksDatabase);

        auto result = updateDataQuery.exec(QStringLiteral("UPDATE `NewTracks` "
                                                          "SET "
                                                          "`BitRate` = NULL "
                                                          "WHERE "
                                                          "`BitRate` = -1"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery updateDataQuery(d->mTracksDatabase);

        auto result = updateDataQuery.exec(QStringLiteral("UPDATE `NewTracks` "
                                                          "SET "
                                                          "`SampleRate` = NULL "
                                                          "WHERE "
                                                          "`SampleRate` = -1"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `Tracks`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `TracksMapping`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("ALTER TABLE `NewTracks` RENAME TO `Tracks`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.commit();

    {
        QSqlQuery enableForeignKeys(d->mTracksDatabase);

        auto result = enableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=ON"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << enableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << enableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksAlbumIndex` ON `Tracks` "
                                                                  "(`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`ArtistNameIndex` ON `Tracks` "
                                                                  "(`ArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`AlbumArtistNameIndex` ON `Tracks` "
                                                                  "(`AlbumArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksUniqueData` ON `Tracks` "
                                                                  "(`Title`, `ArtistName`, "
                                                                  "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksUniqueDataPriority` ON `Tracks` "
                                                                  "(`Priority`, `Title`, `ArtistName`, "
                                                                  "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksFileNameIndex` ON `Tracks` "
                                                                  "(`FileName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    qCInfo(orgKdeElisaDatabase) << "finished update to v11 of database schema";
}

void DatabaseInterface::upgradeDatabaseV12()
{
    qCInfo(orgKdeElisaDatabase) << "begin update to v12 of database schema";

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DatabaseVersionV12` (`Version` INTEGER PRIMARY KEY NOT NULL)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery disableForeignKeys(d->mTracksDatabase);

        auto result = disableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=OFF"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << disableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << disableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.transaction();

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `NewTracks` ("
                                                                   "`ID` INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                                   "`FileName` VARCHAR(255) NOT NULL, "
                                                                   "`Priority` INTEGER NOT NULL, "
                                                                   "`Title` VARCHAR(85) NOT NULL, "
                                                                   "`ArtistName` VARCHAR(55), "
                                                                   "`AlbumTitle` VARCHAR(55), "
                                                                   "`AlbumArtistName` VARCHAR(55), "
                                                                   "`AlbumPath` VARCHAR(255), "
                                                                   "`TrackNumber` INTEGER, "
                                                                   "`DiscNumber` INTEGER, "
                                                                   "`Duration` INTEGER NOT NULL, "
                                                                   "`Rating` INTEGER NOT NULL DEFAULT 0, "
                                                                   "`Genre` VARCHAR(55), "
                                                                   "`Composer` VARCHAR(55), "
                                                                   "`Lyricist` VARCHAR(55), "
                                                                   "`Comment` VARCHAR(255), "
                                                                   "`Year` INTEGER, "
                                                                   "`Channels` INTEGER, "
                                                                   "`BitRate` INTEGER, "
                                                                   "`SampleRate` INTEGER, "
                                                                   "`HasEmbeddedCover` BOOLEAN NOT NULL, "
                                                                   "UNIQUE ("
                                                                   "`FileName`"
                                                                   "), "
                                                                   "UNIQUE ("
                                                                   "`Priority`, `Title`, `ArtistName`, "
                                                                   "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`"
                                                                   "), "
                                                                   "CONSTRAINT fk_fileName FOREIGN KEY (`FileName`) "
                                                                   "REFERENCES `TracksData`(`FileName`) ON DELETE CASCADE, "
                                                                   "CONSTRAINT fk_artist FOREIGN KEY (`ArtistName`) REFERENCES `Artists`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_composer FOREIGN KEY (`Composer`) REFERENCES `Composer`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_lyricist FOREIGN KEY (`Lyricist`) REFERENCES `Lyricist`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_genre FOREIGN KEY (`Genre`) REFERENCES `Genre`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_album FOREIGN KEY ("
                                                                   "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"
                                                                   "REFERENCES `Albums`(`Title`, `ArtistName`, `AlbumPath`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `NewTracksData` ("
                                                                   "`FileName` VARCHAR(255) NOT NULL, "
                                                                   "`FileModifiedTime` DATETIME NOT NULL, "
                                                                   "`ImportDate` INTEGER NOT NULL, "
                                                                   "`FirstPlayDate` INTEGER, "
                                                                   "`LastPlayDate` INTEGER, "
                                                                   "`PlayCounter` INTEGER NOT NULL, "
                                                                   "PRIMARY KEY (`FileName`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastError();
        }
    }

    {
        QSqlQuery copyDataQuery(d->mTracksDatabase);

        auto result = copyDataQuery.exec(QStringLiteral("INSERT INTO `NewTracksData` "
                                                        "SELECT "
                                                        "td.`FileName`, "
                                                        "td.`FileModifiedTime`, "
                                                        "td.`ImportDate`, "
                                                        "td.`FirstPlayDate`, "
                                                        "td.`LastPlayDate`, "
                                                        "td.`PlayCounter` "
                                                        "FROM "
                                                        "`TracksData` td"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << copyDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << copyDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery copyDataQuery(d->mTracksDatabase);

        auto result = copyDataQuery.exec(QStringLiteral("INSERT INTO `NewTracks` "
                                                        "SELECT "
                                                        "t.`ID`, "
                                                        "t.`FileName`, "
                                                        "t.`Priority`, "
                                                        "t.`Title`, "
                                                        "t.`ArtistName`, "
                                                        "t.`AlbumTitle`, "
                                                        "t.`AlbumArtistName`, "
                                                        "t.`AlbumPath`, "
                                                        "t.`TrackNumber`, "
                                                        "t.`DiscNumber`, "
                                                        "t.`Duration`, "
                                                        "t.`Rating`, "
                                                        "t.`Genre`, "
                                                        "t.`Composer`, "
                                                        "t.`Lyricist`, "
                                                        "t.`Comment`, "
                                                        "t.`Year`, "
                                                        "t.`Channels`, "
                                                        "t.`BitRate`, "
                                                        "t.`SampleRate`, "
                                                        "t.`HasEmbeddedCover` "
                                                        "FROM "
                                                        "`Tracks` t"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << copyDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << copyDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `TracksData`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `Tracks`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("ALTER TABLE `NewTracksData` RENAME TO `TracksData`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("ALTER TABLE `NewTracks` RENAME TO `Tracks`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.commit();

    {
        QSqlQuery enableForeignKeys(d->mTracksDatabase);

        auto result = enableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=ON"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << enableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << enableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksAlbumIndex` ON `Tracks` "
                                                                  "(`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`ArtistNameIndex` ON `Tracks` "
                                                                  "(`ArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`AlbumArtistNameIndex` ON `Tracks` "
                                                                  "(`AlbumArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksUniqueData` ON `Tracks` "
                                                                  "(`Title`, `ArtistName`, "
                                                                  "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksUniqueDataPriority` ON `Tracks` "
                                                                  "(`Priority`, `Title`, `ArtistName`, "
                                                                  "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksFileNameIndex` ON `Tracks` "
                                                                  "(`FileName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    qCInfo(orgKdeElisaDatabase) << "finished update to v12 of database schema";
}

void DatabaseInterface::checkDatabaseSchema()
{
    checkAlbumsTableSchema();
    if (d->mIsInBadState)
    {
        resetDatabase();
        return;
    }

    checkArtistsTableSchema();
    if (d->mIsInBadState)
    {
        resetDatabase();
        return;
    }

    checkComposerTableSchema();
    if (d->mIsInBadState)
    {
        resetDatabase();
        return;
    }

    checkGenreTableSchema();
    if (d->mIsInBadState)
    {
        resetDatabase();
        return;
    }

    checkLyricistTableSchema();
    if (d->mIsInBadState)
    {
        resetDatabase();
        return;
    }

    checkTracksTableSchema();
    if (d->mIsInBadState)
    {
        resetDatabase();
        return;
    }

    checkTracksDataTableSchema();
    if (d->mIsInBadState)
    {
        resetDatabase();
        return;
    }
}

void DatabaseInterface::checkAlbumsTableSchema()
{
    auto fieldsList = QStringList{QStringLiteral("ID"), QStringLiteral("Title"),
                                  QStringLiteral("ArtistName"), QStringLiteral("AlbumPath"),
                                  QStringLiteral("CoverFileName")};

    genericCheckTable(QStringLiteral("Albums"), fieldsList);
}

void DatabaseInterface::checkArtistsTableSchema()
{
    auto fieldsList = QStringList{QStringLiteral("ID"), QStringLiteral("Name")};

    genericCheckTable(QStringLiteral("Artists"), fieldsList);
}

void DatabaseInterface::checkComposerTableSchema()
{
    auto fieldsList = QStringList{QStringLiteral("ID"), QStringLiteral("Name")};

    genericCheckTable(QStringLiteral("Composer"), fieldsList);
}

void DatabaseInterface::checkGenreTableSchema()
{
    auto fieldsList = QStringList{QStringLiteral("ID"), QStringLiteral("Name")};

    genericCheckTable(QStringLiteral("Genre"), fieldsList);
}

void DatabaseInterface::checkLyricistTableSchema()
{
    auto fieldsList = QStringList{QStringLiteral("ID"), QStringLiteral("Name")};

    genericCheckTable(QStringLiteral("Lyricist"), fieldsList);
}

void DatabaseInterface::checkTracksTableSchema()
{
    auto fieldsList = QStringList{QStringLiteral("ID"), QStringLiteral("FileName"),
                                  QStringLiteral("Priority"), QStringLiteral("Title"),
                                  QStringLiteral("ArtistName"), QStringLiteral("AlbumTitle"),
                                  QStringLiteral("AlbumArtistName"), QStringLiteral("AlbumPath"),
                                  QStringLiteral("TrackNumber"), QStringLiteral("DiscNumber"),
                                  QStringLiteral("Duration"), QStringLiteral("Rating"),
                                  QStringLiteral("Genre"), QStringLiteral("Composer"),
                                  QStringLiteral("Lyricist"), QStringLiteral("Comment"),
                                  QStringLiteral("Year"), QStringLiteral("Channels"),
                                  QStringLiteral("BitRate"), QStringLiteral("SampleRate"),
                                  QStringLiteral("HasEmbeddedCover")};

    genericCheckTable(QStringLiteral("Tracks"), fieldsList);
}

void DatabaseInterface::checkTracksDataTableSchema()
{
    auto fieldsList = QStringList{QStringLiteral("FileName"), QStringLiteral("FileModifiedTime"),
                                  QStringLiteral("ImportDate"), QStringLiteral("FirstPlayDate"),
                                  QStringLiteral("LastPlayDate"), QStringLiteral("PlayCounter")};

    genericCheckTable(QStringLiteral("TracksData"), fieldsList);
}

void DatabaseInterface::genericCheckTable(const QString &tableName, const QStringList &expectedColumns)
{
    auto columnsList = d->mTracksDatabase.record(tableName);

    if (columnsList.count() != expectedColumns.count()) {
        qCInfo(orgKdeElisaDatabase()) << tableName << "table has wrong number of columns" << columnsList.count() << "expected" << expectedColumns.count();
        d->mIsInBadState = true;
        return;
    }

    for (const auto &oneField : expectedColumns) {
        if (!columnsList.contains(oneField)) {
            qCInfo(orgKdeElisaDatabase()) << tableName << "table has missing column" << oneField;
            d->mIsInBadState = true;
            return;
        }
    }
}

void DatabaseInterface::resetDatabase()
{
    qCInfo(orgKdeElisaDatabase()) << "Full reset of database due to corrupted database";

    auto listTables = d->mTracksDatabase.tables();

    while(!listTables.isEmpty()) {
        for (const auto &oneTable : listTables) {
            QSqlQuery createSchemaQuery(d->mTracksDatabase);

            qCDebug(orgKdeElisaDatabase()) << "dropping table" << oneTable;
            createSchemaQuery.exec(QStringLiteral("DROP TABLE ") + oneTable);
        }

        listTables = d->mTracksDatabase.tables();

        if (listTables == QStringList{QStringLiteral("sqlite_sequence")}) {
            break;
        }
    }

    d->mIsInBadState = false;
}

void DatabaseInterface::initRequest()
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    {
        auto selectAlbumQueryText = QStringLiteral("SELECT "
                                                   "album.`ID`, "
                                                   "album.`Title`, "
                                                   "album.`ArtistName`, "
                                                   "album.`AlbumPath`, "
                                                   "album.`CoverFileName`, "
                                                   "("
                                                   "SELECT "
                                                   "COUNT(*) "
                                                   "FROM "
                                                   "`Tracks` tracks3 "
                                                   "WHERE "
                                                   "tracks3.`AlbumTitle` = album.`Title` AND "
                                                   "(tracks3.`AlbumArtistName` = album.`ArtistName` OR "
                                                   "(tracks3.`AlbumArtistName` IS NULL AND "
                                                   "album.`ArtistName` IS NULL"
                                                   ")"
                                                   ") AND "
                                                   "tracks3.`AlbumPath` = album.`AlbumPath` "
                                                   ") as `TracksCount`, "
                                                   "("
                                                   "SELECT "
                                                   "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                   "FROM "
                                                   "`Tracks` tracks2 "
                                                   "WHERE "
                                                   "tracks2.`AlbumTitle` = album.`Title` AND "
                                                   "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                   "(tracks2.`AlbumArtistName` IS NULL AND "
                                                   "album.`ArtistName` IS NULL"
                                                   ")"
                                                   ") AND "
                                                   "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                   ") as `IsSingleDiscAlbum`, "
                                                   "GROUP_CONCAT(tracks.`ArtistName`, ', ') as AllArtists, "
                                                   "MAX(tracks.`Rating`) as HighestRating, "
                                                   "GROUP_CONCAT(genres.`Name`, ', ') as AllGenres "
                                                   "FROM "
                                                   "`Albums` album LEFT JOIN "
                                                   "`Tracks` tracks ON "
                                                   "tracks.`AlbumTitle` = album.`Title` AND "
                                                   "("
                                                   "tracks.`AlbumArtistName` = album.`ArtistName` OR "
                                                   "("
                                                   "tracks.`AlbumArtistName` IS NULL AND "
                                                   "album.`ArtistName` IS NULL"
                                                   ")"
                                                   ") AND "
                                                   "tracks.`AlbumPath` = album.`AlbumPath`"
                                                   "LEFT JOIN "
                                                   "`Genre` genres ON tracks.`Genre` = genres.`Name` "
                                                   "WHERE "
                                                   "album.`ID` = :albumId "
                                                   "GROUP BY album.`ID`");

        auto result = prepareQuery(d->mSelectAlbumQuery, selectAlbumQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllGenresText = QStringLiteral("SELECT "
                                                  "genre.`ID`, "
                                                  "genre.`Name` "
                                                  "FROM `Genre` genre "
                                                  "ORDER BY genre.`Name` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllGenresQuery, selectAllGenresText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllGenresQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllGenresQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllAlbumsText = QStringLiteral("SELECT "
                                                  "album.`ID`, "
                                                  "album.`Title`, "
                                                  "album.`ArtistName` as SecondaryText, "
                                                  "album.`CoverFileName`, "
                                                  "album.`ArtistName`, "
                                                  "GROUP_CONCAT(tracks.`ArtistName`, ', ') as AllArtists, "
                                                  "MAX(tracks.`Rating`) as HighestRating, "
                                                  "GROUP_CONCAT(genres.`Name`, ', ') as AllGenres, "
                                                  "("
                                                  "SELECT "
                                                  "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                  "FROM "
                                                  "`Tracks` tracks2 "
                                                  "WHERE "
                                                  "tracks2.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks2.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ")"
                                                  ") AND "
                                                  "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                  ") as `IsSingleDiscAlbum` "
                                                  "FROM "
                                                  "`Albums` album, "
                                                  "`Tracks` tracks LEFT JOIN "
                                                  "`Genre` genres ON tracks.`Genre` = genres.`Name` "
                                                  "WHERE "
                                                  "tracks.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ")"
                                                  ") AND "
                                                  "tracks.`AlbumPath` = album.`AlbumPath` "
                                                  "GROUP BY album.`ID`, album.`Title`, album.`AlbumPath` "
                                                  "ORDER BY album.`Title` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllAlbumsShortQuery, selectAllAlbumsText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllAlbumsShortQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllAlbumsShortQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllAlbumsText = QStringLiteral("SELECT "
                                                  "album.`ID`, "
                                                  "album.`Title`, "
                                                  "album.`ArtistName` as SecondaryText, "
                                                  "album.`CoverFileName`, "
                                                  "album.`ArtistName`, "
                                                  "GROUP_CONCAT(tracks.`ArtistName`, ', ') as AllArtists, "
                                                  "MAX(tracks.`Rating`) as HighestRating, "
                                                  "GROUP_CONCAT(genres.`Name`, ', ') as AllGenres "
                                                  "FROM "
                                                  "`Albums` album, "
                                                  "`Tracks` tracks LEFT JOIN "
                                                  "`Genre` genres ON tracks.`Genre` = genres.`Name` "
                                                  "WHERE "
                                                  "tracks.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ")"
                                                  ") AND "
                                                  "tracks.`AlbumPath` = album.`AlbumPath` AND "
                                                  "EXISTS ("
                                                  "  SELECT tracks2.`Genre` "
                                                  "  FROM "
                                                  "  `Tracks` tracks2, "
                                                  "  `Genre` genre2 "
                                                  "  WHERE "
                                                  "  tracks2.`AlbumTitle` = album.`Title` AND "
                                                  "  tracks2.`AlbumArtistName` = album.`ArtistName` AND "
                                                  "  tracks2.`Genre` = genre2.`Name` AND "
                                                  "  genre2.`Name` = :genreFilter AND "
                                                  "  (tracks2.`ArtistName` = :artistFilter OR tracks2.`AlbumArtistName` = :artistFilter) "
                                                  ") "
                                                  "GROUP BY album.`ID`, album.`Title`, album.`AlbumPath` "
                                                  "ORDER BY album.`Title` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllAlbumsShortWithGenreArtistFilterQuery, selectAllAlbumsText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllAlbumsShortWithGenreArtistFilterQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllAlbumsShortWithGenreArtistFilterQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllAlbumsText = QStringLiteral("SELECT "
                                                  "album.`ID`, "
                                                  "album.`Title`, "
                                                  "album.`ArtistName` as SecondaryText, "
                                                  "album.`CoverFileName`, "
                                                  "album.`ArtistName`, "
                                                  "GROUP_CONCAT(tracks.`ArtistName`, ', ') as AllArtists, "
                                                  "MAX(tracks.`Rating`) as HighestRating, "
                                                  "GROUP_CONCAT(genres.`Name`, ', ') as AllGenres "
                                                  "FROM "
                                                  "`Albums` album, "
                                                  "`Tracks` tracks LEFT JOIN "
                                                  "`Genre` genres ON tracks.`Genre` = genres.`Name` "
                                                  "WHERE "
                                                  "tracks.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ")"
                                                  ") AND "
                                                  "tracks.`AlbumPath` = album.`AlbumPath` AND "
                                                  "EXISTS ("
                                                  "  SELECT tracks2.`Genre` "
                                                  "  FROM "
                                                  "  `Tracks` tracks2 "
                                                  "  WHERE "
                                                  "  tracks2.`AlbumTitle` = album.`Title` AND "
                                                  "  tracks2.`AlbumArtistName` = album.`ArtistName` AND "
                                                  "  (tracks2.`ArtistName` = :artistFilter OR tracks2.`AlbumArtistName` = :artistFilter) "
                                                  ") "
                                                  "GROUP BY album.`ID`, album.`Title`, album.`AlbumPath` "
                                                  "ORDER BY album.`Title` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllAlbumsShortWithArtistFilterQuery, selectAllAlbumsText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllAlbumsShortWithArtistFilterQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllAlbumsShortWithArtistFilterQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllArtistsWithFilterText = QStringLiteral("SELECT artists.`ID`, "
                                                             "artists.`Name`, "
                                                             "GROUP_CONCAT(genres.`Name`, ', ') as AllGenres "
                                                             "FROM `Artists` artists  LEFT JOIN "
                                                             "`Tracks` tracks ON artists.`Name` = tracks.`ArtistName` LEFT JOIN "
                                                             "`Genre` genres ON tracks.`Genre` = genres.`Name` "
                                                             "GROUP BY artists.`ID` "
                                                             "ORDER BY artists.`Name` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllArtistsQuery, selectAllArtistsWithFilterText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllArtistsQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllArtistsQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllArtistsWithGenreFilterText = QStringLiteral("SELECT artists.`ID`, "
                                                                  "artists.`Name`, "
                                                                  "GROUP_CONCAT(genres.`Name`, ', ') as AllGenres "
                                                                  "FROM `Artists` artists  LEFT JOIN "
                                                                  "`Tracks` tracks ON (tracks.`ArtistName` = artists.`Name` OR tracks.`AlbumArtistName` = artists.`Name`) LEFT JOIN "
                                                                  "`Genre` genres ON tracks.`Genre` = genres.`Name` "
                                                                  "WHERE "
                                                                  "EXISTS ("
                                                                  "  SELECT tracks2.`Genre` "
                                                                  "  FROM "
                                                                  "  `Tracks` tracks2, "
                                                                  "  `Genre` genre2 "
                                                                  "  WHERE "
                                                                  "  (tracks2.`ArtistName` = artists.`Name` OR tracks2.`AlbumArtistName` = artists.`Name`) AND "
                                                                  "  tracks2.`Genre` = genre2.`Name` AND "
                                                                  "  genre2.`Name` = :genreFilter "
                                                                  ") "
                                                                  "GROUP BY artists.`ID` "
                                                                  "ORDER BY artists.`Name` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllArtistsWithGenreFilterQuery, selectAllArtistsWithGenreFilterText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllArtistsWithGenreFilterQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllArtistsWithGenreFilterQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto artistMatchGenreText = QStringLiteral("SELECT artists.`ID` "
                                                   "FROM `Artists` artists  LEFT JOIN "
                                                   "`Tracks` tracks ON (tracks.`ArtistName` = artists.`Name` OR tracks.`AlbumArtistName` = artists.`Name`) LEFT JOIN "
                                                   "`Genre` genres ON tracks.`Genre` = genres.`Name` "
                                                   "WHERE "
                                                   "EXISTS ("
                                                   "  SELECT tracks2.`Genre` "
                                                   "  FROM "
                                                   "  `Tracks` tracks2, "
                                                   "  `Genre` genre2 "
                                                   "  WHERE "
                                                   "  (tracks2.`ArtistName` = artists.`Name` OR tracks2.`AlbumArtistName` = artists.`Name`) AND "
                                                   "  tracks2.`Genre` = genre2.`Name` AND "
                                                   "  genre2.`Name` = :genreFilter "
                                                   ") AND "
                                                   "artists.`ID` = :databaseId");

        auto result = prepareQuery(d->mArtistMatchGenreQuery, artistMatchGenreText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mArtistMatchGenreQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mArtistMatchGenreQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllComposersWithFilterText = QStringLiteral("SELECT `ID`, "
                                                               "`Name` "
                                                               "FROM `Artists` "
                                                               "ORDER BY `Name` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllComposersQuery, selectAllComposersWithFilterText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllComposersQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllComposersQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllLyricistsWithFilterText = QStringLiteral("SELECT `ID`, "
                                                               "`Name` "
                                                               "FROM `Lyricist` "
                                                               "ORDER BY `Name` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllLyricistsQuery, selectAllLyricistsWithFilterText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllLyricistsQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllLyricistsQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllTracksText = QStringLiteral("SELECT "
                                                  "tracks.`ID`, "
                                                  "tracks.`Title`, "
                                                  "album.`ID`, "
                                                  "tracks.`ArtistName`, "
                                                  "tracks.`AlbumArtistName`, "
                                                  "tracksMapping.`FileName`, "
                                                  "tracksMapping.`FileModifiedTime`, "
                                                  "tracks.`TrackNumber`, "
                                                  "tracks.`DiscNumber`, "
                                                  "tracks.`Duration`, "
                                                  "tracks.`AlbumTitle`, "
                                                  "tracks.`Rating`, "
                                                  "album.`CoverFileName`, "
                                                  "("
                                                  "SELECT "
                                                  "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                  "FROM "
                                                  "`Tracks` tracks2 "
                                                  "WHERE "
                                                  "tracks2.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks2.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ")"
                                                  ") AND "
                                                  "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                  ") as `IsSingleDiscAlbum`, "
                                                  "trackGenre.`Name`, "
                                                  "trackComposer.`Name`, "
                                                  "trackLyricist.`Name`, "
                                                  "tracks.`Comment`, "
                                                  "tracks.`Year`, "
                                                  "tracks.`Channels`, "
                                                  "tracks.`BitRate`, "
                                                  "tracks.`SampleRate`, "
                                                  "tracks.`HasEmbeddedCover`, "
                                                  "tracksMapping.`ImportDate`, "
                                                  "tracksMapping.`FirstPlayDate`, "
                                                  "tracksMapping.`LastPlayDate`, "
                                                  "tracksMapping.`PlayCounter`, "
                                                  "tracksMapping.`PlayCounter` / (strftime('%s', 'now') - tracksMapping.`FirstPlayDate`) as PlayFrequency "
                                                  "FROM "
                                                  "`Tracks` tracks, "
                                                  "`TracksData` tracksMapping "
                                                  "LEFT JOIN "
                                                  "`Albums` album "
                                                  "ON "
                                                  "tracks.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                  "tracks.`AlbumPath` = album.`AlbumPath` "
                                                  "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                  "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                  "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                  "WHERE "
                                                  "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                  "tracks.`Priority` = ("
                                                  "     SELECT "
                                                  "     MIN(`Priority`) "
                                                  "     FROM "
                                                  "     `Tracks` tracks2 "
                                                  "     WHERE "
                                                  "     tracks.`Title` = tracks2.`Title` AND "
                                                  "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                  "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                  "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                  "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                  ")"
                                                  "");

        auto result = prepareQuery(d->mSelectAllTracksQuery, selectAllTracksText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllTracksQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllTracksQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllTracksText = QStringLiteral("SELECT "
                                                  "tracks.`ID`, "
                                                  "tracks.`Title`, "
                                                  "album.`ID`, "
                                                  "tracks.`ArtistName`, "
                                                  "tracks.`AlbumArtistName`, "
                                                  "tracksMapping.`FileName`, "
                                                  "tracksMapping.`FileModifiedTime`, "
                                                  "tracks.`TrackNumber`, "
                                                  "tracks.`DiscNumber`, "
                                                  "tracks.`Duration`, "
                                                  "tracks.`AlbumTitle`, "
                                                  "tracks.`Rating`, "
                                                  "album.`CoverFileName`, "
                                                  "("
                                                  "SELECT "
                                                  "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                  "FROM "
                                                  "`Tracks` tracks2 "
                                                  "WHERE "
                                                  "tracks2.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks2.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ")"
                                                  ") AND "
                                                  "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                  ") as `IsSingleDiscAlbum`, "
                                                  "trackGenre.`Name`, "
                                                  "trackComposer.`Name`, "
                                                  "trackLyricist.`Name`, "
                                                  "tracks.`Comment`, "
                                                  "tracks.`Year`, "
                                                  "tracks.`Channels`, "
                                                  "tracks.`BitRate`, "
                                                  "tracks.`SampleRate`, "
                                                  "tracks.`HasEmbeddedCover`, "
                                                  "tracksMapping.`ImportDate`, "
                                                  "tracksMapping.`FirstPlayDate`, "
                                                  "tracksMapping.`LastPlayDate`, "
                                                  "tracksMapping.`PlayCounter`, "
                                                  "tracksMapping.`PlayCounter` / (strftime('%s', 'now') - tracksMapping.`FirstPlayDate`) as PlayFrequency "
                                                  "FROM "
                                                  "`Tracks` tracks, "
                                                  "`TracksData` tracksMapping "
                                                  "LEFT JOIN "
                                                  "`Albums` album "
                                                  "ON "
                                                  "tracks.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                  "tracks.`AlbumPath` = album.`AlbumPath` "
                                                  "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                  "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                  "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                  "WHERE "
                                                  "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                  "tracksMapping.`PlayCounter` > 0 AND "
                                                  "tracks.`Priority` = ("
                                                  "     SELECT "
                                                  "     MIN(`Priority`) "
                                                  "     FROM "
                                                  "     `Tracks` tracks2 "
                                                  "     WHERE "
                                                  "     tracks.`Title` = tracks2.`Title` AND "
                                                  "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                  "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                  "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                  "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                  ")"
                                                  "ORDER BY tracksMapping.`LastPlayDate` DESC "
                                                  "LIMIT :maximumResults");

        auto result = prepareQuery(d->mSelectAllRecentlyPlayedTracksQuery, selectAllTracksText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllRecentlyPlayedTracksQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllRecentlyPlayedTracksQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllTracksText = QStringLiteral("SELECT "
                                                  "tracks.`ID`, "
                                                  "tracks.`Title`, "
                                                  "album.`ID`, "
                                                  "tracks.`ArtistName`, "
                                                  "tracks.`AlbumArtistName`, "
                                                  "tracksMapping.`FileName`, "
                                                  "tracksMapping.`FileModifiedTime`, "
                                                  "tracks.`TrackNumber`, "
                                                  "tracks.`DiscNumber`, "
                                                  "tracks.`Duration`, "
                                                  "tracks.`AlbumTitle`, "
                                                  "tracks.`Rating`, "
                                                  "album.`CoverFileName`, "
                                                  "("
                                                  "SELECT "
                                                  "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                  "FROM "
                                                  "`Tracks` tracks2 "
                                                  "WHERE "
                                                  "tracks2.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks2.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ")"
                                                  ") AND "
                                                  "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                  ") as `IsSingleDiscAlbum`, "
                                                  "trackGenre.`Name`, "
                                                  "trackComposer.`Name`, "
                                                  "trackLyricist.`Name`, "
                                                  "tracks.`Comment`, "
                                                  "tracks.`Year`, "
                                                  "tracks.`Channels`, "
                                                  "tracks.`BitRate`, "
                                                  "tracks.`SampleRate`, "
                                                  "tracks.`HasEmbeddedCover`, "
                                                  "tracksMapping.`ImportDate`, "
                                                  "tracksMapping.`FirstPlayDate`, "
                                                  "tracksMapping.`LastPlayDate`, "
                                                  "tracksMapping.`PlayCounter`, "
                                                  "CAST(tracksMapping.`PlayCounter` AS REAL) / ((CAST(strftime('%s','now') as INTEGER) - CAST(tracksMapping.`FirstPlayDate` / 1000 as INTEGER)) / CAST(1000 AS REAL)) as PlayFrequency "
                                                  "FROM "
                                                  "`Tracks` tracks, "
                                                  "`TracksData` tracksMapping "
                                                  "LEFT JOIN "
                                                  "`Albums` album "
                                                  "ON "
                                                  "tracks.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                  "tracks.`AlbumPath` = album.`AlbumPath` "
                                                  "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                  "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                  "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                  "WHERE "
                                                  "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                  "tracksMapping.`PlayCounter` > 0 AND "
                                                  "tracks.`Priority` = ("
                                                  "     SELECT "
                                                  "     MIN(`Priority`) "
                                                  "     FROM "
                                                  "     `Tracks` tracks2 "
                                                  "     WHERE "
                                                  "     tracks.`Title` = tracks2.`Title` AND "
                                                  "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                  "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                  "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                  "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                  ")"
                                                  "ORDER BY CAST(tracksMapping.`PlayCounter` AS REAL) / ((CAST(strftime('%s','now') as INTEGER) - CAST(tracksMapping.`FirstPlayDate` / 1000 as INTEGER)) / CAST(1000 AS REAL)) DESC "
                                                  "LIMIT :maximumResults");

        auto result = prepareQuery(d->mSelectAllFrequentlyPlayedTracksQuery, selectAllTracksText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllFrequentlyPlayedTracksQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllFrequentlyPlayedTracksQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto clearAlbumsTableText = QStringLiteral("DELETE FROM `Albums`");

        auto result = prepareQuery(d->mClearAlbumsTable, clearAlbumsTableText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearAlbumsTable.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearAlbumsTable.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto clearArtistsTableText = QStringLiteral("DELETE FROM `Artists`");

        auto result = prepareQuery(d->mClearArtistsTable, clearArtistsTableText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearArtistsTable.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearArtistsTable.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto clearComposerTableText = QStringLiteral("DELETE FROM `Composer`");

        auto result = prepareQuery(d->mClearComposerTable, clearComposerTableText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearComposerTable.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearComposerTable.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto clearGenreTableText = QStringLiteral("DELETE FROM `Genre`");

        auto result = prepareQuery(d->mClearGenreTable, clearGenreTableText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearGenreTable.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearGenreTable.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto clearLyricistTableText = QStringLiteral("DELETE FROM `Lyricist`");

        auto result = prepareQuery(d->mClearLyricistTable, clearLyricistTableText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearLyricistTable.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearLyricistTable.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto clearTracksTableText = QStringLiteral("DELETE FROM `Tracks`");

        auto result = prepareQuery(d->mClearTracksTable, clearTracksTableText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearTracksTable.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearTracksTable.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllTracksShortText = QStringLiteral("SELECT "
                                                       "tracks.`ID`, "
                                                       "tracks.`Title`, "
                                                       "tracks.`ArtistName`, "
                                                       "tracks.`AlbumTitle`, "
                                                       "tracks.`AlbumArtistName`, "
                                                       "tracks.`Duration`, "
                                                       "album.`CoverFileName`, "
                                                       "tracks.`TrackNumber`, "
                                                       "tracks.`DiscNumber`, "
                                                       "tracks.`Rating` "
                                                       "FROM "
                                                       "`Tracks` tracks "
                                                       "LEFT JOIN "
                                                       "`Albums` album "
                                                       "ON "
                                                       "tracks.`AlbumTitle` = album.`Title` AND "
                                                       "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                       "tracks.`AlbumPath` = album.`AlbumPath` "
                                                       "");

        auto result = prepareQuery(d->mSelectAllTracksShortQuery, selectAllTracksShortText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllTracksShortQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllTracksShortQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectArtistByNameText = QStringLiteral("SELECT `ID`, "
                                                     "`Name` "
                                                     "FROM `Artists` "
                                                     "WHERE "
                                                     "`Name` = :name");

        auto result = prepareQuery(d->mSelectArtistByNameQuery, selectArtistByNameText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectArtistByNameQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectArtistByNameQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectComposerByNameText = QStringLiteral("SELECT `ID`, "
                                                       "`Name` "
                                                       "FROM `Composer` "
                                                       "WHERE "
                                                       "`Name` = :name");

        auto result = prepareQuery(d->mSelectComposerByNameQuery, selectComposerByNameText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectComposerByNameQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectComposerByNameQuery.lastError();
        }
    }

    {
        auto selectLyricistByNameText = QStringLiteral("SELECT `ID`, "
                                                       "`Name` "
                                                       "FROM `Lyricist` "
                                                       "WHERE "
                                                       "`Name` = :name");

        auto result = prepareQuery(d->mSelectLyricistByNameQuery, selectLyricistByNameText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectLyricistByNameQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectLyricistByNameQuery.lastError();
        }
    }

    {
        auto selectGenreByNameText = QStringLiteral("SELECT `ID`, "
                                                    "`Name` "
                                                    "FROM `Genre` "
                                                    "WHERE "
                                                    "`Name` = :name");

        auto result = prepareQuery(d->mSelectGenreByNameQuery, selectGenreByNameText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreByNameQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreByNameQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto insertArtistsText = QStringLiteral("INSERT INTO `Artists` (`ID`, `Name`) "
                                                "VALUES (:artistId, :name)");

        auto result = prepareQuery(d->mInsertArtistsQuery, insertArtistsText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertArtistsQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertArtistsQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto insertGenreText = QStringLiteral("INSERT INTO `Genre` (`ID`, `Name`) "
                                              "VALUES (:genreId, :name)");

        auto result = prepareQuery(d->mInsertGenreQuery, insertGenreText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertGenreQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertGenreQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto insertComposerText = QStringLiteral("INSERT INTO `Composer` (`ID`, `Name`) "
                                                 "VALUES (:composerId, :name)");

        auto result = prepareQuery(d->mInsertComposerQuery, insertComposerText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertComposerQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertComposerQuery.lastError();
        }
    }

    {
        auto insertLyricistText = QStringLiteral("INSERT INTO `Lyricist` (`ID`, `Name`) "
                                                 "VALUES (:lyricistId, :name)");

        auto result = prepareQuery(d->mInsertLyricistQuery, insertLyricistText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertLyricistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertLyricistQuery.lastError();
        }
    }

    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.`ID`, "
                                                   "tracks.`Title`, "
                                                   "album.`ID`, "
                                                   "tracks.`ArtistName`, "
                                                   "tracks.`AlbumArtistName`, "
                                                   "tracksMapping.`FileName`, "
                                                   "tracksMapping.`FileModifiedTime`, "
                                                   "tracks.`TrackNumber`, "
                                                   "tracks.`DiscNumber`, "
                                                   "tracks.`Duration`, "
                                                   "tracks.`AlbumTitle`, "
                                                   "tracks.`Rating`, "
                                                   "album.`CoverFileName`, "
                                                   "("
                                                   "SELECT "
                                                   "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                   "FROM "
                                                   "`Tracks` tracks2 "
                                                   "WHERE "
                                                   "tracks2.`AlbumTitle` = album.`Title` AND "
                                                   "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                   "(tracks2.`AlbumArtistName` IS NULL AND "
                                                   "album.`ArtistName` IS NULL"
                                                   ")"
                                                   ") AND "
                                                   "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                   ") as `IsSingleDiscAlbum`, "
                                                   "trackGenre.`Name`, "
                                                   "trackComposer.`Name`, "
                                                   "trackLyricist.`Name`, "
                                                   "tracks.`Comment`, "
                                                   "tracks.`Year`, "
                                                   "tracks.`Channels`, "
                                                   "tracks.`BitRate`, "
                                                   "tracks.`SampleRate`, "
                                                   "tracks.`HasEmbeddedCover`, "
                                                   "tracksMapping.`ImportDate`, "
                                                   "tracksMapping.`FirstPlayDate`, "
                                                   "tracksMapping.`LastPlayDate`, "
                                                   "tracksMapping.`PlayCounter`, "
                                                   "tracksMapping.`PlayCounter` / (strftime('%s', 'now') - tracksMapping.`FirstPlayDate`) as PlayFrequency "
                                                   "FROM "
                                                   "`Tracks` tracks, "
                                                   "`TracksData` tracksMapping "
                                                   "LEFT JOIN "
                                                   "`Albums` album "
                                                   "ON "
                                                   "album.`ID` = :albumId AND "
                                                   "tracks.`AlbumTitle` = album.`Title` AND "
                                                   "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                   "tracks.`AlbumPath` = album.`AlbumPath` "
                                                   "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                   "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                   "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                   "WHERE "
                                                   "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                   "album.`ID` = :albumId AND "
                                                   "tracks.`Priority` = ("
                                                   "     SELECT "
                                                   "     MIN(`Priority`) "
                                                   "     FROM "
                                                   "     `Tracks` tracks2 "
                                                   "     WHERE "
                                                   "     tracks.`Title` = tracks2.`Title` AND "
                                                   "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                   "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                   "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                   "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                   ")"
                                                   "ORDER BY tracks.`DiscNumber` ASC, "
                                                   "tracks.`TrackNumber` ASC");

        auto result = prepareQuery(d->mSelectTrackQuery, selectTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.`ID` "
                                                   "FROM "
                                                   "`Tracks` tracks, "
                                                   "`TracksData` tracksMapping "
                                                   "LEFT JOIN "
                                                   "`Albums` album "
                                                   "ON "
                                                   "album.`ID` = :albumId AND "
                                                   "tracks.`AlbumTitle` = album.`Title` AND "
                                                   "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                   "tracks.`AlbumPath` = album.`AlbumPath` "
                                                   "WHERE "
                                                   "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                   "album.`ID` = :albumId AND "
                                                   "tracks.`Priority` = ("
                                                   "     SELECT "
                                                   "     MIN(`Priority`) "
                                                   "     FROM "
                                                   "     `Tracks` tracks2 "
                                                   "     WHERE "
                                                   "     tracks.`Title` = tracks2.`Title` AND "
                                                   "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                   "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                   "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                   "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                   ")"
                                                   "ORDER BY tracks.`DiscNumber` ASC, "
                                                   "tracks.`TrackNumber` ASC");

        auto result = prepareQuery(d->mSelectTrackIdQuery, selectTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTrackFromIdQueryText = QStringLiteral("SELECT "
                                                         "tracks.`Id`, "
                                                         "tracks.`Title`, "
                                                         "album.`ID`, "
                                                         "tracks.`ArtistName`, "
                                                         "tracks.`AlbumArtistName`, "
                                                         "tracksMapping.`FileName`, "
                                                         "tracksMapping.`FileModifiedTime`, "
                                                         "tracks.`TrackNumber`, "
                                                         "tracks.`DiscNumber`, "
                                                         "tracks.`Duration`, "
                                                         "tracks.`AlbumTitle`, "
                                                         "tracks.`Rating`, "
                                                         "album.`CoverFileName`, "
                                                         "("
                                                         "SELECT "
                                                         "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                         "FROM "
                                                         "`Tracks` tracks2 "
                                                         "WHERE "
                                                         "tracks2.`AlbumTitle` = album.`Title` AND "
                                                         "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                         "(tracks2.`AlbumArtistName` IS NULL AND "
                                                         "album.`ArtistName` IS NULL"
                                                         ")"
                                                         ") AND "
                                                         "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                         ") as `IsSingleDiscAlbum`, "
                                                         "trackGenre.`Name`, "
                                                         "trackComposer.`Name`, "
                                                         "trackLyricist.`Name`, "
                                                         "tracks.`Comment`, "
                                                         "tracks.`Year`, "
                                                         "tracks.`Channels`, "
                                                         "tracks.`BitRate`, "
                                                         "tracks.`SampleRate`, "
                                                         "tracks.`HasEmbeddedCover`, "
                                                         "tracksMapping.`ImportDate`, "
                                                         "tracksMapping.`FirstPlayDate`, "
                                                         "tracksMapping.`LastPlayDate`, "
                                                         "tracksMapping.`PlayCounter`, "
                                                         "tracksMapping.`PlayCounter` / (strftime('%s', 'now') - tracksMapping.`FirstPlayDate`) as PlayFrequency "
                                                         "FROM "
                                                         "`Tracks` tracks, "
                                                         "`TracksData` tracksMapping "
                                                         "LEFT JOIN "
                                                         "`Albums` album "
                                                         "ON "
                                                         "tracks.`AlbumTitle` = album.`Title` AND "
                                                         "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                         "tracks.`AlbumPath` = album.`AlbumPath` "
                                                         "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                         "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                         "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                         "WHERE "
                                                         "tracks.`ID` = :trackId AND "
                                                         "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                         "tracks.`Priority` = ("
                                                         "     SELECT "
                                                         "     MIN(`Priority`) "
                                                         "     FROM "
                                                         "     `Tracks` tracks2 "
                                                         "     WHERE "
                                                         "     tracks.`Title` = tracks2.`Title` AND "
                                                         "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                         "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                         "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                         "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                         ")"
                                                         "");

        auto result = prepareQuery(d->mSelectTrackFromIdQuery, selectTrackFromIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackFromIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackFromIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }
    {
        auto selectCountAlbumsQueryText = QStringLiteral("SELECT count(*) "
                                                         "FROM `Albums` album "
                                                         "WHERE album.`ArtistName` = :artistName ");

        const auto result = prepareQuery(d->mSelectCountAlbumsForArtistQuery, selectCountAlbumsQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectCountAlbumsForArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectCountAlbumsForArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectGenreForArtistQueryText = QStringLiteral("SELECT DISTINCT trackGenre.`Name` "
                                                            "FROM "
                                                            "`Tracks` tracks "
                                                            "LEFT JOIN "
                                                            "`Albums` album "
                                                            "ON "
                                                            "tracks.`AlbumTitle` = album.`Title` AND "
                                                            "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                            "tracks.`AlbumPath` = album.`AlbumPath` "
                                                            "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                            "WHERE "
                                                            "album.`ArtistName` = :artistName");

        const auto result = prepareQuery(d->mSelectGenreForArtistQuery, selectGenreForArtistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreForArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreForArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectGenreForAlbumQueryText = QStringLiteral("SELECT DISTINCT trackGenre.`Name` "
                                                           "FROM "
                                                           "`Tracks` tracks "
                                                           "LEFT JOIN "
                                                           "`Albums` album "
                                                           "ON "
                                                           "tracks.`AlbumTitle` = album.`Title` AND "
                                                           "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                           "tracks.`AlbumPath` = album.`AlbumPath` "
                                                           "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                           "WHERE "
                                                           "album.`ID` = :albumId");

        const auto result = prepareQuery(d->mSelectGenreForAlbumQuery, selectGenreForAlbumQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreForAlbumQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreForAlbumQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectCountAlbumsQueryText = QStringLiteral("SELECT distinct count(album.`ID`) "
                                                         "FROM "
                                                         "`Tracks` tracks, "
                                                         "`Albums` album "
                                                         "LEFT JOIN `Composer` albumComposer ON albumComposer.`Name` = tracks.`Composer` "
                                                         "WHERE "
                                                         "(tracks.`AlbumTitle` = album.`Title` OR tracks.`AlbumTitle` IS NULL ) AND "
                                                         "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                         "(tracks.`AlbumPath` = album.`AlbumPath` OR tracks.`AlbumPath` IS NULL ) AND "
                                                         "albumComposer.`Name` = :artistName");

        const auto result = prepareQuery(d->mSelectCountAlbumsForComposerQuery, selectCountAlbumsQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectCountAlbumsForComposerQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectCountAlbumsForComposerQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectCountAlbumsQueryText = QStringLiteral("SELECT distinct count(album.`ID`) "
                                                         "FROM "
                                                         "`Tracks` tracks, "
                                                         "`Albums` album "
                                                         "LEFT JOIN `Lyricist` albumLyricist ON albumLyricist.`Name` = tracks.`Lyricist` "
                                                         "WHERE "
                                                         "(tracks.`AlbumTitle` = album.`Title` OR tracks.`AlbumTitle` IS NULL ) AND "
                                                         "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                         "(tracks.`AlbumPath` = album.`AlbumPath` OR tracks.`AlbumPath` IS NULL ) AND "
                                                         "albumLyricist.`Name` = :artistName");

        const auto result = prepareQuery(d->mSelectCountAlbumsForLyricistQuery, selectCountAlbumsQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectCountAlbumsForLyricistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectCountAlbumsForLyricistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAlbumIdFromTitleQueryText = QStringLiteral("SELECT "
                                                              "album.`ID` "
                                                              "FROM "
                                                              "`Albums` album "
                                                              "WHERE "
                                                              "album.`ArtistName` = :artistName AND "
                                                              "album.`Title` = :title");

        auto result = prepareQuery(d->mSelectAlbumIdFromTitleQuery, selectAlbumIdFromTitleQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAlbumIdFromTitleAndArtistQueryText = QStringLiteral("SELECT "
                                                                       "album.`ID` "
                                                                       "FROM "
                                                                       "`Albums` album "
                                                                       "WHERE "
                                                                       "album.`ArtistName` = :artistName AND "
                                                                       "album.`Title` = :title AND "
                                                                       "album.`AlbumPath` = :albumPath");

        auto result = prepareQuery(d->mSelectAlbumIdFromTitleAndArtistQuery, selectAlbumIdFromTitleAndArtistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleAndArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleAndArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAlbumIdFromTitleWithoutArtistQueryText = QStringLiteral("SELECT "
                                                                           "album.`ID` "
                                                                           "FROM "
                                                                           "`Albums` album "
                                                                           "WHERE "
                                                                           "album.`AlbumPath` = :albumPath AND "
                                                                           "album.`Title` = :title AND "
                                                                           "album.`ArtistName` IS NULL");

        auto result = prepareQuery(d->mSelectAlbumIdFromTitleWithoutArtistQuery, selectAlbumIdFromTitleWithoutArtistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto insertAlbumQueryText = QStringLiteral("INSERT INTO `Albums` "
                                                   "(`ID`, "
                                                   "`Title`, "
                                                   "`ArtistName`, "
                                                   "`AlbumPath`, "
                                                   "`CoverFileName`) "
                                                   "VALUES "
                                                   "(:albumId, "
                                                   ":title, "
                                                   ":albumArtist, "
                                                   ":albumPath, "
                                                   ":coverFileName)");

        auto result = prepareQuery(d->mInsertAlbumQuery, insertAlbumQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertAlbumQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertAlbumQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto insertTrackMappingQueryText = QStringLiteral("INSERT INTO "
                                                          "`TracksData` "
                                                          "(`FileName`, "
                                                          "`FileModifiedTime`, "
                                                          "`ImportDate`, "
                                                          "`PlayCounter`) "
                                                          "VALUES (:fileName, :mtime, :importDate, 0)");

        auto result = prepareQuery(d->mInsertTrackMapping, insertTrackMappingQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertTrackMapping.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertTrackMapping.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto initialUpdateTracksValidityQueryText = QStringLiteral("UPDATE `TracksData` "
                                                                   "SET "
                                                                   "`FileModifiedTime` = :mtime "
                                                                   "WHERE `FileName` = :fileName");

        auto result = prepareQuery(d->mUpdateTrackFileModifiedTime, initialUpdateTracksValidityQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackFileModifiedTime.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackFileModifiedTime.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto initialUpdateTracksValidityQueryText = QStringLiteral("UPDATE `Tracks` "
                                                                   "SET "
                                                                   "`Priority` = :priority "
                                                                   "WHERE `FileName` = :fileName");

        auto result = prepareQuery(d->mUpdateTrackPriority, initialUpdateTracksValidityQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackPriority.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackPriority.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto removeTracksMappingFromSourceQueryText = QStringLiteral("DELETE FROM `TracksData` "
                                                                     "WHERE `FileName` = :fileName");

        auto result = prepareQuery(d->mRemoveTracksMappingFromSource, removeTracksMappingFromSourceQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveTracksMappingFromSource.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveTracksMappingFromSource.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto removeTracksMappingQueryText = QStringLiteral("DELETE FROM `TracksData` "
                                                           "WHERE `FileName` = :fileName");

        auto result = prepareQuery(d->mRemoveTracksMapping, removeTracksMappingQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveTracksMapping.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveTracksMapping.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTracksWithoutMappingQueryText = QStringLiteral("SELECT "
                                                                  "tracks.`Id`, "
                                                                  "tracks.`Title`, "
                                                                  "album.`ID`, "
                                                                  "tracks.`ArtistName`, "
                                                                  "tracks.`AlbumArtistName`, "
                                                                  "\"\" as FileName, "
                                                                  "NULL as FileModifiedTime, "
                                                                  "tracks.`TrackNumber`, "
                                                                  "tracks.`DiscNumber`, "
                                                                  "tracks.`Duration`, "
                                                                  "tracks.`AlbumTitle`, "
                                                                  "tracks.`Rating`, "
                                                                  "album.`CoverFileName`, "
                                                                  "("
                                                                  "SELECT "
                                                                  "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                                  "FROM "
                                                                  "`Tracks` tracks2 "
                                                                  "WHERE "
                                                                  "tracks2.`AlbumTitle` = album.`Title` AND "
                                                                  "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                                  "(tracks2.`AlbumArtistName` IS NULL AND "
                                                                  "album.`ArtistName` IS NULL"
                                                                  ")"
                                                                  ") AND "
                                                                  "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                                  ") as `IsSingleDiscAlbum`, "
                                                                  "trackGenre.`Name`, "
                                                                  "trackComposer.`Name`, "
                                                                  "trackLyricist.`Name`, "
                                                                  "tracks.`Comment`, "
                                                                  "tracks.`Year`, "
                                                                  "tracks.`Channels`, "
                                                                  "tracks.`BitRate`, "
                                                                  "tracks.`SampleRate`, "
                                                                  "tracks.`HasEmbeddedCover`, "
                                                                  "tracksMapping.`ImportDate`, "
                                                                  "tracksMapping.`FirstPlayDate`, "
                                                                  "tracksMapping.`LastPlayDate`, "
                                                                  "tracksMapping.`PlayCounter`, "
                                                                  "tracksMapping.`PlayCounter` / (strftime('%s', 'now') - tracksMapping.`FirstPlayDate`) as PlayFrequency "
                                                                  "FROM "
                                                                  "`Tracks` tracks, "
                                                                  "`TracksData` tracksMapping "
                                                                  "LEFT JOIN "
                                                                  "`Albums` album "
                                                                  "ON "
                                                                  "tracks.`AlbumTitle` = album.`Title` AND "
                                                                  "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                                  "tracks.`AlbumPath` = album.`AlbumPath` "
                                                                  "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                                  "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                                  "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                                  "WHERE "
                                                                  "tracks.`FileName` = tracksMapping.`FileName` AND "
                                                                  "tracks.`FileName` NOT IN (SELECT tracksMapping2.`FileName` FROM `TracksData` tracksMapping2)");

        auto result = prepareQuery(d->mSelectTracksWithoutMappingQuery, selectTracksWithoutMappingQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksWithoutMappingQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksWithoutMappingQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTracksMappingQueryText = QStringLiteral("SELECT "
                                                           "track.`ID`, "
                                                           "trackData.`FileName`, "
                                                           "track.`Priority`, "
                                                           "trackData.`FileModifiedTime` "
                                                           "FROM "
                                                           "`TracksData` trackData "
                                                           "LEFT JOIN "
                                                           "`Tracks` track "
                                                           "ON "
                                                           "track.`FileName` = trackData.`FileName` "
                                                           "WHERE "
                                                           "trackData.`FileName` = :fileName");

        auto result = prepareQuery(d->mSelectTracksMapping, selectTracksMappingQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksMapping.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksMapping.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTracksMappingPriorityQueryText = QStringLiteral("SELECT "
                                                                   "max(tracks.`Priority`) AS Priority "
                                                                   "FROM "
                                                                   "`Tracks` tracks, "
                                                                   "`Albums` albums "
                                                                   "WHERE "
                                                                   "tracks.`Title` = :title AND "
                                                                   "(tracks.`ArtistName` = :trackArtist OR tracks.`ArtistName` IS NULL) AND "
                                                                   "(tracks.`AlbumTitle` = :album OR tracks.`AlbumTitle` IS NULL) AND "
                                                                   "(tracks.`AlbumArtistName` = :albumArtist OR tracks.`AlbumArtistName` IS NULL) AND "
                                                                   "(tracks.`AlbumPath` = :albumPath OR tracks.`AlbumPath` IS NULL)");

        auto result = prepareQuery(d->mSelectTracksMappingPriority, selectTracksMappingPriorityQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksMappingPriority.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksMappingPriority.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTracksMappingPriorityQueryByTrackIdText = QStringLiteral("SELECT "
                                                                            "MAX(track.`Priority`) "
                                                                            "FROM "
                                                                            "`TracksData` trackData, "
                                                                            "`Tracks` track "
                                                                            "WHERE "
                                                                            "track.`ID` = :trackId AND "
                                                                            "trackData.`FileName` = track.`FileName`");

        auto result = prepareQuery(d->mSelectTracksMappingPriorityByTrackId, selectTracksMappingPriorityQueryByTrackIdText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksMappingPriorityByTrackId.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksMappingPriorityByTrackId.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllTrackFilesFromSourceQueryText = QStringLiteral("SELECT "
                                                                     "tracksMapping.`FileName`, "
                                                                     "tracksMapping.`FileModifiedTime` "
                                                                     "FROM "
                                                                     "`TracksData` tracksMapping, "
                                                                     "`Tracks` tracks "
                                                                     "WHERE "
                                                                     "tracks.`FileName` = tracksMapping.`FileName`");

        auto result = prepareQuery(d->mSelectAllTrackFilesQuery, selectAllTrackFilesFromSourceQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllTrackFilesQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllTrackFilesQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto insertMusicSourceQueryText = QStringLiteral("INSERT OR IGNORE INTO `DiscoverSource` (`ID`, `Name`) "
                                                         "VALUES (:discoverId, :name)");

        auto result = prepareQuery(d->mInsertMusicSource, insertMusicSourceQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertMusicSource.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertMusicSource.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectMusicSourceQueryText = QStringLiteral("SELECT `ID` FROM `DiscoverSource` WHERE `Name` = :name");

        auto result = prepareQuery(d->mSelectMusicSource, selectMusicSourceQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectMusicSource.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectMusicSource.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.`ID`,  tracksMapping.`FileName` "
                                                   "FROM "
                                                   "`Tracks` tracks, "
                                                   "`Albums` album, "
                                                   "`TracksData` tracksMapping "
                                                   "WHERE "
                                                   "tracks.`Title` = :title AND "
                                                   "album.`ID` = :album AND "
                                                   "(tracks.`AlbumTitle` = album.`Title` OR tracks.`AlbumTitle` IS NULL ) AND "
                                                   "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                   "(tracks.`AlbumPath` = album.`AlbumPath` OR tracks.`AlbumPath` IS NULL ) AND "
                                                   "tracks.`ArtistName` = :artist AND "
                                                   "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                   "tracks.`Priority` = ("
                                                   "     SELECT "
                                                   "     MIN(`Priority`) "
                                                   "     FROM "
                                                   "     `Tracks` tracks2 "
                                                   "     WHERE "
                                                   "     tracks.`Title` = tracks2.`Title` AND "
                                                   "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                   "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                   "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                   "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                   ")"
                                                   "");

        auto result = prepareQuery(d->mSelectTrackIdFromTitleAlbumIdArtistQuery, selectTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleAlbumIdArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleAlbumIdArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto insertTrackQueryText = QStringLiteral("INSERT INTO `Tracks` "
                                                   "("
                                                   "`ID`, "
                                                   "`FileName`, "
                                                   "`Priority`, "
                                                   "`Title`, "
                                                   "`ArtistName`, "
                                                   "`AlbumTitle`, "
                                                   "`AlbumArtistName`, "
                                                   "`AlbumPath`, "
                                                   "`Genre`, "
                                                   "`Composer`, "
                                                   "`Lyricist`, "
                                                   "`Comment`, "
                                                   "`TrackNumber`, "
                                                   "`DiscNumber`, "
                                                   "`Channels`, "
                                                   "`BitRate`, "
                                                   "`SampleRate`, "
                                                   "`Year`,  "
                                                   "`Duration`, "
                                                   "`Rating`, "
                                                   "`HasEmbeddedCover`) "
                                                   "VALUES "
                                                   "("
                                                   ":trackId, "
                                                   ":fileName, "
                                                   ":priority, "
                                                   ":title, "
                                                   ":artistName, "
                                                   ":albumTitle, "
                                                   ":albumArtistName, "
                                                   ":albumPath, "
                                                   ":genre, "
                                                   ":composer, "
                                                   ":lyricist, "
                                                   ":comment, "
                                                   ":trackNumber, "
                                                   ":discNumber, "
                                                   ":channels, "
                                                   ":bitRate, "
                                                   ":sampleRate, "
                                                   ":year, "
                                                   ":trackDuration, "
                                                   ":trackRating, "
                                                   ":hasEmbeddedCover)");

        auto result = prepareQuery(d->mInsertTrackQuery, insertTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertTrackQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertTrackQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto updateTrackQueryText = QStringLiteral("UPDATE `Tracks` "
                                                   "SET "
                                                   "`FileName` = :fileName, "
                                                   "`Title` = :title, "
                                                   "`ArtistName` = :artistName, "
                                                   "`AlbumTitle` = :albumTitle, "
                                                   "`AlbumArtistName` = :albumArtistName, "
                                                   "`AlbumPath` = :albumPath, "
                                                   "`Genre` = :genre, "
                                                   "`Composer` = :composer, "
                                                   "`Lyricist` = :lyricist, "
                                                   "`Comment` = :comment, "
                                                   "`TrackNumber` = :trackNumber, "
                                                   "`DiscNumber` = :discNumber, "
                                                   "`Channels` = :channels, "
                                                   "`BitRate` = :bitRate, "
                                                   "`SampleRate` = :sampleRate, "
                                                   "`Year` = :year, "
                                                   " `Duration` = :trackDuration, "
                                                   "`Rating` = :trackRating "
                                                   "WHERE "
                                                   "`ID` = :trackId");

        auto result = prepareQuery(d->mUpdateTrackQuery, updateTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto updateAlbumArtistQueryText = QStringLiteral("UPDATE `Albums` "
                                                         "SET "
                                                         "`ArtistName` = :artistName "
                                                         "WHERE "
                                                         "`ID` = :albumId");

        auto result = prepareQuery(d->mUpdateAlbumArtistQuery, updateAlbumArtistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateAlbumArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateAlbumArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto updateAlbumArtistInTracksQueryText = QStringLiteral("UPDATE `Tracks` "
                                                                 "SET "
                                                                 "`AlbumArtistName` = :artistName "
                                                                 "WHERE "
                                                                 "`AlbumTitle` = :albumTitle AND "
                                                                 "`AlbumPath` = :albumPath AND "
                                                                 "`AlbumArtistName` IS NULL");

        auto result = prepareQuery(d->mUpdateAlbumArtistInTracksQuery, updateAlbumArtistInTracksQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateAlbumArtistInTracksQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateAlbumArtistInTracksQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto queryMaximumTrackIdQueryText = QStringLiteral("SELECT MAX(tracks.`ID`)"
                                                           "FROM "
                                                           "`Tracks` tracks");

        auto result = prepareQuery(d->mQueryMaximumTrackIdQuery, queryMaximumTrackIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumTrackIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumTrackIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto queryMaximumAlbumIdQueryText = QStringLiteral("SELECT MAX(albums.`ID`)"
                                                           "FROM "
                                                           "`Albums` albums");

        auto result = prepareQuery(d->mQueryMaximumAlbumIdQuery, queryMaximumAlbumIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumAlbumIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumAlbumIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto queryMaximumArtistIdQueryText = QStringLiteral("SELECT MAX(artists.`ID`)"
                                                            "FROM "
                                                            "`Artists` artists");

        auto result = prepareQuery(d->mQueryMaximumArtistIdQuery, queryMaximumArtistIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumArtistIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumArtistIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto queryMaximumLyricistIdQueryText = QStringLiteral("SELECT MAX(lyricists.`ID`)"
                                                              "FROM "
                                                              "`Lyricist` lyricists");

        auto result = prepareQuery(d->mQueryMaximumLyricistIdQuery, queryMaximumLyricistIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumLyricistIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumLyricistIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto queryMaximumComposerIdQueryText = QStringLiteral("SELECT MAX(composers.`ID`)"
                                                              "FROM "
                                                              "`Composer` composers");

        auto result = prepareQuery(d->mQueryMaximumComposerIdQuery, queryMaximumComposerIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumComposerIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumComposerIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto queryMaximumGenreIdQueryText = QStringLiteral("SELECT MAX(genres.`ID`)"
                                                           "FROM "
                                                           "`Genre` genres");

        auto result = prepareQuery(d->mQueryMaximumGenreIdQuery, queryMaximumGenreIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumGenreIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumGenreIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.ID "
                                                   "FROM "
                                                   "`Tracks` tracks "
                                                   "WHERE "
                                                   "tracks.`Title` = :title AND "
                                                   "tracks.`AlbumTitle` = :album AND "
                                                   "tracks.`TrackNumber` = :trackNumber AND "
                                                   "tracks.`DiscNumber` = :discNumber AND "
                                                   "tracks.`ArtistName` = :artist");

        auto result = prepareQuery(d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery, selectTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.ID "
                                                   "FROM "
                                                   "`Tracks` tracks, "
                                                   "`Albums` albums "
                                                   "WHERE "
                                                   "tracks.`Title` = :title AND "
                                                   "tracks.`Priority` = :priority AND "
                                                   "(tracks.`ArtistName` = :trackArtist OR tracks.`ArtistName` IS NULL) AND "
                                                   "(tracks.`AlbumTitle` = :album OR tracks.`AlbumTitle` IS NULL) AND "
                                                   "(tracks.`AlbumArtistName` = :albumArtist OR tracks.`AlbumArtistName` IS NULL) AND "
                                                   "(tracks.`AlbumPath` = :albumPath OR tracks.`AlbumPath` IS NULL)");

        auto result = prepareQuery(d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery, selectTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAlbumArtUriFromAlbumIdQueryText = QStringLiteral("SELECT `CoverFileName`"
                                                                    "FROM "
                                                                    "`Albums` "
                                                                    "WHERE "
                                                                    "`ID` = :albumId");

        auto result = prepareQuery(d->mSelectAlbumArtUriFromAlbumIdQuery, selectAlbumArtUriFromAlbumIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumArtUriFromAlbumIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumArtUriFromAlbumIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto updateAlbumArtUriFromAlbumIdQueryText = QStringLiteral("UPDATE `Albums` "
                                                                    "SET `CoverFileName` = :coverFileName "
                                                                    "WHERE "
                                                                    "`ID` = :albumId");

        auto result = prepareQuery(d->mUpdateAlbumArtUriFromAlbumIdQuery, updateAlbumArtUriFromAlbumIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateAlbumArtUriFromAlbumIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateAlbumArtUriFromAlbumIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTracksFromArtistQueryText = QStringLiteral("SELECT "
                                                              "tracks.`ID`, "
                                                              "tracks.`Title`, "
                                                              "album.`ID`, "
                                                              "tracks.`ArtistName`, "
                                                              "tracks.`AlbumArtistName`, "
                                                              "tracksMapping.`FileName`, "
                                                              "tracksMapping.`FileModifiedTime`, "
                                                              "tracks.`TrackNumber`, "
                                                              "tracks.`DiscNumber`, "
                                                              "tracks.`Duration`, "
                                                              "tracks.`AlbumTitle`, "
                                                              "tracks.`Rating`, "
                                                              "album.`CoverFileName`, "
                                                              "("
                                                              "SELECT "
                                                              "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                              "FROM "
                                                              "`Tracks` tracks2 "
                                                              "WHERE "
                                                              "tracks2.`AlbumTitle` = album.`Title` AND "
                                                              "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                              "(tracks2.`AlbumArtistName` IS NULL AND "
                                                              "album.`ArtistName` IS NULL"
                                                              ")"
                                                              ") AND "
                                                              "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                              ") as `IsSingleDiscAlbum`, "
                                                              "trackGenre.`Name`, "
                                                              "trackComposer.`Name`, "
                                                              "trackLyricist.`Name`, "
                                                              "tracks.`Comment`, "
                                                              "tracks.`Year`, "
                                                              "tracks.`Channels`, "
                                                              "tracks.`BitRate`, "
                                                              "tracks.`SampleRate`, "
                                                              "tracks.`HasEmbeddedCover`, "
                                                              "tracksMapping.`ImportDate`, "
                                                              "tracksMapping.`FirstPlayDate`, "
                                                              "tracksMapping.`LastPlayDate`, "
                                                              "tracksMapping.`PlayCounter`, "
                                                              "tracksMapping.`PlayCounter` / (strftime('%s', 'now') - tracksMapping.`FirstPlayDate`) as PlayFrequency "
                                                              "FROM "
                                                              "`Tracks` tracks, "
                                                              "`TracksData` tracksMapping "
                                                              "LEFT JOIN "
                                                              "`Albums` album "
                                                              "ON "
                                                              "tracks.`AlbumTitle` = album.`Title` AND "
                                                              "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                              "tracks.`AlbumPath` = album.`AlbumPath` "
                                                              "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                              "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                              "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                              "WHERE "
                                                              "tracks.`ArtistName` = :artistName AND "
                                                              "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                              "tracks.`Priority` = ("
                                                              "     SELECT "
                                                              "     MIN(`Priority`) "
                                                              "     FROM "
                                                              "     `Tracks` tracks2 "
                                                              "     WHERE "
                                                              "     tracks.`Title` = tracks2.`Title` AND "
                                                              "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                              "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                              "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                              "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                              ")"
                                                              "ORDER BY tracks.`Title` ASC, "
                                                              "album.`Title` ASC");

        auto result = prepareQuery(d->mSelectTracksFromArtist, selectTracksFromArtistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksFromArtist.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksFromArtist.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAlbumIdsFromArtistQueryText = QStringLiteral("SELECT "
                                                                "album.`ID` "
                                                                "FROM "
                                                                "`Albums` album "
                                                                "WHERE "
                                                                "album.`ArtistName` = :artistName");

        auto result = prepareQuery(d->mSelectAlbumIdsFromArtist, selectAlbumIdsFromArtistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdsFromArtist.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdsFromArtist.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectArtistQueryText = QStringLiteral("SELECT `ID`, "
                                                    "`Name` "
                                                    "FROM `Artists` "
                                                    "WHERE "
                                                    "`ID` = :artistId");

        auto result = prepareQuery(d->mSelectArtistQuery, selectArtistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto updateTrackStatisticsQueryText = QStringLiteral("UPDATE `TracksData` "
                                                             "SET "
                                                             "`LastPlayDate` = :playDate, "
                                                             "`PlayCounter` = `PlayCounter` + 1 "
                                                             "WHERE "
                                                             "`FileName` = :fileName");

        auto result = prepareQuery(d->mUpdateTrackStatistics, updateTrackStatisticsQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackStatistics.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackStatistics.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto updateTrackFirstPlayStatisticsQueryText = QStringLiteral("UPDATE `TracksData` "
                                                                      "SET "
                                                                      "`FirstPlayDate` = :playDate "
                                                                      "WHERE "
                                                                      "`FileName` = :fileName AND "
                                                                      "`FirstPlayDate` IS NULL");

        auto result = prepareQuery(d->mUpdateTrackFirstPlayStatistics, updateTrackFirstPlayStatisticsQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackFirstPlayStatistics.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackFirstPlayStatistics.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectGenreQueryText = QStringLiteral("SELECT `ID`, "
                                                   "`Name` "
                                                   "FROM `Genre` "
                                                   "WHERE "
                                                   "`ID` = :genreId");

        auto result = prepareQuery(d->mSelectGenreQuery, selectGenreQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectComposerQueryText = QStringLiteral("SELECT `ID`, "
                                                      "`Name` "
                                                      "FROM `Composer` "
                                                      "WHERE "
                                                      "`ID` = :composerId");

        auto result = prepareQuery(d->mSelectComposerQuery, selectComposerQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectComposerQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectComposerQuery.lastError();
        }
    }

    {
        auto selectLyricistQueryText = QStringLiteral("SELECT `ID`, "
                                                      "`Name` "
                                                      "FROM `Lyricist` "
                                                      "WHERE "
                                                      "`ID` = :lyricistId");

        auto result = prepareQuery(d->mSelectLyricistQuery, selectLyricistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectLyricistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectLyricistQuery.lastError();
        }
    }

    {
        auto removeTrackQueryText = QStringLiteral("DELETE FROM `Tracks` "
                                                   "WHERE "
                                                   "`ID` = :trackId");

        auto result = prepareQuery(d->mRemoveTrackQuery, removeTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveTrackQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveTrackQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto removeAlbumQueryText = QStringLiteral("DELETE FROM `Albums` "
                                                   "WHERE "
                                                   "`ID` = :albumId");

        auto result = prepareQuery(d->mRemoveAlbumQuery, removeAlbumQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveAlbumQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveAlbumQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto removeAlbumQueryText = QStringLiteral("DELETE FROM `Artists` "
                                                   "WHERE "
                                                   "`ID` = :artistId");

        auto result = prepareQuery(d->mRemoveArtistQuery, removeAlbumQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    finishTransaction();

    d->mInitFinished = true;
    Q_EMIT requestsInitDone();
}

qulonglong DatabaseInterface::insertAlbum(const QString &title, const QString &albumArtist, const QString &trackArtist,
                                          const QString &trackPath, const QUrl &albumArtURI)
{
    auto result = qulonglong(0);

    if (title.isEmpty()) {
        return result;
    }

    if (!albumArtist.isEmpty() || !trackArtist.isEmpty()) {
        d->mSelectAlbumIdFromTitleAndArtistQuery.bindValue(QStringLiteral(":title"), title);
        d->mSelectAlbumIdFromTitleAndArtistQuery.bindValue(QStringLiteral(":albumPath"), trackPath);
        if (!albumArtist.isEmpty()) {
            d->mSelectAlbumIdFromTitleAndArtistQuery.bindValue(QStringLiteral(":artistName"), albumArtist);
        } else {
            d->mSelectAlbumIdFromTitleAndArtistQuery.bindValue(QStringLiteral(":artistName"), trackArtist);
        }

        auto queryResult = execQuery(d->mSelectAlbumIdFromTitleAndArtistQuery);

        if (!queryResult || !d->mSelectAlbumIdFromTitleAndArtistQuery.isSelect() || !d->mSelectAlbumIdFromTitleAndArtistQuery.isActive()) {
            Q_EMIT databaseError();

            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleAndArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleAndArtistQuery.boundValues();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleAndArtistQuery.lastError();

            d->mSelectAlbumIdFromTitleAndArtistQuery.finish();

            return result;
        }

        if (d->mSelectAlbumIdFromTitleAndArtistQuery.next()) {
            result = d->mSelectAlbumIdFromTitleAndArtistQuery.record().value(0).toULongLong();

            d->mSelectAlbumIdFromTitleAndArtistQuery.finish();

            return result;
        }

        d->mSelectAlbumIdFromTitleAndArtistQuery.finish();
    }

    if (result == 0) {
        d->mSelectAlbumIdFromTitleWithoutArtistQuery.bindValue(QStringLiteral(":title"), title);
        d->mSelectAlbumIdFromTitleWithoutArtistQuery.bindValue(QStringLiteral(":albumPath"), trackPath);

        auto queryResult = execQuery(d->mSelectAlbumIdFromTitleWithoutArtistQuery);

        if (!queryResult || !d->mSelectAlbumIdFromTitleWithoutArtistQuery.isSelect() || !d->mSelectAlbumIdFromTitleWithoutArtistQuery.isActive()) {
            Q_EMIT databaseError();

            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.boundValues();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastError();

            d->mSelectAlbumIdFromTitleWithoutArtistQuery.finish();

            return result;
        }

        if (d->mSelectAlbumIdFromTitleWithoutArtistQuery.next()) {
            result = d->mSelectAlbumIdFromTitleWithoutArtistQuery.record().value(0).toULongLong();

            d->mSelectAlbumIdFromTitleWithoutArtistQuery.finish();

            return result;
        }

        d->mSelectAlbumIdFromTitleWithoutArtistQuery.finish();
    }

    d->mInsertAlbumQuery.bindValue(QStringLiteral(":albumId"), d->mAlbumId);
    d->mInsertAlbumQuery.bindValue(QStringLiteral(":title"), title);
    if (!albumArtist.isEmpty()) {
        insertArtist(albumArtist);
        d->mInsertAlbumQuery.bindValue(QStringLiteral(":albumArtist"), albumArtist);
    } else {
        d->mInsertAlbumQuery.bindValue(QStringLiteral(":albumArtist"), {});
    }
    d->mInsertAlbumQuery.bindValue(QStringLiteral(":albumPath"), trackPath);
    d->mInsertAlbumQuery.bindValue(QStringLiteral(":coverFileName"), albumArtURI);

    auto queryResult = execQuery(d->mInsertAlbumQuery);

    if (!queryResult || !d->mInsertAlbumQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertAlbum" << d->mInsertAlbumQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertAlbum" << d->mInsertAlbumQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertAlbum" << d->mInsertAlbumQuery.lastError();

        d->mInsertAlbumQuery.finish();

        return result;
    }

    result = d->mAlbumId;

    d->mInsertAlbumQuery.finish();

    ++d->mAlbumId;

    d->mInsertedAlbums.insert(result);

    return result;
}

bool DatabaseInterface::updateAlbumFromId(qulonglong albumId, const QUrl &albumArtUri,
                                          const MusicAudioTrack &currentTrack, const QString &albumPath)
{
    auto modifiedAlbum = false;
    modifiedAlbum = true;

    if (!albumArtUri.isValid()) {
        return modifiedAlbum;
    }

    auto storedAlbumArtUri = internalAlbumArtUriFromAlbumId(albumId);

    if (!storedAlbumArtUri.isValid() || storedAlbumArtUri != albumArtUri) {
        d->mUpdateAlbumArtUriFromAlbumIdQuery.bindValue(QStringLiteral(":albumId"), albumId);
        d->mUpdateAlbumArtUriFromAlbumIdQuery.bindValue(QStringLiteral(":coverFileName"), albumArtUri);

        auto result = execQuery(d->mUpdateAlbumArtUriFromAlbumIdQuery);

        if (!result || !d->mUpdateAlbumArtUriFromAlbumIdQuery.isActive()) {
            Q_EMIT databaseError();

            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumFromId" << d->mUpdateAlbumArtUriFromAlbumIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumFromId" << d->mUpdateAlbumArtUriFromAlbumIdQuery.boundValues();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumFromId" << d->mUpdateAlbumArtUriFromAlbumIdQuery.lastError();

            d->mUpdateAlbumArtUriFromAlbumIdQuery.finish();

            return modifiedAlbum;
        }

        d->mUpdateAlbumArtUriFromAlbumIdQuery.finish();

        modifiedAlbum = true;
    }

    if (!isValidArtist(albumId) && currentTrack.isValidAlbumArtist()) {
        updateAlbumArtist(albumId, currentTrack.albumName(), albumPath, currentTrack.albumArtist());

        modifiedAlbum = true;
    }

    return modifiedAlbum;
}

qulonglong DatabaseInterface::insertArtist(const QString &name)
{
    auto result = qulonglong(0);

    if (name.isEmpty()) {
        return result;
    }

    d->mSelectArtistByNameQuery.bindValue(QStringLiteral(":name"), name);

    auto queryResult = execQuery(d->mSelectArtistByNameQuery);

    if (!queryResult || !d->mSelectArtistByNameQuery.isSelect() || !d->mSelectArtistByNameQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.lastError();

        d->mSelectArtistByNameQuery.finish();

        return result;
    }

    if (d->mSelectArtistByNameQuery.next()) {
        result = d->mSelectArtistByNameQuery.record().value(0).toULongLong();

        d->mSelectArtistByNameQuery.finish();

        return result;
    }

    d->mSelectArtistByNameQuery.finish();

    d->mInsertArtistsQuery.bindValue(QStringLiteral(":artistId"), d->mArtistId);
    d->mInsertArtistsQuery.bindValue(QStringLiteral(":name"), name);

    queryResult = execQuery(d->mInsertArtistsQuery);

    if (!queryResult || !d->mInsertArtistsQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mInsertArtistsQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mInsertArtistsQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mInsertArtistsQuery.lastError();

        d->mInsertArtistsQuery.finish();

        return result;
    }

    result = d->mArtistId;

    ++d->mArtistId;

    d->mInsertedArtists.insert(result);

    d->mInsertArtistsQuery.finish();

    return result;
}

qulonglong DatabaseInterface::insertComposer(const QString &name)
{
    auto result = qulonglong(0);

    if (name.isEmpty()) {
        return result;
    }

    d->mSelectComposerByNameQuery.bindValue(QStringLiteral(":name"), name);

    auto queryResult = execQuery(d->mSelectComposerByNameQuery);

    if (!queryResult || !d->mSelectComposerByNameQuery.isSelect() || !d->mSelectComposerByNameQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertComposer" << d->mSelectComposerByNameQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertComposer" << d->mSelectComposerByNameQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertComposer" << d->mSelectComposerByNameQuery.lastError();

        d->mSelectComposerByNameQuery.finish();

        return result;
    }


    if (d->mSelectComposerByNameQuery.next()) {
        result = d->mSelectComposerByNameQuery.record().value(0).toULongLong();

        d->mSelectComposerByNameQuery.finish();

        return result;
    }

    d->mSelectComposerByNameQuery.finish();

    d->mInsertComposerQuery.bindValue(QStringLiteral(":composerId"), d->mComposerId);
    d->mInsertComposerQuery.bindValue(QStringLiteral(":name"), name);

    queryResult = execQuery(d->mInsertComposerQuery);

    if (!queryResult || !d->mInsertComposerQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertComposer" << d->mInsertComposerQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertComposer" << d->mInsertComposerQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertComposer" << d->mInsertComposerQuery.lastError();

        d->mInsertComposerQuery.finish();

        return result;
    }

    result = d->mComposerId;

    ++d->mComposerId;

    d->mInsertComposerQuery.finish();

    Q_EMIT composersAdded(internalAllComposersPartialData());

    return result;
}

qulonglong DatabaseInterface::insertGenre(const QString &name)
{
    auto result = qulonglong(0);

    if (name.isEmpty()) {
        return result;
    }

    d->mSelectGenreByNameQuery.bindValue(QStringLiteral(":name"), name);

    auto queryResult = execQuery(d->mSelectGenreByNameQuery);

    if (!queryResult || !d->mSelectGenreByNameQuery.isSelect() || !d->mSelectGenreByNameQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertGenre" << d->mSelectGenreByNameQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertGenre" << d->mSelectGenreByNameQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertGenre" << d->mSelectGenreByNameQuery.lastError();

        d->mSelectGenreByNameQuery.finish();

        return result;
    }

    if (d->mSelectGenreByNameQuery.next()) {
        result = d->mSelectGenreByNameQuery.record().value(0).toULongLong();

        d->mSelectGenreByNameQuery.finish();

        return result;
    }

    d->mSelectGenreByNameQuery.finish();

    d->mInsertGenreQuery.bindValue(QStringLiteral(":genreId"), d->mGenreId);
    d->mInsertGenreQuery.bindValue(QStringLiteral(":name"), name);

    queryResult = execQuery(d->mInsertGenreQuery);

    if (!queryResult || !d->mInsertGenreQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertGenre" << d->mInsertGenreQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertGenre" << d->mInsertGenreQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertGenre" << d->mInsertGenreQuery.lastError();

        d->mInsertGenreQuery.finish();

        return result;
    }

    result = d->mGenreId;

    ++d->mGenreId;

    d->mInsertGenreQuery.finish();

    Q_EMIT genresAdded({{{DatabaseIdRole, result}}});

    return result;
}

void DatabaseInterface::insertTrackOrigin(const QUrl &fileNameURI, const QDateTime &fileModifiedTime,
                                          const QDateTime &importDate)
{
    d->mInsertTrackMapping.bindValue(QStringLiteral(":fileName"), fileNameURI);
    d->mInsertTrackMapping.bindValue(QStringLiteral(":priority"), 1);
    d->mInsertTrackMapping.bindValue(QStringLiteral(":mtime"), fileModifiedTime);
    d->mInsertTrackMapping.bindValue(QStringLiteral(":importDate"), importDate.toMSecsSinceEpoch());

    auto queryResult = execQuery(d->mInsertTrackMapping);

    if (!queryResult || !d->mInsertTrackMapping.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mInsertTrackMapping.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mInsertTrackMapping.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mInsertTrackMapping.lastError();

        d->mInsertTrackMapping.finish();

        return;
    }

    d->mInsertTrackMapping.finish();
}

void DatabaseInterface::updateTrackOrigin(const QUrl &fileName, const QDateTime &fileModifiedTime)
{
    d->mUpdateTrackFileModifiedTime.bindValue(QStringLiteral(":fileName"), fileName);
    d->mUpdateTrackFileModifiedTime.bindValue(QStringLiteral(":mtime"), fileModifiedTime);

    auto queryResult = execQuery(d->mUpdateTrackFileModifiedTime);

    if (!queryResult || !d->mUpdateTrackFileModifiedTime.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackOrigin" << d->mUpdateTrackFileModifiedTime.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackOrigin" << d->mUpdateTrackFileModifiedTime.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackOrigin" << d->mUpdateTrackFileModifiedTime.lastError();

        d->mUpdateTrackFileModifiedTime.finish();

        return;
    }

    d->mUpdateTrackFileModifiedTime.finish();
}

qulonglong DatabaseInterface::internalInsertTrack(const MusicAudioTrack &oneTrack,
                                                  const QHash<QString, QUrl> &covers, bool &isInserted)
{
    qulonglong resultId = 0;

    if (oneTrack.title().isEmpty()) {
        return resultId;
    }

    QUrl::FormattingOptions currentOptions = QUrl::PreferLocalFile |
            QUrl::RemoveAuthority | QUrl::RemoveFilename | QUrl::RemoveFragment |
            QUrl::RemovePassword | QUrl::RemovePort | QUrl::RemoveQuery |
            QUrl::RemoveScheme | QUrl::RemoveUserInfo;

    const auto &trackPath = oneTrack.resourceURI().toString(currentOptions);

    auto albumId = insertAlbum(oneTrack.albumName(), (oneTrack.isValidAlbumArtist() ? oneTrack.albumArtist() : QString()),
                               oneTrack.artist(), trackPath, covers[oneTrack.resourceURI().toString()]);

    auto oldAlbumId = albumId;

    auto existingTrackId = internalTrackIdFromFileName(oneTrack.resourceURI());
    bool isModifiedTrack = (existingTrackId != 0);

    if (isModifiedTrack) {
        resultId = existingTrackId;

        auto oldTrack = internalTrackFromDatabaseId(existingTrackId);
        oldAlbumId = oldTrack.albumId();

        auto isSameTrack = true;
        isSameTrack = isSameTrack && (oldTrack.title() == oneTrack.title());
        isSameTrack = isSameTrack && (oldTrack.albumName() == oneTrack.albumName());
        isSameTrack = isSameTrack && (oldTrack.artist() == oneTrack.artist());
        isSameTrack = isSameTrack && (oldTrack.albumArtist() == oneTrack.albumArtist());
        isSameTrack = isSameTrack && (oldTrack.trackNumberIsValid() == oneTrack.trackNumberIsValid());
        if (isSameTrack && oldTrack.trackNumberIsValid()) {
            isSameTrack = isSameTrack && (oldTrack.trackNumber() == oneTrack.trackNumber());
        }
        isSameTrack = isSameTrack && (oldTrack.discNumberIsValid() == oneTrack.discNumberIsValid());
        if (isSameTrack && oldTrack.discNumberIsValid()) {
            isSameTrack = isSameTrack && (oldTrack.discNumber() == oneTrack.discNumber());
        }
        isSameTrack = isSameTrack && (oldTrack.duration() == oneTrack.duration());
        isSameTrack = isSameTrack && (oldTrack.rating() == oneTrack.rating());
        isSameTrack = isSameTrack && (oldTrack.resourceURI() == oneTrack.resourceURI());
        isSameTrack = isSameTrack && (oldTrack.genre() == oneTrack.genre());
        isSameTrack = isSameTrack && (oldTrack.composer() == oneTrack.composer());
        isSameTrack = isSameTrack && (oldTrack.lyricist() == oneTrack.lyricist());
        isSameTrack = isSameTrack && (oldTrack.comment() == oneTrack.comment());
        isSameTrack = isSameTrack && (oldTrack.year() == oneTrack.year());
        isSameTrack = isSameTrack && (oldTrack.channelsIsValid() == oneTrack.channelsIsValid());
        if (isSameTrack && oldTrack.channelsIsValid()) {
            isSameTrack = isSameTrack && (oldTrack.channels() == oneTrack.channels());
        }
        isSameTrack = isSameTrack && (oldTrack.bitRateIsValid() == oneTrack.bitRateIsValid());
        if (isSameTrack && oldTrack.bitRateIsValid()) {
            isSameTrack = isSameTrack && (oldTrack.bitRate() == oneTrack.bitRate());
        }
        isSameTrack = isSameTrack && (oldTrack.sampleRateIsValid() == oneTrack.sampleRateIsValid());
        if (isSameTrack && oldTrack.sampleRateIsValid()) {
            isSameTrack = isSameTrack && (oldTrack.sampleRate() == oneTrack.sampleRate());
        }

        if (isSameTrack) {
            return resultId;
        }

        auto newTrack = oneTrack;
        newTrack.setDatabaseId(resultId);
        updateTrackInDatabase(newTrack, trackPath);
        updateTrackOrigin(oneTrack.resourceURI(), oneTrack.fileModificationTime());
        updateAlbumFromId(albumId, oneTrack.albumCover(), oneTrack, trackPath);

        recordModifiedTrack(existingTrackId);
        if (albumId != 0) {
            recordModifiedAlbum(albumId);
        }
        if (oldAlbumId != 0) {
            auto tracksCount = fetchTrackIds(oldAlbumId).count();

            if (tracksCount) {
                recordModifiedAlbum(oldAlbumId);
            } else {
                removeAlbumInDatabase(oldAlbumId);
                Q_EMIT albumRemoved(oldAlbumId);
            }
        }

        isInserted = false;

        return resultId;
    } else {
        oldAlbumId = 0;
        existingTrackId = d->mTrackId;

        isInserted = true;
    }

    int priority = 1;
    while(true) {
        auto otherTrackId = getDuplicateTrackIdFromTitleAlbumTrackDiscNumber(oneTrack.title(), oneTrack.artist(), oneTrack.albumName(),
                                                                             oneTrack.albumArtist(), trackPath, priority);

        if (otherTrackId) {
            ++priority;
        } else {
            break;
        }
    }

    resultId = existingTrackId;

    const auto &albumData = internalOneAlbumPartialData(albumId);

    d->mInsertTrackQuery.bindValue(QStringLiteral(":trackId"), existingTrackId);
    d->mInsertTrackQuery.bindValue(QStringLiteral(":fileName"), oneTrack.resourceURI());
    d->mInsertTrackQuery.bindValue(QStringLiteral(":priority"), priority);
    d->mInsertTrackQuery.bindValue(QStringLiteral(":title"), oneTrack.title());
    insertArtist(oneTrack.artist());
    d->mInsertTrackQuery.bindValue(QStringLiteral(":artistName"), oneTrack.artist());
    d->mInsertTrackQuery.bindValue(QStringLiteral(":albumTitle"), albumData[AlbumDataType::key_type::TitleRole]);
    d->mInsertTrackQuery.bindValue(QStringLiteral(":albumArtistName"), albumData[AlbumDataType::key_type::ArtistRole]);
    d->mInsertTrackQuery.bindValue(QStringLiteral(":albumPath"), trackPath);
    if (oneTrack.trackNumberIsValid()) {
        d->mInsertTrackQuery.bindValue(QStringLiteral(":trackNumber"), oneTrack.trackNumber());
    }
    if (oneTrack.discNumberIsValid()) {
        d->mInsertTrackQuery.bindValue(QStringLiteral(":discNumber"), oneTrack.discNumber());
    }
    d->mInsertTrackQuery.bindValue(QStringLiteral(":trackDuration"), QVariant::fromValue<qlonglong>(oneTrack.duration().msecsSinceStartOfDay()));
    d->mInsertTrackQuery.bindValue(QStringLiteral(":trackRating"), oneTrack.rating());
    if (insertGenre(oneTrack.genre()) != 0) {
        d->mInsertTrackQuery.bindValue(QStringLiteral(":genre"), oneTrack.genre());
    } else {
        d->mInsertTrackQuery.bindValue(QStringLiteral(":genre"), {});
    }
    if (insertComposer(oneTrack.composer()) != 0) {
        d->mInsertTrackQuery.bindValue(QStringLiteral(":composer"), oneTrack.composer());
    } else {
        d->mInsertTrackQuery.bindValue(QStringLiteral(":composer"), {});
    }
    if (insertLyricist(oneTrack.lyricist()) != 0) {
        d->mInsertTrackQuery.bindValue(QStringLiteral(":lyricist"), oneTrack.lyricist());
    } else {
        d->mInsertTrackQuery.bindValue(QStringLiteral(":lyricist"), {});
    }
    d->mInsertTrackQuery.bindValue(QStringLiteral(":comment"), oneTrack.comment());
    d->mInsertTrackQuery.bindValue(QStringLiteral(":year"), oneTrack.year());
    if (oneTrack.channelsIsValid()) {
        d->mInsertTrackQuery.bindValue(QStringLiteral(":channels"), oneTrack.channels());
    }
    if (oneTrack.bitRateIsValid()) {
        d->mInsertTrackQuery.bindValue(QStringLiteral(":bitRate"), oneTrack.bitRate());
    }
    if (oneTrack.sampleRateIsValid()) {
        d->mInsertTrackQuery.bindValue(QStringLiteral(":sampleRate"), oneTrack.sampleRate());
    }
    d->mInsertTrackQuery.bindValue(QStringLiteral(":hasEmbeddedCover"), oneTrack.hasEmbeddedCover());

    auto result = execQuery(d->mInsertTrackQuery);

    if (result && d->mInsertTrackQuery.isActive()) {
        d->mInsertTrackQuery.finish();

        if (!isModifiedTrack) {
            ++d->mTrackId;
        }

        updateTrackOrigin(oneTrack.resourceURI(), oneTrack.fileModificationTime());

        if (isModifiedTrack) {
            recordModifiedTrack(existingTrackId);
            if (albumId != 0) {
                recordModifiedAlbum(albumId);
            }
            if (oldAlbumId != 0) {
                recordModifiedAlbum(oldAlbumId);
            }
        }

        if (albumId != 0) {
            if (updateAlbumFromId(albumId, covers[oneTrack.resourceURI().toString()], oneTrack, trackPath)) {
                auto modifiedTracks = fetchTrackIds(albumId);
                for (auto oneModifiedTrack : modifiedTracks) {
                    if (oneModifiedTrack != resultId) {
                        recordModifiedTrack(oneModifiedTrack);
                    }
                }
            }
            recordModifiedAlbum(albumId);
        }
    } else {
        d->mInsertTrackQuery.finish();

        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalInsertTrack" << oneTrack << oneTrack.resourceURI();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalInsertTrack" << d->mInsertTrackQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalInsertTrack" << d->mInsertTrackQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalInsertTrack" << d->mInsertTrackQuery.lastError();
    }

    return resultId;
}

MusicAudioTrack DatabaseInterface::buildTrackFromDatabaseRecord(const QSqlRecord &trackRecord) const
{
    auto id = trackRecord.value(0).toULongLong();

    auto result = MusicAudioTrack{};

    if (result.isValid()) {
        return result;
    }

    result.setDatabaseId(id);
    result.setTitle(trackRecord.value(1).toString());
    result.setParentId(trackRecord.value(2).toString());
    result.setArtist(trackRecord.value(3).toString());

    if (trackRecord.value(4).isValid()) {
        result.setAlbumArtist(trackRecord.value(4).toString());
    }

    result.setResourceURI(trackRecord.value(5).toUrl());
    result.setFileModificationTime(trackRecord.value(6).toDateTime());
    if (trackRecord.value(7).isValid()) {
        result.setTrackNumber(trackRecord.value(7).toInt());
    }
    if (trackRecord.value(8).isValid()) {
        result.setDiscNumber(trackRecord.value(8).toInt());
    }
    result.setDuration(QTime::fromMSecsSinceStartOfDay(trackRecord.value(9).toInt()));
    result.setAlbumName(trackRecord.value(10).toString());
    result.setRating(trackRecord.value(11).toInt());
    result.setAlbumCover(trackRecord.value(12).toUrl());
    result.setIsSingleDiscAlbum(trackRecord.value(13).toBool());
    result.setGenre(trackRecord.value(14).toString());
    result.setComposer(trackRecord.value(15).toString());
    result.setLyricist(trackRecord.value(16).toString());
    result.setComment(trackRecord.value(17).toString());
    result.setYear(trackRecord.value(18).toInt());
    if (trackRecord.value(19).isValid()) {
        bool isValid;
        auto value = trackRecord.value(19).toInt(&isValid);
        if (isValid) {
            result.setChannels(value);
        }
    }
    if (trackRecord.value(20).isValid()) {
        bool isValid;
        auto value = trackRecord.value(20).toInt(&isValid);
        if (isValid) {
            result.setBitRate(value);
        }
    }
    if (trackRecord.value(21).isValid()) {
        bool isValid;
        auto value = trackRecord.value(21).toInt(&isValid);
        if (isValid) {
            result.setSampleRate(value);
        }
    }
    result.setAlbumId(trackRecord.value(2).toULongLong());
    result.setHasEmbeddedCover(trackRecord.value(22).toBool());

    result.setValid(true);

    return result;
}

DatabaseInterface::TrackDataType DatabaseInterface::buildTrackDataFromDatabaseRecord(const QSqlRecord &trackRecord) const
{
    TrackDataType result;

    result[TrackDataType::key_type::DatabaseIdRole] = trackRecord.value(0);
    result[TrackDataType::key_type::TitleRole] = trackRecord.value(1);
    if (!trackRecord.value(10).isNull()) {
        result[TrackDataType::key_type::AlbumRole] = trackRecord.value(10);
        result[TrackDataType::key_type::AlbumIdRole] = trackRecord.value(2);
    }
    if (!trackRecord.value(3).isNull()) {
        result[TrackDataType::key_type::ArtistRole] = trackRecord.value(3);
    }
    if (!trackRecord.value(4).isNull()) {
        result[TrackDataType::key_type::AlbumArtistRole] = trackRecord.value(4);
    }
    result[TrackDataType::key_type::ResourceRole] = trackRecord.value(5);
    if (!trackRecord.value(7).isNull()) {
        result[TrackDataType::key_type::TrackNumberRole] = trackRecord.value(7);
    }
    if (!trackRecord.value(8).isNull()) {
        result[TrackDataType::key_type::DiscNumberRole] = trackRecord.value(8);
    }
    result[TrackDataType::key_type::DurationRole] = QTime::fromMSecsSinceStartOfDay(trackRecord.value(9).toInt());
    result[TrackDataType::key_type::MilliSecondsDurationRole] = trackRecord.value(9).toInt();
    result[TrackDataType::key_type::RatingRole] = trackRecord.value(11);
    if (!trackRecord.value(12).isNull()) {
        result[TrackDataType::key_type::ImageUrlRole] = QUrl(trackRecord.value(12).toString());
    }
    result[TrackDataType::key_type::IsSingleDiscAlbumRole] = trackRecord.value(13);
    if (!trackRecord.value(14).isNull()) {
        result[TrackDataType::key_type::GenreRole] = trackRecord.value(14);
    }
    if (!trackRecord.value(15).isNull()) {
        result[TrackDataType::key_type::ComposerRole] = trackRecord.value(15);
    }
    if (!trackRecord.value(16).isNull()) {
        result[TrackDataType::key_type::LyricistRole] = trackRecord.value(16);
    }
    result[TrackDataType::key_type::HasEmbeddedCover] = trackRecord.value(22);
    result[TrackDataType::key_type::FileModificationTime] = trackRecord.value(6);
    if (!trackRecord.value(24).isNull()) {
        result[TrackDataType::key_type::FirstPlayDate] = trackRecord.value(24);
    }
    if (!trackRecord.value(25).isNull()) {
        result[TrackDataType::key_type::LastPlayDate] = trackRecord.value(25);
    }
    result[TrackDataType::key_type::PlayCounter] = trackRecord.value(26);
    result[TrackDataType::key_type::PlayFrequency] = trackRecord.value(27);
    result[DataType::key_type::ElementTypeRole] = ElisaUtils::Track;

    return result;
}

void DatabaseInterface::internalRemoveTracksList(const QList<QUrl> &removedTracks)
{
    QSet<qulonglong> modifiedAlbums;

    QUrl::FormattingOptions currentOptions = QUrl::PreferLocalFile |
            QUrl::RemoveAuthority | QUrl::RemoveFilename | QUrl::RemoveFragment |
            QUrl::RemovePassword | QUrl::RemovePort | QUrl::RemoveQuery |
            QUrl::RemoveScheme | QUrl::RemoveUserInfo;

    for (const auto &removedTrackFileName : removedTracks) {
        auto removedTrackId = internalTrackIdFromFileName(removedTrackFileName);

        Q_EMIT trackRemoved(removedTrackId);

        auto oneRemovedTrack = internalTrackFromDatabaseId(removedTrackId);

        removeTrackInDatabase(removedTrackId);

        const auto &modifiedAlbumId = internalAlbumIdFromTitleAndArtist(oneRemovedTrack.albumName(), oneRemovedTrack.albumArtist());
        const auto &allTracksFromArtist = internalTracksFromAuthor(oneRemovedTrack.artist());
        const auto &allAlbumsFromArtist = internalAlbumIdsFromAuthor(oneRemovedTrack.artist());
        const auto &removedArtistId = internalArtistIdFromName(oneRemovedTrack.artist());
        const auto &trackPath = oneRemovedTrack.resourceURI().toString(currentOptions);

        if (modifiedAlbumId) {
            recordModifiedAlbum(modifiedAlbumId);
            modifiedAlbums.insert(modifiedAlbumId);
            updateAlbumFromId(modifiedAlbumId, oneRemovedTrack.albumCover(), oneRemovedTrack, trackPath);
        }

        if (removedArtistId != 0 && allTracksFromArtist.isEmpty() && allAlbumsFromArtist.isEmpty()) {
            removeArtistInDatabase(removedArtistId);
            Q_EMIT artistRemoved(removedArtistId);
        }

        d->mRemoveTracksMapping.bindValue(QStringLiteral(":fileName"), removedTrackFileName.toString());

        auto result = execQuery(d->mRemoveTracksMapping);

        if (!result || !d->mRemoveTracksMapping.isActive()) {
            Q_EMIT databaseError();

            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalRemoveTracksList" << d->mRemoveTracksMapping.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalRemoveTracksList" << d->mRemoveTracksMapping.boundValues();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalRemoveTracksList" << d->mRemoveTracksMapping.lastError();

            continue;
        }

        d->mRemoveTracksMapping.finish();
    }

    for (auto modifiedAlbumId : modifiedAlbums) {
        const auto &modifiedAlbumData = internalOneAlbumPartialData(modifiedAlbumId);

        auto tracksCount = fetchTrackIds(modifiedAlbumId).count();

        if (!modifiedAlbumData.isEmpty() && tracksCount) {
            Q_EMIT albumModified({{DatabaseIdRole, modifiedAlbumId}}, modifiedAlbumId);
        } else {
            removeAlbumInDatabase(modifiedAlbumId);
            Q_EMIT albumRemoved(modifiedAlbumId);

            const auto &allTracksFromArtist = internalTracksFromAuthor(modifiedAlbumData[AlbumDataType::key_type::ArtistRole].toString());
            const auto &allAlbumsFromArtist = internalAlbumIdsFromAuthor(modifiedAlbumData[AlbumDataType::key_type::ArtistRole].toString());
            const auto &removedArtistId = internalArtistIdFromName(modifiedAlbumData[AlbumDataType::key_type::ArtistRole].toString());

            if (removedArtistId != 0 && allTracksFromArtist.isEmpty() && allAlbumsFromArtist.isEmpty()) {
                removeArtistInDatabase(removedArtistId);
                Q_EMIT artistRemoved(removedArtistId);
            }
        }
    }
}

QUrl DatabaseInterface::internalAlbumArtUriFromAlbumId(qulonglong albumId)
{
    auto result = QUrl();

    d->mSelectAlbumArtUriFromAlbumIdQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto queryResult = execQuery(d->mSelectAlbumArtUriFromAlbumIdQuery);

    if (!queryResult || !d->mSelectAlbumArtUriFromAlbumIdQuery.isSelect() || !d->mSelectAlbumArtUriFromAlbumIdQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectAlbumArtUriFromAlbumIdQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectAlbumArtUriFromAlbumIdQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectAlbumArtUriFromAlbumIdQuery.lastError();

        d->mSelectAlbumArtUriFromAlbumIdQuery.finish();

        return result;
    }

    if (!d->mSelectAlbumArtUriFromAlbumIdQuery.next()) {
        d->mSelectAlbumArtUriFromAlbumIdQuery.finish();

        return result;
    }

    result = d->mSelectAlbumArtUriFromAlbumIdQuery.record().value(0).toUrl();

    d->mSelectAlbumArtUriFromAlbumIdQuery.finish();

    return result;
}

bool DatabaseInterface::isValidArtist(qulonglong albumId)
{
    auto result = false;

    d->mSelectAlbumQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto queryResult = execQuery(d->mSelectAlbumQuery);

    if (!queryResult || !d->mSelectAlbumQuery.isSelect() || !d->mSelectAlbumQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumFromId" << d->mSelectAlbumQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumFromId" << d->mSelectAlbumQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumFromId" << d->mSelectAlbumQuery.lastError();

        d->mSelectAlbumQuery.finish();

        return result;
    }

    if (!d->mSelectAlbumQuery.next()) {
        d->mSelectAlbumQuery.finish();

        return result;
    }

    const auto &currentRecord = d->mSelectAlbumQuery.record();

    result = !currentRecord.value(2).toString().isEmpty();

    return result;
}

bool DatabaseInterface::internalGenericPartialData(QSqlQuery &query)
{
    auto result = false;

    auto queryResult = execQuery(query);

    if (!queryResult || !query.isSelect() || !query.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAllGenericPartialData" << query.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAllGenericPartialData" << query.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAllGenericPartialData" << query.lastError();

        query.finish();

        auto transactionResult = finishTransaction();
        if (!transactionResult) {
            return result;
        }

        return result;
    }

    result = true;

    return result;
}

qulonglong DatabaseInterface::insertLyricist(const QString &name)
{
    auto result = qulonglong(0);

    if (name.isEmpty()) {
        return result;
    }

    d->mSelectLyricistByNameQuery.bindValue(QStringLiteral(":name"), name);

    auto queryResult = execQuery(d->mSelectLyricistByNameQuery);

    if (!queryResult || !d->mSelectLyricistByNameQuery.isSelect() || !d->mSelectLyricistByNameQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertLyricist" << d->mSelectLyricistByNameQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertLyricist" << d->mSelectLyricistByNameQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertLyricist" << d->mSelectLyricistByNameQuery.lastError();

        d->mSelectLyricistByNameQuery.finish();

        return result;
    }

    if (d->mSelectLyricistByNameQuery.next()) {
        result = d->mSelectLyricistByNameQuery.record().value(0).toULongLong();

        d->mSelectLyricistByNameQuery.finish();

        return result;
    }

    d->mSelectLyricistByNameQuery.finish();

    d->mInsertLyricistQuery.bindValue(QStringLiteral(":lyricistId"), d->mLyricistId);
    d->mInsertLyricistQuery.bindValue(QStringLiteral(":name"), name);

    queryResult = execQuery(d->mInsertLyricistQuery);

    if (!queryResult || !d->mInsertLyricistQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertLyricist" << d->mInsertLyricistQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertLyricist" << d->mInsertLyricistQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertLyricist" << d->mInsertLyricistQuery.lastError();

        d->mInsertLyricistQuery.finish();

        return result;
    }

    result = d->mLyricistId;

    ++d->mLyricistId;

    d->mInsertLyricistQuery.finish();

    Q_EMIT lyricistsAdded(internalAllLyricistsPartialData());

    return result;
}

QHash<QUrl, QDateTime> DatabaseInterface::internalAllFileName()
{
    auto allFileNames = QHash<QUrl, QDateTime>{};

    auto queryResult = execQuery(d->mSelectAllTrackFilesQuery);

    if (!queryResult || !d->mSelectAllTrackFilesQuery.isSelect() || !d->mSelectAllTrackFilesQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertMusicSource" << d->mSelectAllTrackFilesQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertMusicSource" << d->mSelectAllTrackFilesQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertMusicSource" << d->mSelectAllTrackFilesQuery.lastError();

        d->mSelectAllTrackFilesQuery.finish();

        return allFileNames;
    }

    while(d->mSelectAllTrackFilesQuery.next()) {
        auto fileName = d->mSelectAllTrackFilesQuery.record().value(0).toUrl();
        auto fileModificationTime = d->mSelectAllTrackFilesQuery.record().value(1).toDateTime();

        allFileNames[fileName] = fileModificationTime;
    }

    d->mSelectAllTrackFilesQuery.finish();

    return allFileNames;
}

qulonglong DatabaseInterface::internalArtistIdFromName(const QString &name)
{
    auto result = qulonglong(0);

    if (name.isEmpty()) {
        return result;
    }

    d->mSelectArtistByNameQuery.bindValue(QStringLiteral(":name"), name);

    auto queryResult = execQuery(d->mSelectArtistByNameQuery);

    if (!queryResult || !d->mSelectArtistByNameQuery.isSelect() || !d->mSelectArtistByNameQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.lastError();

        d->mSelectArtistByNameQuery.finish();

        return result;
    }

    if (!d->mSelectArtistByNameQuery.next()) {
        d->mSelectArtistByNameQuery.finish();

        return result;
    }

    result = d->mSelectArtistByNameQuery.record().value(0).toULongLong();

    d->mSelectArtistByNameQuery.finish();

    return result;
}

void DatabaseInterface::removeTrackInDatabase(qulonglong trackId)
{
    d->mRemoveTrackQuery.bindValue(QStringLiteral(":trackId"), trackId);

    auto result = execQuery(d->mRemoveTrackQuery);

    if (!result || !d->mRemoveTrackQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeTrackInDatabase" << d->mRemoveTrackQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeTrackInDatabase" << d->mRemoveTrackQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeTrackInDatabase" << d->mRemoveTrackQuery.lastError();
    }

    d->mRemoveTrackQuery.finish();
}

void DatabaseInterface::updateTrackInDatabase(const MusicAudioTrack &oneTrack, const QString &albumPath)
{
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":fileName"), oneTrack.resourceURI());
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":trackId"), oneTrack.databaseId());
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":title"), oneTrack.title());
    insertArtist(oneTrack.artist());
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":artistName"), oneTrack.artist());
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":albumTitle"), oneTrack.albumName());
    if (oneTrack.isValidAlbumArtist()) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":albumArtistName"), oneTrack.albumArtist());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":albumArtistName"), {});
    }
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":albumPath"), albumPath);
    if (oneTrack.trackNumberIsValid()) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":trackNumber"), oneTrack.trackNumber());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":trackNumber"), {});
    }
    if (oneTrack.discNumberIsValid()) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":discNumber"), oneTrack.discNumber());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":discNumber"), {});
    }
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":trackDuration"), QVariant::fromValue<qlonglong>(oneTrack.duration().msecsSinceStartOfDay()));
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":trackRating"), oneTrack.rating());
    if (insertGenre(oneTrack.genre()) != 0) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":genre"), oneTrack.genre());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":genre"), {});
    }
    if (insertComposer(oneTrack.composer()) != 0) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":composer"), oneTrack.composer());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":composer"), {});
    }
    if (insertLyricist(oneTrack.lyricist()) != 0) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":lyricist"), oneTrack.lyricist());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":lyricist"), {});
    }
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":comment"), oneTrack.comment());
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":year"), oneTrack.year());
    if (oneTrack.channelsIsValid()) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":channels"), oneTrack.channels());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":channels"), {});
    }
    if (oneTrack.bitRateIsValid()) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":bitRate"), oneTrack.bitRate());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":bitRate"), {});
    }
    if (oneTrack.sampleRateIsValid()) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":sampleRate"), oneTrack.sampleRate());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":sampleRate"), {});
    }

    auto result = execQuery(d->mUpdateTrackQuery);

    if (!result || !d->mUpdateTrackQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackInDatabase" << d->mUpdateTrackQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackInDatabase" << d->mUpdateTrackQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackInDatabase" << d->mUpdateTrackQuery.lastError();
    }

    d->mUpdateTrackQuery.finish();
}

void DatabaseInterface::removeAlbumInDatabase(qulonglong albumId)
{
    d->mRemoveAlbumQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto result = execQuery(d->mRemoveAlbumQuery);

    if (!result || !d->mRemoveAlbumQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeAlbumInDatabase" << d->mRemoveAlbumQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeAlbumInDatabase" << d->mRemoveAlbumQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeAlbumInDatabase" << d->mRemoveAlbumQuery.lastError();
    }

    d->mRemoveAlbumQuery.finish();
}

void DatabaseInterface::removeArtistInDatabase(qulonglong artistId)
{
    d->mRemoveArtistQuery.bindValue(QStringLiteral(":artistId"), artistId);

    auto result = execQuery(d->mRemoveArtistQuery);

    if (!result || !d->mRemoveArtistQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeArtistInDatabase" << d->mRemoveArtistQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeArtistInDatabase" << d->mRemoveArtistQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeArtistInDatabase" << d->mRemoveArtistQuery.lastError();
    }

    d->mRemoveArtistQuery.finish();
}

void DatabaseInterface::reloadExistingDatabase()
{
    qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::reloadExistingDatabase";

    d->mArtistId = initialId(DataUtils::DataType::AllArtists);
    d->mComposerId = initialId(DataUtils::DataType::AllComposers);
    d->mLyricistId = initialId(DataUtils::DataType::AllLyricists);
    d->mAlbumId = initialId(DataUtils::DataType::AllAlbums);
    d->mTrackId = initialId(DataUtils::DataType::AllTracks);
    d->mGenreId = initialId(DataUtils::DataType::AllGenres);;
}

qulonglong DatabaseInterface::initialId(DataUtils::DataType aType)
{
    switch (aType)
    {
    case DataUtils::DataType::AllAlbums:
        return genericInitialId(d->mQueryMaximumAlbumIdQuery);
    case DataUtils::DataType::AllArtists:
        return genericInitialId(d->mQueryMaximumArtistIdQuery);
    case DataUtils::DataType::AllComposers:
        return genericInitialId(d->mQueryMaximumComposerIdQuery);
    case DataUtils::DataType::AllGenres:
        return genericInitialId(d->mQueryMaximumGenreIdQuery);
    case DataUtils::DataType::AllLyricists:
        return genericInitialId(d->mQueryMaximumLyricistIdQuery);
    case DataUtils::DataType::AllTracks:
        return genericInitialId(d->mQueryMaximumTrackIdQuery);
    case DataUtils::DataType::UnknownType:
        break;
    }
    return 1;
}

qulonglong DatabaseInterface::genericInitialId(QSqlQuery &request)
{
    auto result = qulonglong(0);

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    auto queryResult = execQuery(request);

    if (!queryResult || !request.isSelect() || !request.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertMusicSource" << request.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertMusicSource" << request.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertMusicSource" << request.lastError();

        request.finish();

        transactionResult = finishTransaction();
        if (!transactionResult) {
            return result;
        }

        return result;
    }

    if (request.next()) {
        result = request.record().value(0).toULongLong() + 1;

        request.finish();
    }

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

QList<qulonglong> DatabaseInterface::fetchTrackIds(qulonglong albumId)
{
    auto allTracks = QList<qulonglong>();

    d->mSelectTrackIdQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto result = execQuery(d->mSelectTrackIdQuery);

    if (!result || !d->mSelectTrackIdQuery.isSelect() || !d->mSelectTrackIdQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::fetchTrackIds" << d->mSelectTrackIdQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::fetchTrackIds" << d->mSelectTrackIdQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::fetchTrackIds" << d->mSelectTrackIdQuery.lastError();
    }

    while (d->mSelectTrackIdQuery.next()) {
        const auto &currentRecord = d->mSelectTrackIdQuery.record();

        allTracks.push_back(currentRecord.value(0).toULongLong());
    }

    d->mSelectTrackIdQuery.finish();

    return allTracks;
}

qulonglong DatabaseInterface::internalAlbumIdFromTitleAndArtist(const QString &title, const QString &artist)
{
    auto result = qulonglong(0);

    d->mSelectAlbumIdFromTitleQuery.bindValue(QStringLiteral(":title"), title);
    d->mSelectAlbumIdFromTitleQuery.bindValue(QStringLiteral(":artistName"), artist);

    auto queryResult = execQuery(d->mSelectAlbumIdFromTitleQuery);

    if (!queryResult || !d->mSelectAlbumIdFromTitleQuery.isSelect() || !d->mSelectAlbumIdFromTitleQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleQuery.lastError();

        d->mSelectAlbumIdFromTitleQuery.finish();

        return result;
    }

    if (d->mSelectAlbumIdFromTitleQuery.next()) {
        result = d->mSelectAlbumIdFromTitleQuery.record().value(0).toULongLong();
    }

    d->mSelectAlbumIdFromTitleQuery.finish();

    if (result == 0) {
        d->mSelectAlbumIdFromTitleWithoutArtistQuery.bindValue(QStringLiteral(":title"), title);

        auto queryResult = execQuery(d->mSelectAlbumIdFromTitleWithoutArtistQuery);

        if (!queryResult || !d->mSelectAlbumIdFromTitleWithoutArtistQuery.isSelect() || !d->mSelectAlbumIdFromTitleWithoutArtistQuery.isActive()) {
            Q_EMIT databaseError();

            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.boundValues();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastError();

            d->mSelectAlbumIdFromTitleWithoutArtistQuery.finish();

            return result;
        }

        if (d->mSelectAlbumIdFromTitleWithoutArtistQuery.next()) {
            result = d->mSelectAlbumIdFromTitleWithoutArtistQuery.record().value(0).toULongLong();
        }

        d->mSelectAlbumIdFromTitleWithoutArtistQuery.finish();
    }

    return result;
}

MusicAudioTrack DatabaseInterface::internalTrackFromDatabaseId(qulonglong id)
{
    auto result = MusicAudioTrack();

    if (result.isValid()) {
        return result;
    }

    if (!d || !d->mTracksDatabase.isValid() || !d->mInitFinished) {
        return result;
    }

    d->mSelectTrackFromIdQuery.bindValue(QStringLiteral(":trackId"), id);

    auto queryResult = execQuery(d->mSelectTrackFromIdQuery);

    if (!queryResult || !d->mSelectTrackFromIdQuery.isSelect() || !d->mSelectTrackFromIdQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalTrackFromDatabaseId" << d->mSelectTrackFromIdQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalTrackFromDatabaseId" << d->mSelectTrackFromIdQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalTrackFromDatabaseId" << d->mSelectTrackFromIdQuery.lastError();

        d->mSelectTrackFromIdQuery.finish();

        return result;
    }

    if (!d->mSelectTrackFromIdQuery.next()) {
        d->mSelectTrackFromIdQuery.finish();

        return result;
    }

    const auto &currentRecord = d->mSelectTrackFromIdQuery.record();

    result = buildTrackFromDatabaseRecord(currentRecord);

    d->mSelectTrackFromIdQuery.finish();

    return result;
}

qulonglong DatabaseInterface::internalTrackIdFromTitleAlbumTracDiscNumber(const QString &title, const QString &artist, const QString &album,
                                                                          int trackNumber, int discNumber)
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":title"), title);
    d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":artist"), artist);
    d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":album"), album);
    d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":trackNumber"), trackNumber);
    d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":discNumber"), discNumber);

    auto queryResult = execQuery(d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery);

    if (!queryResult || !d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.isSelect() || !d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.lastError();

        d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.finish();

        return result;
    }

    if (d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.next()) {
        result = d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.record().value(0).toULongLong();
    }

    d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.finish();

    return result;
}

qulonglong DatabaseInterface::getDuplicateTrackIdFromTitleAlbumTrackDiscNumber(const QString &title, const QString &trackArtist,
                                                                               const QString &album, const QString &albumArtist,
                                                                               const QString &trackPath, int priority)
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":title"), title);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":trackArtist"), trackArtist);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":album"), album);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":albumPath"), trackPath);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":albumArtist"), albumArtist);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":priority"), priority);

    auto queryResult = execQuery(d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery);

    if (!queryResult || !d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.isSelect() || !d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.lastError();

        d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.finish();

        return result;
    }

    if (d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.next()) {
        result = d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.record().value(0).toULongLong();
    }

    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.finish();

    return result;
}

qulonglong DatabaseInterface::internalTrackIdFromFileName(const QUrl &fileName)
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    d->mSelectTracksMapping.bindValue(QStringLiteral(":fileName"), fileName.toString());

    auto queryResult = execQuery(d->mSelectTracksMapping);

    if (!queryResult || !d->mSelectTracksMapping.isSelect() || !d->mSelectTracksMapping.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMapping.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMapping.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMapping.lastError();

        d->mSelectTracksMapping.finish();

        return result;
    }

    if (d->mSelectTracksMapping.next()) {
        const auto &currentRecordValue = d->mSelectTracksMapping.record().value(0);
        if (currentRecordValue.isValid()) {
            result = currentRecordValue.toULongLong();
        }
    }

    d->mSelectTracksMapping.finish();

    return result;
}

DatabaseInterface::ListTrackDataType DatabaseInterface::internalTracksFromAuthor(const QString &ArtistName)
{
    auto allTracks = ListTrackDataType{};

    d->mSelectTracksFromArtist.bindValue(QStringLiteral(":artistName"), ArtistName);

    auto result = execQuery(d->mSelectTracksFromArtist);

    if (!result || !d->mSelectTracksFromArtist.isSelect() || !d->mSelectTracksFromArtist.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::tracksFromAuthor" << d->mSelectTracksFromArtist.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::tracksFromAuthor" << d->mSelectTracksFromArtist.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::tracksFromAuthor" << d->mSelectTracksFromArtist.lastError();

        return allTracks;
    }

    while (d->mSelectTracksFromArtist.next()) {
        const auto &currentRecord = d->mSelectTracksFromArtist.record();

        allTracks.push_back(buildTrackDataFromDatabaseRecord(currentRecord));
    }

    d->mSelectTracksFromArtist.finish();

    return allTracks;
}

QList<qulonglong> DatabaseInterface::internalAlbumIdsFromAuthor(const QString &ArtistName)
{
    auto allAlbumIds = QList<qulonglong>();

    d->mSelectAlbumIdsFromArtist.bindValue(QStringLiteral(":artistName"), ArtistName);

    auto result = execQuery(d->mSelectAlbumIdsFromArtist);

    if (!result || !d->mSelectAlbumIdsFromArtist.isSelect() || !d->mSelectAlbumIdsFromArtist.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::tracksFromAuthor" << d->mSelectAlbumIdsFromArtist.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::tracksFromAuthor" << d->mSelectAlbumIdsFromArtist.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::tracksFromAuthor" << d->mSelectAlbumIdsFromArtist.lastError();

        return allAlbumIds;
    }

    while (d->mSelectAlbumIdsFromArtist.next()) {
        const auto &currentRecord = d->mSelectAlbumIdsFromArtist.record();

        allAlbumIds.push_back(currentRecord.value(0).toULongLong());
    }

    d->mSelectAlbumIdsFromArtist.finish();

    return allAlbumIds;
}

DatabaseInterface::ListArtistDataType DatabaseInterface::internalAllArtistsPartialData(QSqlQuery &artistsQuery)
{
    auto result = ListArtistDataType{};

    if (!internalGenericPartialData(artistsQuery)) {
        return result;
    }

    while(artistsQuery.next()) {
        auto newData = ArtistDataType{};

        const auto &currentRecord = artistsQuery.record();

        newData[DataType::key_type::DatabaseIdRole] = currentRecord.value(0);
        newData[DataType::key_type::TitleRole] = currentRecord.value(1);
        newData[DataType::key_type::GenreRole] = QVariant::fromValue(currentRecord.value(2).toString().split(QStringLiteral(", ")));
        newData[DataType::key_type::ElementTypeRole] = ElisaUtils::Artist;

        result.push_back(newData);
    }

    artistsQuery.finish();

    return result;
}

DatabaseInterface::ListAlbumDataType DatabaseInterface::internalAllAlbumsPartialData(QSqlQuery &query)
{
    auto result = ListAlbumDataType{};

    if (!internalGenericPartialData(query)) {
        return result;
    }

    while(query.next()) {
        auto newData = AlbumDataType{};

        const auto &currentRecord = query.record();

        newData[DataType::key_type::DatabaseIdRole] = currentRecord.value(0);
        newData[DataType::key_type::TitleRole] = currentRecord.value(1);
        newData[DataType::key_type::SecondaryTextRole] = currentRecord.value(2);
        newData[DataType::key_type::ImageUrlRole] = currentRecord.value(3);
        newData[DataType::key_type::ArtistRole] = currentRecord.value(4);
        newData[DataType::key_type::AllArtistsRole] = QVariant::fromValue(currentRecord.value(5).toString().split(QStringLiteral(", ")));
        newData[DataType::key_type::HighestTrackRating] = currentRecord.value(6);
        newData[DataType::key_type::IsSingleDiscAlbumRole] = currentRecord.value(8);
        newData[DataType::key_type::GenreRole] = QVariant::fromValue(currentRecord.value(7).toString().split(QStringLiteral(", ")));
        newData[DataType::key_type::ElementTypeRole] = ElisaUtils::Album;

        result.push_back(newData);
    }

    query.finish();

    return result;
}

DatabaseInterface::AlbumDataType DatabaseInterface::internalOneAlbumPartialData(qulonglong databaseId)
{
    auto result = AlbumDataType{};

    d->mSelectAlbumQuery.bindValue(QStringLiteral(":albumId"), databaseId);

    if (!internalGenericPartialData(d->mSelectAlbumQuery)) {
        return result;
    }

    if (d->mSelectAlbumQuery.next()) {
        const auto &currentRecord = d->mSelectAlbumQuery.record();

        result[DataType::key_type::DatabaseIdRole] = currentRecord.value(0);
        result[DataType::key_type::TitleRole] = currentRecord.value(1);
        result[DataType::key_type::SecondaryTextRole] = currentRecord.value(2);
        result[DataType::key_type::ImageUrlRole] = currentRecord.value(4);
        result[DataType::key_type::ArtistRole] = currentRecord.value(2);
        result[DataType::key_type::AllArtistsRole] = QVariant::fromValue(currentRecord.value(7).toString().split(QStringLiteral(", ")));
        result[DataType::key_type::HighestTrackRating] = currentRecord.value(8);
        result[DataType::key_type::IsSingleDiscAlbumRole] = currentRecord.value(6);
        result[DataType::key_type::GenreRole] = QVariant::fromValue(currentRecord.value(9).toString().split(QStringLiteral(", ")));
        result[DataType::key_type::ElementTypeRole] = ElisaUtils::Album;
    }

    d->mSelectAlbumQuery.finish();

    return result;
}

DatabaseInterface::ListTrackDataType DatabaseInterface::internalAllTracksPartialData()
{
    auto result = ListTrackDataType{};

    if (!internalGenericPartialData(d->mSelectAllTracksQuery)) {
        return result;
    }

    while(d->mSelectAllTracksQuery.next()) {
        const auto &currentRecord = d->mSelectAllTracksQuery.record();

        auto newData = buildTrackDataFromDatabaseRecord(currentRecord);

        result.push_back(newData);
    }

    d->mSelectAllTracksQuery.finish();

    return result;
}

DatabaseInterface::ListTrackDataType DatabaseInterface::internalRecentlyPlayedTracksData(int count)
{
    auto result = ListTrackDataType{};

    d->mSelectAllRecentlyPlayedTracksQuery.bindValue(QStringLiteral(":maximumResults"), count);

    if (!internalGenericPartialData(d->mSelectAllRecentlyPlayedTracksQuery)) {
        return result;
    }

    while(d->mSelectAllRecentlyPlayedTracksQuery.next()) {
        const auto &currentRecord = d->mSelectAllRecentlyPlayedTracksQuery.record();

        auto newData = buildTrackDataFromDatabaseRecord(currentRecord);

        result.push_back(newData);
    }

    d->mSelectAllRecentlyPlayedTracksQuery.finish();

    return result;
}

DatabaseInterface::ListTrackDataType DatabaseInterface::internalFrequentlyPlayedTracksData(int count)
{
    auto result = ListTrackDataType{};

    d->mSelectAllFrequentlyPlayedTracksQuery.bindValue(QStringLiteral(":maximumResults"), count);

    if (!internalGenericPartialData(d->mSelectAllFrequentlyPlayedTracksQuery)) {
        return result;
    }

    while(d->mSelectAllFrequentlyPlayedTracksQuery.next()) {
        const auto &currentRecord = d->mSelectAllFrequentlyPlayedTracksQuery.record();

        auto newData = buildTrackDataFromDatabaseRecord(currentRecord);

        result.push_back(newData);
    }

    d->mSelectAllFrequentlyPlayedTracksQuery.finish();

    return result;
}

DatabaseInterface::TrackDataType DatabaseInterface::internalOneTrackPartialData(qulonglong databaseId)
{
    auto result = TrackDataType{};

    d->mSelectTrackFromIdQuery.bindValue(QStringLiteral(":trackId"), databaseId);

    if (!internalGenericPartialData(d->mSelectTrackFromIdQuery)) {
        return result;
    }

    if (d->mSelectTrackFromIdQuery.next()) {
        const auto &currentRecord = d->mSelectTrackFromIdQuery.record();

        result = buildTrackDataFromDatabaseRecord(currentRecord);
    }

    d->mSelectTrackFromIdQuery.finish();

    return result;
}

DatabaseInterface::ListGenreDataType DatabaseInterface::internalAllGenresPartialData()
{
    ListGenreDataType result;

    if (!internalGenericPartialData(d->mSelectAllGenresQuery)) {
        return result;
    }

    while(d->mSelectAllGenresQuery.next()) {
        auto newData = GenreDataType{};

        const auto &currentRecord = d->mSelectAllGenresQuery.record();

        newData[DataType::key_type::DatabaseIdRole] = currentRecord.value(0);
        newData[DataType::key_type::TitleRole] = currentRecord.value(1);
        newData[DataType::key_type::ElementTypeRole] = ElisaUtils::Genre;

        result.push_back(newData);
    }

    d->mSelectAllGenresQuery.finish();

    return result;
}

DatabaseInterface::ListArtistDataType DatabaseInterface::internalAllComposersPartialData()
{
    ListArtistDataType result;

    if (!internalGenericPartialData(d->mSelectAllComposersQuery)) {
        return result;
    }

    while(d->mSelectAllComposersQuery.next()) {
        auto newData = ArtistDataType{};

        const auto &currentRecord = d->mSelectAllComposersQuery.record();

        newData[DataType::key_type::DatabaseIdRole] = currentRecord.value(0);
        newData[DataType::key_type::TitleRole] = currentRecord.value(1);
        newData[DataType::key_type::ElementTypeRole] = ElisaUtils::Composer;

        result.push_back(newData);
    }

    d->mSelectAllComposersQuery.finish();

    return result;
}

DatabaseInterface::ListArtistDataType DatabaseInterface::internalAllLyricistsPartialData()
{
    ListArtistDataType result;

    if (!internalGenericPartialData(d->mSelectAllLyricistsQuery)) {
        return result;
    }

    while(d->mSelectAllLyricistsQuery.next()) {
        auto newData = ArtistDataType{};

        const auto &currentRecord = d->mSelectAllLyricistsQuery.record();

        newData[DataType::key_type::DatabaseIdRole] = currentRecord.value(0);
        newData[DataType::key_type::TitleRole] = currentRecord.value(1);
        newData[DataType::key_type::ElementTypeRole] = ElisaUtils::Lyricist;

        result.push_back(newData);
    }

    d->mSelectAllLyricistsQuery.finish();

    return result;
}

bool DatabaseInterface::prepareQuery(QSqlQuery &query, const QString &queryText) const
{
    query.setForwardOnly(true);
    return query.prepare(queryText);
}

bool DatabaseInterface::execQuery(QSqlQuery &query)
{
#if !defined NDEBUG
    auto timer = QElapsedTimer{};
    timer.start();
#endif

    auto result = query.exec();

#if !defined NDEBUG
    if (timer.nsecsElapsed() > 10000000) {
        qCDebug(orgKdeElisaDatabase) << "[[" << timer.nsecsElapsed() << "]]" << query.lastQuery();
    }
#endif

    return result;
}

void DatabaseInterface::updateAlbumArtist(qulonglong albumId, const QString &title,
                                          const QString &albumPath,
                                          const QString &artistName)
{
    d->mUpdateAlbumArtistQuery.bindValue(QStringLiteral(":albumId"), albumId);
    insertArtist(artistName);
    d->mUpdateAlbumArtistQuery.bindValue(QStringLiteral(":artistName"), artistName);

    auto queryResult = execQuery(d->mUpdateAlbumArtistQuery);

    if (!queryResult || !d->mUpdateAlbumArtistQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumArtist" << d->mUpdateAlbumArtistQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumArtist" << d->mUpdateAlbumArtistQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumArtist" << d->mUpdateAlbumArtistQuery.lastError();

        d->mUpdateAlbumArtistQuery.finish();

        return;
    }

    d->mUpdateAlbumArtistQuery.finish();

    d->mUpdateAlbumArtistInTracksQuery.bindValue(QStringLiteral(":albumTitle"), title);
    d->mUpdateAlbumArtistInTracksQuery.bindValue(QStringLiteral(":albumPath"), albumPath);
    d->mUpdateAlbumArtistInTracksQuery.bindValue(QStringLiteral(":artistName"), artistName);

    queryResult = execQuery(d->mUpdateAlbumArtistInTracksQuery);

    if (!queryResult || !d->mUpdateAlbumArtistInTracksQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumArtist" << d->mUpdateAlbumArtistInTracksQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumArtist" << d->mUpdateAlbumArtistInTracksQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumArtist" << d->mUpdateAlbumArtistInTracksQuery.lastError();

        d->mUpdateAlbumArtistInTracksQuery.finish();

        return;
    }

    d->mUpdateAlbumArtistInTracksQuery.finish();
}

void DatabaseInterface::updateTrackStatistics(const QUrl &fileName, const QDateTime &time)
{
    d->mUpdateTrackStatistics.bindValue(QStringLiteral(":fileName"), fileName);
    d->mUpdateTrackStatistics.bindValue(QStringLiteral(":playDate"), time.toMSecsSinceEpoch());

    auto queryResult = execQuery(d->mUpdateTrackStatistics);

    if (!queryResult || !d->mUpdateTrackStatistics.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackStatistics" << d->mUpdateTrackStatistics.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackStatistics" << d->mUpdateTrackStatistics.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackStatistics" << d->mUpdateTrackStatistics.lastError();

        d->mUpdateTrackStatistics.finish();

        return;
    }

    d->mUpdateTrackStatistics.finish();

    d->mUpdateTrackFirstPlayStatistics.bindValue(QStringLiteral(":fileName"), fileName);
    d->mUpdateTrackFirstPlayStatistics.bindValue(QStringLiteral(":playDate"), time.toMSecsSinceEpoch());

    queryResult = execQuery(d->mUpdateTrackFirstPlayStatistics);

    if (!queryResult || !d->mUpdateTrackFirstPlayStatistics.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackStatistics" << d->mUpdateTrackFirstPlayStatistics.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackStatistics" << d->mUpdateTrackFirstPlayStatistics.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackStatistics" << d->mUpdateTrackFirstPlayStatistics.lastError();

        d->mUpdateTrackFirstPlayStatistics.finish();

        return;
    }

    d->mUpdateTrackFirstPlayStatistics.finish();
}


#include "moc_databaseinterface.cpp"
