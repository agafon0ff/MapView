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

struct MapLoader::MapLoaderPrivate
{
    MapGlobal &settings = MapGlobal::instance();
    QNetworkAccessManager *netAccessManager;
    QVector<QNetworkReply*> replies;
    QString cachePath = "/z%1/%2/x%3/%4/y%5.png";
    QString currentUrl;
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
    if (d->settings.providerName().isEmpty()) return;

    QString cache = createCachePath(pos);
    QPixmap pix;

    if(QPixmapCache::find(cache, &pix))
    {
        emit loaded(pos, pix);
    }

    if(loadFile(cache, pix))
    {
        emit loaded(pos, pix);
    }
    else
    {
        d->settings.calculateUrl(pos.x(), pos.y(), d->settings.zoom() - 1, d->currentUrl);
        QNetworkRequest request = QNetworkRequest(QUrl(d->currentUrl));
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

QString MapLoader::createCachePath(const QPoint &pos)
{
    return d->settings.cachePathSuffix() + d->cachePath.arg(d->settings.zoom()).
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
