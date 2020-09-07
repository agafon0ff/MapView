#include "mapitem.h"
#include "mapglobal.h"

#include <QStyleOptionGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QBitmap>
#include <QStyle>
#include <QDebug>

struct MapItem::MapItemPrivate
{
    MapGlobal &settings = MapGlobal::instance();
    MapItemPath *itemPath = Q_NULLPTR;
    MapItemPixmap *itemPixmap = Q_NULLPTR;
    QGraphicsSimpleTextItem *itemText = Q_NULLPTR;

    bool isSelectable = false;
    bool isMovable    = false;
    bool isStatic     = false;

    QFont   font = QFont("mono", 12, QFont::Medium);
    QColor  color = QColor(Qt::black);

    QMap<MapItemState, QPen>   pens;
    QMap<MapItemState, QBrush> brushes;

    QPainterPath path;
};

MapItem::MapItem(QGraphicsItem *parent) : QGraphicsObject(parent),
    d(new MapItemPrivate)
{
    d->pens = { {MapItemState::StateDefault,    QPen(QBrush(QColor(Qt::black)), 1)},
                {MapItemState::StateHovered,    QPen(QBrush(QColor(Qt::black)), 1)},
                {MapItemState::StateSelected,   QPen(QBrush(QColor(Qt::black)), 1)} };

    d->brushes = { {MapItemState::StateDefault,     QBrush(QColor(0, 0, 0, 0))},
                   {MapItemState::StateHovered,     QBrush(QColor(0, 0, 0, 0))},
                   {MapItemState::StateSelected,    QBrush(QColor(0, 0, 0, 0))} };

    setZValue(1);
    setAcceptHoverEvents(true);
    setCacheMode(DeviceCoordinateCache);
}

MapItem::~MapItem()
{
    if(d->itemPixmap)
        delete d->itemPixmap;

    if (d->itemText)
        delete d->itemText;

    delete d;
}

void MapItem::move(const QPointF &coords)
{
    setPos(d->settings.toPoint(coords));
}

QPointF MapItem::coords()
{
    return d->settings.toCoords(pos());
}

void MapItem::rotate(qreal angle)
{
    if (d->itemPixmap)
        d->itemPixmap->setRotation(angle);
}

void MapItem::setSelectable(bool state)
{
    d->isSelectable = state;
    setFlag(QGraphicsItem::ItemIsSelectable, state);

    if (d->itemPixmap)
        d->itemPixmap->setFlag(QGraphicsItem::ItemIsSelectable, state);

    if (d->itemPath)
        d->itemPath->setFlag(QGraphicsItem::ItemIsSelectable, state);
}

void MapItem::setMovable(bool state)
{
    d->isMovable = state;
    setFlag(QGraphicsItem::ItemIsMovable, state);

    if (d->itemPixmap)
        d->itemPixmap->setFlag(QGraphicsItem::ItemIsMovable, state);

    if (d->itemPath)
        d->itemPath->setFlag(QGraphicsItem::ItemIsMovable, state);
}

void MapItem::setPen(const QPen &pen, MapItemState state)
{
    d->pens[state] = pen;

    if (d->itemPath)
        d->itemPath->setPen(pen, state);
}

void MapItem::setBrush(const QBrush &brush, MapItemState state)
{
    d->brushes[state] = brush;

    if (d->itemPath)
        d->itemPath->setBrush(brush ,state);
}

void MapItem::setFont(const QFont &font)
{
    d->font = font;

    if(d->itemText)
    {
        d->itemText->setFont(font);
        d->itemText->update(d->itemText->boundingRect());
    }
}

void MapItem::setColor(const QColor &color)
{
    d->color = color;

    if(d->itemText)
        d->itemText->setBrush(QBrush(color));
}

void MapItem::setPixmap(const QPixmap &pixmap, const QSize &size, MapItemState state)
{
    QPixmap pix(size);
    pix.fill(QColor(0, 0, 0, 0));

    QPainter painter(&pix);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(pix.rect(), pixmap, pixmap.rect());
    painter.end();

    if (!d->itemPixmap)
    {
        d->itemPixmap = new MapItemPixmap(pix, this);
        d->itemPixmap->setItemPixmap(pix, state);
        d->itemPixmap->setFlag(QGraphicsItem::ItemIsSelectable, d->isSelectable);
        d->itemPixmap->setFlag(QGraphicsItem::ItemIsMovable, d->isMovable);
        d->itemPixmap->setTransformOriginPoint(d->itemPixmap->boundingRect().center());
        d->itemPixmap->setTransformationMode(Qt::SmoothTransformation);
        d->itemPixmap->setPos(-d->itemPixmap->boundingRect().center().x(),
                              -d->itemPixmap->boundingRect().center().y());
    }
    else d->itemPixmap->setItemPixmap(pix, state);
}

void MapItem::setText(const QString &text, const QPoint &indent)
{
    if (!d->itemText)
    {
        d->itemText = new QGraphicsSimpleTextItem(text, this);
        d->itemText->setFont(d->font);
        d->itemText->setBrush(QBrush(d->color));
        d->itemText->setTransformOriginPoint(d->itemText->boundingRect().center());
    }
    else d->itemText->setText(text);

    QPointF pathIndent(0., 0.);
    if (!d->path.isEmpty())
        pathIndent = d->path.boundingRect().center();

    d->itemText->setPos(-d->itemText->boundingRect().center()
                        + static_cast<QPointF>(indent) + pathIndent);
}

void MapItem::setPath(const QPainterPath &path)
{
    if (d->itemPath) return;

    d->itemPath = new MapItemPath(path, this);
    d->itemPath->setFlag(QGraphicsItem::ItemIsSelectable, d->isSelectable);
    d->itemPath->setFlag(QGraphicsItem::ItemIsMovable, d->isMovable);
    d->itemPath->setTransformOriginPoint(d->itemPath->boundingRect().center());
    d->itemPath->setPos(-d->itemPath->boundingRect().center().x(),
                        -d->itemPath->boundingRect().center().y());
}

void MapItem::setRect(const QSize &size, const QSize &radius)
{
    QPainterPath path;
    path.addRoundedRect(0, 0, size.width(), size.height(),
                        radius.width(), radius.height());
    setPath(path);
}

void MapItem::setEllipse(const QSize &size)
{
    QPainterPath path;
    path.addEllipse(QRectF(-size.width() / 2, -size.height() / 2,
                           size.width(), size.height()));
    setPath(path);
}

void MapItem::setStaticPath(const QVector<QPointF> &points, bool close)
{
    prepareGeometryChange();
    d->isStatic = true;

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

    update(d->path.boundingRect());
}

void MapItem::setStaticRect(const QPointF &boundLeftTop, const QPointF &boundRightBottom)
{
    prepareGeometryChange();
    d->isStatic = true;

    QRectF rect = QRectF(d->settings.toPoint(boundLeftTop),
                         d->settings.toPoint(boundRightBottom));

#if QT_VERSION >= 0x051300
    d->path.clear();
#else
    d->path = QPainterPath();
#endif

    d->path.addRect(rect);
    update(d->path.boundingRect());
}

void MapItem::setStaticEllipse(const QPointF &boundLeftTop, const QPointF &boundRightBottom)
{
    prepareGeometryChange();
    d->isStatic = true;

    QRectF rect = QRectF(d->settings.toPoint(boundLeftTop),
                         d->settings.toPoint(boundRightBottom));

#if QT_VERSION >= 0x051300
    d->path.clear();
#else
    d->path = QPainterPath();
#endif

    d->path.addEllipse(rect);
    update(d->path.boundingRect());
}

bool MapItem::isStatic()
{
    return d->isStatic;
}

QRectF MapItem::boundingRect() const
{
    if (!d->path.isEmpty())
    {
        qreal penWidth = d->pens[MapItemState::StateDefault].widthF();
        return d->path.boundingRect().adjusted(-penWidth, -penWidth,
                                               penWidth, penWidth);
    }
    else if (d->itemPath) return d->itemPath->boundingRect();
    else if (d->itemPixmap) return d->itemPixmap->boundingRect();

    return QRectF();
}

QPainterPath MapItem::shape() const
{
    if (!d->path.isEmpty() && d->isSelectable)
        return d->path;
    else return QPainterPath();
}

void MapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    Q_UNUSED(item);
    Q_UNUSED(widget);

    checkScale();

    if (d->path.isEmpty()) return;

    bool isHovered = item->state & QStyle::State_MouseOver;
    bool isSelected = item->state & QStyle::State_Selected;

    MapItemState state = MapItemState::StateDefault;
    if (isSelected) state = MapItemState::StateSelected;
    else if(isHovered) state = MapItemState::StateHovered;

    QPen pen = d->pens[state];
    pen.setWidthF(pen.widthF() * d->settings.factor());

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(pen);
    painter->setBrush(d->brushes[state]);
    painter->drawPath(d->path);
}

void MapItem::checkScale()
{
    if (d->path.isEmpty())
    {
        if (scale() != d->settings.factor())
            setScale(d->settings.factor());
    }
    else
    {
        if(d->itemText)
        {
            if (d->itemText->scale() != d->settings.factor())
                d->itemText->setScale(d->settings.factor());
        }
    }
}

//! \brief The MapItemPixmap class
struct MapItemPixmap::MapItemPixmapPrivate
{
    QMap<MapItemState, QPixmap> pixmaps;
};

MapItemPixmap::MapItemPixmap(const QPixmap &pixmap, QGraphicsItem *parent) :
    QGraphicsPixmapItem(pixmap, parent), d(new MapItemPixmapPrivate)
{
    setAcceptHoverEvents(true);

    d->pixmaps = { { MapItemState::StateDefault, pixmap },
                   { MapItemState::StateHovered, QPixmap() },
                   { MapItemState::StateSelected, QPixmap() } };
}

MapItemPixmap::~MapItemPixmap()
{

}

void MapItemPixmap::setItemPixmap(const QPixmap &pixmap, MapItemState state)
{
    d->pixmaps[state] = pixmap;
}

void MapItemPixmap::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    QStyleOptionGraphicsItem option(*item);
    option.state &= ~QStyle::State_Selected;

    QGraphicsPixmapItem::paint(painter, &option, widget);

    bool isHovered = item->state & QStyle::State_MouseOver;
    bool isSelected = item->state & QStyle::State_Selected;

    if(parentItem()->isSelected() != isSelected)
        parentItem()->setSelected(isSelected);

    if(isSelected && !d->pixmaps.value(MapItemState::StateSelected).isNull())
        setPixmap(d->pixmaps.value(MapItemState::StateSelected));
    else if(isHovered && !d->pixmaps.value(MapItemState::StateHovered).isNull())
        setPixmap(d->pixmaps.value(MapItemState::StateHovered));
    else setPixmap(d->pixmaps.value(MapItemState::StateDefault));
}

//! \brief The MapItemPath class
struct MapItemPath::MapItemShapePrivate
{
    QPainterPath path;
    QMap<MapItemState, QPen> pens;
    QMap<MapItemState, QBrush> brushes;
};

MapItemPath::MapItemPath(const QPainterPath &path, QGraphicsItem *parent):
    QGraphicsItem(parent), d(new MapItemShapePrivate)
{
    setAcceptHoverEvents(true);

    d->path = path;
    d->pens = { {MapItemState::StateDefault,    QPen(QBrush(QColor(Qt::black)), 1)},
                {MapItemState::StateHovered,    QPen(QBrush(QColor(Qt::black)), 1)},
                {MapItemState::StateSelected,   QPen(QBrush(QColor(Qt::black)), 1)} };

    d->brushes = { {MapItemState::StateDefault,     QBrush(QColor(0, 0, 0, 0))},
                   {MapItemState::StateHovered,     QBrush(QColor(0, 0, 0, 0))},
                   {MapItemState::StateSelected,    QBrush(QColor(0, 0, 0, 0))} };
}

MapItemPath::~MapItemPath()
{
    delete d;
}

void MapItemPath::setPen(const QPen &pen, MapItemState state)
{
    d->pens[state] = pen;
    update(boundingRect());
}

void MapItemPath::setBrush(const QBrush &brush, MapItemState state)
{
    d->brushes[state] = brush;
    update(boundingRect());
}

QRectF MapItemPath::boundingRect() const
{
    return d->path.boundingRect();
}

QPainterPath MapItemPath::shape() const
{
    return d->path;
}

void MapItemPath::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    Q_UNUSED(widget);

    QStyleOptionGraphicsItem option(*item);
    option.state &= ~QStyle::State_Selected;

    bool isHovered = item->state & QStyle::State_MouseOver;
    bool isSelected = item->state & QStyle::State_Selected;

    if (parentItem()->isSelected() != isSelected)
        parentItem()->setSelected(isSelected);

    MapItemState state = MapItemState::StateDefault;
    if (isSelected) state = MapItemState::StateSelected;
    else if(isHovered) state = MapItemState::StateHovered;

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(d->brushes.value(state));
    painter->setPen(d->pens.value(state));
    painter->drawPath(d->path);
}
