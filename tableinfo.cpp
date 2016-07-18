#include "tableinfo.h"

#include <QStringList>
#include <QHeaderView>
#include <QStyle>
#include <QScrollBar>
#include <QDebug>

tableInfo::tableInfo(QWidget *parent) :
    QWidget(parent)
{
    this->setMouseTracking(true);

    tableWidget = new QTableWidget(this);
    tableWidget->setRowCount(10);     //设置行数为10
    tableWidget->setColumnCount(15);   //设置列数为13
    tableWidget->setWindowTitle("Band Information");
    QStringList header;
    header<<"Name"<<"VarRg"<<"AvgFr"<<"MaxAp"<<"BanStr"<<"BanE"<<"BanAp";
    tableWidget->setHorizontalHeaderLabels(header);

	tableWidget->verticalHeader()->setVisible(false);
	for(int i=0; i<15; i++)
		tableWidget->setColumnWidth(i, 70);

    tableWidget->setGeometry(QRect(0,0,500,220));
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	tableWidget->horizontalHeader()->setFixedHeight(20);
    tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(0, 154, 205);}");
    tableWidget->verticalHeader()->setStyleSheet("QHeaderView::section{background:rgb(0, 154, 205);}");
    tableWidget->setStyleSheet("background-color: rgb(211, 211, 211)");
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

	tableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
		"QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
		"QScrollBar::handle:hover{background:gray;}"
		"QScrollBar::sub-line{background:transparent;}"
		"QScrollBar::add-line{background:transparent;}");
	tableWidget->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 10px;}"
		"QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
		"QScrollBar::handle:hover{background:gray;}"
		"QScrollBar::sub-line{background:transparent;}"
		"QScrollBar::add-line{background:transparent;}");

    connect(tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(setCurrentRow(int)));

    minValue = 0;
    maxAlpha = 50;
    maxFrequency = 100;
    currentRow = 0;

    deleteArea = new QAction("deleteThisArea", this);
    connect(deleteArea, SIGNAL(triggered()), this, SLOT(deleteTableArea()));

    rightMenu = new QMenu(this->tableWidget);

	if(tableAreaList.size() == 0)
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
		tableAreaList.append(globalArea);
	}
	writeTable();
}

void tableInfo::writeTable()              //---------------------------------------------
{                                         //此函数只设定table中数据，对areaList数据无影响
    tableWidget->clear();
	QStringList header;
	header<<"Name"<<"VarRg"<<"AvgFr"<<"MaxAp"<<"BanStr"<<"BanE"<<"BanAp";
	tableWidget->setHorizontalHeaderLabels(header);

    for(int i=0; i<tableAreaList.size(); i++)//处理全局区域
    {
        QStringList area = tableAreaList.at(i);

		if(i==0)
		{
			QTableWidgetItem *itemIndex = new QTableWidgetItem("Global");
			tableWidget->setItem(0, 0, itemIndex);
			itemIndex->setTextAlignment(Qt::AlignCenter);
		}
		else
		{
			if(area.at(5).toInt()%6 == 1)
			{
				QTableWidgetItem *itemIndex = new QTableWidgetItem(QIcon("E:/QtCode/Maginification2/images/c2.png"), area.at(0));
				tableWidget->setItem(area.at(0).toInt(), 0, itemIndex);
				itemIndex->setTextAlignment(Qt::AlignCenter);
			}
			if(area.at(5).toInt()%6 == 2)
			{
				QTableWidgetItem *itemIndex = new QTableWidgetItem(QIcon("E:/QtCode/Maginification2/images/c3.png"), area.at(0));
				tableWidget->setItem(area.at(0).toInt(), 0, itemIndex);
				itemIndex->setTextAlignment(Qt::AlignCenter);
			}
			if(area.at(5).toInt()%6 == 3)
			{
				QTableWidgetItem *itemIndex = new QTableWidgetItem(QIcon("E:/QtCode/Maginification2/images/c4.png"), area.at(0));
				tableWidget->setItem(area.at(0).toInt(), 0, itemIndex);
				itemIndex->setTextAlignment(Qt::AlignCenter);
			}
			if(area.at(5).toInt()%6 == 4)
			{
				QTableWidgetItem *itemIndex = new QTableWidgetItem(QIcon("E:/QtCode/Maginification2/images/c5.png"), area.at(0));
				tableWidget->setItem(area.at(0).toInt(), 0, itemIndex);
				itemIndex->setTextAlignment(Qt::AlignCenter);
			}
			if(area.at(5).toInt()%6 == 5)
			{
				QTableWidgetItem *itemIndex = new QTableWidgetItem(QIcon("E:/QtCode/Maginification2/images/c6.png"), area.at(0));
				tableWidget->setItem(area.at(0).toInt(), 0, itemIndex);
				itemIndex->setTextAlignment(Qt::AlignCenter);
			}
			if(area.at(5).toInt()%6 == 0)
			{
				QTableWidgetItem *itemIndex = new QTableWidgetItem(QIcon("E:/QtCode/Maginification2/images/c1.png"), area.at(0));
				tableWidget->setItem(area.at(0).toInt(), 0, itemIndex);
				itemIndex->setTextAlignment(Qt::AlignCenter);
			}
		}

        QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(minValue) + "--" + QString::number(maxFrequency));
        QTableWidgetItem *item2 = new QTableWidgetItem("50");
        QTableWidgetItem *item3 = new QTableWidgetItem("50");
        tableWidget->setItem(area.at(0).toInt(), 1, item1);
        tableWidget->setItem(area.at(0).toInt(), 2, item2);
        tableWidget->setItem(area.at(0).toInt(), 3, item3);
        item1->setTextAlignment(Qt::AlignCenter);
        item2->setTextAlignment(Qt::AlignCenter);
        item3->setTextAlignment(Qt::AlignCenter);
    }

    this->tableWidget->setCurrentCell(currentRow, 0);
}

void tableInfo::setCurrentRow(int i)
{    
    if(tableWidget->rowCount() < 1)
    {
        return;
    }
    this->tableWidget->setCurrentCell(i, 0);
    this->currentRow = i;
    emit tableAreaChanged();
}

/*void tableInfo::addBandTable()
{
    QStringList str = this->tableAreaList.at(currentRow);
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

    this->tableAreaList.insert(currentRow+1, str);
    this->tableAreaList.removeAt(currentRow);
    emit tableAddedBand();
}*/

void tableInfo::deleteTableArea()
{
    this->tableAreaList.removeAt(this->currentRow);
    for(int j=this->currentRow; j<this->tableAreaList.size(); j++)
    {
        QStringList strList = this->tableAreaList.at(j);
        int index = strList.at(0).toInt()-1;
        strList.removeAt(0);
        strList.insert(0, QString::number(index));
        this->tableAreaList.removeAt(j);
        this->tableAreaList.insert(j, strList);
    }

    this->setCurrentRow(0);
    emit tableDeleteArea();
    this->writeTable();
}

void tableInfo::contextMenuEvent(QContextMenuEvent *event)
{
    int i = tableWidget->itemAt(QPoint(event->pos().x(), event->pos().y()-30))->row();
    int j = this->tableAreaList.size();

    if(i == j)  return;

    this->currentRow = i;
    this->setCurrentRow(i);

    rightMenu->clear();
    //rightMenu->addAction(addBand);
    rightMenu->addAction(deleteArea);
    rightMenu->exec(QCursor::pos());
    event->accept();

    emit rightAreaChanged();
}

/*void tableInfo::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		return;
	}
}*/

void tableInfo::enterEvent(QEvent *event)
{
	if(event->Enter == 10)
		this->setCursor(Qt::ArrowCursor);
}
