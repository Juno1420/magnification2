#ifndef GRAPHICSBAND_H
#define GRAPHICSBAND_H

#include <QWidget>
#include <QPaintEvent>

class GraphicsBand : public QWidget
{
    Q_OBJECT
public:
    explicit GraphicsBand(QWidget *parent = 0);

    int maxAlpha;
    int minValue;
    int maxFrequency;

    QPoint pntC;
    QPoint pntE;
    QPoint pntD;
    QPoint pntA;
    QPoint pntB;
    
signals:
    
public slots:

private:
    void paintEvent(QPaintEvent *);
    
};

#endif // GRAPHICSBAND_H
