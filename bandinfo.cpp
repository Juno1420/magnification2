#include "bandinfo.h"
#include <QPen>
#include <QColor>
#include <QVector>
#include <QRect>
#include <QInputDialog>
#include <QDebug>
#include <qmath.h>

bandInfo::bandInfo(QWidget *parent) :
    QWidget(parent),
	moveControl_1(false),
	moveControl_2(false),
	move_i(-1),
	move_j(0),
	delete_k(0),
	minValue(0),
	maxAlpha(50),
	maxFrequency(100)
{
    this->setStyleSheet("background-color: rgb(211, 211, 211)");
    this->setMouseTracking(true);

    pntA = QPoint(40, 40);
    pntB = QPoint(640, 190);
    pntC = QPoint(40, 20);
    pntD = QPoint(660, 190);
    pntE = QPoint(40, 190);

	penColor.append(QColor(0, 245, 255)); //锰色
    penColor.append(QColor(255, 48, 48));//红  
    penColor.append(QColor(255, 0, 255));//紫色
    penColor.append(QColor(0, 255, 0));//绿
    penColor.append(QColor(0, 191, 255));//蓝
    penColor.append(QColor(255, 255, 0));//镉黄

    minBtn = new QPushButton(QString::number(minValue), this);
    minBtn->setStyleSheet("font: century gothic; border:2px; border-radius: 1px; border-style:solid; border-color:rgb(112,128,144); background-color:rgb(0, 154, 205);");
    connect(minBtn, SIGNAL(clicked()), this, SLOT(minValueChanged()));

    maxAlphaBtn = new QPushButton(QString::number(maxAlpha), this);
    maxAlphaBtn->setStyleSheet("font: century gothic; border:2px; border-radius: 1px; border-style:solid; border-color:rgb(112,128,144); background-color:rgb(0, 154, 205);");
    connect(maxAlphaBtn, SIGNAL(clicked()), this, SLOT(maxAlphaChanged()));

    maxFrequencyBtn = new QPushButton(QString::number(maxFrequency), this);
    maxFrequencyBtn->setStyleSheet("font: century gothic; border:2px; border-radius: 1px; border-style:solid; border-color:rgb(112,128,144); background-color:rgb(0, 154, 205);");
    connect(maxFrequencyBtn, SIGNAL(clicked()), this, SLOT(maxFrequencyChanged()));

    minBtn->setGeometry(pntE.x()-24, pntE.y(), 20, 15);
    maxAlphaBtn->setGeometry(pntA.x()-24, pntA.y()-8, 20, 15);
    maxFrequencyBtn->setGeometry(pntB.x()-12, pntB.y()+4, 25, 15);

    deleteBandAction = new QAction("deleteThisBand", this);
    connect(deleteBandAction, SIGNAL(triggered()), this, SLOT(deleteBandHere()));

	addBand = new QAction("addBandToThisArea", this);
	connect(addBand, SIGNAL(triggered()), this, SLOT(addBandHere()));
    rightMenu = new QMenu(this);
}

void bandInfo::minValueChanged()
{
    bool ok;
    this->minValue =  QInputDialog::getInt(this, tr("QInputDialog::get_minValue()"), tr("minValue:"), 0, 0, 100, 1, &ok);
    minBtn->setText(QString::number(minValue));
    emit bandValueChanged();
    update();
}

void bandInfo::maxAlphaChanged()
{
    bool ok;
    this->maxAlpha =  QInputDialog::getInt(this, tr("QInputDialog::get_maxAlpha()"), tr("maxAlpha:"), 50, 5, 100, 5, &ok);
    maxAlphaBtn->setText(QString::number(maxAlpha));
    emit bandValueChanged();
    update();
}

void bandInfo::maxFrequencyChanged()
{
    bool ok;
    this->maxFrequency =  QInputDialog::getInt(this, tr("QInputDialog::get_maxFrequency()"), tr("maxFrequency:"), 100, 10, 200, 10, &ok);
    maxFrequencyBtn->setText(QString::number(maxFrequency));
    emit bandValueChanged();
    update();
}

void bandInfo::deleteBandHere()
{
    int i = delete_k;
    QRect rectBand(pntA, pntB);
    QStringList strList = this->currentAreaBand;
    int j = strList.at(7).toInt();
    if(j > 1)
    {
        int bandCount = strList.at(7).toInt()-1;
        strList.removeAt(7);
        strList.insert(7, QString::number(bandCount));
        strList.removeAt(9+i*4);
        strList.removeAt(9+i*4);
        strList.removeAt(9+i*4);
        strList.removeAt(9+i*4);
    }
    if(j == 1)
    {
        strList.removeAt(7);
        strList.insert(7, QString::number(0));
        strList.removeAt(8);
        strList.removeAt(8);
        strList.removeAt(8);
        strList.removeAt(8);
        strList.removeAt(8);
    }

    this->currentAreaBand = strList;
    emit bandValueChanged();
    update(rectBand);
}

void bandInfo::mousePressEvent(QMouseEvent *event)
{
    QVector<QPoint> oriPntVec;
    for(int i=0; i< drawPntVec.size(); i++)
        oriPntVec.append(drawPntVec.at(i));

    QRect rectBand(pntA, pntB);
    if(event->button() == Qt::LeftButton && rectBand.contains(event->pos()) )//band相关移动
    {
        if(oriPntVec.size() != 0)//计算左右上下移动区域
        {
            rectLeftRight.clear();
            rectUpDown.clear();
            for(int i=1; i<oriPntVec.size()-1; i++)//计算左右平移区域
            {
                rectLeftRight.append(QRect(oriPntVec.at(i+1).x()-2, oriPntVec.at(i+1).y(), 4, qFabs(oriPntVec.at(i+1).y()-oriPntVec.at(i).y())));
                rectLeftRight.append(QRect(oriPntVec.at(i+2).x()-2, oriPntVec.at(i+2).y(), 4, qFabs(oriPntVec.at(i+3).y()-oriPntVec.at(i+2).y())));
                i += 3;
            }
            for(int i=0; i<oriPntVec.size()-1; i++)//计算上下平移区域
            {
                if(i==0)
                    rectUpDown.append(QRect(oriPntVec.at(0).x(), oriPntVec.at(0).y()-2, qFabs(oriPntVec.at(1).x()-oriPntVec.at(0).x()-4), 4));
                if(i!=0)
                {
                    rectUpDown.append(QRect(oriPntVec.at(i).x()+1, oriPntVec.at(i+1).y()-2, qFabs(oriPntVec.at(i+2).x()-oriPntVec.at(i+1).x()-4), 4));
                    i += 1;
                }
            }
        }

        if(event->button() == Qt::LeftButton)
        {
            move_i = -1;
            move_j = -1;
            for(int i=0; i<rectLeftRight.size(); i++)//判断左右移动信号
            {
                if(rectLeftRight.at(i).contains(event->pos()))
                {
                    setCursor(Qt::SizeHorCursor);
                    moveControl_1 = true;
                    move_i = i;
                }
            }
            for(int i=0; i<rectUpDown.size(); i++)//判断上下移动信号
            {
                if(rectUpDown.at(i).contains(event->pos()))
                {
                    setCursor(Qt::SizeVerCursor);
                    moveControl_2 = true;
                    move_j = i;
                }
            }
        }
    }
}

void bandInfo::mouseMoveEvent(QMouseEvent *event)
{
    QRect rectBand(pntA, pntB);
	QStringList strList = currentAreaBand;
    int Ax = pntA.x();//537
    int Bx = pntB.x();//1137
    int Ay = pntA.y();//542
    int By = pntB.y();//692

	if(moveControl_1 == true && event->buttons() == Qt::LeftButton)//左右移动
    {
        if(move_i == 0)//若选择的是第一个区域
        {
            if(event->pos().x() > Ax && event->pos().x() < rectLeftRight.at(move_i+1).x())
            {
                int value = event->pos().x();
                value = (value+1-Ax)*(maxFrequency-minValue)/10/60+minValue;//转换成标示坐标值
                strList.insert(9, QString::number(value));
                strList.removeAt(10);
            }
            if(event->pos().x() < Ax)
            {
                strList.insert(9, QString::number(minValue+1));
                strList.removeAt(10);
            }
            if(event->pos().x() > rectLeftRight.at(move_i+1).x())
            {
                int value = rectLeftRight.at(move_i+1).x()-1;
                value = (value+1-Ax)*(maxFrequency-minValue)/10/60+minValue;//转换成标示坐标值
                strList.insert(9, QString::number(value-1));//防止两条线重合
                strList.removeAt(10);
            }
        }

        if(move_i > 0 && move_i < rectLeftRight.size()-1)//若选择的是除了第一个和最后一个之外的其他区域
        {
            if(event->pos().x()>rectLeftRight.at(move_i-1).x() && event->pos().x() < rectLeftRight.at(move_i+1).x())//若移动未超过前后界限
            {
                int value = event->pos().x();
                value = (value+1-Ax)*(maxFrequency-minValue)/10/60+minValue;//转换成标示坐标值
                int bandIndex = move_i/2;
                if(move_i % 2 == 0)
                {
                    strList.insert(9+bandIndex*4, QString::number(value));
                    strList.removeAt(10+bandIndex*4);
                }
                if(move_i % 2 == 1)
                {
                    strList.insert(10+bandIndex*4, QString::number(value));
                    strList.removeAt(11+bandIndex*4);
                }
            }
            if(event->pos().x() < rectLeftRight.at(move_i-1).x())//若移动超过前面界限
            {
                int value = rectLeftRight.at(move_i-1).x();
                value = (value+1-Ax)*(maxFrequency-minValue)/60/10+minValue;//转换成标示坐标值
                int bandIndex = move_i/2;
                if(move_i % 2 == 0)
                {
                    strList.insert(9+bandIndex*4, QString::number(value+1));
                    strList.removeAt(10+bandIndex*4);
                }
                if(move_i % 2 == 1)
                {
                    strList.insert(10+bandIndex*4, QString::number(value+1));
                    strList.removeAt(11+bandIndex*4);
                }
            }
            if(event->pos().x() > rectLeftRight.at(move_i+1).x())//若移动超过后面界限
            {
                int value = rectLeftRight.at(move_i+1).x();
                value = (value+1-Ax)*(maxFrequency-minValue)/60/10+minValue;//转换成标示坐标值
                int bandIndex = move_i/2;
                if(move_i % 2 == 0)
                {
                    strList.insert(9+bandIndex*4, QString::number(value-1));
                    strList.removeAt(10+bandIndex*4);
                }
                if(move_i % 2 == 1)
                {
                    strList.insert(10+bandIndex*4, QString::number(value-1));
                    strList.removeAt(11+bandIndex*4);
                }
            }
        }

        if(move_i == rectLeftRight.size()-1)//若选择的是最后一个区域
        {
            if(event->pos().x() < Bx && event->pos().x() > rectLeftRight.at(move_i-1).x())
            {
                int value = event->pos().x();
                value = (value+1-Ax)*(maxFrequency-minValue)/10/60+minValue;//转换成标示坐标值
                strList.insert(strList.size()-3, QString::number(value));
                strList.removeAt(strList.size()-3);
            }
            if(event->pos().x() < rectLeftRight.at(move_i-1).x())
            {
                int value = rectLeftRight.at(move_i-1).x();
                value = (value+1-Ax)*(maxFrequency-minValue)/60/10+minValue;//转换成标示坐标值
                strList.insert(strList.size()-3, QString::number(value+1));
                strList.removeAt(strList.size()-3);
            }
            if(event->pos().x() > Bx)
            {
                strList.insert(strList.size()-3, QString::number(maxFrequency-1));
                strList.removeAt(strList.size()-3);
            }
        }

        currentAreaBand = strList;  //改变band数据，并重新绘制
    }

	if(moveControl_2 == true && event->buttons() == Qt::LeftButton)//上下移动
    {
        if(move_j == 0)//若选择的是第一个区域
        {
            if(event->pos().y() > Ay && event->pos().y() < By)
            {
                int value = event->pos().y();
                value = (By-value-1)*(maxAlpha-minValue)/5/30+minValue;//转换成标示坐标值
                strList.insert(8, QString::number(value));
                strList.removeAt(9);
            }
            if(event->pos().y() > By)
            {
                strList.insert(8, QString::number(minValue+1));
                strList.removeAt(9);
            }
            if(event->pos().y() < Ay)
            {
                strList.insert(8, QString::number(maxAlpha-1));
                strList.removeAt(9);
            }
        }
        if(move_j > 0 && move_j < rectUpDown.size()-1)//若选择的是除了第一和最后一个之外的区域
        {            
            int bandIndex = (move_j-1)/2;
            if(event->pos().y() > Ay && event->pos().y() < By)
            {
                int value = event->pos().y();
                value = (By-value-1)*(maxAlpha-minValue)/5/30+minValue;//转换成标示坐标值
                if(move_j % 2 == 1)
                {
                    strList.insert(11+bandIndex*4, QString::number(value));
                    strList.removeAt(12+bandIndex*4);
                }
                if(move_j % 2 == 0)
                {
                    strList.insert(12+bandIndex*4, QString::number(value));
                    strList.removeAt(13+bandIndex*4);
                }
            }
            if(event->pos().y() < Ay)
            {
                if(move_j % 2 == 1)
                {
                    strList.insert(11+bandIndex*4, QString::number(maxAlpha-1));
                    strList.removeAt(12+bandIndex*4);
                }
                if(move_j % 2 == 0)
                {
                    strList.insert(12+bandIndex*4, QString::number(maxAlpha-1));
                    strList.removeAt(13+bandIndex*4);
                }
            }
            if(event->pos().y() > By)
            {
                if(move_j % 2 == 1)
                {
                    strList.insert(11+bandIndex*4, QString::number(minValue+1));
                    strList.removeAt(12+bandIndex*4);
                }
                if(move_j % 2 == 0)
                {
                    strList.insert(12+bandIndex*4, QString::number(minValue+1));
                    strList.removeAt(13+bandIndex*4);
                }
            }
        }
        if(move_j == rectUpDown.size()-1)//若选择的是最后一个区域
        {
            if(event->pos().y() > Ay && event->pos().y() < By)
            {
                int value = event->pos().y();
                value = (By-value-1)*(maxAlpha-minValue)/5/30+minValue;//转换成标示坐标值
                strList.insert(strList.size()-1, QString::number(value));
                strList.removeAt(strList.size()-1);
            }
            if(event->pos().y() > By)
            {
                strList.insert(strList.size()-1, QString::number(minValue+1));
                strList.removeAt(strList.size()-1);
            }
            if(event->pos().y() < Ay)
            {
                strList.insert(strList.size()-1, QString::number(maxAlpha-1));
                strList.removeAt(strList.size()-1);
            }
        }
                                             
        currentAreaBand = strList;      //数据变换完成，更新绘制
    }
	repaint();
}

void bandInfo::mouseReleaseEvent(QMouseEvent *event)
{
    QRect rectBand(pntA, pntB);
    QStringList strList = currentAreaBand;
    int Ax = pntA.x();//537
    int Bx = pntB.x();//1137
    int Ay = pntA.y();//542
    int By = pntB.y();//692
	if(event->button() == Qt::LeftButton && moveControl_1 == true)//左右移动
    {
        if(move_i == 0)//若选择的是第一个区域
        {
            if(event->pos().x() > Ax && event->pos().x() < rectLeftRight.at(move_i+1).x())
            {
                int value = event->pos().x();
                value = (value+1-Ax)*(maxFrequency-minValue)/10/60+minValue;//转换成标示坐标值
                strList.insert(9, QString::number(value));
                strList.removeAt(10);
            }
            if(event->pos().x() < Ax)
            {
                strList.insert(9, QString::number(minValue+1));
                strList.removeAt(10);
            }
            if(event->pos().x() > rectLeftRight.at(move_i+1).x())
            {
                int value = rectLeftRight.at(move_i+1).x()-1;
                value = (value+1-Ax)*(maxFrequency-minValue)/10/60+minValue;//转换成标示坐标值
                strList.insert(9, QString::number(value-1));//防止两条线重合
                strList.removeAt(10);
            }
        }

        if(move_i > 0 && move_i < rectLeftRight.size()-1)//若选择的是除了第一个和最后一个之外的其他区域
        {
            if(event->pos().x()>rectLeftRight.at(move_i-1).x() && event->pos().x() < rectLeftRight.at(move_i+1).x())//若移动未超过前后界限
            {
                int value = event->pos().x();
                value = (value+1-Ax)*(maxFrequency-minValue)/10/60+minValue;//转换成标示坐标值
                int bandIndex = move_i/2;
                if(move_i % 2 == 0)
                {
                    strList.insert(9+bandIndex*4, QString::number(value));
                    strList.removeAt(10+bandIndex*4);
                }
                if(move_i % 2 == 1)
                {
                    strList.insert(10+bandIndex*4, QString::number(value));
                    strList.removeAt(11+bandIndex*4);
                }
            }
            if(event->pos().x() < rectLeftRight.at(move_i-1).x())//若移动超过前面界限
            {
                int value = rectLeftRight.at(move_i-1).x();
                value = (value+1-Ax)*(maxFrequency-minValue)/60/10+minValue;//转换成标示坐标值
                int bandIndex = move_i/2;
                if(move_i % 2 == 0)
                {
                    strList.insert(9+bandIndex*4, QString::number(value+1));
                    strList.removeAt(10+bandIndex*4);
                }
                if(move_i % 2 == 1)
                {
                    strList.insert(10+bandIndex*4, QString::number(value+1));
                    strList.removeAt(11+bandIndex*4);
                }
            }
            if(event->pos().x() > rectLeftRight.at(move_i+1).x())//若移动超过后面界限
            {
                int value = rectLeftRight.at(move_i+1).x();
                value = (value+1-Ax)*(maxFrequency-minValue)/60/10+minValue;//转换成标示坐标值
                int bandIndex = move_i/2;
                if(move_i % 2 == 0)
                {
                    strList.insert(9+bandIndex*4, QString::number(value-1));
                    strList.removeAt(10+bandIndex*4);
                }
                if(move_i % 2 == 1)
                {
                    strList.insert(10+bandIndex*4, QString::number(value-1));
                    strList.removeAt(11+bandIndex*4);
                }
            }
        }

        if(move_i == rectLeftRight.size()-1)//若选择的是最后一个区域
        {
            if(event->pos().x() < Bx && event->pos().x() > rectLeftRight.at(move_i-1).x())
            {
                int value = event->pos().x();
                value = (value+1-Ax)*(maxFrequency-minValue)/10/60+minValue;//转换成标示坐标值
                strList.insert(strList.size()-3, QString::number(value));
                strList.removeAt(strList.size()-3);
            }
            if(event->pos().x() < rectLeftRight.at(move_i-1).x())
            {
                int value = rectLeftRight.at(move_i-1).x();
                value = (value+1-Ax)*(maxFrequency-minValue)/60/10+minValue;//转换成标示坐标值
                strList.insert(strList.size()-3, QString::number(value+1));
                strList.removeAt(strList.size()-3);
            }
            if(event->pos().x() > Bx)
            {
                strList.insert(strList.size()-3, QString::number(maxFrequency-1));
                strList.removeAt(strList.size()-3);
            }
        }
        currentAreaBand = strList;
        moveControl_1 = false;
        setCursor(Qt::ArrowCursor);
    }

	if(moveControl_2 == true && event->button() == Qt::LeftButton)//上下移动
    {
        if(move_j == 0)//若选择的是第一个区域
        {
            if(event->pos().y() > Ay && event->pos().y() < By)
            {
                int value = event->pos().y();
                value = (By-value-1)*(maxAlpha-minValue)/5/30+minValue;//转换成标示坐标值
                strList.insert(8, QString::number(value));
                strList.removeAt(9);
            }
            if(event->pos().y() > By)
            {
                strList.insert(8, QString::number(minValue+1));
                strList.removeAt(9);
            }
            if(event->pos().y() < Ay)
            {
                strList.insert(8, QString::number(maxAlpha-1));
                strList.removeAt(9);
            }
        }
        if(move_j > 0 && move_j < rectUpDown.size()-1)//若选择的是除了第一和最后一个之外的区域
        {
            int bandIndex = (move_j-1)/2;
            if(event->pos().y() > Ay && event->pos().y() < By)
            {
                int value = event->pos().y();
                value = (By-value-1)*(maxAlpha-minValue)/5/30+minValue;//转换成标示坐标值
                if(move_j % 2 == 1)
                {
                    strList.insert(11+bandIndex*4, QString::number(value));
                    strList.removeAt(12+bandIndex*4);
                }
                if(move_j % 2 == 0)
                {
                    strList.insert(12+bandIndex*4, QString::number(value));
                    strList.removeAt(13+bandIndex*4);
                }
            }
            if(event->pos().y() < Ay)
            {
                if(move_j % 2 == 1)
                {
                    strList.insert(11+bandIndex*4, QString::number(maxAlpha-1));
                    strList.removeAt(12+bandIndex*4);
                }
                if(move_j % 2 == 0)
                {
                    strList.insert(12+bandIndex*4, QString::number(maxAlpha-1));
                    strList.removeAt(13+bandIndex*4);
                }
            }
            if(event->pos().y() > By)
            {
                if(move_j % 2 == 1)
                {
                    strList.insert(11+bandIndex*4, QString::number(minValue+1));
                    strList.removeAt(12+bandIndex*4);
                }
                if(move_j % 2 == 0)
                {
                    strList.insert(12+bandIndex*4, QString::number(minValue+1));
                    strList.removeAt(13+bandIndex*4);
                }
            }
        }
        if(move_j == rectUpDown.size()-1)//若选择的是最后一个区域
        {
            if(event->pos().y() > Ay && event->pos().y() < By)
            {
                int value = event->pos().y();
                value = (By-value-1)*(maxAlpha-minValue)/5/30+minValue;//转换成标示坐标值
                strList.insert(strList.size()-1, QString::number(value));
                strList.removeAt(strList.size()-1);
            }
            if(event->pos().y() > By)
            {
                strList.insert(strList.size()-1, QString::number(minValue+1));
                strList.removeAt(strList.size()-1);
            }
            if(event->pos().y() < Ay)
            {
                strList.insert(strList.size()-1, QString::number(maxAlpha-1));
                strList.removeAt(strList.size()-1);
            }
        }
        //数据变换完成，更新绘制
        currentAreaBand = strList;
        moveControl_2 = false;
        setCursor(Qt::ArrowCursor);
    }    
    emit bandValueChanged();
	repaint();
}

void bandInfo::paintEvent(QPaintEvent *event)
{
    QPainter *painter = new QPainter(this);
    QPen pen;
    pen.setColor(QColor(0, 154, 205));
    pen.setWidth(2);
    painter->setPen(pen);

    painter->drawLine(pntC, pntE);
    painter->drawLine(pntE, pntD);
    painter->drawEllipse(QPoint(pntC.x(), pntC.y()), 2, 2);
    painter->drawEllipse(QPoint(pntD.x(), pntD.y()), 2, 2);

    for(int i=1; i<6; i++)
        painter->drawLine(QPoint(pntA.x(), pntA.y()+(i-1)*30), QPoint(pntA.x()+3, pntA.y()+(i-1)*30));//绘制纵坐标点
    for(int i=0; i<10; i++)
        painter->drawLine(QPoint(pntE.x()+(i+1)*60, pntE.y()), QPoint(pntE.x()+(i+1)*60, pntE.y()-3));//绘制横坐标点

    for(int i=1; i<5; i++)
    {
        int k = 5-i;
        painter->drawText(QRect(pntA.x()-20, pntA.y()+(i-1)*30+25, 13, 10), QString::number((maxAlpha-minValue)/5*k+minValue));//绘制纵向坐标值
    }

    for(int i=1; i<10; i++)
    {
        painter->drawText(QRect(pntE.x()-6+i*60, pntE.y()+3, 20, 10), QString::number((maxFrequency-minValue)/10*i+minValue));//绘制横向坐标值
    }

    drawBand(painter);
}

void bandInfo::drawBand(QPainter *painter)
{
    QStringList currentList = currentAreaBand;	

    if(currentList.size() == 0) return;
    if(currentList.at(7).toInt() == 0) return;
    if(1)
    {
        QPen pen;
        int Ax = pntA.x();//537
        int Ey = pntE.y();//692
        int Bx = pntB.x();//1137
        int bandIndex = currentList.at(7).toInt();
        if(bandIndex == 1)
        {
            QVector<QPoint> pntVec;
            pntVec.append(QPoint(Ax, Ey-(currentList.at(8).toInt()-minValue)*30*5/(maxAlpha-minValue)));
            pntVec.append(QPoint((currentList.at(9).toInt()-minValue)*60*10/(maxFrequency-minValue)+Ax, Ey-(currentList.at(8).toInt()-minValue)*30*5/(maxAlpha-minValue)));

            int s = (currentList.at(9).toInt()-minValue)*60*10/(maxFrequency-minValue)+Ax;
            int e = (currentList.at(10).toInt()-minValue)*60*10/(maxFrequency-minValue)+Ax;
            int ha = Ey-(currentList.at(11).toInt()-minValue)*30*5/(maxAlpha-minValue);
            int la = Ey-(currentList.at(12).toInt()-minValue)*30*5/(maxAlpha-minValue);
            pntVec.append(QPoint(s, ha));
            pntVec.append(QPoint(e, ha));
            pntVec.append(QPoint(e, la));
            pntVec.append(QPoint(Bx, la));

            pen.setColor(penColor.at(currentList.at(5).toInt()));
            pen.setWidth(3);
            painter->setPen(pen);

            drawPntVec.clear();
            int i = 0;
            while(pntVec.at(i).x() <= Ax) i += 1;
            int j = pntVec.size()-1;
            while(pntVec.at(j).x() >= Bx) j -= 1;

            drawPntVec.append(QPoint(Ax, pntVec.at(i).y()));
            for(int k=i; k<=j; k++)
                drawPntVec.append(pntVec.at(k));
            drawPntVec.append(QPoint(Bx, pntVec.at(j).y()));
            painter->drawPolyline(drawPntVec);
        }

        if(bandIndex >= 2)
        {
            QVector<QPoint> pntVec;
            pntVec.append(QPoint(Ax, Ey-(currentList.at(8).toInt()-minValue)*30*5/(maxAlpha-minValue)));
            pntVec.append(QPoint((currentList.at(9).toInt()-minValue)*60*10/(maxFrequency-minValue)+Ax, Ey-(currentList.at(8).toInt()-minValue)*30*5/(maxAlpha-minValue)));

            for(int i=0; i<bandIndex; i++)
            {
                if(i<bandIndex-1)
                {
                    int s = (currentList.at(9+i*4).toInt()-minValue)*60*10/(maxFrequency-minValue)+Ax;
                    int e = (currentList.at(10+i*4).toInt()-minValue)*60*10/(maxFrequency-minValue)+Ax;
                    int ha = Ey-(currentList.at(11+i*4).toInt()-minValue)*30*5/(maxAlpha-minValue);
                    int la = Ey-(currentList.at(12+i*4).toInt()-minValue)*30*5/(maxAlpha-minValue);
                    pntVec.append(QPoint(s, ha));
                    pntVec.append(QPoint(e, ha));
                    pntVec.append(QPoint(e, la));
                    pntVec.append(QPoint((currentList.at(13+i*4).toInt()-minValue)*60*10/(maxFrequency-minValue)+Ax, la));
                }
                if(i==bandIndex-1)
                {
                    int s = (currentList.at(9+i*4).toInt()-minValue)*60*10/(maxFrequency-minValue)+Ax;
                    int e = (currentList.at(10+i*4).toInt()-minValue)*60*10/(maxFrequency-minValue)+Ax;
                    int ha = Ey-(currentList.at(11+i*4).toInt()-minValue)*30*5/(maxAlpha-minValue);
                    int la = Ey-(currentList.at(12+i*4).toInt()-minValue)*30*5/(maxAlpha-minValue);
                    pntVec.append(QPoint(s, ha));
                    pntVec.append(QPoint(e, ha));
                    pntVec.append(QPoint(e, la));
                    pntVec.append(QPoint(Bx, la));
                }
            }
            pen.setColor(penColor.at(currentList.at(5).toInt()));
            pen.setWidth(3);
            painter->setPen(pen);

            drawPntVec.clear();
            int i2 = 0;
            while(pntVec.at(i2).x() <= Ax) i2 += 1;
            int j = pntVec.size()-1;
            while(pntVec.at(j).x() >= Bx) j -= 1;

            drawPntVec.append(QPoint(Ax, pntVec.at(i2).y()));
            for(int k=i2; k<=j; k++)
                drawPntVec.append(pntVec.at(k));
            drawPntVec.append(QPoint(Bx, pntVec.at(j+1).y()));
            painter->drawPolyline(drawPntVec);
        }
    }
}

void bandInfo::contextMenuEvent(QContextMenuEvent *event)
{
    QRect rectBand(pntA, pntB);

    QVector<QPoint> oriPntVec;
    for(int i=0; i< drawPntVec.size(); i++)
        oriPntVec.append(drawPntVec.at(i));

    if(rectBand.contains(event->pos()) && oriPntVec.size() != 0)//band删除相关
    {
        rectDelete.clear();
        for(int i=1; i<oriPntVec.size()-1; i++)//计算删除区域
        {
            rectDelete.append(QRect(oriPntVec.at(i+1).x(), oriPntVec.at(i+1).y(), oriPntVec.at(i+2).x()-oriPntVec.at(i+1).x(),
                                    oriPntVec.at(i+3).y()-oriPntVec.at(i+2).y()));
            i += 3;
        }
        for(int i = 0; i<rectDelete.size(); i++)
        {
            if(rectDelete.at(i).contains(event->pos()))
            {
                delete_k = i;
				rightMenu->addAction(deleteBandAction);
            }
        }
    }
		rightMenu->addAction(addBand);
		rightMenu->exec(QCursor::pos());	
}

void bandInfo::enterEvent(QEvent *event)
{
	if(event->Enter == 10)
	{
		this->setCursor(Qt::ArrowCursor);
	}
}

void bandInfo::addBandHere()
{
	QStringList str = this->currentAreaBand;
	int j = str.at(7).toInt();
	if(j == 0)
	{
		QString index;
		int i = minValue+1;
		index.setNum(i);
		str.append(index); 

		i = (maxFrequency-minValue)/10;
		index.setNum(i);
		str.append(index);               //插入最小值

		i = (maxFrequency-minValue)*3/10;
		index.setNum(i);
		str.append(index);               //插入最大值

		i = (maxAlpha-minValue)/5*3;
		index.setNum(i);
		str.append(index);               //插入alpha值

		i = minValue+1;
		index.setNum(i);
		str.append(index);

		i = str.at(7).toInt()+1;        
		index.setNum(i);
		str.insert(7, index);           //改变band个数值
		str.removeAt(8);
	}
	if(j>0)
	{
		int l = str.at(str.size()-3).toInt();
		QString index;

		int i = (maxFrequency-l)/3+l;
		index.setNum(i);
		str.append(index);             //插入最小值

		i = (maxFrequency-l)/3*2+l;
		index.setNum(i);
		str.append(index);             //插入最大值 

		i = (maxAlpha-minValue)/5*3;
		index.setNum(i);
		str.append(index);            //插入alpha值

		i = minValue+1;
		index.setNum(i);
		str.append(index);

		i = str.at(7).toInt()+1;
		index.setNum(i);
		str.insert(7, index);        //改变band个数值
		str.removeAt(8);
	}

	this->currentAreaBand = str;
	emit addBandChanged();
	this->repaint();
}
