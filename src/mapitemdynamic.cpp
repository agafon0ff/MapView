#include "mapitemdynamic.h"
#include "mapglobal.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include "QBitmap"
#include "QRegion"
#include <QDebug>

struct MapItemDynamic::MapItemDynamicPrivate
{
    enum ItemType
    {
        TypeRect,
        TypeEllipse,
        TypePixmap
    };

    MapGlobal &settings = MapGlobal::instance();

    QPointF pos = QPointF(0, 0);
    QSizeF size = QSizeF(20, 20);
    QRectF rect = QRectF(0, 0, size.width(), size.height());
    ItemType type = TypeRect;

    QPen pen = QPen(QBrush(QColor(Qt::black)), 1);
    QBrush brush = QBrush(QColor(0, 0, 0, 0));

    bool isInMove = false;
    qreal radius = 0;
    qreal angle = 0;
    QPixmap pixmap;
    QPointF indent = QPointF(0, 0);
    QGraphicsSimpleTextItem *textItem = Q_NULLPTR;
    QGraphicsPixmapItem *pixmapItem = Q_NULLPTR;
};

MapItemDynamic::MapItemDynamic(QGraphicsItem *parent) : QGraphicsObject(parent),
    d(new MapItemDynamicPrivate)
{
    setZValue(1);
    setCacheMode(DeviceCoordinateCache);
    setAcceptHoverEvents(true);
}

MapItemDynamic::~MapItemDynamic()
{
    if (d->textItem)
        delete d->textItem;

    delete d;
}

void MapItemDynamic::move(const QPointF &coords)
{
    d->pos = d->settings.toPoint(coords);
    setPos(d->pos.x(), d->pos.y());
    updateSizes();
}

QPointF MapItemDynamic::coords()
{
    return d->settings.toCoords(d->pos);
}

void MapItemDynamic::rotate(qreal angle)
{
    prepareGeometryChange();
    d->angle = angle;
    update(d->rect);
}

QFont MapItemDynamic::font()
{
    if(d->textItem)
        return d->textItem->font();

    return QFont();
}

bool MapItemDynamic::isInMove()
{
    return d->isInMove;
}

void MapItemDynamic::setPen(const QPen &pen)
{
    d->pen = pen;
    update(d->rect);
}

void MapItemDynamic::setBrush(const QBrush &brush)
{
    d->brush = brush;
    update(d->rect);
}

void MapItemDynamic::setText(const QString &text, const QPoint &indent)
{
    d->indent = static_cast<QPointF>(indent);

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

void MapItemDynamic::drawRect(const QSize &size, int radius)
{
    d->type = MapItemDynamicPrivate::TypeRect;
    d->radius = static_cast<qreal>(radius);
    d->size = static_cast<QSizeF>(size);
    updateSizes();
}

void MapItemDynamic::drawEllipse(const QSize &size)
{
    d->type = MapItemDynamicPrivate::TypeEllipse;
    d->size = static_cast<QSizeF>(size);
    updateSizes();
}

void MapItemDynamic::drawPixmap(const QPixmap &pixmap, const QSize &size)
{
    d->type = MapItemDynamicPrivate::TypePixmap;
    d->pixmap = pixmap;
    d->size = static_cast<QSizeF>(size);

    d->pixmapItem = new QGraphicsPixmapItem(d->pixmap, this);

    updateSizes();
}

void MapItemDynamic::updateSizes()
{
    prepareGeometryChange();

    const qreal width = d->size.width() * d->settings.factor();
    const qreal height = d->size.height() * d->settings.factor();

    d->rect = QRectF(-width / 2, -height / 2, width, height);

    if (d->textItem)
    {
        QFont font = d->textItem->font();
        font.setPixelSize(14.);
        font.setWeight(QFont::Medium);
        d->textItem->setFont(font);
        d->textItem->setBrush(QBrush(d->pen.color()));

        d->textItem->setScale(d->settings.factor());
        QRectF rect = d->textItem->boundingRect();

        d->textItem->setPos(d->rect.center().x() -                      // ******
                            rect.center().x() * d->settings.factor() +  // x
                            d->indent.x() * d->settings.factor(),       // ******
                            d->rect.center().y() -                      // ******
                            rect.center().y() * d->settings.factor() +  // y
                            d->indent.y() * d->settings.factor());      // ******
    }

    if (d->pixmapItem)
    {
        d->pixmapItem->setScale(d->settings.factor());
        d->pixmapItem->setPos(pos());
    }

    update(d->rect);
}

QRectF MapItemDynamic::boundingRect() const
{
    if (d->angle == 0) return d->rect;

    QTransform t;
    t.translate(d->rect.center().x(), d->rect.center().y());
    t.rotate(d->angle);

    return QRectF(t.mapRect(d->rect));
}

QPainterPath MapItemDynamic::shape() const
{
    QPainterPath path;
    path.addRect(d->rect);
    if (d->angle == 0) return path;

    QTransform t;
    t.translate(d->rect.center().x(), d->rect.center().y());
    t.rotate(d->angle);
    path = t.map(path);

    return path;
}

void MapItemDynamic::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    Q_UNUSED(item);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen = d->pen;
    pen.setWidthF(d->pen.widthF() * d->settings.factor());

    painter->setPen(pen);
    painter->setBrush(d->brush);

    if (d->angle != 0)
    {
        painter->translate(d->rect.center());
        painter->rotate(d->angle);
    }

    if (d->type == MapItemDynamicPrivate::TypeRect)
    {
        const qreal radius = d->radius * d->settings.factor();
        painter->drawRoundedRect(d->rect, radius, radius);
    }
    else if (d->type == MapItemDynamicPrivate::TypeEllipse)
    {
        painter->drawEllipse(d->rect);
    }
    else if (d->type == MapItemDynamicPrivate::TypePixmap)
    {
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        painter->drawPixmap(d->rect, d->pixmap, d->pixmap.rect());
    }
}

QVariant MapItemDynamic::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged)
    {
        const qreal width = d->size.width() * d->settings.factor();
        const qreal height = d->size.height() * d->settings.factor();

        d->pos = QPointF(x() + width / 2, y() + height / 2);
    }
    else if (change == ItemSelectedHasChanged)
    {
        uint state = value.toUInt();

        if (state == 1) emit selected();
        else emit unselected();
    }

    return QGraphicsItem::itemChange(change, value);
}

void MapItemDynamic::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsItem::mousePressEvent(e);
    emit pressed();
}

void MapItemDynamic::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsItem::mouseMoveEvent(e);

    if (e->buttons() == Qt::LeftButton)
        d->isInMove = true;

    emit moved();
}

void MapItemDynamic::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsItem::mouseReleaseEvent(e);
    emit released();

    if (d->isInMove)
        emit movedTo(d->settings.toCoords(pos()));

    d->isInMove = false;
}
