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

