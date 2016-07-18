#ifndef GRAPHICSINFO_H
#define GRAPHICSINFO_H

#include <QWidget>
#include <QPaintEvent>
#include <QPoint>
#include <QVector>
#include <QColor>
#include <QTimer>
#include <QPushButton>

using namespace std;

class GraphicsInfo : public QWidget
{
    Q_OBJECT
public:
    explicit GraphicsInfo(QWidget *parent = 0);
    
	int graphFrameId;
	int pramKR;  //参数Kr
	float pramKS;  //参数Ks
	int pramL;   //参数L
	int timeId;
	vector<float*> oriData;
	vector<float*> maginficationData;
	QVector<QVector<int> > *data;
	QVector<QVector<int> > *adjData;
	QPushButton *rePosBtn;

	QTimer *timer;
	void initPntPos();
    
signals:
    
public slots:
	void rePos();
	void timeoutPos();

private:
    void paintEvent(QPaintEvent *);
};

#endif // GRAPHICSINFO_H
