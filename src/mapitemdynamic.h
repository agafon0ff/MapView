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

    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setText(const QString &text, const QPoint &indent = QPoint(0, 0));

    void drawRect(const QSize &rect, int radius = 0);
    void drawEllipse(const QSize &size);
    void drawPixmap(const QPixmap &pixmap, const QSize &size);

    void updateSizes();

    QRectF boundingRect() const;
    QPainterPath shape() const;

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    struct MapItemDynamicPrivate;
    MapItemDynamicPrivate * const d;
};

