/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QGEOTILECACHE_P_H
#define QGEOTILECACHE_P_H

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

#include <QtLocation/qlocationglobal.h>

#include <QObject>
#include <QCache>
#include "qcache3q_p.h"
#include <QSet>
#include <QMutex>
#include <QTimer>

#include "qgeotilespec_p.h"
#include "qgeotiledmappingmanagerengine_p.h"

QT_BEGIN_NAMESPACE

class QGeoMappingManager;

class QGeoTile;
class QGeoCachedTileMemory;
class QGeoTileCache;
#ifndef NO_QT3D_RENDERER
class QGLTexture2D;
#else
class QSGTexture;
class QQuickWindow;
#endif

class QPixmap;
class QThread;

/* This would be internal to qgeotilecache.cpp except that the eviction
 * policy can't be defined without it being concrete here */
class QGeoCachedTileDisk
{
public:
    ~QGeoCachedTileDisk();

    QGeoTileSpec spec;
    QString filename;
    QString format;
    QGeoTileCache *cache;
};

/* This is also used in the mapgeometry */
class Q_LOCATION_EXPORT QGeoTileTexture
{
public:

    QGeoTileTexture();
    ~QGeoTileTexture();

    QGeoTileSpec spec;
#ifndef NO_QT3D_RENDERER
    QGLTexture2D *texture;
#else
    QSGTexture *texture; //need a custom texture class here
#endif
    bool textureBound;
};

/* Custom eviction policy for the disk cache, to avoid deleting all the files
 * when the application closes */
class QCache3QTileEvictionPolicy : public QCache3QDefaultEvictionPolicy<QGeoTileSpec,QGeoCachedTileDisk>
{
protected:
    void aboutToBeRemoved(const QGeoTileSpec &key, QSharedPointer<QGeoCachedTileDisk> obj);
    void aboutToBeEvicted(const QGeoTileSpec &key, QSharedPointer<QGeoCachedTileDisk> obj);
};

class Q_LOCATION_EXPORT QGeoTileCache : public QObject
{
    Q_OBJECT
public:
    QGeoTileCache(const QString &directory = QString(), QObject *parent = 0);
    ~QGeoTileCache();

    void setMaxDiskUsage(int diskUsage);
    int maxDiskUsage() const;
    int diskUsage() const;

    void setMaxMemoryUsage(int memoryUsage);
    int maxMemoryUsage() const;
    int memoryUsage() const;

    void setMinTextureUsage(int textureUsage);
    void setExtraTextureUsage(int textureUsage);
    int maxTextureUsage() const;
    int minTextureUsage() const;
    int textureUsage() const;

#ifndef NO_QT3D_RENDERER
    void GLContextAvailable();
#else
    void setQQuickWindow(QQuickWindow *window);
#endif

    QSharedPointer<QGeoTileTexture> get(const QGeoTileSpec &spec);

    // can be called without a specific tileCache pointer
    static void evictFromDiskCache(QGeoCachedTileDisk *td);
    static void evictFromMemoryCache(QGeoCachedTileMemory *tm);
    static void evictFromTextureCache(QGeoTileTexture *tt);

    void insert(const QGeoTileSpec &spec,
                const QByteArray &bytes,
                const QString &format,
                QGeoTiledMappingManagerEngine::CacheAreas areas = QGeoTiledMappingManagerEngine::AllCaches);
    void handleError(const QGeoTileSpec &spec, const QString &errorString);

public Q_SLOTS:
    void printStats();

private:
    void loadTiles();

    QSharedPointer<QGeoCachedTileDisk> addToDiskCache(const QGeoTileSpec &spec, const QString &filename);
    QSharedPointer<QGeoCachedTileMemory> addToMemoryCache(const QGeoTileSpec &spec, const QByteArray &bytes, const QString &format);
    QSharedPointer<QGeoTileTexture> addToTextureCache(const QGeoTileSpec &spec, const QPixmap &pixmap);

    static QString tileSpecToFilename(const QGeoTileSpec &spec, const QString &format, const QString &directory);
    static QGeoTileSpec filenameToTileSpec(const QString &filename);

    QString directory_;
    QCache3Q<QGeoTileSpec, QGeoCachedTileDisk, QCache3QTileEvictionPolicy > diskCache_;
    QCache3Q<QGeoTileSpec, QGeoCachedTileMemory > memoryCache_;
    QCache3Q<QGeoTileSpec, QGeoTileTexture > textureCache_;

    int minTextureUsage_;
    int extraTextureUsage_;

    static QMutex cleanupMutex_;
#ifndef NO_QT3D_RENDERER
    static QList<QGLTexture2D*> cleanupList_;
#else
    static QList<QSGTexture*> cleanupList_;
    QQuickWindow *m_containingWindow;
#endif
};

QT_END_NAMESPACE

#endif // QGEOTILECACHE_P_H
