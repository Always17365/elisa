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

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as Controls1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import org.kde.elisa 1.0

FocusScope {
    id: playListEntry

    property var index
    property bool isSingleDiscAlbum
    property int isPlaying
    property bool isSelected
    property bool isValid
    property bool isAlternateColor
    property bool containsMouse
    property bool hasAlbumHeader
    property string titleDisplay
    property alias trackData: dataHelper.trackData

    signal startPlayback()
    signal pausePlayback()
    signal removeFromPlaylist(var trackIndex)
    signal switchToTrack(var trackIndex)

    height: (hasAlbumHeader ? elisaTheme.delegateWithHeaderHeight : elisaTheme.delegateHeight)

    Controls1.Action {
        id: removeFromPlayList
        text: i18nc("Remove current track from play list", "Remove")
        iconName: "error"
        onTriggered: {
            playListEntry.removeFromPlaylist(playListEntry.index)
        }
    }

    Controls1.Action {
        id: playNow
        text: i18nc("Play now current track from play list", "Play Now")
        iconName: "media-playback-start"
        enabled: !(isPlaying === MediaPlayList.IsPlaying) && isValid
        onTriggered: {
            if (isPlaying === MediaPlayList.NotPlaying) {
                playListEntry.switchToTrack(playListEntry.index)
            }
            playListEntry.startPlayback()
        }
    }

    Controls1.Action {
        id: pauseNow
        text: i18nc("Pause current track from play list", "Pause")
        iconName: "media-playback-pause"
        enabled: isPlaying == MediaPlayList.IsPlaying && isValid
        onTriggered: playListEntry.pausePlayback()
    }

    Controls1.Action {
        id: showInfo
        text: i18nc("Show track metadata", "View Details")
        iconName: "help-about"
        enabled: isValid
        onTriggered: {
            if (metadataLoader.active === false) {
                metadataLoader.active = true
            }
            else {
                metadataLoader.item.close();
                metadataLoader.active = false
            }
        }
    }

    TrackDataHelper {
        id: dataHelper
    }

    Loader {
        id: metadataLoader
        active: false
        onLoaded: item.show()

        sourceComponent:  MediaTrackMetadataView {
            trackDataHelper: dataHelper

            onRejected: metadataLoader.active = false;
        }
    }

    Rectangle {
        id: entryBackground

        anchors.fill: parent

        color: (isAlternateColor ? myPalette.alternateBase : myPalette.base)

        height: (hasAlbumHeader ? elisaTheme.delegateWithHeaderHeight : elisaTheme.delegateHeight)

        focus: true

        ColumnLayout {
            spacing: 0

            anchors.fill: parent
            anchors.leftMargin: elisaTheme.layoutHorizontalMargin
            anchors.rightMargin: elisaTheme.layoutHorizontalMargin

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: elisaTheme.delegateWithHeaderHeight - elisaTheme.delegateHeight
                Layout.minimumHeight: elisaTheme.delegateWithHeaderHeight - elisaTheme.delegateHeight
                Layout.maximumHeight: elisaTheme.delegateWithHeaderHeight - elisaTheme.delegateHeight

                visible: hasAlbumHeader

                RowLayout {
                    id: headerRow

                    spacing: elisaTheme.layoutHorizontalMargin

                    anchors.fill: parent

                    Image {
                        id: mainIcon

                        source: (isValid ? (dataHelper.hasValidAlbumCover() ? dataHelper.albumCover : Qt.resolvedUrl(elisaTheme.defaultAlbumImage)) : Qt.resolvedUrl(elisaTheme.errorIcon))

                        Layout.minimumWidth: headerRow.height - 4
                        Layout.maximumWidth: headerRow.height - 4
                        Layout.preferredWidth: headerRow.height - 4
                        Layout.minimumHeight: headerRow.height - 4
                        Layout.maximumHeight: headerRow.height - 4
                        Layout.preferredHeight: headerRow.height - 4
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                        sourceSize.width: headerRow.height - 4
                        sourceSize.height: parent.height - 4

                        fillMode: Image.PreserveAspectFit
                        asynchronous: true

                        visible: isValid
                    }

                    BrightnessContrast {
                        source: mainIcon

                        cached: true

                        visible: !isValid

                        contrast: -0.9

                        Layout.minimumWidth: headerRow.height - 4
                        Layout.maximumWidth: headerRow.height - 4
                        Layout.preferredWidth: headerRow.height - 4
                        Layout.minimumHeight: headerRow.height - 4
                        Layout.maximumHeight: headerRow.height - 4
                        Layout.preferredHeight: headerRow.height - 4
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        spacing: 0

                        LabelWithToolTip {
                            id: mainLabel
                            text: dataHelper.albumName

                            font.weight: Font.Bold
                            color: myPalette.text

                            horizontalAlignment: Text.AlignHCenter

                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignCenter
                            Layout.topMargin: elisaTheme.layoutVerticalMargin

                            elide: Text.ElideRight
                        }

                        Item {
                            Layout.fillHeight: true
                        }

                        LabelWithToolTip {
                            id: authorLabel

                            text: dataHelper.albumArtist

                            font.weight: Font.Light
                            color: myPalette.text

                            horizontalAlignment: Text.AlignHCenter

                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignCenter
                            Layout.bottomMargin: elisaTheme.layoutVerticalMargin

                            elide: Text.ElideRight
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                RowLayout {
                    id: trackRow

                    anchors.fill: parent

                    spacing: elisaTheme.layoutHorizontalMargin

                    LabelWithToolTip {
                        id: mainCompactLabel

                        text: {
                            if (dataHelper.hasValidTrackNumber()) {
                                if (dataHelper.hasValidDiscNumber() && !isSingleDiscAlbum)
                                    return i18nc("%1: disk number. %2: track number. %3: track title", "%1 - %2 - %3",
                                                 Number(dataHelper.discNumber).toLocaleString(Qt.locale(), 'f', 0),
                                                 Number(dataHelper.trackNumber).toLocaleString(Qt.locale(), 'f', 0), dataHelper.title);
                                else
                                    return i18nc("%1: track number. %2: track title", "%1 - %2",
                                                 Number(dataHelper.trackNumber).toLocaleString(Qt.locale(), 'f', 0), dataHelper.title);
                            } else {
                                return dataHelper.title;
                            }
                        }

                        font.weight: (isPlaying ? Font.Bold : Font.Normal)
                        color: myPalette.text

                        Layout.maximumWidth: mainCompactLabel.implicitWidth + 1
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                        visible: isValid

                        elide: Text.ElideRight
                        horizontalAlignment: Text.AlignLeft
                    }

                    LabelWithToolTip {
                        id: mainInvalidCompactLabel

                        text: titleDisplay

                        font.weight: Font.Normal
                        color: myPalette.text

                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                        visible: !isValid

                        elide: Text.ElideRight
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 0
                    }

                    Controls1.ToolButton {
                        id: infoButton
                        objectName: 'infoButton'

                        implicitHeight: elisaTheme.smallDelegateToolButtonSize
                        implicitWidth: elisaTheme.smallDelegateToolButtonSize

                        opacity: 0

                        visible: opacity > 0.1

                        action: showInfo
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    }

                    Controls1.ToolButton {
                        id: playPauseButton
                        objectName: 'playPauseButton'

                        implicitHeight: elisaTheme.smallDelegateToolButtonSize
                        implicitWidth: elisaTheme.smallDelegateToolButtonSize

                        opacity: 0
                        scale: LayoutMirroring.enabled ? -1 : 1 // We can mirror the symmetrical pause icon

                        visible: opacity > 0.1
                        action: !(isPlaying === MediaPlayList.IsPlaying) ? playNow : pauseNow
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    }

                    Item {
                        implicitHeight: elisaTheme.smallDelegateToolButtonSize
                        implicitWidth: elisaTheme.smallDelegateToolButtonSize
                        Layout.maximumWidth: elisaTheme.smallDelegateToolButtonSize
                        Layout.maximumHeight: elisaTheme.smallDelegateToolButtonSize
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                        Controls1.ToolButton {
                            id: removeButton
                            objectName: 'removeButton'

                            anchors.fill: parent

                            opacity: 0

                            visible: opacity > 0.1
                            action: removeFromPlayList
                        }

                        Image {
                            id: playIcon

                            anchors.fill: parent

                            opacity: 0

                            source: Qt.resolvedUrl(elisaTheme.playIndicatorIcon)

                            width: parent.height * 1.
                            height: parent.height * 1.
                            sourceSize.width: parent.height * 1.
                            sourceSize.height: parent.height * 1.
                            fillMode: Image.PreserveAspectFit
                            mirror: LayoutMirroring.enabled
                            visible: opacity > 0.0
                        }
                    }

                    RatingStar {
                        id: ratingWidget

                        starRating: dataHelper.rating

                        starSize: elisaTheme.ratingStarSize
                    }

                    TextMetrics {
                        id: durationTextMetrics
                        text: i18nc("This is used to preserve a fixed width for the duration text.", "00:00")
                    }

                    LabelWithToolTip {
                        id: durationLabel

                        text: dataHelper.duration

                        color: myPalette.text

                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                        Layout.preferredWidth: durationTextMetrics.width + 1

                        horizontalAlignment: Text.AlignRight
                    }
                }
            }
        }
    }

    states: [
        State {
            name: 'notSelected'
            when: !containsMouse && (!playListEntry.activeFocus || !isSelected)
            PropertyChanges {
                target: removeButton
                opacity: 0
            }
            PropertyChanges {
                target: infoButton
                opacity: 0
            }
            PropertyChanges {
                target: playPauseButton
                opacity: 0
            }
            PropertyChanges {
                target: playIcon
                opacity: (isPlaying === MediaPlayList.IsPlaying ? 1.0 : 0.0)
            }
            PropertyChanges {
                target: entryBackground
                color: (isAlternateColor ? myPalette.alternateBase : myPalette.base)
            }
        },
        State {
            name: 'hoveredOrSelected'
            when: containsMouse || (playListEntry.activeFocus && isSelected)
            PropertyChanges {
                target: removeButton
                opacity: 1
            }
            PropertyChanges {
                target: playPauseButton
                opacity: 1
            }
            PropertyChanges {
                target: infoButton
                opacity: 1
            }
            PropertyChanges {
                target: playIcon
                opacity: 0
            }
            PropertyChanges {
                target: entryBackground
                color: myPalette.mid
            }
        }
    ]
    transitions: Transition {
        ParallelAnimation {
            NumberAnimation {
                properties: "opacity"
                easing.type: Easing.InOutQuad
                duration: 250
            }
            ColorAnimation {
                properties: "color"
                duration: 250
            }
        }
    }
}

