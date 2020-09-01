#pragma once

#include <QGraphicsItem>

class MapItemDynamic : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit MapItemDynamic(QGraphicsItem *parent = Q_NULLPTR);
    ~MapItemDynamic();

    void move(const QPointF &coords); // QPointF(longitude, latitude)
    QPointF coords();
    void rotate(qreal angle);
    QFont font();
    bool isInMove();

    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setText(const QString &text, const QPoint &indent = QPoint(0, 0));

    void drawRect(const QSize &rect, int radius = 0);
    void drawEllipse(const QSize &size);
    void drawPixmap(const QPixmap &pixmap, const QSize &size);

    void updateSizes();

    QRectF boundingRect() const;
    QPainterPath shape() const;

signals:
    void selected();
    void unselected();
    void pressed();
    void moved();
    void released();
    void movedTo(const QPointF &coords);

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

    struct MapItemDynamicPrivate;
    MapItemDynamicPrivate * const d;
};

