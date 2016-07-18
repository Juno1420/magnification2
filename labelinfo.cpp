#include "labelinfo.h"

#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>
#include <QRegion>

labelInfo::labelInfo(QWidget *parent) :
    QWidget(parent),
	fileName(),
	frameID(0)
{
    setMouseTracking(true);	
    for(int i=0; i<19; i++)
    {
        poiIndexLabel[i] = 0;
    }                        //-------------------------------------------热点文件设定不读取-------------------------

	/*QFile file("E:/poi3.csv");
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
		qDebug() << file.errorString();
	int indexTest = 0;//计数变量
	while(!file.atEnd())
	{
		QString str = file.readLine();
		QStringList list = str.split(";", QString::SkipEmptyParts);
		QStringList list2 = list.at(2).split("/", QString::SkipEmptyParts);

		QString s1("丽人");
		QString s2("运动健身");
		QString s3("购物");
		QString s4("医疗");
		QString s5("汽车服务");
		QString s6("transitfacilities");
		QString s7("realestate");
		QString s8("行政区划");
		QString s9("自然地物");
		QString s10("政府机构");
		QString s11("教育培训");
		QString s12("休闲娱乐");
		QString s13("文化传媒");
		QString s14("美食");
		QString s15("宾馆");
		QString s16("旅游景点");
		QString s17("生活服务");
		QString s18("公司企业");
		QString s19("金融");

		if(list2.at(0) == s1)
			poiVec1.append(list);
		if(list2.at(0) == s2)
			poiVec2.append(list);
		if(list2.at(0) == s3)
			poiVec3.append(list);
		if(list2.at(0) == s4)
			poiVec4.append(list);
		if(list2.at(0) == s5)
			poiVec5.append(list);
		if(list2.at(0) == s6)
			poiVec6.append(list);
		if(list2.at(0) == s7)
			poiVec7.append(list);
		if(list2.at(0) == s8)
			poiVec8.append(list);
		if(list2.at(0) == s9)
			poiVec9.append(list);
		if(list2.at(0) == s10)
			poiVec10.append(list);
		if(list2.at(0) == s11)
			poiVec11.append(list);
		if(list2.at(0) == s12)
			poiVec12.append(list);
		if(list2.at(0) == s13)
			poiVec13.append(list);
		if(list2.at(0) == s14)
			poiVec14.append(list);
		if(list2.at(0) == s15)
			poiVec15.append(list);
		if(list2.at(0) == s16)
			poiVec16.append(list);
		if(list2.at(0) == s17)
			poiVec17.append(list);
		if(list2.at(0) == s18)
			poiVec18.append(list);
		if(list2.at(0) == s19)
			poiVec19.append(list);
		qDebug() << indexTest;
		indexTest++;
	}
	file.close();*/

    colorVec.append(QColor(0, 245, 255)); //锰色
    colorVec.append(QColor(255, 48, 48));//红  
    colorVec.append(QColor(255, 0, 255));//紫色
    colorVec.append(QColor(0, 255, 0));//绿
    colorVec.append(QColor(0, 191, 255));//蓝
    colorVec.append(QColor(255, 255, 0));//镉黄

    scribble = false;
    index = 0;
    currentArea = 0;
	colorIndex = 0;

    deleteLabelArea = new QAction("deleteLabelAreaHere", this);
    connect(deleteLabelArea, SIGNAL(triggered()), this, SLOT(deleteArea()));

    rightMenu = new QMenu(this);
    rightMenu->addAction(deleteLabelArea);

    sourcePix = new QPixmap("E:/QtCode/Maginification2/images/mapcolor.png");
    resultPix = new QPixmap("E:/QtCode/Maginification2/images/mapcolor.png");

    frameID = 0;
    threshhold_ = 0;

	if(areaList.size() == 0)             //此处把arealist初始化，加入第一个全局变量
	{
		QStringList globalArea;
		globalArea.append("0");
		globalArea.append("0");
		globalArea.append("0");
		globalArea.append("511");
		globalArea.append("511");
		globalArea.append("0");//颜色
		globalArea.append("Rectangle");//形状
		globalArea.append("0");//band数目
		areaList.append(globalArea);
	}
}

void labelInfo::mousePressEvent(QMouseEvent *event)
{
    QRect rect(0, 0, 512, 512);
    if(event->button() == Qt::LeftButton
            && rect.contains(event->pos())
            && (shape == "Rectangle"|| shape == "Circle")
            && (this->cursor().shape() == Qt::CrossCursor || this->cursor().shape() == Qt::SizeAllCursor))
    {
        scribble = true;
        index += 1;
        startPnt = event->pos();
    }

    if(event->button() == Qt::LeftButton && this->cursor().shape() == Qt::ArrowCursor)
    {
        if(areaList.size() > 1)//左键移动选择Area区域的计算
        {
            regionList.clear();
            for(int i=1; i<areaList.size(); i++)//计算当下所有的Area区域，保存在regionAll中
            {                
                QStringList strList = areaList.at(i);
                int x = strList.at(1).toInt();
                int y = strList.at(2).toInt();
                int w = strList.at(3).toInt()-strList.at(1).toInt();
                int h = strList.at(4).toInt()-strList.at(2).toInt();
                if(strList.at(6) == "Rectangle")
                    regionList.append(QRegion(QRect(x, y, w, h)));
                if(strList.at(6) == "Circle")
                    regionList.append(QRegion(QRect(x, y, w, h), QRegion::Ellipse));
            }
        }
        if(regionList.size() != 0)
        {
            for(int i=0; i<regionList.size(); i++)
                if(regionList.at(i).contains(event->pos()))
                {
                    currentArea = i+1;
                    emit currentTableChanged();//设定当前选定行
                    repaint();
                }
        }
    }
}

void labelInfo::mouseMoveEvent(QMouseEvent *event)
{
    QRect rect(0, 0, 512, 512);
    if(event->buttons() == Qt::LeftButton
            && scribble && rect.contains(event->pos())
            && (this->cursor().shape() == Qt::CrossCursor || this->cursor().shape() == Qt::SizeAllCursor))
    {
        endPnt = event->pos();
        QRect rectDraw(startPnt, endPnt);
        update(rectDraw);
    }
}

void labelInfo::mouseReleaseEvent(QMouseEvent *event)
{
    QRect rect(0, 0, 512, 512);
    if(event->button() == Qt::LeftButton && scribble == true && rect.contains(event->pos()))
    {
        endPnt = event->pos();

        QStringList areaIndex;
        QString j;
        j.setNum(areaList.size());
        areaIndex.append(j);//插入区域编号-0
        j.setNum(startPnt.x());
        areaIndex.append(j);
        j.setNum(startPnt.y());
        areaIndex.append(j);//插入区域起始点横纵坐标-1,-2
        j.setNum(endPnt.x());
        areaIndex.append(j);
        j.setNum(endPnt.y());
        areaIndex.append(j);//插入区域终点横纵坐标-3, -4
        j.setNum(colorIndex);
		colorIndex = (colorIndex+1)%6;
        areaIndex.append(j);//插入颜色序号-5
        areaIndex.append(shape);//插入形状-6
        QString count;
        int k = 0;
        count.setNum(k);
        areaIndex.append(count);//插入band数目-7

        areaList.append(areaIndex);

        currentArea = areaIndex.at(0).toInt();
        update(rect);
        scribble = false;
        emit tableChanged();
    }
}

void labelInfo::contextMenuEvent(QContextMenuEvent *event)//右键删除区域
{
    if(areaList.size() > 1)//计算删除区域
    {
        regionList.clear();
        for(int i=1; i<areaList.size(); i++)//计算当下所有的Area区域，保存在regionAll中
        {                                   //区域从第一个开始计算，忽略第0个
            QStringList strList = areaList.at(i);
            int x = strList.at(1).toInt();
            int y = strList.at(2).toInt();
            int w = strList.at(3).toInt()-strList.at(1).toInt();
            int h = strList.at(4).toInt()-strList.at(2).toInt();
            if(strList.at(6) == "Rectangle")
                regionList.append(QRegion(QRect(x, y, w, h)));
            if(strList.at(6) == "Circle")
                regionList.append(QRegion(QRect(x, y, w, h), QRegion::Ellipse));
        }
    }
    if(regionList.size() == 0) return;

    for(int i=0; i<regionList.size(); i++)
    {
        if(regionList.at(i).contains(event->pos()))
        {
            rightMenu->clear();
            deleteAreaIndex = i+1;            
            rightMenu->exec(QCursor::pos());
            event->accept();
        }
    }
}

void labelInfo::paintEvent(QPaintEvent *event)
{
    QPainter *painter = new QPainter(this);
    painter->drawPixmap(QPoint(0, 0), *sourcePix);
    painter->drawPixmap(QPoint(512, 0), *resultPix);
    painter->drawLine(0, 0, 0, 512);
    painter->drawLine(0, 0, 431, 0);
    painter->drawLine(512, 0, 1024, 0);
    painter->drawLine(0, 511, 1024, 512);
    painter->drawLine(1023, 0, 1023, 512);
    painter->drawLine(511, 0, 511, 512);

    drawPoi(painter);
    drawAreaTo(painter);
    drawAllAreas(painter);

    if(areaList.size() > 1)
    {
		for(int i=1; i<areaList.size(); i++)
		{
			painter->setBrush(QColor(0, 0, 0, 0));
			QStringList strList = areaList.at(i);
			QRect rect(strList.at(1).toInt()+512, strList.at(2).toInt(), strList.at(3).toInt()-strList.at(1).toInt(), strList.at(4).toInt()-strList.at(2).toInt());
			if(strList.at(6) == "Rectangle")
			{
				QPen penBlack;
				penBlack.setWidth(2);
				penBlack.setColor(Qt::darkGray);
				painter->setPen(penBlack);
				painter->drawRect(rect.x()-1, rect.y()-1, rect.width()+2, rect.height()+2);//绘制阴影
				painter->drawRect(rect.x()+1, rect.y()+1, rect.width()-2, rect.height()-2);//绘制阴影

				QPen pen;
				pen.setWidth(2);
				int index = strList.at(5).toInt();
				pen.setColor(colorVec.at(index % 6));
				painter->setPen(pen);
				painter->drawRect(rect);                           //绘制矩形
			}
			else
			{
				if(strList.at(6) == "Circle")
				{
					QPen penBlack;
					penBlack.setWidth(2);
					penBlack.setColor(Qt::darkGray);
					painter->setPen(penBlack);
					painter->drawEllipse(rect.x()-1, rect.y()-1, rect.width()+2, rect.height()+2);//绘制阴影
					painter->drawEllipse(rect.x()+1, rect.y()+1, rect.width()-2, rect.height()-2);//绘制阴影

					QPen pen;
					pen.setWidth(2);
					int index = strList.at(5).toInt();
					pen.setColor(colorVec.at(index % 6));
					painter->setPen(pen);
					painter->drawEllipse(rect);                    //绘制矩形
				}
			}
		}
    }
}

void labelInfo::drawAreaTo(QPainter *painter)
{
    if(scribble == true)
    {
        QPen pen;
        pen.setWidth(3);
        pen.setColor(colorVec.at(colorIndex));
        painter->setPen(pen);
        if(shape == "Rectangle")
            painter->drawRect(QRect(startPnt, endPnt));
        if(shape == "Circle")
            painter->drawEllipse(QRect(startPnt, endPnt));
    }
}

void labelInfo::drawAllAreas(QPainter *painter)
{
    for(int i=1; i<areaList.size(); i++)
    {
        QStringList strList = areaList.at(i);
        if(i == currentArea)                    //当前区域
        {
            QRect rect(strList.at(1).toInt(), strList.at(2).toInt(), strList.at(3).toInt()-strList.at(1).toInt(), strList.at(4).toInt()-strList.at(2).toInt());
            if(strList.at(6) == "Rectangle")
            {                
				QPen penBlack;
				penBlack.setWidth(2);
				penBlack.setColor(Qt::darkGray);
				painter->setPen(penBlack);
				painter->setBrush(QColor(0, 0, 0, 0));
				painter->drawRect(rect.x()-1, rect.y()-1, rect.width()+2, rect.height()+2);//绘制阴影
				painter->drawRect(rect.x()+1, rect.y()+1, rect.width()-2, rect.height()-2);//绘制阴影

				QPen pen;
				pen.setWidth(2);
				int index = strList.at(5).toInt();
				pen.setColor(colorVec.at(index % 6));
				painter->setPen(pen);
				painter->drawRect(rect);

				int m = 4;                                            //绘制手柄
				int n = 15;
				int x = strList.at(3).toInt();
				int y = strList.at(4).toInt();
				QPoint f = QPoint(x, y);
				QPoint a = QPoint(x-m, y);
				QPoint b = QPoint(x+n-m, y+n);
				QPoint c = QPoint(x+n, y+n);
				QPoint d = QPoint(x+n, y+n-m);
				QPoint e = QPoint(x, y-m);

				QPen penHandle;
				penHandle.setWidth(2);
				penHandle.setColor(Qt::black);
				painter->setBrush(colorVec.at(index % 6));
				painter->setPen(penHandle);
				const QPoint points1[4] = {a, b, d, e};
				painter->drawPolygon(points1, 4);

				QPen penBigCircle;
				penBigCircle.setColor(Qt::black);
				penBigCircle.setWidth(2);
				painter->setBrush(colorVec.at(index % 6));
				painter->setPen(penBigCircle);
				painter->drawEllipse(QPoint(f.x()-3, f.y()-3), 8, 8);           //绘制放大镜圆

				QPen penLittleCircle;                       //绘制当前图形的选择圆形
				penLittleCircle.setColor(Qt::blue);
				penLittleCircle.setWidth(3);
				penLittleCircle.setBrush(Qt::blue);
				painter->setPen(penLittleCircle);
				int circleR = 3;
				painter->drawEllipse(QPoint(rect.x()+rect.width()/2, rect.y()), circleR, circleR);
				painter->drawEllipse(QPoint(rect.x()+rect.width(), rect.y()+rect.height()/2), circleR, circleR);
				painter->drawEllipse(QPoint(rect.x()+rect.width()/2, rect.y()+rect.height()), circleR, circleR);
				painter->drawEllipse(QPoint(rect.x(), rect.y()+rect.height()/2), circleR, circleR);
            }
            if(strList.at(6) == "Circle")
			{

				int m = (strList.at(3).toInt()-strList.at(1).toInt())/2;                      //开始绘制手柄             
				int n = (strList.at(4).toInt()-strList.at(2).toInt())/2;
				int x = m*0.707 + strList.at(3).toInt()/2 + strList.at(1).toInt()/2 + 1;
				int y = n*0.707 + strList.at(4).toInt()/2 + strList.at(2).toInt()/2 + 1;
				QPoint a = QPoint(x-5, y+5);
				QPoint b = QPoint(x+25, y+35);
				QPoint c = QPoint(x+35, y+25);
				QPoint d = QPoint(x+5, y-5);

				QPen penHandle;
				penHandle.setWidth(2);
				penHandle.setColor(Qt::black);
				int index = strList.at(5).toInt();
				painter->setBrush(colorVec.at(index % 6));
				painter->setPen(penHandle);
				const QPoint points1[4] = {a, b, c, d};
				painter->drawPolygon(points1, 4);                           //绘制手柄结束   

				QPen penBlack;
				penBlack.setWidth(2);
				penBlack.setColor(Qt::darkGray);
				painter->setPen(penBlack);
				painter->setBrush(QColor(0, 0, 0, 0));
				painter->drawEllipse(rect.x()-1, rect.y()-1, rect.width()+2, rect.height()+2);//绘制阴影
				painter->drawEllipse(rect.x()+1, rect.y()+1, rect.width()-2, rect.height()-2);//绘制阴影

				QPen pen;
				pen.setWidth(2);
				pen.setColor(colorVec.at(index % 6));
				painter->setPen(pen);
				painter->drawEllipse(rect);    

				QPen penLittleCircle;
				penLittleCircle.setColor(Qt::blue);
				penLittleCircle.setWidth(3);
				painter->setPen(penLittleCircle);
				int circleR = 3;

				painter->drawEllipse(QPoint(rect.x()+rect.width()/2, rect.y()), circleR, circleR);
				painter->drawEllipse(QPoint(rect.x()+rect.width(), rect.y()+rect.height()/2), circleR, circleR);
				painter->drawEllipse(QPoint(rect.x()+rect.width()/2, rect.y()+rect.height()), circleR, circleR);
				painter->drawEllipse(QPoint(rect.x(), rect.y()+rect.height()/2), circleR, circleR);
            }
        }
        else                  //非当前区域
        {
            QRect rect(strList.at(1).toInt(), strList.at(2).toInt(), strList.at(3).toInt()-strList.at(1).toInt(), strList.at(4).toInt()-strList.at(2).toInt());
            if(strList.at(6) == "Rectangle")
			{
				QPen penBlack;
				penBlack.setWidth(2);
				penBlack.setColor(Qt::darkGray);
				painter->setPen(penBlack);
				painter->setBrush(QColor(0, 0, 0, 0));
				painter->drawRect(rect.x()-1, rect.y()-1, rect.width()+2, rect.height()+2);//绘制阴影
				painter->drawRect(rect.x()+1, rect.y()+1, rect.width()-2, rect.height()-2);//绘制阴影

				QPen pen;
				pen.setWidth(2);
				int index = strList.at(5).toInt();
				pen.setColor(colorVec.at(index % 6));
				painter->setPen(pen);
				painter->drawRect(rect);

				int m = 4;                                            //开始绘制手柄
				int n = 15;
				int x = strList.at(3).toInt();
				int y = strList.at(4).toInt();
				QPoint f = QPoint(x, y);
				QPoint a = QPoint(x-m, y);
				QPoint b = QPoint(x+n-m, y+n);
				QPoint c = QPoint(x+n, y+n);
				QPoint d = QPoint(x+n, y+n-m);
				QPoint e = QPoint(x, y-m);

				QPen penHandle;
				penHandle.setWidth(2);
				penHandle.setColor(Qt::black);
				painter->setBrush(colorVec.at(index % 6));
				painter->setPen(penHandle);
				const QPoint points1[4] = {a, b, d, e};
				painter->drawPolygon(points1, 4);

				QPen penBigCircle;
				penBigCircle.setColor(Qt::black);
				penBigCircle.setWidth(2);
				painter->setBrush(colorVec.at(index % 6));
				painter->setPen(penBigCircle);
				painter->drawEllipse(QPoint(f.x()-3, f.y()-3), 8, 8);           //绘制放大镜圆，结束手柄绘制
			}
            if(strList.at(6) == "Circle")
			{
				int m = (strList.at(3).toInt()-strList.at(1).toInt())/2;                      //开始绘制手柄             
				int n = (strList.at(4).toInt()-strList.at(2).toInt())/2;
				int x = m*0.707 + strList.at(3).toInt()/2 + strList.at(1).toInt()/2 + 1;
				int y = n*0.707 + strList.at(4).toInt()/2 + strList.at(2).toInt()/2 + 1;
				QPoint a = QPoint(x-5, y+5);
				QPoint b = QPoint(x+25, y+35);
				QPoint c = QPoint(x+35, y+25);
				QPoint d = QPoint(x+5, y-5);

				QPen penHandle;
				penHandle.setWidth(2);
				penHandle.setColor(Qt::black);
				int index = strList.at(5).toInt();
				painter->setBrush(colorVec.at(index % 6));
				painter->setPen(penHandle);
				const QPoint points1[4] = {a, b, c, d};
				painter->drawPolygon(points1, 4);                           //绘制手柄结束   

				QPen penBlack;
				penBlack.setWidth(2);
				penBlack.setColor(Qt::darkGray);
				painter->setPen(penBlack);
				painter->setBrush(QColor(0, 0, 0, 0));
				painter->drawEllipse(rect.x()-1, rect.y()-1, rect.width()+2, rect.height()+2);//绘制阴影
				painter->drawEllipse(rect.x()+1, rect.y()+1, rect.width()-2, rect.height()-2);//绘制阴影

				QPen pen;
				pen.setWidth(2);
				pen.setColor(colorVec.at(index % 6));
				painter->setPen(pen);
				painter->drawEllipse(rect);
			}
        }

    }
}

void labelInfo::showFrame()
{
	float *data1 = oriData[frameID];
	float *data2 = maginficationData[frameID];
    displayResult(data1, data2);//展示转换之后的label_show
    QRect rectShow(0, 0, 1024, 1024);
    update(rectShow);
}

void labelInfo::displayResult(float *data1, float *data2)
{
	QImage mapImg; 
	mapImg.load("E:/QtCode/Maginification2/images/mapcolor.png");//地图
	QImage showImg1 = QImage(mapImg.size(), QImage::Format_RGB32);//融合结果图1
	QImage showImg2 = QImage(mapImg.size(), QImage::Format_RGB32);//融合结果图2
	//showImg1.load("E:/QtCode/Maginification2/images/mapcolor.png");
	//showImg2.load("E:/QtCode/Maginification2/images/mapcolor.png");

	uchar* rgb11= showImg1.bits();
	uchar* rgb12= showImg2.bits();
	uchar* s = mapImg.bits();

	int imgHeight = showImg1.height();//512, 宽度512
	int imgWidth = showImg1.width();
	int mbytesperline = mapImg.bytesPerLine();//2048
	int sbytesperline = showImg1.bytesPerLine();//2048
	int dStep,sStep,mStep;
	float r, alpha;

	for(int i=0; i<imgHeight; i++)                               //处理原始播放数据
	{   
		dStep = i * imgWidth;
		sStep = i * sbytesperline;
		mStep = i * mbytesperline;
		for(int j=0; j<imgWidth; j++)
		{
			r = data1[dStep + j];
			alpha = (r-threshhold_)/(255.0-threshhold_);//r1/255.0;

			rgb11[sStep +j * 4]     = 38.0 * alpha + s[mStep + j * 4] * (1-alpha);
			rgb11[sStep +j * 4 + 1] = 45.0 * alpha + s[mStep + j * 4 + 1]*(1-alpha);
			rgb11[sStep +j * 4 + 2] = r   * alpha + s[mStep + j * 4 + 2]*(1-alpha);
			rgb11[sStep +j * 4 + 3] = 1;
		}
	}
	
	for(int i=0; i<imgHeight; i++)                              //处理放大数据
	{   
		dStep = i * imgWidth;
		sStep = i * sbytesperline;
		mStep = i * mbytesperline;
		for(int j=0; j<imgWidth; j++)
		{
			r = data2[dStep + j];
			alpha = (r-threshhold_)/(255.0-threshhold_);//r1/255.0;

			rgb12[sStep +j * 4]     = 38.0 * alpha + s[mStep + j * 4] * (1-alpha);
			rgb12[sStep +j * 4 + 1] = 45.0 * alpha + s[mStep + j * 4 + 1]*(1-alpha);
			rgb12[sStep +j * 4 + 2] = r    * alpha + s[mStep + j * 4 + 2]*(1-alpha);
			rgb12[sStep +j * 4 + 3] = 1;
		}
	}    

	float aa, max = 0;
	for(int i=0; i<imgHeight; i++)                              //处理放大数据
	{   
		dStep = i * imgWidth;
		sStep = i * sbytesperline;
		mStep = i * mbytesperline;
		for(int j=0; j<imgWidth; j++)
		{
			aa = data1[dStep + j]- data2[dStep + j];
			
			if(aa<0)
				aa =-aa;
			if(aa>1)
				int llll=0;
			if(max < aa)
				max = aa;
		}
	}

	
    QPixmap tempPix1 = sourcePix->fromImage(showImg1, Qt::AutoColor);
    *sourcePix = tempPix1;
	QPixmap tempPix2 = resultPix->fromImage(showImg2, Qt::AutoColor);
	*resultPix = tempPix2;
}

void labelInfo::drawPoi(QPainter *painter)
{
	return;
    bool poiCheckedSize = false;
    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(2);
    painter->setPen(pen);

    QVector<bool> poiChecked;

    for(int i=0; i<19; i++)
    {
        if(poiIndexLabel[i] == 0)
            poiChecked.append(false);
        else
        {
            poiChecked.append(true);
            poiCheckedSize = true;
        }
    }

    if(poiCheckedSize)
    {
        if(poiChecked.at(0))
        {
            QVector<QStringList>::iterator strVec = poiVec1.begin();
            while(strVec != poiVec1.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(1))
        {
            QVector<QStringList>::iterator strVec = poiVec2.begin();
            while(strVec != poiVec2.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(2))
        {
            QVector<QStringList>::iterator strVec = poiVec3.begin();
            while(strVec != poiVec3.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(3))
        {
            QVector<QStringList>::iterator strVec = poiVec4.begin();
            while(strVec != poiVec4.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(4))
        {
            QVector<QStringList>::iterator strVec = poiVec5.begin();
            while(strVec != poiVec5.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(5))
        {
            QVector<QStringList>::iterator strVec = poiVec6.begin();
            while(strVec != poiVec6.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(6))
        {
            QVector<QStringList>::iterator strVec = poiVec7.begin();
            while(strVec != poiVec7.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(7))
        {
            QVector<QStringList>::iterator strVec = poiVec8.begin();
            while(strVec != poiVec8.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(8))
        {
            QVector<QStringList>::iterator strVec = poiVec9.begin();
            while(strVec != poiVec9.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(9))
        {
            QVector<QStringList>::iterator strVec = poiVec10.begin();
            while(strVec != poiVec10.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(10))
        {
            QVector<QStringList>::iterator strVec = poiVec11.begin();
            while(strVec != poiVec11.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(11))
        {
            QVector<QStringList>::iterator strVec = poiVec12.begin();
            while(strVec != poiVec12.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(12))
        {
            QVector<QStringList>::iterator strVec = poiVec13.begin();
            while(strVec != poiVec13.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(4).toFloat()-120.4122)*10000/1*512/4532, (present.at(5).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(13))
        {
            QVector<QStringList>::iterator strVec = poiVec14.begin();
            while(strVec != poiVec14.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(14))
        {
            QVector<QStringList>::iterator strVec = poiVec15.begin();
            while(strVec != poiVec15.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(15))
        {
            QVector<QStringList>::iterator strVec = poiVec16.begin();
            while(strVec != poiVec16.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(16))
        {
            QVector<QStringList>::iterator strVec = poiVec17.begin();
            while(strVec != poiVec17.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(17))
        {
            QVector<QStringList>::iterator strVec = poiVec18.begin();
            while(strVec != poiVec18.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
        if(poiChecked.at(18))
        {
            QVector<QStringList>::iterator strVec = poiVec19.begin();
            while(strVec != poiVec19.end())
            {
                QStringList present = *strVec;
                painter->drawPoint((present.at(3).toFloat()-120.4122)*10000/1*512/4532, (present.at(4).toFloat()-27.8788)*10000/1*512/2613);
                strVec++;
            }
        }
    }
    poiChecked.clear();
}

void labelInfo::deleteArea()
{
    this->areaList.removeAt(deleteAreaIndex);
    for(int j=deleteAreaIndex; j<this->areaList.size(); j++)
    {
        QStringList strList = this->areaList.at(j);
        int index = strList.at(0).toInt()-1;
        strList.removeAt(0);
        strList.insert(0, QString::number(index));
        this->areaList.removeAt(j);
        this->areaList.insert(j, strList);
    }
    this->currentArea = 0;
    emit tableChanged();
    this->repaint();
}

