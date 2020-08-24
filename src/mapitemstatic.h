#pragma once

#include <QGraphicsItem>

class MapItemStatic : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit MapItemStatic(QGraphicsItem *parent = Q_NULLPTR);
    ~MapItemStatic();

public:
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setText(const QString &text);

    void drawPath(const QVector<QPointF> &points, bool close = false);
    void drawRect(const QPointF &boundLeftTop, const QPointF &boundRightBottom);
    void drawEllipse(const QPointF &boundLeftTop, const QPointF &boundRightBottom);

    void updateSizes();

    QRectF boundingRect() const;
    QPainterPath shape() const;

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

    struct MapItemStaticPrivate;
    MapItemStaticPrivate * const d;
};
