#ifndef POIINFO_H
#define POIINFO_H

#include <QWidget>
#include <QCheckBox>
#include <QList>
#include <QPushButton>

class poiInfo : public QWidget
{
    Q_OBJECT
public:
    explicit poiInfo(QWidget *parent = 0);
    int chkIndex[19];
    
signals:
    void poiIndexChanged();
    
public slots:
    void poiInfoChanged(int);

private:
    QList<QCheckBox *> *checkBoxList;
    QPushButton *poiBtn;
    
};

#endif // POIINFO_H
