#pragma once

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsItem>

enum class MapItemState
{
    StateDefault = 0,
    StateHovered,
    StateSelected
};

class MapItemPixmap;

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

    void setPen(const QPen &pen, MapItemState state = MapItemState::StateDefault);
    void setBrush(const QBrush &brush, MapItemState state = MapItemState::StateDefault);
    void setFont(const QFont &font);
    void setColor(const QColor &color); //text color

    void setPixmap(const QPixmap &pixmap, const QSize &size, MapItemState state = MapItemState::StateDefault);
    void setText(const QString &text, const QPoint &indent = QPoint(0, 0));

    void setPath(const QPainterPath &path);
    void setRect(const QSize &size, const QSize &radius = QSize(0, 0));
    void setEllipse(const QSize &size);

    void setStaticPath(const QVector<QPointF> &points, bool close = false);
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
class MapItemPixmap : public QGraphicsPixmapItem
{
public:
    MapItemPixmap(const QPixmap &pixmap, QGraphicsItem *parent);
    ~MapItemPixmap();

    void setItemPixmap(const QPixmap &pixmap, MapItemState state = MapItemState::StateDefault);

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

    struct MapItemPixmapPrivate;
    MapItemPixmapPrivate * const d;
};

//! \brief The MapItemShape class
class MapItemPath : public QGraphicsItem
{
public:
    MapItemPath(const QPainterPath &path, QGraphicsItem *parent);
    ~MapItemPath();

    void setPen(const QPen &pen, MapItemState state = MapItemState::StateDefault);
    void setBrush(const QBrush &brush, MapItemState state = MapItemState::StateDefault);

    QRectF boundingRect() const;
    QPainterPath shape() const;

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

    struct MapItemShapePrivate;
    MapItemShapePrivate * const d;
};
