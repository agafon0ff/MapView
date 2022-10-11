#pragma once

#include <QObject>

enum MapProviders
{
    GoogleMap,
    GoogleSat,
    GoogleLand,
    BingSat,
    BingRoads,
    OsmMap,
    YandexMap,
    YandexSat,
    StamenToner,
    ThunderforestTransport,
    ThunderforestLandscape,
    ThunderforestOutdoors,
};

static const char* MAP_PROVIDERS[] = {
    "http://mt0.google.com/vt/lyrs=m&hl=en&x=%1&y=%2&z=%3",                         // GoogleMap
    "http://mt0.google.com/vt/lyrs=y&hl=ru&x=%1&y=%2&z=%3",                         // GoogleSat
    "http://mt0.google.com/vt/lyrs=p&hl=ru&x=%1&y=%2&z=%3",                         // GoogleLand
    "http://ecn.t0.tiles.virtualearth.net/tiles/a%1.jpeg?g=0",                      // BingSat
    "http://ecn.dynamic.t0.tiles.virtualearth.net/comp/CompositionHandler/%1?mkt=ru-ru&it=G,VE,BX,L,LA&shading=hill", // BingRoads
    "https://tile.openstreetmap.org/%3/%1/%2.png",                                  // OsmMap
    "http://vec04.maps.yandex.net/tiles?l=map&lang=ru-RU&v=2.26.0&x=%1&y=%2&z=%3",  // YandexMap
    "http://sat01.maps.yandex.net/tiles?l=sat&v=3.379.0&x=%1&y=%2&z=%3",            // YandexSat
    "http://a.tile.stamen.com/toner/%3/%1/%2.png",                                  // StamenToner
    "http://tile.thunderforest.com/transport/%3/%1/%2.png",                         // ThunderforestTransport
    "http://tile.thunderforest.com/landscape/%3/%1/%2.png",                         // ThunderforestLandscape
    "http://tile.thunderforest.com/outdoors/%3/%1/%2.png"                           // ThunderforestOutdoors
};

static const char* MAP_CACHE_SUFFIXES[] = {
    "/map",             // GoogleMap
    "/sat",             // GoogleSat
    "/land",            // GoogleLand
    "/vesat",           // BingSat
    "/bing_roads_ru",   // BingRoads
    "/osm",             // OsmMap
    "/yam",             // YandexMap
    "/yas",             // YandexSat
    "/stamen",          // StamenToner
    "/tht",             // ThunderforestTransport
    "/thl",             // ThunderforestLandscape
    "/tho"              // ThunderforestOutdoors
};

class MapGlobal
{
public:
    static MapGlobal &instance();

    int tileWidth() const;
    int tilesCount() const;
    int zoomMax() const;

    int zoom() const;
    void setZoom(int value);

    qreal factor();
    void setFactor(qreal value);

    MapProviders provider() const;
    void setProvider(MapProviders provider);

    QString cachePath();
    void setCachePath(const QString &path);

    QPointF toCoords(const QPointF &point);
    QPointF toPoint(const QPointF &coords);

    float distance(const QPointF &coords1, const QPointF &coords2);

protected:
    MapGlobal();
    ~MapGlobal();

private:
    struct MapGlobalPrivate;
    MapGlobalPrivate * const d;
};

