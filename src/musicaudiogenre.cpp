/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "musicaudiogenre.h"

#include <QDebug>

class MusicAudioGenrePrivate
{
public:

    QString mName;

    qulonglong mDatabaseId = 0;

};

MusicAudioGenre::MusicAudioGenre() : d(std::make_unique<MusicAudioGenrePrivate>())
{
}

MusicAudioGenre::MusicAudioGenre(MusicAudioGenre &&other)
{
    d.swap(other.d);
}

MusicAudioGenre::MusicAudioGenre(const MusicAudioGenre &other) : d(std::make_unique<MusicAudioGenrePrivate>(*other.d))
{
}

MusicAudioGenre& MusicAudioGenre::operator=(MusicAudioGenre &&other)
{
    if (&other != this) {
        d.reset();
        d.swap(other.d);
    }

    return *this;
}

MusicAudioGenre &MusicAudioGenre::operator=(const MusicAudioGenre &other)
{
    if (&other != this) {
        (*d) = (*other.d);
    }

    return *this;
}

MusicAudioGenre::~MusicAudioGenre()
= default;

void MusicAudioGenre::setName(const QString &aName)
{
    d->mName = aName;
}

QString MusicAudioGenre::name() const
{
    return d->mName;
}

void MusicAudioGenre::setDatabaseId(qulonglong value)
{
    d->mDatabaseId = value;
}

qulonglong MusicAudioGenre::databaseId() const
{
    return d->mDatabaseId;
}

QDebug& operator<<(QDebug &stream, const MusicAudioGenre &data)
{
    stream << data.name() << data.databaseId();

    return stream;
}

bool operator==(const MusicAudioGenre &genre1, const MusicAudioGenre &genre2)
{
    return genre1.name() == genre2.name();
}
