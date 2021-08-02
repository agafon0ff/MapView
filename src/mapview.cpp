#include "mapview.h"
#include "maploader.h"

#include <QGraphicsScene>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPainter>
#include <QDebug>
#include <QTimer>
#include <QtMath>

inline bool operator <(const QPoint &p1, const QPoint &p2)
{
    return (static_cast<qint64>(p1.x()) | (static_cast<qint64>(p1.y()) << 32)) <
           (static_cast<qint64>(p2.x()) | (static_cast<qint64>(p2.y()) << 32));
}

struct MapView::MapViewPrivate
{
    MapGlobal &settings = MapGlobal::instance();
    MapLoader *tileLoader = Q_NULLPTR;
    MapObject *map = Q_NULLPTR;

    bool        isMove = false;
    quint64     tileWidthScaled;
    QRect       indentRect;
    qreal       scale = settings.tilesCount();

    QVector<MapItem*> items;
};

MapView::MapView(QWidget *parent) : QGraphicsView(parent),
       d(new MapViewPrivate)
{
    d->tileLoader = new MapLoader(this);

    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setBackgroundBrush(QBrush(QColor(Qt::darkGray)));

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setCacheMode(QGraphicsView::CacheBackground);
    viewport()->setCursor(Qt::ArrowCursor);

    QGraphicsScene *scene = new QGraphicsScene(this);
    setScene(scene);
    setSceneRect(0., 0.,
                 d->scale * static_cast<qreal>(d->settings.tileWidth()),
                 d->scale * static_cast<qreal>(d->settings.tileWidth()));

    d->map = new MapObject;
    scene->addItem(d->map);

    connect(d->map, &MapObject::tileRequest, d->tileLoader, &MapLoader::loadTile);
    connect(d->tileLoader, &MapLoader::loaded, d->map, &MapObject::setTile);

    calculateMapGeometry();
}

MapView::~MapView()
{
    clearMap();

    delete d->tileLoader;
    delete d->map;
    delete d;
}

void MapView::setProvider(MapProviders provider)
{
    d->settings.setProvider(provider);
}

void MapView::setCachePath(const QString &path)
{
    d->settings.setCachePath(path);
}

void MapView::setZoom(int value)
{
    d->settings.setZoom(qBound(1, value, d->settings.zoomMax()));
    d->scale = qPow(2., static_cast<qreal>(d->settings.zoom())) / 2.;
    d->settings.setFactor(qPow(2., static_cast<qreal>(d->settings.zoomMax() - d->settings.zoom() + 1)) / 2.);
    d->tileLoader->update();

    QMatrix matrix;
    matrix.scale(1 / d->settings.factor(),
                 1 / d->settings.factor());

    setMatrix(matrix);
    calculateMapGeometry();

    emit zoomChanged(d->settings.zoom());
    emit scaleFactorChanged(d->settings.factor());
}

int MapView::zoom() const
{
    return d->settings.zoom();
}

void MapView::setCenterOn(const QPointF &coords)
{
    centerOn(d->settings.toPoint(coords));
    calculateMapGeometry();
}

MapItem *MapView::createItem()
{
    MapItem *item = new MapItem;
    d->items.append(item);
    scene()->addItem(item);

    return item;
}

void MapView::removeItem(MapItem *item)
{
    if (!d->items.contains(item)) return;

    d->items.removeOne(item);
    item->deleteLater();
}

void MapView::clearMap()
{
    foreach (MapItem *item, d->items)
        item->deleteLater();

    d->items.clear();
}

void MapView::showEvent(QShowEvent *e)
{
    QGraphicsView::showEvent(e);
    calculateMapGeometry();
}

void MapView::resizeEvent(QResizeEvent *e)
{
    QGraphicsView::resizeEvent(e);
    calculateMapGeometry();
}

void MapView::wheelEvent(QWheelEvent *e)
{
    Q_UNUSED(e);

    if (e->angleDelta().y() > 0)
    {
        if(d->settings.zoom() < d->settings.zoomMax())
        {
            setZoom(d->settings.zoom() + 1);
        }
    }
    else if(d->settings.zoom() > 1)
    {
        setZoom(d->settings.zoom() - 1);
    }
}

void MapView::drawBackground(QPainter *painter, const QRectF &r)
{
    QGraphicsView::drawBackground(painter, r);

    painter->setPen(QPen(QColor(Qt::black), 2 * d->settings.factor()));
    painter->setBrush(QBrush(QColor(Qt::lightGray)));
    painter->drawRect(sceneRect());
}

void MapView::mousePressEvent(QMouseEvent *e)
{
    QGraphicsView::mousePressEvent(e);
}

void MapView::mouseMoveEvent(QMouseEvent *e)
{
    QGraphicsView::mouseMoveEvent(e);

    if (e->buttons() == Qt::LeftButton)
    {
        d->isMove = true;
        calculateMapGeometry();
    }
    else
    {
        emit cursorCoords(d->settings.toCoords(mapToScene(e->pos())));
    }
}

void MapView::mouseReleaseEvent(QMouseEvent *e)
{
    QGraphicsView::mouseReleaseEvent(e);
    viewport()->setCursor(Qt::ArrowCursor);

    emit clickCoords(d->settings.toCoords(mapToScene(e->pos())));

    d->isMove = false;
}

void MapView::calculateMapGeometry()
{
    QRectF visibleRect = QRectF(mapToScene(0, 0), mapToScene(width(), height()));
    qreal tileWidth = static_cast<qreal>(d->settings.tileWidth()) * d->settings.factor();
    d->map->setBoundingRect(visibleRect);

    QRect mapRect;

    if(visibleRect.width() > sceneRect().width() ||
       visibleRect.height() > sceneRect().height())
    {
        mapRect = QRect(0, 0, static_cast<int>(sceneRect().width() / tileWidth),
                        static_cast<int>(sceneRect().height() / tileWidth));
    }
    else
    {
        mapRect = QRect(static_cast<quint64>(visibleRect.x()) / tileWidth,
                        static_cast<quint64>(visibleRect.y()) / tileWidth,
                        visibleRect.width() / tileWidth + 2,
                        visibleRect.height() / tileWidth + 2);

    }

    if(d->tileWidthScaled != tileWidth)
    {
        d->tileWidthScaled = tileWidth;
        d->map->setTileWidth(tileWidth);
        d->map->setGeometry(mapRect);
    }
    else if(d->indentRect != mapRect)
    {
        d->indentRect = mapRect;
        d->map->setGeometry(mapRect);
    }
}

/*********************** MapObject ***********************/
struct MapObject::MapObjectPrivate
{
    QRectF rect;
    qreal tileWidth;
    QMap<QPoint, QPixmap> tiles;
};

MapObject::MapObject(QGraphicsItem *parent) : QGraphicsObject(parent),
    d(new MapObjectPrivate)
{
    setCacheMode(DeviceCoordinateCache);
}

MapObject::~MapObject()
{
    delete d;
}

void MapObject::setTile(const QPoint &pos, const QPixmap &pix)
{
    if (!d->tiles.contains(pos)) return;

    d->tiles[pos] = pix;
    update();
}

void MapObject::setTileWidth(qreal tileWidth)
{
    d->tileWidth = static_cast<qreal>(tileWidth);
    d->tiles.clear();
}

void MapObject::setGeometry(const QRect &rect)
{
    for (int i=rect.x(); i<rect.width() + rect.x(); ++i)
    {
        for (int j=rect.y(); j<rect.height() + rect.y(); ++j)
        {
            QPoint pos(i, j);
            if (!d->tiles.contains(pos))
            {
                QPixmap pix(256, 256);
                pix.fill();

                d->tiles.insert(pos, pix);
                emit tileRequest(pos);
            }
        }
    }

    update();
}

void MapObject::setBoundingRect(const QRectF &rect)
{
    prepareGeometryChange();
    d->rect = rect;
    update();
}

QRectF MapObject::boundingRect() const
{
    return QRectF(d->rect);
}

void MapObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    Q_UNUSED(item);
    Q_UNUSED(widget);

    painter->setPen(QPen(Qt::NoPen));

    QMapIterator<QPoint, QPixmap> it(d->tiles);
    while (it.hasNext())
    {
        it.next();
        const QRectF rect(static_cast<qreal>(it.key().x()) * d->tileWidth,
                          static_cast<qreal>(it.key().y()) * d->tileWidth,
                          d->tileWidth, d->tileWidth);

        const QPixmap pix = d->tiles.value(it.key());
        painter->drawPixmap(rect, pix, pix.rect());
        painter->drawRect(rect);
    }
}
