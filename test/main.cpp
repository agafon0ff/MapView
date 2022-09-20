#include "mapview.h"
#include "mapglobal.h"
#include "mapitem.h"

#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QPolygon>
#include <QComboBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create map-widget
    MapView w;
    w.resize(800,800);
    w.show();

    // Set map provider and view settings
    w.setProvider(GoogleMap);
    w.setZoom(16);
    w.setCenterOn(QPointF(-21.94336653, 64.15625817));
    w.setCachePath(".");

    // Receive mouse-click signals
    QObject::connect(&w, &MapView::clickCoords,
                     [=](const QPointF &coords){
        qDebug() << "coords:"
                 << QString::number(coords.x(), 'f', 8)
                 << QString::number(coords.y(), 'f', 8);
    });

    // Receive mouse-wheel zoom signals
    QObject::connect(&w, &MapView::zoomChanged, [&](int zoom){qDebug() << "zoom:" << zoom;});

    // Create item with pixmap and text
    MapItem *itemPixmap = w.createItem();
    itemPixmap->move(QPointF(-21.93180084, 64.15701582));
    itemPixmap->setSelectable(true);
    itemPixmap->setMovable(true);
    itemPixmap->setPixmap(QPixmap("../test/point.png"), QSize(40, 40));
    itemPixmap->setText("itemPixmap", {0, 30});
    itemPixmap->setZValue(2);

    // Create item with text
    MapItem *itemLabel = w.createItem();
    itemLabel->move(QPointF(-21.93381786, 64.15841882));
    itemLabel->setSelectable(true);
    itemLabel->setMovable(true);
    itemLabel->setRect({50, 20}, {5, 5});
//    itemLabel->setEllipse({50, 30});
    itemLabel->setText("Label");
    itemLabel->setPen(QPen(QColor(Qt::blue), 1));
    itemLabel->setPen(QPen(QColor(Qt::gray), 1), MapItemState::Hovered);
    itemLabel->setPen(QPen(QColor(Qt::red), 1), MapItemState::Selected);

    itemLabel->setBrush(QBrush(QColor("#eee")));
    itemLabel->setBrush(QBrush(QColor("#bbf")), MapItemState::Hovered);
    itemLabel->setBrush(QBrush(QColor("#fbb")), MapItemState::Selected);

    itemLabel->setColor(QColor(Qt::blue));

    // Receive signals when item changed
    QObject::connect(itemLabel, &MapItem::pressed, [&](bool state){ qDebug() << "pressed:" << state;});
    QObject::connect(itemLabel, &MapItem::hovered, [&](bool state){ qDebug() << "hovered:" << state;});
    QObject::connect(itemLabel, &MapItem::selected, [&](bool state){ qDebug() << "selected:" << state;});
    QObject::connect(itemLabel, &MapItem::moved, [&](const QPointF &coords){ qDebug() << "moved:" << coords;});
    QObject::connect(itemLabel, &MapItem::movedTo, [&](const QPointF &coords){ qDebug() << "moved to:" << coords;});

    // Create polygon item
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

    // Create path item
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

    // Create ellipse item
    MapItem *itemEllipse = w.createItem();
    itemEllipse->setPen(QPen(QColor(70, 190, 70), 2));
    itemEllipse->setBrush(QBrush(QColor(70, 190, 70, 120)));
    itemEllipse->setStaticEllipse(QPointF(-21.95303321, 64.15305315),
                                  QPointF(-21.95251286, 64.15282746));
    itemEllipse->setFont(QFont("mono", 12., QFont::Medium));
    itemEllipse->setText("Ellipse");
    itemEllipse->setColor(QColor(70, 190, 70));

    // Create rect item
    MapItem *itemRect = w.createItem();
    itemRect->setPen(QPen(QColor(210, 160, 20), 2));
    itemRect->setBrush(QBrush(QColor(210, 210, 77, 120)));
    itemRect->setStaticRect(QPointF(-21.94624186, 64.16027534),
                            QPointF(-21.94255114, 64.15900338));
    itemRect->setFont(QFont("mono", 12., QFont::Medium));
    itemRect->setText("Rect");
    itemRect->setColor(QColor(210, 160, 20));

    // Create line item with text
    MapItem *itemLine = w.createItem();
    itemLine->setPen(QPen(QColor(210, 160, 20), 2));
    itemLine->setColor(QColor(210, 160, 20));
    itemLine->setBrush(QBrush(QColor(210, 210, 77, 120)));

    QVector<QPointF> line = { {-21.95071578, 64.15693785}, {-21.94615602, 64.15699873} };
    itemLine->setStaticPath(line);

    // Calculate line length and show
    QString lineText(QString::number(MapGlobal::instance().distance(line.at(0), line.at(1)), 'f', 0) + "m");
    itemLine->setText(lineText, {0, -1000});

    // Change map provider
    QComboBox comboBoxProviders(&w);
    comboBoxProviders.show();
    comboBoxProviders.setGeometry(5, 5, 200, 30);
    comboBoxProviders.addItems({"GoogleMap", "GoogleSat", "GoogleLand",
                                "BingSat", "BingRoads", "OsmMap",
                                "YandexMap", "YandexSat", "StamenToner",
                                "ThunderforestTransport", "ThunderforestLandscape", "ThunderforestOutdoors"});

    QObject::connect(&comboBoxProviders, qOverload<int>(&QComboBox::currentIndexChanged), &w, [&](int index){
        w.setProvider(static_cast<MapProviders>(index));
    });

    return a.exec();
}
