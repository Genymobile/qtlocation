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

#ifndef QDECLARATIVEGEOMAPITEMBASE_H
#define QDECLARATIVEGEOMAPITEMBASE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtLocation/private/qlocationglobal_p.h>

#include <QtQuick/QQuickItem>
#include <QtPositioning/QGeoShape>

#include <QtLocation/private/qdeclarativegeomap_p.h>
#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qgeomap_p.h>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QGeoMapViewportChangeEvent
{
public:
    explicit QGeoMapViewportChangeEvent();
    QGeoMapViewportChangeEvent(const QGeoMapViewportChangeEvent &other);
    QGeoMapViewportChangeEvent &operator=(const QGeoMapViewportChangeEvent &other);

    QGeoCameraData cameraData;
    QSizeF mapSize;

    bool zoomLevelChanged;
    bool centerChanged;
    bool mapSizeChanged;
    bool tiltChanged;
    bool bearingChanged;
    bool rollChanged;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeGeoMapItemBase : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QGeoShape geoShape READ geoShape STORED false )
public:
    explicit QDeclarativeGeoMapItemBase(QQuickItem *parent = 0);
    virtual ~QDeclarativeGeoMapItemBase();

    virtual void setMap(QDeclarativeGeoMap *quickMap, QGeoMap *map);
    virtual void setPositionOnMap(const QGeoCoordinate &coordinate, const QPointF &offset);

    QDeclarativeGeoMap *quickMap() { return quickMap_; }
    QGeoMap *map() { return map_; }
    virtual const QGeoShape &geoShape() const = 0;

    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    virtual QSGNode *updateMapItemPaintNode(QSGNode *, UpdatePaintNodeData *);

    virtual QGeoMap::ItemType itemType() const = 0;

    // Data-related bool. Used by QGeoMaps that render the item directly.
    bool isDirty() const;
    bool isGeoMaterialDirty() const;
    bool isGeoGeometryDirty() const;
    void markClean();

protected Q_SLOTS:
    virtual void afterChildrenChanged();
    virtual void afterViewportChanged(const QGeoMapViewportChangeEvent &event) = 0;
    void polishAndUpdate();
    inline void markGeoMaterialDirty()
    {
        geoMaterialDirty_ = true;
    }

    inline void markGeoGeometryDirty()
    {
        geoGeometryDirty_ = true;
    }

protected:
    float zoomLevelOpacity() const;
    bool childMouseEventFilter(QQuickItem *item, QEvent *event);
    bool isPolishScheduled() const;

private Q_SLOTS:
    void baseCameraDataChanged(const QGeoCameraData &camera);

protected:
    // For consumption by QGeoMaps that are capable of drawing items
    bool geoGeometryDirty_;
    bool geoMaterialDirty_;

private:
    QGeoMap *map_;
    QDeclarativeGeoMap *quickMap_;

    QSizeF lastSize_;
    QGeoCameraData lastCameraData_;

    friend class QDeclarativeGeoMap;
};

QT_END_NAMESPACE

#endif
