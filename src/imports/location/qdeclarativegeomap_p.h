/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVE3DGRAPHICSGEOMAP_H
#define QDECLARATIVE3DGRAPHICSGEOMAP_H

#include <QPointer>
#include <QTouchEvent>
#include <QtQuick/QQuickItem>
#include <QtCore/QMutex>

#include <QtCore/QCoreApplication>

#include <QtQuick/QSGTexture>
#include <QtQuick/QQuickPaintedItem>
#include <QtQml/QQmlParserStatus>
#include "qgeoserviceprovider.h"
#include "qdeclarativegeomapitemview_p.h"
#include "qdeclarativegeomapgesturearea_p.h"
#include "qgeomapcontroller_p.h"
#include "qdeclarativegeomapcopyrightsnotice_p.h"

//#define QT_DECLARATIVE_LOCATION_TRACE 1

#ifdef QT_DECLARATIVE_LOCATION_TRACE
#define QLOC_TRACE0 qDebug() << __FILE__ << __FUNCTION__;
#define QLOC_TRACE1(msg1) qDebug() << __FILE__ << __FUNCTION__ << msg1;
#define QLOC_TRACE2(msg1, msg2) qDebug() << __FILE__ << __FUNCTION__ << msg1 << msg2;
#else
#define QLOC_TRACE0
#define QLOC_TRACE1(msg1)
#define QLOC_TRACE2(msg1, msg2)
#endif

#include "qgeocameradata_p.h"
#include "qgeomap_p.h"
#include "qdeclarativegeomaptype_p.h"

QT_BEGIN_NAMESPACE

class QGLSceneNode;
class QGeoTileCache;
class Tile;
class QGeoTileSpec;
class QGeoMapSphere;
class QGeoMappingManager;

class QGeoCoordinate;
class QGeoMapObject;
class QGeoMapData;
class QGeoServiceProvider;
class QDeclarativeGeoServiceProvider;
class QDeclarativeGeoMap;
class QDeclarativeGeoMapItem;
class QDeclarativeGeoMapItemBase;
class QDeclarativeGeoMapType;

class QDeclarativeGeoMap : public QQuickItem
{
    Q_OBJECT
    Q_ENUMS(QGeoServiceProvider::Error)
    Q_PROPERTY(QDeclarativeGeoMapGestureArea *gesture READ gesture CONSTANT)
    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(qreal minimumZoomLevel READ minimumZoomLevel WRITE setMinimumZoomLevel NOTIFY minimumZoomLevelChanged)
    Q_PROPERTY(qreal maximumZoomLevel READ maximumZoomLevel WRITE setMaximumZoomLevel NOTIFY maximumZoomLevelChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QDeclarativeGeoMapType *activeMapType READ activeMapType WRITE setActiveMapType NOTIFY activeMapTypeChanged)
    Q_PROPERTY(QQmlListProperty<QDeclarativeGeoMapType> supportedMapTypes READ supportedMapTypes NOTIFY supportedMapTypesChanged)
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(QList<QObject *> mapItems READ mapItems NOTIFY mapItemsChanged)
    Q_PROPERTY(QGeoServiceProvider::Error error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged)
    Q_INTERFACES(QQmlParserStatus)

public:

    explicit QDeclarativeGeoMap(QQuickItem *parent = 0);
    ~QDeclarativeGeoMap();

    // From QQmlParserStatus
    virtual void componentComplete();

    // from QQuickItem
    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider *plugin() const;

    void setActiveMapType(QDeclarativeGeoMapType *mapType);
    QDeclarativeGeoMapType *activeMapType() const;

    void setMinimumZoomLevel(qreal minimumZoomLevel);
    qreal minimumZoomLevel() const;

    void setMaximumZoomLevel(qreal maximumZoomLevel);
    qreal maximumZoomLevel() const;

    void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    QQmlListProperty<QDeclarativeGeoMapType> supportedMapTypes();

    Q_INVOKABLE void removeMapItem(QDeclarativeGeoMapItemBase *item);
    Q_INVOKABLE void addMapItem(QDeclarativeGeoMapItemBase *item);
    Q_INVOKABLE void clearMapItems();
    QList<QObject *> mapItems();

    Q_INVOKABLE QGeoCoordinate toCoordinate(const QPointF &position) const;
    Q_INVOKABLE QPointF fromCoordinate(const QGeoCoordinate &coordinate) const;

#if QT_DEPRECATED_SINCE(5,5)
    QT_DEPRECATED Q_INVOKABLE QPointF toScreenPosition(const QGeoCoordinate &coordinate) const;
#endif

    QDeclarativeGeoMapGestureArea *gesture();

    Q_INVOKABLE void fitViewportToGeoShape(const QVariant &shape);
    Q_INVOKABLE void fitViewportToMapItems();
    Q_INVOKABLE void pan(int dx, int dy);
    Q_INVOKABLE void cameraStopped(); // optional hint for prefetch

    QString errorString() const;
    QGeoServiceProvider::Error error() const;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseUngrabEvent();

    void touchEvent(QTouchEvent *event);
    void wheelEvent(QWheelEvent *event);

    bool childMouseEventFilter(QQuickItem *item, QEvent *event);

Q_SIGNALS:
    void wheelAngleChanged(QPoint angleDelta);
    void pluginChanged(QDeclarativeGeoServiceProvider *plugin);
    void zoomLevelChanged(qreal zoomLevel);
    void centerChanged(const QGeoCoordinate &coordinate);
    void activeMapTypeChanged();
    void supportedMapTypesChanged();
    void minimumZoomLevelChanged();
    void maximumZoomLevelChanged();
    void mapItemsChanged();
    void errorChanged();
    void copyrightLinkActivated(const QString &link);

private Q_SLOTS:
    void updateMapDisplay(const QRectF &target);
    void mappingManagerInitialized();
    void mapZoomLevelChanged(qreal zoom);
    void pluginReady();
    void onMapChildrenChanged();

protected:
    void setError(QGeoServiceProvider::Error error, const QString &errorString);

private:
    void setupMapView(QDeclarativeGeoMapItemView *view);
    void populateMap();
    void fitViewportToMapItemsRefine(bool refine);

    QDeclarativeGeoServiceProvider *plugin_;
    QGeoServiceProvider *serviceProvider_;
    QGeoMappingManager *mappingManager_;

    qreal zoomLevel_;
    qreal bearing_;
    qreal tilt_;
    QGeoCoordinate center_;

    QDeclarativeGeoMapType *activeMapType_;
    QList<QDeclarativeGeoMapType *> supportedMapTypes_;
    bool componentCompleted_;
    bool mappingManagerInitialized_;
    QList<QDeclarativeGeoMapItemView *> mapViews_;

    QDeclarativeGeoMapGestureArea *gestureArea_;

    int touchTimer_;

    QGeoMap *map_;
    QPointer<QDeclarativeGeoMapCopyrightNotice> copyrightsWPtr_;

    QList<QPointer<QDeclarativeGeoMapItemBase> > mapItems_;

    QMutex updateMutex_;

    QString errorString_;
    QGeoServiceProvider::Error error_;

    friend class QDeclarativeGeoMapItem;
    friend class QDeclarativeGeoMapItemView;
    friend class QDeclarativeGeoMapGestureArea;
    Q_DISABLE_COPY(QDeclarativeGeoMap)
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeGeoMap)

#endif
