/***************************************************************************
 *   Copyright 2014 Sujith Haridasan <sujith.haridasan@kdemail.net>        *
 *   Copyright 2014 Ashish Madeti <ashishmadeti@gmail.com>                 *
 *   Copyright 2016 Matthieu Gallien <mgallien@mgallien.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef MEDIACENTER_MEDIAPLAYER2_H
#define MEDIACENTER_MEDIAPLAYER2_H

#include "elisaLib_export.h"

#include <QDBusAbstractAdaptor>
#include <QStringList>


class ELISALIB_EXPORT MediaPlayer2 : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2") // Docs: http://specifications.freedesktop.org/mpris-spec/latest/Media_Player.html

    Q_PROPERTY(bool CanQuit READ CanQuit CONSTANT)
    Q_PROPERTY(bool CanRaise READ CanRaise CONSTANT)
    Q_PROPERTY(bool HasTrackList READ HasTrackList CONSTANT)

    Q_PROPERTY(QString Identity READ Identity CONSTANT)
    Q_PROPERTY(QString DesktopEntry READ DesktopEntry CONSTANT)

    Q_PROPERTY(QStringList SupportedUriSchemes READ SupportedUriSchemes CONSTANT)
    Q_PROPERTY(QStringList SupportedMimeTypes READ SupportedMimeTypes CONSTANT)

public:
    explicit MediaPlayer2(QObject* parent = nullptr);
    ~MediaPlayer2() override;

    bool CanQuit() const;
    bool CanRaise() const;
    bool HasTrackList() const;

    QString Identity() const;
    QString DesktopEntry() const;

    QStringList SupportedUriSchemes() const;
    QStringList SupportedMimeTypes() const;

public Q_SLOTS:
    void Quit();
    void Raise();

Q_SIGNALS:
    void raisePlayer();

};

#endif //MEDIACENTER_MEDIAPLAYER2_H
