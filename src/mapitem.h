#pragma once

#include <QGraphicsPixmapItem>
#include <QGraphicsItem>

class MapItemPixmapD;

class MapItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit MapItem(QGraphicsItem *parent = Q_NULLPTR);
    ~MapItem();

    void move(const QPointF &coords); // QPointF(longitude, latitude)
    QPointF coords();
    void rotate(qreal angle);

    void setSelectable(bool state);
    void setMovable(bool state);

    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setFont(const QFont &font);
    void setColor(const QColor &color); // text color

    void setRect(const QSize &size, const QSize &radius = QSize(0, 0));
    void setEllipse(const QSize &size);
    void setPath(const QPainterPath &path);
    void setPixmap(const QPixmap &pixmap, const QSize &size);
    void setText(const QString &text, const QPoint &indent = QPoint(0, 0));

    void setStaticPath(const QVector<QPointF> &points, bool close);
    void setStaticRect(const QPointF &boundLeftTop, const QPointF &boundRightBottom);
    void setStaticEllipse(const QPointF &boundLeftTop, const QPointF &boundRightBottom);

    bool isStatic();

    QRectF boundingRect() const;
    QPainterPath shape() const;

signals:

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
    void checkScale();

    struct MapItemPrivate;
    MapItemPrivate * const d;
};

//! \brief The MapItemPixmap class
class MapItemPixmapD : public QGraphicsPixmapItem
{
public:
    MapItemPixmapD(const QPixmap &pixmap, QGraphicsItem *parent):
        QGraphicsPixmapItem(pixmap, parent){}
private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
};

//! \brief The MapItemShape class
class MapItemPath : public QGraphicsItem
{
public:
    MapItemPath(const QPainterPath &path, QGraphicsItem *parent);

    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);

    QRectF boundingRect() const;
    QPainterPath shape() const;

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

    struct MapItemShapePrivate;
    MapItemShapePrivate * const d;
};
