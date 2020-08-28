#pragma once

#include "mapitemdynamic.h"
#include "mapitemstatic.h"
#include "mapglobal.h"

#include <QGraphicsObject>
#include <QGraphicsView>

class MapView : public QGraphicsView
{
    Q_OBJECT

public:

    MapView(QWidget *parent = Q_NULLPTR);
    ~MapView();

    void setProvider(MapProviders provider = GoogleMap);

    void setZoom(int value);
    inline int zoom() const;

    void setCenterOn(const QPointF &coords); // QPointF(longitude, latitude)

    MapItemDynamic *createDynamicItem();
    MapItemStatic *createStaticItem();

    void removeDynamicItem(MapItemDynamic *item);
    void removeStaticItem(MapItemStatic *item);

    void clearMap();

private:

    void showEvent(QShowEvent *e);
    void resizeEvent(QResizeEvent *e);
    void wheelEvent(QWheelEvent *e);
    void drawBackground(QPainter *painter, const QRectF &r);

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void calculateMapGeometry();
    void updateItemsSizes();

    struct MapViewPrivate;
    MapViewPrivate * const d;
};


/*********************** MapObject ***********************/

class MapObject : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit MapObject(QGraphicsItem *parent = Q_NULLPTR);
    ~MapObject();

public slots:
    void setTile(const QPoint &pos, const QPixmap &pix);
    void setTileWidth(qreal tileWidth);
    void setGeometry(const QRect &rect);
    void setBoundingRect(const QRectF &rect);

signals:
    void tileRequest(const QPoint &pos);

private:

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

    struct MapObjectPrivate;
    MapObjectPrivate * const d;
};
