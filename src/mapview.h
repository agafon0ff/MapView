#pragma once

#include "mapitem.h"
#include "mapglobal.h"

#include <QGraphicsObject>
#include <QGraphicsView>

class MapView : public QGraphicsView
{
    Q_OBJECT

public:

    MapView(QWidget *parent = Q_NULLPTR);
    ~MapView();

    void setProvider(const QString &provider);
    QString provider();

    void addProvider(const QString &name, const Provider &provider);
    void removeProvider(const QString &name);

    void setCachePath(const QString &path);

    void setZoom(int value);
    int zoom() const;

    void setCenterOn(const QPointF &coords); // QPointF(longitude, latitude)
    QPointF center();

    MapItem *createItem();
    void removeItem(MapItem *item);
    void clearMap();

signals:
    void zoomChanged(int zoom);
    void scaleFactorChanged(qreal factor);
    void cursorCoords(const QPointF &point);
    void pressCoords(const QPointF &point, bool pressed, Qt::MouseButton btn);
    void clickCoords(const QPointF &point, Qt::MouseButton btn);

private:

    void showEvent(QShowEvent *e);
    void resizeEvent(QResizeEvent *e);
    void wheelEvent(QWheelEvent *e);
    void drawBackground(QPainter *painter, const QRectF &r);

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void calculateMapGeometry();

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
    void updateTiles();

signals:
    void tileRequest(const QPoint &pos);

private:

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

    struct MapObjectPrivate;
    MapObjectPrivate * const d;
};
