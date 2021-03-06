/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef VIEWCONFIGURATIONDATA_H
#define VIEWCONFIGURATIONDATA_H

#include "elisaLib_export.h"

#include "elisautils.h"
#include "datatypes.h"
#include "viewmanager.h"

#include <QObject>
#include <QString>
#include <QUrl>

#include <memory>

class ViewConfigurationDataPrivate;
class QAbstractItemModel;
class QAbstractProxyModel;

class ELISALIB_EXPORT ViewConfigurationData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(ElisaUtils::FilterType filterType
               READ filterType
               CONSTANT)

    Q_PROPERTY(int expectedDepth
               READ expectedDepth
               CONSTANT)

    Q_PROPERTY(QString mainTitle
               READ mainTitle
               CONSTANT)

    Q_PROPERTY(QString secondaryTitle
               READ secondaryTitle
               CONSTANT)

    Q_PROPERTY(QUrl imageUrl
               READ imageUrl
               CONSTANT)

    Q_PROPERTY(ElisaUtils::PlayListEntryType dataType
               READ dataType
               CONSTANT)

    Q_PROPERTY(QAbstractItemModel *model
               READ model
               CONSTANT)

    Q_PROPERTY(QAbstractProxyModel *associatedProxyModel
               READ associatedProxyModel
               CONSTANT)

    Q_PROPERTY(QUrl viewDefaultIcon
               READ viewDefaultIcon
               CONSTANT)

    Q_PROPERTY(DataTypes::DataType dataFilter
               READ dataFilter
               CONSTANT)

    Q_PROPERTY(ViewManager::ViewCanBeRated viewShowRating
               READ viewShowRating
               CONSTANT)

    Q_PROPERTY(ViewManager::DelegateUseSecondaryText viewDelegateDisplaySecondaryText
               READ viewDelegateDisplaySecondaryText
               CONSTANT)

    Q_PROPERTY(ViewManager::IsTreeModelType isTreeModel
               READ isTreeModel
               CONSTANT)

    Q_PROPERTY(int sortRole
               READ sortRole
               CONSTANT)

    Q_PROPERTY(Qt::SortOrder sortOrder
               READ sortOrder
               CONSTANT)

    Q_PROPERTY(ViewManager::AlbumCardinality displaySingleAlbum
               READ displaySingleAlbum
               CONSTANT)

    Q_PROPERTY(ViewManager::AlbumViewStyle showDiscHeaders
               READ showDiscHeaders
               CONSTANT)

    Q_PROPERTY(ViewManager::RadioSpecificStyle radioCase
               READ radioCase
               CONSTANT)

public:
    explicit ViewConfigurationData(QObject *parent = nullptr);

    explicit ViewConfigurationData(ElisaUtils::FilterType filterType, int expectedDepth,
                                   QString mainTitle, QString secondaryTitle, QUrl imageUrl,
                                   ElisaUtils::PlayListEntryType dataType, QAbstractItemModel *model,
                                   QAbstractProxyModel *associatedProxyModel, QUrl viewDefaultIcon,
                                   DataTypes::DataType dataFilter, ViewManager::ViewCanBeRated viewShowRating,
                                   ViewManager::DelegateUseSecondaryText viewDelegateDisplaySecondaryText,
                                   ViewManager::IsTreeModelType isTreeModel, QObject *parent = nullptr);

    explicit ViewConfigurationData(ElisaUtils::FilterType filterType, int expectedDepth,
                                   QString mainTitle, QString secondaryTitle,
                                   QUrl imageUrl, ElisaUtils::PlayListEntryType dataType, QAbstractItemModel *model,
                                   QAbstractProxyModel *associatedProxyModel, DataTypes::DataType dataFilter, int sortRole,
                                   Qt::SortOrder sortOrder, ViewManager::AlbumCardinality displaySingleAlbum,
                                   ViewManager::AlbumViewStyle showDiscHeaders, ViewManager::RadioSpecificStyle radioCase,
                                   ViewManager::IsTreeModelType isTreeModel, QObject *parent = nullptr);

    ~ViewConfigurationData();

    ElisaUtils::FilterType filterType() const;

    int expectedDepth() const;

    QString mainTitle() const;

    QString secondaryTitle() const;

    QUrl imageUrl() const;

    ElisaUtils::PlayListEntryType dataType() const;

    QAbstractItemModel * model() const;

    QAbstractProxyModel * associatedProxyModel() const;

    QUrl viewDefaultIcon() const;

    DataTypes::DataType dataFilter() const;

    ViewManager::ViewCanBeRated viewShowRating() const;

    ViewManager::DelegateUseSecondaryText viewDelegateDisplaySecondaryText() const;

    ViewManager::IsTreeModelType isTreeModel() const;

    int sortRole() const;

    Qt::SortOrder sortOrder() const;

    ViewManager::AlbumCardinality displaySingleAlbum() const;

    ViewManager::AlbumViewStyle showDiscHeaders() const;

    ViewManager::RadioSpecificStyle radioCase() const;

Q_SIGNALS:

private:

    std::unique_ptr<ViewConfigurationDataPrivate> d;
};

#endif // VIEWCONFIGURATIONDATA_H
