#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "poiinfo.h"
#include "tableinfo.h"
#include "labelinfo.h"
#include "bandinfo.h"
#include "graphicsinfo.h"
#include "Magnify.h"
#include "GraphMagnification.h"
#include "GridMagnification.h"
#include "VideoMagnification.h"
#include "VolumeMagnification.h"

#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QHBoxLayout>
#include <QTabBar>
#include <QFileDialog>
#include <QtGui>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	int graphFrame;
	Magnification<float> *pMagData;
	Area area[10];
	int nArea, nLobes;
	Filter filter;

private:
    Ui::MainWindow *ui;
    poiInfo *myPoiInfo;
    tableInfo *myTableInfo;
    labelInfo *myLabelInfo;
    bandInfo *myBandInfo;
    bandInfo *GraphicsBand;
    tableInfo *GraphicsTable;
    GraphicsInfo *myGraphicsInfo;

    QSlider *horizontalSlider;
    QHBoxLayout *HBoxLayout;
    QTabBar *tab;
    QTimer *timer;	

    QPushButton *openButton;
    QPushButton *saveButton;
    QPushButton *cursorButton;
    QPushButton *rectangleButton;
    QPushButton *circleButton;
    QPushButton *sectionButton;
    QPushButton *userDefinedButton;
    QPushButton *chexiaoButton;
    QPushButton *redoButton;
    QPushButton *mapiconButton;
    QPushButton *rangeButton;
    QPushButton *liebiaoButton;
    QPushButton *zoominButton;
    QPushButton *zoomoutButton;
    QPushButton *moveButton;
    QPushButton *locateButton;
    QPushButton *typeinButton;
    QPushButton *dianzanButton;
    QPushButton *playButton;
    QPushButton *pauseButton;
    QPushButton *firstButton;
    QPushButton *nextButton;

private slots:
    void currentTab(int);
    void myPoiIndexChanged();
    void myLabelAreaChanged();
    void myLabelCurrentAreaChanged();
    void myTableCurrentAreaChanged();
    void myTableBandChanged();
    void myBandValueChanged();

    void setShape1();
    void setShape2();
    void setShapeOri();
	void setFile();

    void play();
    void pause();
    void first();
    void next();
    void timerEvent();

};

#endif // MAINWINDOW_H
