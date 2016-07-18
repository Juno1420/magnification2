#include "graphicsband.h"

#include <QPainter>
#include <QPen>
#include <QColor>

GraphicsBand::GraphicsBand(QWidget *parent) :
    QWidget(parent)
{
    this->setStyleSheet("background-color: rgb(211, 211, 211)");
    this->setMouseTracking(true);

    minValue = 0;
    maxAlpha = 50;
    maxFrequency = 100;

    pntA = QPoint(40, 40);
    pntB = QPoint(640, 190);
    pntC = QPoint(40, 20);
    pntD = QPoint(660, 190);
    pntE = QPoint(40, 190);
}

void GraphicsBand::paintEvent(QPaintEvent *)
{

}
