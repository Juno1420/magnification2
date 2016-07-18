#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtGui>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
	graphFrame(0),
	nArea(0),
	nLobes(0)
{
    ui->setupUi(this);
    this->setWindowTitle("Illustrative Visualization");
    this->setWindowIcon(QIcon("E:/QtCode/Maginification2/images/magnification.png"));
    this->setWindowFlags(Qt::Window  | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    this->setStyleSheet("QMainWindow {background-color: rgb(211, 211, 211); border-style: outset}");

//----------------以下为界面部件初始化-------------------
    tab = new QTabBar(this);
    tab->addTab("Map");
    tab->addTab("Graph");
    tab->setCurrentIndex(1);
    connect(tab, SIGNAL(currentChanged(int)), this, SLOT(currentTab(int)));
    tab->setShape(QTabBar::TriangularNorth);
    tab->setFixedWidth(240);
    tab->move(432, 40);

    myLabelInfo = new labelInfo(this);
    myPoiInfo = new poiInfo(this);
    myTableInfo = new tableInfo(this);
    myBandInfo = new bandInfo(this);

    myLabelInfo->hide();
    //myPoiInfo->hide();
	myPoiInfo->hide();
    myTableInfo->show();
    myBandInfo->show();

    myLabelInfo->setGeometry(1, tab->height()+25, 1024, 512);
    connect(myLabelInfo, SIGNAL(tableChanged()), this, SLOT(myLabelAreaChanged()));
    connect(myLabelInfo, SIGNAL(currentTableChanged()), this, SLOT(myLabelCurrentAreaChanged()));//改变PMagData

    myPoiInfo->setGeometry(myLabelInfo->width(), tab->height()+25, 150, 450);
    connect(myPoiInfo, SIGNAL(poiIndexChanged()), this, SLOT(myPoiIndexChanged()));

    myTableInfo->setGeometry(1, myLabelInfo->height()+tab->height()+27, 500, 220);
    connect(myTableInfo, SIGNAL(tableAreaChanged()), this, SLOT(myTableCurrentAreaChanged()));
    connect(myTableInfo, SIGNAL(rightAreaChanged()), this, SLOT(myTableCurrentAreaChanged()));
    //connect(myTableInfo, SIGNAL(tableAddedBand()), this, SLOT(myTableBandChanged()));            //改变PMagData
    connect(myTableInfo, SIGNAL(tableDeleteArea()), this, SLOT(myTableBandChanged()));           //改变PMagData

    myBandInfo->setGeometry(myTableInfo->width(), myLabelInfo->height()+tab->height()+25, 750, 240);
    connect(myBandInfo, SIGNAL(bandValueChanged()), this, SLOT(myBandValueChanged()));          //改变PMagData
	connect(myBandInfo, SIGNAL(addBandChanged()), this, SLOT(myBandValueChanged()));            //改变PMagData

    GraphicsBand = new bandInfo(this);
    GraphicsTable = new tableInfo(this);
    myGraphicsInfo = new GraphicsInfo(this);

    GraphicsBand->setGeometry(myTableInfo->width(), myLabelInfo->height()+tab->height()+25, 750, 240);
    GraphicsTable->setGeometry(1, myLabelInfo->height()+tab->height()+27, 500, 220);
    myGraphicsInfo->setGeometry(0, tab->height()+25, 1174, 512);


    myGraphicsInfo->show();
    GraphicsBand->show();
    GraphicsTable->show();

//---------------------------以下为总界面上的图标-------------------------------------------------------------------
    openButton = new QPushButton(this);
    openButton->setIcon(QIcon("E:/QtCode/Maginification2/images/open.png"));
	connect(openButton, SIGNAL(clicked()), this, SLOT(setFile()));

    saveButton = new QPushButton(this);
    saveButton->setIcon(QIcon("E:/QtCode/Maginification2/images/save.png"));

    cursorButton = new QPushButton(this);
    cursorButton->setIcon(QIcon("E:/QtCode/Maginification2/images/cursor.png"));
    connect(cursorButton, SIGNAL(clicked()), this, SLOT(setShapeOri()));

    rectangleButton = new QPushButton(this);
    rectangleButton->setIcon(QIcon("E:/QtCode/Maginification2/images/rectangle.png"));
    rectangleButton->setStyleSheet("QPushButton:pressed{border-image: url(e:/rectBtn.png);}");
    connect(rectangleButton, SIGNAL(clicked()), this, SLOT(setShape1()));

    circleButton = new QPushButton(this);
    circleButton->setIcon(QIcon("E:/QtCode/Maginification2/images/Circle.png"));
    circleButton->setStyleSheet("QPushButton:pressed{border-image: url(e:/circleBtn.png);}");
    connect(circleButton, SIGNAL(clicked()), this, SLOT(setShape2()));

    sectionButton = new QPushButton(this);
    sectionButton->setIcon(QIcon("E:/QtCode/Maginification2/images/sector.png"));

    userDefinedButton = new QPushButton(this);
    userDefinedButton->setIcon(QIcon("E:/QtCode/Maginification2/images/userdefined.png"));

    chexiaoButton = new QPushButton(this);
    chexiaoButton->setIcon(QIcon("E:/QtCode/Maginification2/images/chexiao.png"));

    redoButton = new QPushButton(this);
    redoButton->setIcon(QIcon("E:/QtCode/Maginification2/images/redo.png"));

    mapiconButton = new QPushButton(this);
    mapiconButton->setIcon(QIcon("E:/QtCode/Maginification2/images/mapicon.png"));

    rangeButton = new QPushButton(this);
    rangeButton->setIcon(QIcon("E:/QtCode/Maginification2/images/range.png"));

    liebiaoButton = new QPushButton(this);
    liebiaoButton->setIcon(QIcon("E:/QtCode/Maginification2/images/liebiao.png"));

    zoominButton = new QPushButton(this);
    zoominButton->setIcon(QIcon("E:/QtCode/Maginification2/images/zoomin.png"));

    zoomoutButton = new QPushButton(this);
    zoomoutButton->setIcon(QIcon("E:/QtCode/Maginification2/images/zoomout.png"));

    moveButton = new QPushButton(this);
    moveButton->setIcon(QIcon("E:/QtCode/Maginification2/images/move.png"));

    locateButton = new QPushButton(this);
    locateButton->setIcon(QIcon("E:/QtCode/Maginification2/images/locate.png"));

    typeinButton = new QPushButton(this);
    typeinButton->setIcon(QIcon("E:/QtCode/Maginification2/images/typein.png"));

    dianzanButton = new QPushButton(this);
    dianzanButton->setIcon(QIcon("E:/QtCode/Maginification2/images/dianzan.png"));

    playButton = new QPushButton(this);
    playButton->setIcon(QIcon("E:/QtCode/Maginification2/images/play.png"));

    pauseButton = new QPushButton(this);
    pauseButton->setIcon(QIcon("E:/QtCode/Maginification2/images/pause.png"));

    firstButton = new QPushButton(this);
    firstButton->setIcon(QIcon("E:/QtCode/Maginification2/images/first.png"));

    nextButton = new QPushButton(this);
    nextButton->setIcon(QIcon("E:/QtCode/Maginification2/images/next.png"));

    horizontalSlider = new QSlider(Qt::Horizontal, this);//滑块控制已经设定好
    horizontalSlider->setTickInterval(20);
    horizontalSlider->setMinimum(0);
    horizontalSlider->setMaximum(240);//设置最大值，跟1比较，选择最大值

    connect(playButton, SIGNAL(clicked()), this, SLOT(play()));
    connect(pauseButton, SIGNAL(clicked()), this, SLOT(pause()));
    connect(firstButton, SIGNAL(clicked()), this, SLOT(first()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(next()));

    timer = new QTimer(this);
    timer->setInterval(30);
    connect(timer,SIGNAL(timeout()),this, SLOT(timerEvent()));

    HBoxLayout = new QHBoxLayout(this);
    HBoxLayout->addWidget(openButton, 0);
    HBoxLayout->addWidget(saveButton, 0);
    HBoxLayout->addWidget(cursorButton, 0);//
    HBoxLayout->addWidget(rectangleButton, 0);
    HBoxLayout->addWidget(circleButton, 0);
    HBoxLayout->addWidget(sectionButton, 0);
    HBoxLayout->addWidget(userDefinedButton, 0);
    HBoxLayout->addWidget(chexiaoButton, 0);
    HBoxLayout->addWidget(redoButton, 0);
    HBoxLayout->addWidget(mapiconButton, 0);
    HBoxLayout->addWidget(rangeButton, 0);
    HBoxLayout->addWidget(liebiaoButton, 0);
    HBoxLayout->addWidget(zoominButton, 0);
    HBoxLayout->addWidget(zoomoutButton, 0);
    HBoxLayout->addWidget(moveButton, 0);
    HBoxLayout->addWidget(locateButton, 0);
    HBoxLayout->addWidget(typeinButton, 0);
    HBoxLayout->addWidget(dianzanButton, 0);
    HBoxLayout->addWidget(playButton, 0);
    HBoxLayout->addWidget(pauseButton, 0);
    HBoxLayout->addWidget(firstButton, 0);
    HBoxLayout->addWidget(nextButton, 0);
    HBoxLayout->addWidget(horizontalSlider, 1);
    HBoxLayout->setGeometry(QRect(0, 0, 1174, 40));

//------------------------------------完成界面，设置界面尺寸---------------------------------------------------
    /*this->setGeometry(100, 100, myPoiInfo->width()+myLabelInfo->width(), 60+myLabelInfo->height()+myTableInfo->height());*/

	this->setFixedSize(myPoiInfo->width()+myLabelInfo->width(), 60+myLabelInfo->height()+myTableInfo->height());
    this->setMouseTracking(true);
}

void MainWindow::currentTab(int index)
{
	if(index == 0)
	{
		myPoiInfo->show();
		myLabelInfo->show();
		myTableInfo->show();
		myBandInfo->show();

		GraphicsBand->hide();
		GraphicsTable->hide();
		myGraphicsInfo->hide();
	}
	else if(index == 1)
	{
		myPoiInfo->hide();
		myLabelInfo->hide();
		myTableInfo->hide();
		myBandInfo->hide();

		GraphicsBand->show();
		GraphicsTable->show();
		myGraphicsInfo->show();
	}
}

void MainWindow::myPoiIndexChanged()
{
    for(int i=0; i<19; i++)
        myLabelInfo->poiIndexLabel[i] = myPoiInfo->chkIndex[i];
    myLabelInfo->repaint();
}

void MainWindow::myLabelAreaChanged()//绘制label区域，需要改变table中数据，根据区域绘制band中区域
{    
    myTableInfo->tableAreaList.clear();
    for(int i=0; i<myLabelInfo->areaList.size(); i++)
    {
        myTableInfo->tableAreaList.append(myLabelInfo->areaList.at(i));
    }
    myTableInfo->writeTable();
    myTableInfo->setCurrentRow(myLabelInfo->currentArea);
	pMagData->nArea += 1;                                                          //Area个数加1，但是不改变PMagData信息
    myBandInfo->currentAreaBand = myLabelInfo->areaList.at(myLabelInfo->currentArea);
    myBandInfo->repaint();    
}

void MainWindow::myLabelCurrentAreaChanged()//改变label中选择区域，table中选择区域也改变，band中绘制图像也改变
{
    myTableInfo->setCurrentRow(myLabelInfo->currentArea);
    myBandInfo->currentAreaBand = myLabelInfo->areaList.at(myLabelInfo->currentArea);
    myBandInfo->repaint();
}

void MainWindow::myTableCurrentAreaChanged()//table选择区域改变，label中选择区域也需要改变，band中绘制图像也改变
{
    myLabelInfo->currentArea = myTableInfo->currentRow;

    if(myTableInfo->tableAreaList.size() == 0) return;

    myBandInfo->currentAreaBand = myTableInfo->tableAreaList.at(myTableInfo->currentRow);
    myBandInfo->repaint();
    myLabelInfo->repaint();
}

void MainWindow::myTableBandChanged()//table中增加band，band中需要重新绘制，label中区域的数据也需要更新
{
    myLabelInfo->areaList.clear();
    for(int i=0; i<myTableInfo->tableAreaList.size(); i++)
        myLabelInfo->areaList.append(myTableInfo->tableAreaList.at(i));
    myBandInfo->currentAreaBand = myTableInfo->tableAreaList.at(myTableInfo->currentRow);
    myBandInfo->repaint();
    myLabelInfo->repaint();

	nArea = this->myLabelInfo->areaList.size();
	float temporalFL[10]; // = {2.8, 4.5, 6.5};
	float temporalFH[10]; // = {3.2, 5.5, 7.5};
	float magnifyRatio[10]; // = {10, 10, 10};
	for(int i=0; i < nArea; i ++)                            //此处默认不处理Global区域
	{
		QStringList strList2 = this->myLabelInfo->areaList.at(i);
		nLobes = strList2.at(7).toInt();//此变量表示band个数

		if(nLobes > 0)                                      //判断若band个数为零，默认minValue，maxFrequency，maxAlpha 
		{
			for(int j=0; j< nLobes; j++)                    //band个数不为零，即当前区域的band信息
			{
				temporalFL[j] = strList2.at(8+4*j+1).toFloat();           //band的最小值
				temporalFH[j] = strList2.at(8+4*j+2).toFloat();           //band的最大值
				magnifyRatio[j] = strList2.at(8+4*j+3).toFloat();         //band的高度	
				qDebug() << temporalFL[0] << temporalFH[0] << magnifyRatio[0];
			}
		}
		filter.SetLobes(nLobes, temporalFL, temporalFH, magnifyRatio);
		int x1 = strList2.at(1).toInt();
		int y1 = strList2.at(2).toInt();
		int x2 = strList2.at(3).toInt();
		int y2 = strList2.at(4).toInt();
		area[i].SetRectangle(x1, x2, y1, y2, filter);
	}
	pMagData->UpdateAreas(nArea, area);
}

void MainWindow::myBandValueChanged()
{
    myTableInfo->minValue = myBandInfo->minValue;
    myTableInfo->maxAlpha = myBandInfo->maxAlpha;
    myTableInfo->maxFrequency = myBandInfo->maxFrequency;

    QStringList strList = myBandInfo->currentAreaBand;
	if(strList.size() == 0) return;
    int index = strList.at(0).toInt();
    myTableInfo->tableAreaList.removeAt(index);
    myTableInfo->tableAreaList.insert(index, strList);

    myLabelInfo->areaList.removeAt(index);
    myLabelInfo->areaList.insert(index, strList);

    myLabelInfo->repaint();
    myTableInfo->writeTable();

	nArea = this->myLabelInfo->areaList.size();
	float temporalFL[10]; // = {2.8, 4.5, 6.5};
	float temporalFH[10]; // = {3.2, 5.5, 7.5};
	float magnifyRatio[10]; // = {10, 10, 10};
	for(int i=0; i < nArea; i ++)                            //此处默认不处理Global区域
	{
		QStringList strList2 = this->myLabelInfo->areaList.at(i);
		nLobes = strList2.at(7).toInt();//此变量表示band个数

		if(nLobes > 0)
		{
			for(int j=0; j< nLobes; j++)                    //band个数不为零，即当前区域的band信息
			{
				temporalFL[j] = strList2.at(8+4*j+1).toFloat();           //band的最小值
				temporalFH[j] = strList2.at(8+4*j+2).toFloat();           //band的最大值
				magnifyRatio[j] = strList2.at(8+4*j+3).toFloat();         //band的高度
			}
		}

		filter.SetLobes(nLobes, temporalFL, temporalFH, magnifyRatio);
		int x1 = strList2.at(1).toInt();
		int y1 = strList2.at(2).toInt();
		int x2 = strList2.at(3).toInt();
		int y2 = strList2.at(4).toInt();
		area[i].SetRectangle(x1, x2, y1, y2, filter);

	}
	pMagData->UpdateAreas(nArea, area);
}

void MainWindow::setShape1()
{
    myLabelInfo->shape = "Rectangle";
    rectangleButton->setDown(true);
    this->setCursor(Qt::CrossCursor);
    cursorButton->setDown(false);
    circleButton->setDown(false);
}

void MainWindow::setShape2()
{
    myLabelInfo->shape = "Circle";
    rectangleButton->setDown(false);
    cursorButton->setDown(false);
    circleButton->setDown(true);
    this->setCursor(Qt::SizeAllCursor);
}

void MainWindow::setShapeOri()
{
    myLabelInfo->shape.clear();
    rectangleButton->setDown(false);
    cursorButton->setDown(true);
    circleButton->setDown(false);
    this->setCursor(Qt::ArrowCursor);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::play()//若已经处于播放状态，则停止；若为播放状态，则开始播放
{
	if(tab->currentIndex() == 0)
	{
		myPoiInfo->show();
		myLabelInfo->show();
		myTableInfo->show();
		myBandInfo->show();

		GraphicsBand->hide();
		GraphicsTable->hide();
		myGraphicsInfo->hide();

		if(timer->isActive())
		{
			timer->stop();
			myLabelInfo->frameID = 0;
			horizontalSlider->setValue(0);
			myLabelInfo->showFrame();
			myLabelInfo->repaint();
		}
		else
		{
			myLabelInfo->frameID = 0;
			horizontalSlider->setValue(0);
			timer->start(30);
			myLabelInfo->showFrame();
			playButton->setChecked(true);
		} 
	}
	else if(tab->currentIndex() == 1)
	{
		myPoiInfo->hide();
		myLabelInfo->hide();
		myTableInfo->hide();
		myBandInfo->hide();
		GraphicsBand->show();
		GraphicsTable->show();
		myGraphicsInfo->show();

		if(timer->isActive())
		{
			timer->stop();
			myGraphicsInfo->graphFrameId = 0;
			myGraphicsInfo->update();
		}
		else
		{
			myGraphicsInfo->graphFrameId = 0;
			horizontalSlider->setValue(0);
			timer->start(150);
			myGraphicsInfo->update();
			playButton->setChecked(true);
		}
	}
}

void MainWindow::pause()//若为播放状态，则停止播放，否则直接设置为停止播放
{
    if(timer->isActive())
    {
        timer->stop();
        pauseButton->setChecked(true);
        playButton->setChecked(false);        
        horizontalSlider->setValue(0);

		if(tab->currentIndex() == 0)
		{
			myLabelInfo->frameID = 0;
			myLabelInfo->showFrame();
			myLabelInfo->repaint();
		}
		else if(tab->currentIndex() == 1)
		{
			myGraphicsInfo->graphFrameId = 0;			
			myGraphicsInfo->repaint();
		}
    }
}

void MainWindow::first()// 回到第一帧开始播放
{
    if(timer->isActive())
    {
        timer->stop();
    }
    else
    {
        timer->start();
    }

    playButton->setChecked(true);

	if(tab->currentIndex() == 0)
	{
		myLabelInfo->frameID = 0;
		myLabelInfo->showFrame();
		myLabelInfo->repaint();
	}
	else if(tab->currentIndex() == 1)
	{		
		myGraphicsInfo->graphFrameId = 0;
		myGraphicsInfo->repaint();
	}
}

void MainWindow::next()//往后增加1帧开始播放
{
	if(tab->currentIndex() == 0)
	{
		myLabelInfo->frameID += 1;
		horizontalSlider->setValue(myLabelInfo->frameID);

		if(myLabelInfo->frameID == 240)
		{
			myLabelInfo->frameID = 0;
			horizontalSlider->setValue(0);
		}

		playButton->setChecked(true);
		myLabelInfo->showFrame();
	}
	else if(tab->currentIndex() == 1)
	{
		myGraphicsInfo->graphFrameId += 1;
		horizontalSlider->setValue(myGraphicsInfo->graphFrameId*240/437);
		if(myGraphicsInfo->graphFrameId == 437)
		{
			myGraphicsInfo->graphFrameId = 0;
			horizontalSlider->setValue(0);
		}
		playButton->setChecked(true);
		myGraphicsInfo->update();
	}
}

void MainWindow::timerEvent()
{
    this->next();
}

void MainWindow::setFile()
{

	QFileDialog::Options option;
	option |= QFileDialog::DontUseNativeDialog;

	QString selectedFilter;
	QString fileName = QFileDialog::getOpenFileName(this,
								"QFileDialog::getOpenFileName()",
								"/",
								"Video files (*.avi);;Text files (*.txt)",
								&selectedFilter,
								option);
	if(fileName.size() == 0) 
	{
		qDebug() << "Please enter the FILENAME!";
		return;
	}

	QStringList strList = fileName.split(".", QString::KeepEmptyParts);
	
	if(strList.at(1) == "avi")
	{
		myPoiInfo->show();
		myLabelInfo->show();
		myTableInfo->show();
		myBandInfo->show();
		GraphicsBand->hide();
		GraphicsTable->hide();
		myGraphicsInfo->hide();
		this->tab->setCurrentIndex(0);

		float spatialFilter[5]={0.0625, 0.2500, 0.3750, 0.2500, 0.0625};
		pMagData = new VideoMagnification<float>(30, 5, spatialFilter, GAUSS, 3, IDEAL);
		pMagData->ReadData(fileName.toStdString());		
		
		nArea = this->myLabelInfo->areaList.size();
		float temporalFL[10]; // = {2.8, 4.5, 6.5};
		float temporalFH[10]; // = {3.2, 5.5, 7.5};
		float magnifyRatio[10]; // = {10, 10, 10};
		for(int i=0; i < nArea; i ++)                            //此处默认不处理Global区域
		{
			QStringList strList2 = this->myLabelInfo->areaList.at(i);
			nLobes = strList2.at(7).toInt();//此变量表示band个数

			if(nLobes > 0)                                      //判断若band个数为零，默认minValue，maxFrequency，maxAlpha 
			{
				for(int j=0; j< nLobes; j++)                    //band个数不为零，即当前区域的band信息
				{
					temporalFL[j] = strList2.at(8+4*j+1).toFloat();           //band的最小值
					temporalFH[j] = strList2.at(8+4*j+2).toFloat();           //band的最大值
					magnifyRatio[j] = strList2.at(8+4*j+3).toFloat();         //band的高度
				}
			}

			filter.SetLobes(nLobes, temporalFL, temporalFH, magnifyRatio);
			int x1 = strList2.at(1).toInt();
			int y1 = strList2.at(2).toInt();
			int x2 = strList2.at(3).toInt();
			int y2 = strList2.at(4).toInt();
			area[i].SetRectangle(x1, x2, y1, y2, filter);

		}
		pMagData->UpdateAreas(nArea, area);

		for(int i=0; i<pMagData->data.size(); i++)
		{
			myLabelInfo->oriData.push_back(pMagData->data[i]);
			myLabelInfo->maginficationData.push_back(pMagData->magnifiedData[i]);
		}

		myBandInfo->maxFrequency = pMagData->frameRate;
		myBandInfo->maxFrequencyBtn->setText(QString::number(myBandInfo->maxFrequency));
		myBandValueChanged();
	}
	else
	{
// 		myPoiInfo->hide();
// 		myLabelInfo->hide();
// 		myTableInfo->hide();
// 		myBandInfo->hide();
// 		GraphicsBand->show();
// 		GraphicsTable->show();
// 		myGraphicsInfo->show();
// 		this->tab->setCurrentIndex(1);
// 
// 		pMagData = new GraphMagnification<float>(30, LAPLACIAN, 3, IDEAL);	
// 		pMagData->ReadData(fileName.toStdString());
// 
// 		for(int i=0; i<pMagData->data.size(); i++)
// 		{
// 			myGraphicsInfo->oriData.push_back(pMagData->data[i]);			
// 			myGraphicsInfo->maginficationData.push_back(pMagData->magnifiedData[i]);	
// 		}
// 		myGraphicsInfo->drawFlag = true;
// 
// 		myBandInfo->maxFrequency = pMagData->frameRate;
// 		myBandInfo->maxFrequencyBtn->setText(QString::number(myBandInfo->maxFrequency));
// 		myBandValueChanged();
	}
}

