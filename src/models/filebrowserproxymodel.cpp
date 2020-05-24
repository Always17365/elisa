/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "filebrowserproxymodel.h"

#include "filebrowsermodel.h"
#include "mediaplaylistproxymodel.h"

#include <QReadLocker>
#include <QDir>
#include <QtConcurrent>

#include <KIOWidgets/KDirLister>
#include "elisautils.h"

FileBrowserProxyModel::FileBrowserProxyModel(QObject *parent) : KDirSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    mThreadPool.setMaxThreadCount(1);
    setSortFoldersFirst(true);
    sort(Qt::AscendingOrder);
}

FileBrowserProxyModel::~FileBrowserProxyModel() = default;

QString FileBrowserProxyModel::filterText() const
{
    return mFilterText;
}

void FileBrowserProxyModel::setFilterText(const QString &filterText)
{
    QWriteLocker writeLocker(&mDataLock);

    if (mFilterText == filterText)
        return;

    mFilterText = filterText;

    mFilterExpression.setPattern(mFilterText);
    mFilterExpression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    mFilterExpression.optimize();

    invalidate();

    Q_EMIT filterTextChanged(mFilterText);
}

bool FileBrowserProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool result = false;

    for (int column = 0, columnCount = sourceModel()->columnCount(source_parent); column < columnCount; ++column) {
        auto currentIndex = sourceModel()->index(source_row, column, source_parent);

        const auto &nameValue = sourceModel()->data(currentIndex, Qt::DisplayRole).toString();

        if (mFilterExpression.match(nameValue).hasMatch()) {
            result = true;
            continue;
        }

        if (result) {
            continue;
        }

        if (!result) {
            break;
        }
    }

    return result;
}

void FileBrowserProxyModel::genericEnqueueToPlayList(QModelIndex rootIndex,
                                                     ElisaUtils::PlayListEnqueueMode enqueueMode,
                                                     ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allData = DataTypes::EntryDataList{};
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0, rootIndex);
            if (!data(currentIndex, DataTypes::IsDirectoryRole).toBool()) {
                allData.push_back(DataTypes::EntryData{data(currentIndex, DataTypes::FullDataRole).value<DataTypes::MusicDataType>(),
                                                       data(currentIndex, Qt::DisplayRole).toString(),
                                                       data(currentIndex, DataTypes::ResourceRole).toUrl()});
            }
        }
        Q_EMIT entriesToEnqueue(allData, enqueueMode, triggerPlay);
    });
}

void FileBrowserProxyModel::enqueueToPlayList(QModelIndex rootIndex)
{
    genericEnqueueToPlayList(rootIndex,
                             ElisaUtils::AppendPlayList,
                             ElisaUtils::DoNotTriggerPlay);
}

void FileBrowserProxyModel::replaceAndPlayOfPlayList(QModelIndex rootIndex)
{
    genericEnqueueToPlayList(rootIndex,
                             ElisaUtils::ReplacePlayList,
                             ElisaUtils::TriggerPlay);
}

void FileBrowserProxyModel::disconnectPlayList()
{
    if (mPlayList) {
        disconnect(this, &FileBrowserProxyModel::entriesToEnqueue,
                   mPlayList, static_cast<void(MediaPlayListProxyModel::*)(const DataTypes::EntryDataList&, ElisaUtils::PlayListEnqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayListProxyModel::enqueue));
    }
}

void FileBrowserProxyModel::connectPlayList()
{
    if (mPlayList) {
        connect(this, &FileBrowserProxyModel::entriesToEnqueue,
                mPlayList, static_cast<void(MediaPlayListProxyModel::*)(const DataTypes::EntryDataList&, ElisaUtils::PlayListEnqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayListProxyModel::enqueue));
    }
}

void FileBrowserProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    KDirSortFilterProxyModel::setSourceModel(sourceModel);

    auto fileBrowserModel = dynamic_cast<FileBrowserModel*>(sourceModel);

    if (!fileBrowserModel) {
        return;
    }
}

MediaPlayListProxyModel *FileBrowserProxyModel::playList() const
{
    return mPlayList;
}

int FileBrowserProxyModel::filterRating() const
{
    return mFilterRating;
}

bool FileBrowserProxyModel::sortedAscending() const
{
    return sortOrder() ? false : true;
}

void FileBrowserProxyModel::sortModel(Qt::SortOrder order)
{
    sort(0, order);
    Q_EMIT sortedAscendingChanged();
}

void FileBrowserProxyModel::setPlayList(MediaPlayListProxyModel *playList)
{
    disconnectPlayList();

    if (mPlayList == playList) {
        return;
    }

    mPlayList = playList;
    Q_EMIT playListChanged();

    connectPlayList();
}

void FileBrowserProxyModel::setFilterRating(int filterRating)
{
    if (mFilterRating == filterRating) {
        return;
    }

    mFilterRating = filterRating;
    Q_EMIT filterRatingChanged();
}

#include "moc_filebrowserproxymodel.cpp"
