#include "mapglobal.h"
#include <QtMath>
#include <QtCore>

static const float EARTH_RADIUS_SK42 = 6371109.0;   // усредненный радиус земли SK42
static const float EARTH_RADIUS_WGS84 = 6378137.0;  // усредненный радиус земли WGS84
static const double MERCATOR_QUARTER = 20037508.3427892430765884088807;  //2*PI*6378137/(2^z)

struct MapGlobal::MapGlobalPrivate
{
    QMap<QString, Provider> providers;
    Provider provider;
    QString providerName;
    QString cachePath;
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

qreal MapGlobal::factor() const
{
    return d->factor;
}

void MapGlobal::setFactor(qreal value)
{
    d->factor = value;
}

QString MapGlobal::providerName() const
{
    return d->providerName;
}

bool MapGlobal::setCurrentProvider(const QString &name)
{
    if (!d->providers.contains(name)) return false;

    d->providerName = name;
    d->provider = d->providers.value(name);

    return true;
}

QString MapGlobal::cachePath() const
{
    return d->cachePath;
}

QString MapGlobal::cachePathSuffix() const
{
    return d->provider.cachePathSuffix;
}

void MapGlobal::setCachePath(const QString &path)
{
    d->cachePath = path;
}

void MapGlobal::calculateUrl(int x, int y, int z, QString &url)
{
    url = d->provider.url;
    d->provider.calcUrlFunc(x, y, z, url);
}

void MapGlobal::addProvider(const QString &name, const Provider &provider)
{
    d->providers.insert(name, provider);
}

void MapGlobal::removeProvider(const QString &name)
{
    d->providers.remove(name);
}

QPointF MapGlobal::toCoords(const QPointF &point)
{
    qreal tileWidth = static_cast<qreal>(d->tileWidth);
    qreal tilesCount = static_cast<qreal>(d->tilesCount);

    qreal lon = (point.x() - tileWidth / 2) / (tilesCount * tileWidth) * 360. - 180.;
    qreal lat = 0;

    if (d->provider.coordsType == CoordsTypes::Ellipsoidal)
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

    if (d->provider.coordsType == CoordsTypes::Ellipsoidal)
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
    auto calcUrlFuncMain = [](int x, int y, int z, QString &url) {
        url = url.arg(x).arg(y).arg(z);
    };

    auto calcUrlFuncBing = [](int x, int y, int z, QString &url) {
        QString key;
        for (int i = z; i > 0; i--)
        {
            char digit = '0';
            int mask = 1 << (i - 1);
            if ((x & mask) != 0) digit++;
            if ((y & mask) != 0) digit += 2;

            key.append(digit);
        }
        url = (url.arg(key));
    };

    addProvider(ProviderGoogleMap, {"http://mt0.google.com/vt/lyrs=m&hl=en&x=%1&y=%2&z=%3", "/map", Spherical, calcUrlFuncMain});
    addProvider(ProviderGoogleSat, {"http://mt0.google.com/vt/lyrs=y&hl=en&x=%1&y=%2&z=%3", "/sat", Spherical, calcUrlFuncMain});
    addProvider(ProviderGoogleLand, {"http://mt0.google.com/vt/lyrs=p&hl=en&x=%1&y=%2&z=%3", "/land", Spherical, calcUrlFuncMain});
    addProvider(ProviderBingSat, {"http://ecn.t0.tiles.virtualearth.net/tiles/a%1.jpeg?g=0", "/vesat", Spherical, calcUrlFuncBing});
    addProvider(ProviderBingRoads, {"http://ecn.dynamic.t0.tiles.virtualearth.net/comp/CompositionHandler/%1?mkt=en-en&it=G,VE,BX,L,LA&shading=hill", "/bing_roads_en", Spherical, calcUrlFuncBing});
    addProvider(ProviderOsmMap, {"https://tile.openstreetmap.org/%3/%1/%2.png", "/osm", Spherical, calcUrlFuncMain});
    addProvider(ProviderYandexMap, {"http://vec04.maps.yandex.net/tiles?l=map&lang=en-EN&v=2.26.0&x=%1&y=%2&z=%3", "/yam", Ellipsoidal, calcUrlFuncMain});
    addProvider(ProviderYandexSat, {"http://sat01.maps.yandex.net/tiles?l=sat&v=3.379.0&x=%1&y=%2&z=%3", "/yas", Ellipsoidal, calcUrlFuncMain});
    addProvider(ProviderStamenToner, {"http://a.tile.stamen.com/toner/%3/%1/%2.png", "/stamen", Spherical, calcUrlFuncMain});
    addProvider(ProviderThunderforestTransport, {"http://tile.thunderforest.com/transport/%3/%1/%2.png", "/tht", Spherical, calcUrlFuncMain});
    addProvider(ProviderThunderforestLandscape, {"http://tile.thunderforest.com/landscape/%3/%1/%2.png", "/thl", Spherical, calcUrlFuncMain});
    addProvider(ProviderThunderforestOutdoors, {"http://tile.thunderforest.com/outdoors/%3/%1/%2.png", "/tho", Spherical, calcUrlFuncMain});
}

MapGlobal::~MapGlobal()
{
    delete d;
}
