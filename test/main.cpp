#include "mapview.h"
#include "mapglobal.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MapView w;
    w.resize(800,800);
    w.show();

    w.setZoom(16);
    w.setCenterOn(QPointF(-21.94336653, 64.15625817));

    QVector<QPointF> poligon = {
        {-21.94048047, 64.15304964},
        {-21.94271207, 64.15447622},
        {-21.94470763, 64.15396172},
        {-21.94398880, 64.15343318},
        {-21.94470763, 64.15315254},
        {-21.94594145, 64.15389624},
        {-21.94844127, 64.15314319},
        {-21.94781899, 64.15268479},
        {-21.94687486, 64.15286722},
        {-21.94570541, 64.15240414},
        {-21.94573760, 64.15213752},
        {-21.94489002, 64.15186622},
        {-21.94342017, 64.15246963},
        {-21.94279790, 64.15217027}
    };

    MapItemStatic *itemPoligon = w.createStaticItem();
    itemPoligon->setPen(QPen(QBrush(QColor(190, 70, 70)), 2));
    itemPoligon->setBrush(QBrush(QColor(190, 70, 70, 120)));
    itemPoligon->drawPath(poligon, true);
    itemPoligon->setText("Poligon");

    QVector<QPointF> path = {
        {-21.95284545, 64.15330923},
        {-21.95283473, 64.15344020},
        {-21.95275962, 64.15355713},
        {-21.95263624, 64.15366705},
        {-21.95244849, 64.15376528},
        {-21.95212662, 64.15385648},
        {-21.94175184, 64.15697139},
        {-21.94158554, 64.15705323},
        {-21.94144070, 64.15720055},
        {-21.94139779, 64.15738996},
        {-21.94144607, 64.15764952},
        {-21.94154799, 64.15791141},
        {-21.94165528, 64.15817563},
        {-21.94168210, 64.15908287},
        {-21.94188058, 64.15979134},
        {-21.94184840, 64.15986850},
        {-21.94174647, 64.15995268},
        {-21.94158554, 64.16001581},
        {-21.94139779, 64.16005088},
        {-21.93938613, 64.16029170}
    };

    MapItemStatic *itemPath = w.createStaticItem();
    itemPath->setPen(QPen(QBrush(QColor(Qt::blue)), 2));
    itemPath->drawPath(path, false);
    itemPath->setText("Path");

    MapItemStatic *itemEllipse = w.createStaticItem();
    itemEllipse->setPen(QPen(QBrush(QColor(70, 190, 70)), 2));
    itemEllipse->setBrush(QBrush(QColor(70, 190, 70, 120)));
    itemEllipse->drawEllipse(QPointF(-21.95960999, 64.16207562),
                             QPointF(-21.95478201, 64.15971418));
    itemEllipse->setText("Ellipse");

    MapItemStatic *itemRect = w.createStaticItem();
    itemRect->setPen(QPen(QBrush(QColor(210, 160, 20)), 2));
    itemRect->setBrush(QBrush(QColor(210, 210, 77, 120)));
    itemRect->drawRect(QPointF(-21.94624186, 64.16027534),
                       QPointF(-21.94255114, 64.15900338));
    itemRect->setText("Rect");


    MapItemDynamic *itemLabel = w.createDynamicItem();
    itemLabel->move(QPointF(-21.93969727, 64.16144904));
    itemLabel->setPen(QPen(QBrush(QColor(70, 70, 190)), 1));
    itemLabel->setBrush(QBrush(QColor(250, 250, 250, 250)));
    itemLabel->drawRect(QSize(40, 20), 3);
    itemLabel->setText("Label");

    MapItemDynamic *itemTag = w.createDynamicItem();
    itemTag->move(QPointF(-21.92931175, 64.15997138));
    itemTag->setPen(QPen(QBrush(QColor(190, 70, 70)), 1));
    itemTag->setBrush(QBrush(QColor(220, 70, 70, 220)));
    itemTag->drawEllipse(QSize(10, 10));
    itemTag->setText("Tag item", QPoint(0, -15));

    MapItemDynamic *itemPix = w.createDynamicItem();
    itemPix->move(QPointF(-21.93879604, 64.15478959));
    itemPix->setPen(QPen(QBrush(QColor(108, 48, 140)), 1));
    itemPix->setBrush(QBrush(QColor(190, 70, 70, 190)));
    itemPix->drawPixmap(QPixmap("../MapView/test/point.png"), QSize(40, 40));
    itemPix->setText("Pixmap", QPoint(0, 30));
    itemPix->setFlags(QGraphicsItem::ItemIsSelectable |
                      QGraphicsItem::ItemIsMovable);

    return a.exec();
}
