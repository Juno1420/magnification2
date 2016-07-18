#ifndef LABELINFO_H
#define LABELINFO_H

#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPixmap>
#include <QStringList>
#include <QString>
#include <QList>
#include <QVector>
#include <QPoint>
#include <QRegion>
#include <QContextMenuEvent>
#include <QAction>
#include <QMenu>
#include <QPixmap>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <cv.h>
#include <highgui.h>

using namespace cv;
using cv::Mat;

class labelInfo : public QWidget
{
    Q_OBJECT
public:
    explicit labelInfo(QWidget *parent = 0);    
    QList<QStringList> areaList;
    QList<QRegion> regionList;
    QVector<QColor> colorVec;
    QMenu *rightMenu;
    QAction *deleteLabelArea;
    QString shape;
    QPoint startPnt;
    QPoint endPnt;
    bool scribble;
    int poiIndexLabel[19];
    int index;
    int currentArea;
    int deleteAreaIndex;
    int frameID;
    int threshhold_;

    QPixmap *sourcePix;
    QPixmap *resultPix;
    bool showFlag;
	int colorIndex;
	vector<float*> oriData;
	vector<float*> maginficationData;
	QString fileName;
    void showFrame();
    
signals:
    void tableChanged();
    void currentTableChanged();
    
public slots:
    void deleteArea();

private:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void contextMenuEvent(QContextMenuEvent*);

    void paintEvent(QPaintEvent *);
    void drawPoi(QPainter *);
    void drawAreaTo(QPainter*);
    void drawAllAreas(QPainter*);

    void displayResult(float *, float *);
    QPixmap *mapSource;

    QVector<QStringList> poiVec1;
    QVector<QStringList> poiVec2;
    QVector<QStringList> poiVec3;
    QVector<QStringList> poiVec4;
    QVector<QStringList> poiVec5;
    QVector<QStringList> poiVec6;
    QVector<QStringList> poiVec7;
    QVector<QStringList> poiVec8;
    QVector<QStringList> poiVec9;
    QVector<QStringList> poiVec10;
    QVector<QStringList> poiVec11;
    QVector<QStringList> poiVec12;
    QVector<QStringList> poiVec13;
    QVector<QStringList> poiVec14;
    QVector<QStringList> poiVec15;
    QVector<QStringList> poiVec16;
    QVector<QStringList> poiVec17;
    QVector<QStringList> poiVec18;
    QVector<QStringList> poiVec19;
    
};

#endif // LABELINFO_H
