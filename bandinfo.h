#ifndef BANDINFO_H
#define BANDINFO_H

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QStringList>
#include <QPushButton>
#include <QLabel>
#include <QPoint>
#include <QColor>
#include <QVector>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include <QRect>
#include <QEvent>
#include <QTimer>
#include <QPainter>

class bandInfo : public QWidget
{
    Q_OBJECT
public:
    explicit bandInfo(QWidget *parent = 0);
    QStringList currentAreaBand;
    int maxAlpha;
    int minValue;
    int maxFrequency;

    QAction *deleteBandAction;
    QMenu *rightMenu;
	QAction *addBand;

    QPushButton *minBtn;
    QPushButton *maxAlphaBtn;
    QPushButton *maxFrequencyBtn;
    QLabel *label;
    QPoint pntC;
    QPoint pntE;
    QPoint pntD;
    QPoint pntA;
    QPoint pntB;
    QVector<QColor> penColor;
    QVector<QPoint> drawPntVec;
    QVector<QRect> rectLeftRight;
    QVector<QRect> rectUpDown;
    QVector<QRect> rectDelete;
    int move_i;
    int move_j;
    int delete_k;
    bool moveControl_1;
    bool moveControl_2;
    
signals:
    void bandValueChanged();
	void addBandChanged();
    
public slots:
    void minValueChanged();
    void maxAlphaChanged();
    void maxFrequencyChanged();
    void deleteBandHere();
	void addBandHere();

private:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
    void drawBand(QPainter *);
    void contextMenuEvent(QContextMenuEvent*);
	void enterEvent(QEvent *);
    
};

#endif // BANDINFO_H
