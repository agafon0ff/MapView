#include "maploader.h"
#include "mapglobal.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPixmapCache>
#include <QApplication>
#include <QFileInfo>
#include <QPixmap>
#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QDir>

static const QMap<int, QString> MAP_PROVIDERS = {
    {GoogleMap,         "http://mt0.google.com/vt/lyrs=m&hl=en&x=%1&y=%2&z=%3"},
    {GoogleSat,         "http://mt0.google.com/vt/lyrs=y&hl=ru&x=%1&y=%2&z=%3"},
    {GoogleLand,        "http://mt0.google.com/vt/lyrs=p&hl=ru&x=%1&y=%2&z=%3"},
    {BingSat,           "http://ecn.t0.tiles.virtualearth.net/tiles/a%1.jpeg?g=0"},
    {BingRoads,         "http://ecn.dynamic.t0.tiles.virtualearth.net/comp/CompositionHandler/%1?mkt=ru-ru&it=G,VE,BX,L,LA&shading=hill"},
    {OsmMap,            "https://tile.openstreetmap.org/%3/%1/%2.png"},
    {YandexMap,         "http://vec04.maps.yandex.net/tiles?l=map&lang=ru-RU&v=2.26.0&x=%1&y=%2&z=%3"},
    {YandexSat,         "http://sat01.maps.yandex.net/tiles?l=sat&v=3.379.0&x=%1&y=%2&z=%3"},
    {StamenToner,       "http://a.tile.stamen.com/toner/%3/%1/%2.png"},
    {ThunderforestTransport,    "http://tile.thunderforest.com/transport/%3/%1/%2.png"},
    {ThunderforestLandscape,    "http://tile.thunderforest.com/landscape/%3/%1/%2.png"},
    {ThunderforestOutdoors,     "http://tile.thunderforest.com/outdoors/%3/%1/%2.png"}
};

static const QMap<int, QString> MAP_CACHE_PATHS = {
    {GoogleMap,     "/map/z%1/%2/x%3/%4/y%5.png"},
    {GoogleSat,     "/sat/z%1/%2/x%3/%4/y%5.jpg"},
    {GoogleLand,    "/land/z%1/%2/x%3/%4/y%5.jpg"},
    {BingSat,       "/vesat/z%1/%2/x%3/%4/y%5.jpg"},
    {BingRoads,     "/bing_roads_ru/z%1/%2/x%3/%4/y%5.png"},
    {OsmMap,        "/osm/z%1/%2/x%3/%4/y%5.png"},
    {YandexMap,     "/yam/z%1/%2/x%3/%4/y%5.png"},
    {YandexSat,     "/yas/z%1/%2/x%3/%4/y%5.png"},
    {StamenToner,   "/stamen/z%1/%2/x%3/%4/y%5.png"},
    {ThunderforestTransport, "/tht/z%1/%2/x%3/%4/y%5.png"},
    {ThunderforestLandscape, "/thl/z%1/%2/x%3/%4/y%5.png"},
    {ThunderforestOutdoors, "/tho/z%1/%2/x%3/%4/y%5.png"}
};

struct MapLoader::MapLoaderPrivate
{
    MapGlobal &settings = MapGlobal::instance();
    QNetworkAccessManager *netAccessManager;
    QVector<QNetworkReply*> replies;
};

MapLoader::MapLoader(QObject *parent) : QObject(parent),
    d(new MapLoaderPrivate)
{
    d->netAccessManager = new QNetworkAccessManager;
    QPixmapCache::setCacheLimit(100000); //100MB
}

MapLoader::~MapLoader()
{
    foreach (QNetworkReply *reply, d->replies)
    {
        reply->disconnect();
        reply->abort();
        delete reply;
    }

    d->replies.clear();


    delete d->netAccessManager;
    delete d;
}

void MapLoader::update()
{
    foreach (QNetworkReply *reply, d->replies)
    {
        reply->disconnect();
        reply->abort();
        delete reply;
    }

    d->replies.clear();
}

void MapLoader::loadTile(const QPoint &pos)
{
    QString cache = createCachePath(pos);
    QPixmap pix;

    if(QPixmapCache::find(cache, &pix))
    {
        emit loaded(pos, pix);
    }
    else if(loadFile(cache, pix))
    {
        emit loaded(pos, pix);
    }
    else
    {
        QUrl url = createUrl(pos);
        QNetworkRequest request = QNetworkRequest(QUrl(url));
        request.setRawHeader("User-Agent", "Mozilla/5.0 (PC; U; Intel; Linux; en) AppleWebKit/420+ (KHTML, like Gecko)");

        QNetworkReply *reply = d->netAccessManager->get(request);
        d->replies.append(reply);

        reply->setProperty("id", pos);
        connect(reply, &QNetworkReply::finished, this, &MapLoader::requestFinished);
    }
}

void MapLoader::requestFinished()
{
    QNetworkReply *reply = dynamic_cast<QNetworkReply*>(sender());
    d->replies.removeOne(reply);

    if(reply->error() != QNetworkReply::NoError)
        return;

    QByteArray data = reply->readAll();
    const QPoint pos = reply->property("id").toPoint();

    QPixmap pix;
    pix.loadFromData(data);

    emit loaded(pos, pix);

    reply->disconnect();
    delete reply;

    QString cache = createCachePath(pos);
    QPixmapCache::insert(cache, pix);
    saveFile(cache, data);
}

QUrl MapLoader::createUrl(const QPoint &pos)
{
    QUrl url;
    QString path = MAP_PROVIDERS.value(d->settings.provider());
    int provider = d->settings.provider();
    int z = d->settings.zoom() - 1;

    if(provider == BingSat || provider == BingRoads)
    {
        QString key;
        for (int i = z; i > 0; i--)
        {
            char digit = '0';
            int mask = 1 << (i - 1);
            if ((pos.x() & mask) != 0) digit++;
            if ((pos.y() & mask) != 0) digit += 2;

            key.append(digit);
        }
        url.setUrl(path.arg(key));
    }
    else
    {
        url.setUrl(path.arg(pos.x()).arg(pos.y()).arg(z));
    }

    return url;
}

QString MapLoader::createCachePath(const QPoint &pos)
{
    return MAP_CACHE_PATHS.value(d->settings.provider()).arg(d->settings.zoom()).
            arg(pos.x()/1024).arg(pos.x()).arg(pos.y()/1024).arg(pos.y());
}

void MapLoader::saveFile(const QString &path, const QByteArray &data)
{
    if (d->settings.cachePath().isEmpty()) return;

    QFileInfo fInfo(d->settings.cachePath() + path);
    if (!fInfo.dir().exists())
    {
        QDir dir = fInfo.dir();
        dir.mkpath(".");
    }

    QFile file(fInfo.filePath());
    if (!file.open(QIODevice::WriteOnly)) return;

    file.write(data);
    file.close();
}

bool MapLoader::loadFile(const QString &path, QPixmap &pix)
{
    if (d->settings.cachePath().isEmpty()) return false;
    QFile file(d->settings.cachePath() + path);

    if (!file.exists()) return false;
    if (!file.open(QIODevice::ReadOnly)) return false;

    bool result = pix.loadFromData(file.readAll());
    file.close();

    return result;
}
