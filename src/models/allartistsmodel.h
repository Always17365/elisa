/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef ALLARTISTSMODEL_H
#define ALLARTISTSMODEL_H

#include "elisaLib_export.h"

#include <QAbstractItemModel>
#include <QVector>
#include <QHash>
#include <QString>

#include "musicartist.h"

#include <memory>

class DatabaseInterface;
class AllArtistsModelPrivate;
class AllAlbumsModel;

class ELISALIB_EXPORT AllArtistsModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(AllAlbumsModel* allAlbums
               READ allAlbums
               WRITE setAllAlbums
               NOTIFY allAlbumsChanged)

public:

    explicit AllArtistsModel(QObject *parent = nullptr);

    ~AllArtistsModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    AllAlbumsModel* allAlbums() const;

Q_SIGNALS:

    void allAlbumsChanged();

public Q_SLOTS:

    void artistsAdded(const QList<MusicArtist> &newArtists);

    void artistRemoved(const MusicArtist &removedArtist);

    void artistModified(const MusicArtist &modifiedArtist);

    void setAllAlbums(AllAlbumsModel *model);

private:

    std::unique_ptr<AllArtistsModelPrivate> d;

};

#endif // ALLARTISTSMODEL_H
