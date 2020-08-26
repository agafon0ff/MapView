#include "mapitemstatic.h"
#include "mapglobal.h"

#include <QPainter>
#include <QDebug>

struct MapItemStatic::MapItemStaticPrivate
{
    MapGlobal &settings = MapGlobal::instance();

    QRectF rect = QRectF(0, 0, 20, 20);

    QPen pen = QPen(QBrush(QColor(Qt::black)), 1);
    QBrush brush = QBrush(QColor(0, 0, 0, 0));

    QVector<QPointF> pathPoints;
    QPainterPath path;

    QGraphicsSimpleTextItem *textItem = Q_NULLPTR;
};

MapItemStatic::MapItemStatic(QGraphicsItem *parent) : QGraphicsObject(parent),
    d(new MapItemStaticPrivate)
{
    setZValue(1);
    setCacheMode(DeviceCoordinateCache);
    setAcceptHoverEvents(true);
}

MapItemStatic::~MapItemStatic()
{
    if (d->textItem)
        delete d->textItem;

    delete d;
}

void MapItemStatic::setPen(const QPen &pen)
{
    d->pen = pen;
    update();
}

void MapItemStatic::setBrush(const QBrush &brush)
{
    d->brush = brush;
    update();
}

void MapItemStatic::setText(const QString &text)
{
    if (text.isEmpty())
    {
        if (d->textItem)
        {
            delete d->textItem;
            d->textItem = Q_NULLPTR;
        }
    }
    else
    {
        if (d->textItem)
            d->textItem->setText(text);
        else d->textItem = new QGraphicsSimpleTextItem(text, this);
    }

    updateSizes();
}

void MapItemStatic::drawPath(const QVector<QPointF> &points, bool close)
{
    prepareGeometryChange();

    d->pathPoints = points;

#if QT_VERSION >= 0x051300
    d->path.clear();
#else
    d->path = QPainterPath();
#endif

    for (int i=0; i<points.size(); ++i)
    {
        const QPointF &coords = points.at(i);
        const QPointF point = d->settings.toPoint(coords);

        if (i == 0) d->path.moveTo(point);
        else d->path.lineTo(point);
    }

    if (close) d->path.closeSubpath();

    d->rect = d->path.boundingRect();
    update();
}

void MapItemStatic::drawRect(const QPointF &boundLeftTop, const QPointF &boundRightBottom)
{
    prepareGeometryChange();

    QRectF rect = QRectF(d->settings.toPoint(boundLeftTop),
                         d->settings.toPoint(boundRightBottom));

    d->pathPoints.clear();

#if QT_VERSION >= 0x051300
    d->path.clear();
#else
    d->path = QPainterPath();
#endif

    d->path.addRect(rect);
    d->rect = d->path.boundingRect();

    update();
}

void MapItemStatic::drawEllipse(const QPointF &boundLeftTop, const QPointF &boundRightBottom)
{
    prepareGeometryChange();

    QRectF rect = QRectF(d->settings.toPoint(boundLeftTop),
                         d->settings.toPoint(boundRightBottom));

    d->pathPoints.clear();

#if QT_VERSION >= 0x051300
    d->path.clear();
#else
    d->path = QPainterPath();
#endif

    d->path.addEllipse(rect);
    d->rect = d->path.boundingRect();

    update();
}

void MapItemStatic::updateSizes()
{
    if (d->textItem)
    {
        QFont font = d->textItem->font();
        font.setPixelSize(14.);
        font.setWeight(QFont::Medium);
        d->textItem->setFont(font);
        d->textItem->setBrush(QBrush(d->pen.color()));

        const qreal factor = d->settings.factor();
        d->textItem->setScale(factor);
        QRectF boundRect = d->textItem->boundingRect();
        QRectF rect = QRect(boundRect.x() * factor, boundRect.y() * factor,
                            boundRect.width() * factor,
                            boundRect.height() * factor);

        if(rect.width() + rect.height() <
           d->rect.width() + d->rect.height())
            d->textItem->show();
        else d->textItem->hide();

        d->textItem->setPos(d->rect.center().x() - rect.center().x(),
                            d->rect.center().y() - rect.center().y());
    }
}

QRectF MapItemStatic::boundingRect() const
{
    return QRectF(d->rect);
}

QPainterPath MapItemStatic::shape() const
{
    return d->path;
}

void MapItemStatic::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    Q_UNUSED(item);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen = d->pen;
    pen.setWidthF(d->pen.widthF() * d->settings.factor());

    painter->setPen(pen);
    painter->setBrush(d->brush);

    if(!d->path.isEmpty())
        painter->drawPath(d->path);
}
