#pragma once

#include <QObject>
#include <functional>

static const char* ProviderGoogleMap = "GoogleMap";
static const char* ProviderGoogleSat = "GoogleSat";
static const char* ProviderGoogleLand = "GoogleLand";
static const char* ProviderBingSat = "BingSat";
static const char* ProviderBingRoads = "BingRoads";
static const char* ProviderOsmMap = "OsmMap";
static const char* ProviderYandexMap = "YandexMap";
static const char* ProviderYandexSat = "YandexSat";
static const char* ProviderStamenToner = "StamenToner";
static const char* ProviderThunderforestTransport = "ThunderforestTransport";
static const char* ProviderThunderforestLandscape = "ThunderforestLandscape";
static const char* ProviderThunderforestOutdoors = "ThunderforestOutdoors";

enum CoordsTypes
{
    Spherical,
    Ellipsoidal
};

struct Provider
{
    QString url;
    QString cachePathSuffix;
    CoordsTypes coordsType = Spherical;
    std::function<void(int, int, int, QString&)> calcUrlFunc;
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

    qreal factor() const;
    void setFactor(qreal value);

    QString providerName() const;
    bool setCurrentProvider(const QString &name);

    QString cachePath() const;
    QString cachePathSuffix() const;
    void setCachePath(const QString &path);

    void calculateUrl(int x, int y, int z, QString &url);

    void addProvider(const QString &name, const Provider &provider);
    void removeProvider(const QString &name);

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

