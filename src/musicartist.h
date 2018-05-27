/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef MUSICARTIST_H
#define MUSICARTIST_H

#include "elisaLib_export.h"

#include <QString>
#include <QMetaType>

#include <memory>

class MusicArtistPrivate;
class QDebug;

class ELISALIB_EXPORT MusicArtist
{
public:

    MusicArtist();

    MusicArtist(MusicArtist &&other);

    MusicArtist(const MusicArtist &other);

    MusicArtist& operator=(MusicArtist &&other);

    MusicArtist& operator=(const MusicArtist &other);

    ~MusicArtist();

    void setValid(bool value);

    bool isValid() const;

    void setName(const QString &aName);

    QString name() const;

    void setGenres(const QStringList &aGenres);

    QStringList genres() const;

    void setDatabaseId(qulonglong value);

    qulonglong databaseId() const;

    void setAlbumsCount(int value);

    int albumsCount() const;

private:

    std::unique_ptr<MusicArtistPrivate> d;

};

QDebug& operator<<(QDebug &stream, const MusicArtist &data);

bool operator==(const MusicArtist &artist1, const MusicArtist &artist2);

Q_DECLARE_METATYPE(MusicArtist)

#endif // MUSICARTIST_H
