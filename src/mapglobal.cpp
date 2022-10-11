#include "mapglobal.h"
#include <QtMath>
#include <QtCore>

static const float EARTH_RADIUS_SK42 = 6371109.0;   // усредненный радиус земли SK42
static const float EARTH_RADIUS_WGS84 = 6378137.0;  // усредненный радиус земли WGS84
static const double MERCATOR_QUARTER = 20037508.3427892430765884088807;  //2*PI*6378137/(2^z)

struct MapGlobal::MapGlobalPrivate
{
    MapProviders provider = OsmMap;
    QString cachePath;
    QString cacheSuffix = "/map";
    int zoomMax = 23;
    int zoom = zoomMax;
    int tileWidth = 256;
    int tilesCount = qPow(2., static_cast<qreal>(zoom)) / 2.; // 2097152
    qreal factor = 1;
};

MapGlobal &MapGlobal::instance()
{
    static MapGlobal * const ptr = new MapGlobal();
    return *ptr;
}

int MapGlobal::tileWidth() const
{
    return d->tileWidth;
}

int MapGlobal::tilesCount() const
{
    return d->tilesCount;
}

int MapGlobal::zoomMax() const
{
    return d->zoomMax;
}

int MapGlobal::zoom() const
{
    return d->zoom;
}

void MapGlobal::setZoom(int value)
{
    if(value > 0 && value <= d->zoomMax)
        d->zoom = value;
}

qreal MapGlobal::factor()
{
    return d->factor;
}

void MapGlobal::setFactor(qreal value)
{
    d->factor = value;
}

MapProviders MapGlobal::provider() const
{
    return d->provider;
}

void MapGlobal::setProvider(MapProviders provider)
{
    d->provider = provider;
}

QString MapGlobal::cachePath()
{
    return d->cachePath;
}

void MapGlobal::setCachePath(const QString &path)
{
    d->cachePath = path;
}

QPointF MapGlobal::toCoords(const QPointF &point)
{
    qreal tileWidth = static_cast<qreal>(d->tileWidth);
    qreal tilesCount = static_cast<qreal>(d->tilesCount);

    qreal lon = (point.x() - tileWidth / 2) / (tilesCount * tileWidth) * 360. - 180.;
    qreal lat = 0;

    if (d->provider == YandexMap || d->provider == YandexSat)
    {
        double c1 = 0.00335655146887969;
        double c2 = 0.00000657187271079536;
        double c3 = 0.00000001764564338702;
        double c4 = 0.00000000005328478445;

        double mercY = MERCATOR_QUARTER - (point.y() * pow(2, 24 - d->zoomMax)) / 53.5865938;
        double g = M_PI / 2 - 2 * atan(1 / exp(mercY / EARTH_RADIUS_WGS84));
        double zz = g + c1 * sin(2 * g) + c2 * sin(4 * g) + c3 * sin(6 * g) + c4 * sin(8 * g);
        lat = zz * 180 / M_PI;
    }
    else
    {
        lat = M_PI - 2. * M_PI * (point.y() - tileWidth / 2) / (tilesCount * tileWidth);
        lat = 180. / M_PI * atan(0.5 * (exp(lat) - exp(-lat)));
    }

    return QPointF(lon, lat);
}

QPointF MapGlobal::toPoint(const QPointF &coords)
{
    qreal tileWidth = static_cast<qreal>(d->tileWidth);
    qreal tilesCount = static_cast<qreal>(d->tilesCount);

    qreal x = (coords.x() + 180.) * (tilesCount * tileWidth) / 360.;
    x += tileWidth / 2;

    qreal y = 0;

    if (d->provider == YandexMap || d->provider == YandexSat)
    {
        double rLat = coords.y() * M_PI / 180;
        double k = 0.0818191908426;

        double zz = tan(M_PI / 4 + rLat / 2)  / pow((tan(M_PI / 4 + asin(k * sin(rLat)) / 2)), k);
        y = static_cast<int>((20037508.342789 - EARTH_RADIUS_WGS84 * log(zz)) * 53.5865938 / pow(2, 24 - d->zoomMax));
    }
    else
    {
        y = (1. - log(tan(coords.y() * M_PI / 180.) + 1. /
             cos(coords.y() * M_PI / 180.)) / M_PI) / 2. * (tilesCount * tileWidth);

        y += tileWidth / 2;
    }

    return QPoint(x, y);
}

float MapGlobal::distance(const QPointF &coords1, const QPointF &coords2)
{
    if (qFuzzyCompare(coords1.x(), coords2.x()) &&
        qFuzzyCompare(coords1.y(), coords2.y())) return 0.f;

    double x1 = qDegreesToRadians(coords1.x());
    double y1 = qDegreesToRadians(coords1.y());
    double x2 = qDegreesToRadians(coords2.x());
    double y2 = qDegreesToRadians(coords2.y());

    double sin1 = qSin(y1);
    double cos1 = qCos(y1);
    double sin2 = qSin(y2);
    double cos2 = qCos(y2);

    double cos21 = qCos(x2 - x1);
    double d1 = sin1;
    double d2 = cos1;

    d1 *= sin2;
    d2 *= cos2;
    d2 *= cos21;
    d1 += d2;

    d1 = EARTH_RADIUS_SK42 * qAcos(d1);

    return d1;
}

MapGlobal::MapGlobal() :
    d(new MapGlobalPrivate)
{
}

MapGlobal::~MapGlobal()
{
    delete d;
}
