#include "graphicsinfo.h"

#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QString>
#include <QStringList>
#include <qmath.h>
#include <QFile>
#include <cstdlib>

GraphicsInfo::GraphicsInfo(QWidget *parent) :
    QWidget(parent),
	pramKR(40000),
	pramKS(0.005),
	pramL(0),
	timeId(0)
{
    this->setFixedSize(1174, 512);

// 	QFile file("E:/QtCode/Maginification2/lev1.csv");
// 	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
// 		qDebug() << file.errorString();
// 	pntIndex[0] = 49;  //pntIndex的值包括中心结点
// 	int index = 1;
// 	while(!file.atEnd())
// 	{
// 		QString str = file.readLine();
// 		QStringList strList = str.split(",");
// 		pntIndex[index] = strList.at(1).toInt();
// 		index += 1;
// 	}

	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(timeoutPos()));

	rePosBtn = new QPushButton("rePos", this);    //重新布局顶点位置按钮
	rePosBtn->setGeometry(505, 0, 50, 30);
	connect(rePosBtn, SIGNAL(clicked(bool)), this, SLOT(rePos()));
	data = new QVector<QVector<int> >;
	adjData = new QVector<QVector<int> >;

    initPntPos();
}

void GraphicsInfo::paintEvent(QPaintEvent *event)
{
    QPainter *painter = new QPainter(this);
	painter->setBrush(Qt::gray);
	painter->drawRect(0, 0, 1174, 512); //绘制背景

	QPen penLine;
	penLine.setWidth(1);
	penLine.setColor(QColor(0, 245, 255, 120));
	painter->setPen(penLine);
	painter->setBrush(Qt::NoBrush);
	for(int i=0; i<adjData->size(); i++)
	{
		for(int j=0; j<adjData->at(i).size(); j++)
		{
			if(adjData->at(i).at(j) > i)
			{
				QPoint s = QPoint(data->at(i).at(2), data->at(i).at(3));
				QPoint e = QPoint(data->at(adjData->at(i).at(j)).at(2), data->at(adjData->at(i).at(j)).at(3));
				painter->drawLine(s, e);              //绘制连线
			}			           
		}
	}                                

	QPen penDot;
	penDot.setWidth(2);
	penDot.setColor(Qt::black);
	painter->setPen(penDot);
	painter->setBrush(QColor(0, 0, 255, 255));
	int r = 5;
	for(int i=0; i<data->size(); i++)
		painter->drawEllipse(QPoint(data->at(i).at(2), data->at(i).at(3)), r, r);//绘制点
}

void GraphicsInfo::initPntPos()  //此函数设定初始位置和边的链接
{
	for(int i=0; i<10; i++)
	{
		QVector<int> temp;
		temp.push_back(0);  //force_x
		temp.push_back(0);  //force_y
		temp.push_back(rand()%500+10); //位置x分量
		temp.push_back(rand()%500+10); //位置y分量
		data->push_back(temp);   //顶点相关信息容器
	}

	for(int i=0; i<10; i++)
	{
		QVector<int> temp;
		temp.append(rand()%10); //随机加入三个连接顶点编号
		temp.append(rand()%10);
		temp.append(rand()%10);
		adjData->push_back(temp);//顶点连接边信息容器
	}
}

void GraphicsInfo::rePos()  //此函数重新随机安排点的位置
{
	data->clear();
	for(int i=0; i<10; i++)
	{
		QVector<int> temp;
		temp.push_back(0);
		temp.push_back(0);
		temp.push_back(rand()%500+10);
		temp.push_back(rand()%500+10);
		data->push_back(temp);
	}
	repaint();
	timer->start(30);
}

void GraphicsInfo::timeoutPos()
{
	QVector<QVector<int> > *tempData = new QVector<QVector<int> >;
	for(int i=0; i<data->size(); i++)
	{
		QVector<int> temp = data->at(i);
		QVector<int> adjTemp = adjData->at(i);

		int dx = 0;
		int dy = 0;
		for(int j=0; j<adjTemp.size(); j++)
		{
			int x = abs(data->at(adjTemp[j]).at(2) - data->at(i).at(2));
			int y = abs(data->at(adjTemp[j]).at(3) - data->at(i).at(3));
			float r = qMax(qSqrt(x*x + y*y), 1.0);
			dx += pramKR*x/qPow(r, 3);  //此两行为库伦力算法计算公式
			dy += pramKR*y/qPow(r, 3);		

// 			dx += pramKR*x/qPow(r, 2); //此两行为Qt中程序算法计算公式
// 			dy += pramKS*y/qPow(r, 2);
		}
		for(int j=0; j<adjTemp.size(); j++)
		{
			int x = abs(data->at(adjTemp[j]).at(2) - data->at(i).at(2));
			int y = abs(data->at(adjTemp[j]).at(3) - data->at(i).at(3));
			float r = qMax(qSqrt(x*x + y*y), 1.0);
			dx -= pramKS*(r-pramL)*x/r;  //此两行为库伦力算法计算公式
			dy -= pramKS*(r-pramL)*y/r;

// 			dx -= x/200;               //此两行为Qt中程序算法计算公式
// 			dy -= y/200;
		}

// 		qDebug() << temp[2] << temp[3];

		temp[2] += dx;
		temp[3] += dy;

		temp[2] = qMin(temp[2], 505);    //以下四行代码为限制点移动范围在500*500区域内，可用可不用
		temp[3] = qMin(temp[3], 505);
		temp[2] = qMax(temp[2], 10);
		temp[3] = qMax(temp[3], 10);
		
		tempData->push_back(temp);
	}
	data->clear();
	for(int i=0; i<tempData->size(); i++)
	{
		data->push_back(tempData->at(i));
	}
	repaint();
	timeId += 1;
	if(timeId > 500)    //循环次数可以改变
	{
		timer->stop();
		timeId = 0;
	}
}