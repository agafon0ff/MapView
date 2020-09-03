#include "mapview.h"
#include "mapglobal.h"
#include "mapitem.h"

#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QPolygon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MapView w;
    w.resize(800,800);
    w.show();

    w.setProvider(GoogleMap);
    w.setZoom(16);
    w.setCenterOn(QPointF(-21.94336653, 64.15625817));

    MapItem *itemPixmap = w.createItem();
    itemPixmap->move(QPointF(-21.93180084, 64.15701582));
    itemPixmap->setSelectable(true);
    itemPixmap->setMovable(true);
    itemPixmap->setPixmap(QPixmap("../MapView/test/point.png"), QSize(40, 40));
    itemPixmap->setText("itemPixmap", {0, 30});
    itemPixmap->setZValue(2);

    MapItem *itemLabel = w.createItem();
    itemLabel->move(QPointF(-21.93381786, 64.15841882));
    itemLabel->setSelectable(true);
    itemLabel->setMovable(true);
    itemLabel->setRect({50, 20}, {5, 5});
//    itemLabel->setEllipse({50, 30});
    itemLabel->setText("Label");
    itemLabel->setPen(QPen(QColor(Qt::blue), 1));
    itemLabel->setBrush(QBrush(QColor(220, 220, 220, 220)));
    itemLabel->setColor(QColor(Qt::blue));

    QVector<QPointF> polygon = {
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

    MapItem *itemPoligon = w.createItem();
    itemPoligon->setPen(QPen(QBrush(QColor(190, 70, 70)), 2));
    itemPoligon->setBrush(QBrush(QColor(190, 70, 70, 120)));
    itemPoligon->setStaticPath(polygon, true);

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

    MapItem *itemPath = w.createItem();
    itemPath->setPen(QPen(QColor(Qt::blue), 3));
    itemPath->setStaticPath(path, false);

    MapItem *itemEllipse = w.createItem();
    itemEllipse->setPen(QPen(QColor(70, 190, 70), 2));
    itemEllipse->setBrush(QBrush(QColor(70, 190, 70, 120)));
    itemEllipse->setStaticEllipse(QPointF(-21.96008205, 64.16210368),
                                  QPointF(-21.95424557, 64.15974692));
    itemEllipse->setFont(QFont("mono", 12., QFont::Medium));
    itemEllipse->setText("Ellipse");
    itemEllipse->setColor(QColor(70, 190, 70));


    MapItem *itemRect = w.createItem();
    itemRect->setPen(QPen(QColor(210, 160, 20), 2));
    itemRect->setBrush(QBrush(QColor(210, 210, 77, 120)));
    itemRect->setStaticRect(QPointF(-21.94624186, 64.16027534),
                            QPointF(-21.94255114, 64.15900338));
    itemRect->setFont(QFont("mono", 12., QFont::Medium));
    itemRect->setText("Rect");
    itemRect->setColor(QColor(210, 160, 20));

    return a.exec();
}
