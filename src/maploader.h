#pragma once

#include <QObject>

class MapLoader : public QObject
{
    Q_OBJECT
public:
    explicit MapLoader(QObject *parent = Q_NULLPTR);
    ~MapLoader();

signals:
    void loaded(const QPoint &pos, const QPixmap &pix);

public slots:

    void update();
    void loadTile(const QPoint &pos);

private:

    void requestFinished();
    QUrl createUrl(const QPoint &pos);
    QString createCachePath(const QPoint &pos);

    void saveFile(const QString &path, const QByteArray &data);
    bool loadFile(const QString &path, QPixmap &pix);

    struct MapLoaderPrivate;
    MapLoaderPrivate * const d;

};
