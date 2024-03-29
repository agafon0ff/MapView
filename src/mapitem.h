#pragma once

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsItem>

enum class MapItemState
{
    Default = 0,
    Hovered,
    Selected,
    AllState
};

enum class MapItemType
{
    DynamicItem = 0,
    StaticPath,
    StaticRect,
    StaticEllipse
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
    void setSelected(bool state);

    void setMovable(bool state);

    void setPen(const QPen &pen, MapItemState state = MapItemState::Default);
    void setBrush(const QBrush &brush, MapItemState state = MapItemState::Default);
    void setFont(const QFont &font);
    void setColor(const QColor &color); //text color
    void setMaskBrush(const QBrush &brush, MapItemState state = MapItemState::Selected); // if used as pixmap item

    void setPixmap(const QPixmap &pixmap, const QSize &size, MapItemState state = MapItemState::Default);
    void setText(const QString &text, const QPoint &indent = QPoint(0, 0));

    void setPath(const QPainterPath &path);
    void setRect(const QSize &size, const QSize &radius = QSize(0, 0));
    void setEllipse(const QSize &size);

    void setStaticPath(const QVector<QPointF> &points, bool close = false);
    void setStaticRect(const QPointF &boundLeftTop, const QPointF &boundRightBottom);
    void setStaticEllipse(const QPointF &boundLeftTop, const QPointF &boundRightBottom);

    QVector<QPointF> coordsList();

    bool isStatic();
    bool isInMove();
    QFont font();

    void updateCoords();

    QRectF boundingRect() const;
    QPainterPath shape() const;

signals:
    void selected(bool state);
    void hovered(bool state);
    void pressed(bool state, Qt::MouseButton button);
    void moved(const QPointF &coords);
    void movedTo(const QPointF &coords);

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void checkScale();

    void onPressEvent(bool state, Qt::MouseButton button);
    void onSelectEvent(bool state);
    void onHoverEvent(bool state);

    friend class MapItemPixmap;
    friend class MapItemPath;

    struct MapItemPrivate;
    MapItemPrivate * const d;
};

//! \brief The MapItemPixmap class
class MapItemPixmap : public QGraphicsPixmapItem
{
public:
    MapItemPixmap(const QPixmap &pixmap, QGraphicsItem *parent);
    ~MapItemPixmap();

    void setItemPixmap(const QPixmap &pixmap, MapItemState state = MapItemState::Default);
    void setMaskBrush(const QBrush &brush, MapItemState state = MapItemState::Default);

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    struct MapItemPixmapPrivate;
    MapItemPixmapPrivate * const d;
};

//! \brief The MapItemPath class
class MapItemPath : public QGraphicsItem
{
public:
    MapItemPath(const QPainterPath &path, QGraphicsItem *parent);
    ~MapItemPath();

    void setPen(const QPen &pen, MapItemState state = MapItemState::Default);
    void setBrush(const QBrush &brush, MapItemState state = MapItemState::Default);

    QRectF boundingRect() const;
    QPainterPath shape() const;

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    struct MapItemShapePrivate;
    MapItemShapePrivate * const d;
};
