#pragma once

#include <cstddef>
#include <QObject>

enum MapProviders
{
    GoogleMap,
    GoogleSat,
    GoogleLand,
    BingSat,
    BingRoads,
    OsmMap
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

    QPointF toCoords(const QPointF &point);
    QPointF toPoint(const QPointF &coords);

protected:
    MapGlobal();
    ~MapGlobal();

private:
    struct MapGlobalPrivate;
    MapGlobalPrivate * const d;
};

