#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "heattransfer.h"
#include "ui_heattransfer.h"
#include <QPainter>
#include <QPainterPath>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>
#include <iostream>
#include <QFile>
#include <QVector>

HeatTransfer::HeatTransfer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HeatTransfer)
{
    ui->setupUi(this);

    auto size=this->size();

    QPushButton *pushButton = new QPushButton(this);
    pushButton->setText("Calculate");
    pushButton->move(size.width()-100, size.height()-100);
    connect(pushButton, SIGNAL(clicked(bool)), this, SLOT(on_pushButton_clicked()));

    QLabel *label0 = new QLabel(this);
    label0->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label0->setStyleSheet("font-weight: bold; color: red");
    label0->setText("INPUT");
    label0->move(10,size.height()-90);

    QLabel *label = new QLabel(this);
    label->setText("Nozzle thickness(m)");
    label->move(30,size.height()-60);

    line = new QLineEdit(this);
    line->move(150,size.height()-60);
    label->setBuddy(line);

    QLabel *label1 = new QLabel(this);
    label1->setText("Environment temp.(K)");
    label1->move(30,size.height()-30);

    line1 = new QLineEdit(this);
    line1->move(150,size.height()-30);
    label1->setBuddy(line1);

    QLabel *label2 = new QLabel(this);
    label2->setText("Solid thermal conduct.(W/MK)");
    label2->move(300,size.height()-60);

    line2 = new QLineEdit(this);
    line2->move(470,size.height()-60);
    label2->setBuddy(line2);

    QLabel *label3 = new QLabel(this);
    label3->setText("Fluid thermal conduct.(W/MK)");
    label3->move(300,size.height()-30);

    line3 = new QLineEdit(this);
    line3->move(470,size.height()-30);
    label3->setBuddy(line3);

    QLabel *label4 = new QLabel(this);
    label4->setText("Radiation cons.(uW/M^2K^4)");
    label4->move(640,size.height()-60);

    line4 = new QLineEdit(this);
    line4->move(800,size.height()-60);
    label4->setBuddy(line4);

    QLabel *label5 = new QLabel(this);
    label5->setText("Solid emmision const.");
    label5->move(640,size.height()-30);

    line5 = new QLineEdit(this);
    line5->move(800,size.height()-30);
    label5->setBuddy(line5);

}

HeatTransfer::~HeatTransfer()
{
    delete ui;
}

void HeatTransfer::readData(){
    QString file = "Profile.dat";
    QString list1;

    int len=Px.size();
    if (len>0){
        Px.clear();
        Py.clear();
        PM.clear();
        Pt1.clear();
        Pt2.clear();
        Pt3.clear();
        Pu.clear();
        ResErr.clear();
    }

    QFile inputFile(file);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList myStringList = line.split(',');
          float f1 = myStringList[0].toFloat();
          float f2 = myStringList[1].toFloat();
          float f3 = myStringList[2].toFloat();
          float f4 = myStringList[3].toFloat();
          float f5 = myStringList[4].toFloat();
//          qDebug() << f1 << "\t" << f2 << "\t" << f3<< "\t" << f4 << "\t" << f5;
          Px.append(f1);
          Py.append(f2);
          PM.append(f3);
          Pt1.append(f4);
          Pu.append(f5);
          Pt2.append(f4);
          Pt3.append(0);
          ResErr.append(0);
       }
       inputFile.close();
    }
    len = Px.size();
    MFR = Px[len-1];
    Gamma = Py[len-1];
    Tt = Pt1[0];
    Cp = Pt1[len-1];
}

void HeatTransfer::res(){

    int len = Px.size();
    float h1A;
    float h2A;
    float h3A;
    float ds;
    float ds1;
    float ds2;
    float resError;
    float re;
    float nu;
    float fac;
    float nuCool;
    float a1;
    float a2;
    float a3;

    for(int i = 0; i<len-1; i++){
        re = MFR/(3.1415926*Py[i]*mu)*2;
        nu = qPow(re, 0.8)*qPow(Pr, 0.3)*0.023;

        if(i<len-2){
            ds = 3.1415926*(Py[i]+Py[i+1])*(Px[i+1]-Px[i]);
            a1 = (Px[i]-Px[i-1]);
            if(i>0)
            {
                a2 = 3.1415926*(Py[i]+Py[i+1]-Thick)*Thick;
                ds1 = a2 * (Pt2[i]-Pt2[i-1]);
            }
            else{
                ds1 = 0;
                a2 = 0;
            }
            if(i<len-3){
                a3 = 3.1415926*(Py[i+1]+Py[i+2]-Thick)*Thick;
                ds2 = a3*(Pt2[i+1]-Pt2[i]);
            }
            else{
                a3 = 3.1415926*(Py[i+1]*2-Thick)*Thick;
                ds2 = a3*(Pt2[i+1]-Pt2[i]);
            }
        }
        else{
            a1 = 0;
            ds = 3.1415926*(Py[i]*2.0)*(Px[i]-Px[i-1]);
            a2 = 3.1415926*(Py[i]*2-Thick)*Thick;;
            ds1 = a2*(Pt2[i]-Pt2[i-1]);
            ds2 = 0;
            a3 = 0;
        }
        h1A = nu * KF /Py[i] *ds *0.5;
        h2A = radia * emis * ds;
        Pt3[i] = 100 + (Pt2[i]-100)*(3.1415926*KF1*ds*4.31/(MFR*Px[len-3]*cpf));

        resError = h1A*(Pt1[i]-Pt2[i]) - h2A*1000000.0*qPow(Pt2[i]*0.001, 4);
        resError = resError-ds1*KS+ds2*KS;
        resError = resError - MFR*a1/Px[len-3]*cpf*(Pt3[i]-100);
        a1 = h1A+h2A*4000.0*qPow(Pt2[i]*0.001, 3)+(a2+a3)*KS;
        a1 = a1 + (3.1415926*KF1*ds*4.31/Px[len-3])*a1/Px[len-3];
        ResErr[i] = resError/a1;
        Aup.append(a2*KS/a1);
        Adown.append(a3*KS/a1);

//        qDebug() << h2A <<"\t"<< Aup[i] <<"\t"<< ResErr[i] <<"\t"<< Adown[i];

    }
}

float HeatTransfer::Solver(){
    float relex;
    float a;
    float b;
    float c = 0.0;
    float ds1;
    float ds2;

    res();
    int len = Px.size();

    for(int i = len-2; i>0; i--){

        a = Aup[i];
        b = Adown[i-1];
        ResErr[i-1] = (ResErr[i-1] - ResErr[i]*b)/(1-a*b);
        Aup[i-1] = Aup[i-1] / (1-a*b);
//        qDebug() << i <<"\t"<< Aup[i] <<"\t"<< ResErr[i] <<"\t"<< Adown[i];
    }

    a = ResErr[0];
    Pt2[0] = Pt2[0] + a;
    for(int i = 1; i < len-1; i++){
        ResErr[i] = ResErr[i] - a*Aup[i];
        a = ResErr[i] ;
        Pt2[i] = Pt2[i] + a;
        c = fmax(c, abs(ResErr[i]));
        qDebug() << Pt1[i] << "\t"<< Pt2[i] <<"\t"<< Pt3[i] <<"\t"<< ResErr[i] <<"\t"<< Px[i];
    }

    return c;

}

void HeatTransfer::on_pushButton_clicked()
{
//    float Cp1 = Cp;
//    int j = Px.size();

    float a;
    int factor1 = 400;

    readData();
    auto size=this->size();

    QLabel *label1 = new QLabel(this);
    char buffer[80];
    sprintf(buffer, "Max gas Temperature:%.1f\nblue curve: Gas temperature\nred curve: Wall temperature",Tt);
    //label1->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label1->setText(buffer);
    //label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    //Here is how to change position:
    label1->move(100,100);
    label1->show();

    QLabel *label6 = new QLabel(this);
    sprintf(buffer, "T = 1000 K");
    //label1->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label6->setText(buffer);
    //label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    //Here is how to change position:
    int jmax = size.height()-150;
    int j1 = jmax - 1000*(jmax-100)/Tt;
    label6->move(100,j1);
    label6->show();

    QLabel *label7 = new QLabel(this);
    sprintf(buffer, "T = 2000 K");
    //label1->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label7->setText(buffer);
    //label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    //Here    is how to change position:
    j1 = jmax - 2000*(jmax-100)/Tt;
    label7->move(100,j1);
    label7->show();

    QLabel *label8 = new QLabel(this);
    sprintf(buffer, "0.5 meter");
    //label1->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label8->setText(buffer);
    //label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    //Here    is how to change position:
    label8->move(factor1/2+15,jmax-25);
    label8->show();

    QLabel *label9 = new QLabel(this);
    sprintf(buffer, "Central axis distance from the throat");
    //label1->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label9->setText(buffer);
    //label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    //Here    is how to change position:
    label9->move(200,jmax);
    label9->show();

    const char* str = line->text().toStdString().c_str();
    a = atof(str);
    Thick = (a!=0)?a:Thick;

    str = line1->text().toStdString().c_str();
    a = atof(str);
    Te = (a!=0)?a:Te;

    str = line2->text().toStdString().c_str();
    a = atof(str);
    KS = (a!=0)?a:KS;

    str = line3->text().toStdString().c_str();
    a = atof(str);
    KF = (a!=0)?a:KF;

    str = line4->text().toStdString().c_str();
    a = atof(str);
    radia = (a!=0)?a:radia;

    str = line5->text().toStdString().c_str();
    a = atof(str);
    emis =(a!=0)?a:emis;

    a = 10000;

    while(a > 1.0){
        a = Solver();
    }
    update();

}

void HeatTransfer::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    auto size=this->size();
    auto offset = 8;

//    painter.drawEllipse(offset/2,offset/2,size.width()-offset, size.height()-offset);

    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::blue);
    painter.setPen(pen);

    int len=Px.size()-1;
    int factor2 = 8;
    int factor1 = 400;
    int j1 = 0;
    int j2 = 0;
    int i1 = 0;
    int i2 = 0;
    int jmax = size.height()-150;
    int jmin = 100;
    factor2 = Tt/(jmax-jmin);
    if(len > 0){
        factor2 = Pt1[0]/(jmax-jmin);
    }

    for(int i =2; i<len; i++){
        j1 = jmax - Pt1[i-1]/factor2;
        j2 = jmax - Pt1[i]/factor2;
        i1 = Px[i-1]*factor1+40;
        i2 = Px[i]*factor1+40;
        painter.drawLine(i1, j1, i2, j2);
    }

    QPainter painter1(this);
    pen.setColor(Qt::red);
    painter1.setPen(pen);

    for(int i =2; i<len; i++){
        j1 = jmax - Pt2[i-1]/factor2;
        j2 = jmax - Pt2[i]/factor2;
        i1 = Px[i-1]*factor1+40;
        i2 = Px[i]*factor1+40;
        painter1.drawLine(i1, j1, i2, j2);
    }

    QPainter painter2(this);
    pen.setColor(Qt::green);
    painter2.setPen(pen);

    if(len > 0){
        i1 = Px[1]*factor1+40;
        painter2.drawLine(i1, jmin, i1, jmax);
    }

    for(int i =2; i<len; i++){
        i1 = Px[i-1]*factor1+40;
        i2 = Px[i]*factor1+40;
        painter2.drawLine(i1, jmax, i2, jmax);
    }

    pen.setWidth(1);
    pen.setColor(Qt::green);
    painter2.setPen(pen);
    if(len > 0){
        i1 = Px[1]*factor1+40;
        i2 = i1 + 400;
        j1 = jmax - 1000*(jmax-jmin)/Pt1[0];
        painter2.drawLine(i1, j1, i2, j1);
        j1 = jmax - 2000*(jmax-jmin)/Pt1[0];
        painter2.drawLine(i1, j1, i2, j1);
        j1 = jmax - 3000*(jmax-jmin)/Pt1[0];
        painter2.drawLine(i1, j1, i2, j1);

        i1 = factor1/2+40;
        painter2.drawLine(i1, jmax, i1, jmax-10);

    }
}
