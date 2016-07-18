#ifndef TABLEINFO_H
#define TABLEINFO_H

#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QList>
#include <QStringList>
#include <QAction>
//#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QEvent>

class tableInfo : public QWidget
{
    Q_OBJECT
public:
    explicit tableInfo(QWidget *parent = 0);
    QTableWidget *tableWidget;
    QList<QStringList> tableAreaList;
    QMenu *rightMenu;
    //QAction *addBand;
    QAction *deleteArea;

    void writeTable();
    int minValue;
    int maxAlpha;
    int maxFrequency;
    int currentRow;
    
signals:
    void rightAreaChanged();
    void tableAddedBand();
    void tableDeleteArea();
    void tableAreaChanged();
    
public slots:
    void setCurrentRow(int);
    //void addBandTable();
    void deleteTableArea();

private:
    void contextMenuEvent(QContextMenuEvent*);
    //void mousePressEvent(QMouseEvent *);
	void enterEvent(QEvent *);
    
};

#endif // TABLEINFO_H
