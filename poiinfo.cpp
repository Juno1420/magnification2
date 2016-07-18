#include "poiinfo.h"
#include <QFile>
#include <QDebug>

poiInfo::poiInfo(QWidget *parent) :
    QWidget(parent)
{
    poiBtn = new QPushButton(this);
    poiBtn->setText("Choose Your POIs");
    poiBtn->setGeometry(15, 0, 112, 20);

	this->poiBtn->setStyleSheet("QPushButton{background-color:rgb(0, 154, 205); border-radius:3px; font:Microsoft YaHei; border:1px; border-style:solid}");

    checkBoxList = new QList<QCheckBox *>;
    QFile f("e:/poitypeEnglish.csv");
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug() << f.errorString();
    int pos = 1;
    while(!f.atEnd())
    {
        const QString str = f.readLine();
        QCheckBox *checkBox = new QCheckBox(str, this);
        checkBox->setDown(false);
        checkBox->setChecked(false);
        checkBox->setGeometry(20, pos*20+5, 120, 11);
        checkBox->setStyleSheet("font: century gothic; color: rgb(70,130,180)");
        checkBoxList->append(checkBox);
        connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(poiInfoChanged(int)));
        pos += 1;
    }
    f.close();
}

void poiInfo::poiInfoChanged(int)
{
    for(int i=0; i<checkBoxList->size(); i++)
    {
        if(checkBoxList->at(i)->checkState() == Qt::Checked)
        {
            chkIndex[i] = 1;
            checkBoxList->at(i)->setChecked(true);
        }
        else
        {
            chkIndex[i] = 0;
            checkBoxList->at(i)->setChecked(false);
        }
    }
//    for(int j=0; j<19; j++)
//        if(chkIndex[j] != 0)
//            qDebug() << "chkIndex" << j+1 << "--" << chkIndex[j];
    emit poiIndexChanged();
}
