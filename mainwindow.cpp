#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rocket.h"
#include <QMessageBox>
#include <QDebug>
#include <QPixmap>
#include <QPainterPath>
#include <string>
#include <iostream>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    parameterupdates();
    update();
    profileGenerator();
}


void MainWindow::on_comboBox_activated(int index)
{
    parameterupdates();
    update();
    profileGenerator();
}


void MainWindow::parameterupdates(){

    Rocket Rt;

    int i = ui->comboBox->currentIndex();
    if (i == 0){
        Rt.MassWeight = 31;
    }
    else if (i == 1){
        Rt.MassWeight = 22;
    }
    else{
        Rt.MassWeight = 26;
    }

    i = ui->comboBox_2->currentIndex();
    if(i == 0){
        Rt.pressureAmbiant = 101000;
    }
    else{
        Rt.pressureAmbiant = 0;
    }

    const char* str = ui->lineEdit->text().toStdString().c_str();
    Rt.Gamma =atof(str);

    str = ui->lineEdit_2->text().toStdString().c_str();
    Rt.pressureExit  = atof(str);

    str = ui->lineEdit_3->text().toStdString().c_str();
    Rt.pressureChamber  = atof(str);

    str = ui->lineEdit_4->text().toStdString().c_str();
    Rt.machChamber  = atof(str);

    str = ui->lineEdit_5->text().toStdString().c_str();
    Rt.temperatureChamber = atof(str);

    str = ui->lineEdit_6->text().toStdString().c_str();
    Rt.areaExit= atof(str)*0.5;
    re = Rt.areaExit;
    Rt.areaExit=3.1415926*Rt.areaExit*Rt.areaExit;

    Rt.parameterUpdate();

    char buffer[16];
    sprintf(buffer, "AreaThust (M^2):\t%.4f", Rt.areaThrust);
    ui->label_8->setText(buffer);

    sprintf(buffer, "Thrust (KN):\t\t%.4f", Rt.thrust/1000);
    ui->label_9->setText(buffer);

    sprintf(buffer, "ISP (S):\t\t\t%.4f", Rt.Isp);
    ui->label_10->setText(buffer);

    sprintf(buffer, "AreaChamber (M^2):\t%.4f", Rt.areaChamber);
    ui->label_11->setText(buffer);

    sprintf(buffer, "ExpansionRatio:   \t%.4f", Rt.expansionRatio);
    ui->label_12->setText(buffer);

    sprintf(buffer, "ExitMach:\t\t%.4f", Rt.machExit);
    ui->label_13->setText(buffer);

    sprintf(buffer, "ExitVelocity (m/s): \t%.4f", Rt.velocityExit);
    ui->label_14->setText(buffer);

    sprintf(buffer, "MassFlowRate (kg/s):\t%.4f", Rt.MFR);
    ui->label_15->setText(buffer);

    r0 = sqrt(Rt.areaThrust /3.1415926);
    rc = sqrt(Rt.areaChamber / 3.1415926);
}


void MainWindow::profileGenerator(){
    QString file = "Profile.dat";

    QFile outputFile(file);
    outputFile.open(QIODevice::WriteOnly);

    if(!outputFile.isOpen())
    {
        //alert that file did not open
    }

    QTextStream outStream(&outputFile);

    float x1 = 0;
    float y1 = r0*(1+r1);
    float dr1 = r0*r1;

    float x2 = x1 + dr1*sin(theta0*3.1415926/180);
    float y2 = y1 - dr1*cos(theta0*3.1415926/180);
    float x3;
    float y3;
    float theta;

    outStream << x2 << "," << y2 << "\n";;

    for (int i = theta0; i < theta1; i++){
        theta = i*3.1415926/180;
        x3 = x1 + dr1*sin(theta);
        y3 = y1 - dr1*cos(theta);
        if(x3 != x2){
            outStream << x3 << "," << y3 << "\n";
            x2 = x3;
            y2 = y3;
        }
    }

    x3 = x2 + r2;
    y3 = y2 + r2 * qTan(theta1*3.1415926/180);

    outStream << x3 << "," << y3 << "\n";

    float x4 = x3 + r3*qSin(theta1*3.1415926/180);
    float y4 = y3 - r3*qCos(theta1*3.1415926/180);

    x2 = x3;
    y2 = y3;
    for (int i = theta1; i < theta2; i--){
         theta = i*3.1415926/180;
         x3 = x4 - r3*qSin(theta);
         y3 = y4 + r3*qCos(theta);
         if(x3 != x2){
             outStream << x3 << "," << y3 << "\n";
             x2 = x3;
             y2 = y3;
         }
    }


    float y5 = re;
    float x5 = (y5 - y3) * 2 / (qTan(theta3*3.1415926/180)+qTan(theta2*3.1415926/180))+x3;
    float c = y3;
    float b = qTan(theta2*3.1415926/180);
    float a = (qTan(theta3*3.1415926/180)-qTan(theta2*3.1415926/180))/2/(x5-x3);
    float x6=x3;
    float y6=y3;
    float y7;
    float x7;

    int N = 100;
    for (int i = 1; i<N+1; i++){
        x7 = x3+(x5-x3)*i/N;
        y7 = ((a*(x7-x3)+b)*(x7-x3)+c);
        outStream << x7 << "," << y7 << "\n";
    }

    for (int i = N; i>0; i--){
        x7 = x3+(x5-x3)*i/N;
        y7 = -((a*(x7-x3)+b)*(x7-x3)+c);
        outStream << x7 << "," << y7 << "\n";
    }


    for (int i = theta2; i > theta1; i++){
         theta = i*3.1415926/180;
         x3 = x4 - r3*qSin(theta);
         y3 = y4 + r3*qCos(theta);
         if(x3 != x2){
            outStream << x3 << "," << -y3 << "\n";
            x2 = x3;
            y2 = y3;
         }
    }

    x3 = x3 - r2;
    y3 = -y3 + r2 * qTan(theta1*3.1415926/180);
    outStream << x3 << "," << y3 << "\n";

    x2 = x3;
    y2 = y3;
    for (int i = theta1; i > theta0; i--){
        theta = i*3.1415926/180;
        x3 = x1 + dr1*qSin(theta);
        y3 = y1 - dr1*qCos(theta);
        if(x3 != x2){
            outStream << x3 << "," << -y3 << "\n";
            x2 = x3;
            y2 = -y3;
        }
    }

    y3 = - rc;
    x3 = x2 - (y2-y3);
    float l = 0.01;
    outStream << x3+l << "," << y3+l << "\n";
    outStream << x3-l << "," << y3 << "\n";
    outStream << x3-lc << "," << y3 << "\n";
    outStream << x3-lc << "," << -y3 << "\n";
    outStream << x3-l << "," << -y3 << "\n";
    outStream << x3+l << "," << -y3-l << "\n";
    outStream << x2 << "," << -y2 << "\n";

    outputFile.close();

/*
    QFile inputFile(file);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList myStringList = line.split(',');
          qDebug() << myStringList.first() << myStringList.last();
          float f1 = myStringList.first().toFloat();
          float f2 = myStringList.last().toFloat();
          qDebug() << f1 << "\t" << f2 ;
       }
       inputFile.close();
    }
*/

}

void MainWindow::paintEvent(QPaintEvent *event){

    QPainter painter(this);
    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::blue);
    painter.setPen(pen);

    int x0 = 300;
    int y0 = 300;

    int x1 = x0;
    int y1 = y0 + r0*(1+r1)*factor;
    float dr1 = r0*r1*factor;

    int x2 = x1 + dr1*sin(theta0*3.1415926/180);
    int y2 = y1 - dr1*cos(theta0*3.1415926/180);
    int x3;
    int y3;
    float theta;

    for (int i = theta0; i < theta1; i++){
        theta = i*3.1415926/180;
        x3 = x1 + dr1*sin(theta);
        y3 = y1 - dr1*cos(theta);
        if(x3 != x2){
            painter.drawLine(x2,y2,x3,y3);
            x2 = x3;
            y2 = y3;
        }
    }

    x3 = x2 + r2*factor;
    y3 = y2 + r2*factor * qTan(theta1*3.1415926/180);

    painter.drawLine(x2,y2,x3,y3);

    int x4 = x3 + r3*factor*qSin(theta1*3.1415926/180);
    int y4 = y3 - r3*factor*qCos(theta1*3.1415926/180);

    x2 = x3;
    y2 = y3;
    for (int i = theta1; i < theta2; i--){
         theta = i*3.1415926/180;
         x3 = x4 - r3*factor*sin(theta);
         y3 = y4 + r3*factor*cos(theta);
         if(x3 != x2){
             painter.drawLine(x2,y2,x3,y3);
             x2 = x3;
             y2 = y3;
         }
    }


    int y5 = y0 + re * factor;
    int x5 = (y5 - y3) * 2 / (qTan(theta3*3.1415926/180)+qTan(theta2*3.1415926/180))+x3;
    float c = y3;
    float b = qTan(theta2*3.1415926/180);
    float a = (qTan(theta3*3.1415926/180)-qTan(theta2*3.1415926/180))/2/(x5-x3);
    int x6=x3;
    int y6=y3;
    int y7;

    for (int i = x3; i<x5; i=i+2){
        y7 = ((a*(i-x3)+b)*(i-x3)+c);
        painter.drawLine(x6,y6,i,y7);
        x6 = i;
        y6 = y7;
    }

    x6 = x5;
    y6 = 2*y0-y7;
    painter.drawLine(x5,y7, x6, y6);

//    std::cout << y1-dr1 <<"\t" << x6 << "\t" << y6 << "\t" << y7 << std::endl;

    for (int i = x5; i>x3; i=i-2){
        y7 = 2*y0-((a*(i-x3)+b)*(i-x3)+c);
        painter.drawLine(x6,y6,i,y7);
        x6 = i;
        y6 = y7;
    }

    x2 = x6;
    y2 = y6;
    for (int i = theta2; i > theta1; i++){
         theta = i*3.1415926/180;
         x3 = x4 - r3*factor*sin(theta);
         y3 = y4 + r3*factor*cos(theta);
         y3 = 2 * y0 - y3;
         if(x3 != x2){
             painter.drawLine(x2,y2,x3,y3);
             x2 = x3;
             y2 = y3;
         }
    }

    x3 = x2 - r2*factor;
    y3 = y2 + r2*factor * qTan(theta1*3.1415926/180);
    painter.drawLine(x2,y2,x3,y3);

    x2 = x3;
    y2 = y3;
    for (int i = theta1; i > theta0; i--){
        theta = i*3.1415926/180;
        x3 = x1 + dr1*sin(theta);
        y3 = y1 - dr1*cos(theta);
        y3 = 2 * y0 - y3;
        if(x3 != x2){
            painter.drawLine(x2,y2,x3,y3);
            x2 = x3;
            y2 = y3;
        }
    }

    y3 = y0 - rc*factor;
    x3 = x2 - (y2-y3);

    painter.drawLine(x2,y2,x3+4,y3+4);
    painter.drawLine(x3+4,y3+4,x3-4,y3);

    painter.drawLine(x3-4,y3,x3-lc*factor,y3);
    painter.drawLine(x3-lc*factor,y3,x3-lc*factor,2*y0-y3);
    painter.drawLine(x3-lc*factor,2*y0-y3,x3-4,2*y0-y3);

    painter.drawLine(x3-4,2*y0-y3, x3+4, 2*y0-y3-4);
    painter.drawLine(x3+4,2*y0-y3-4, x2, 2*y0-y2);

    pen.setWidth(1);
    pen.setColor(Qt::green);
    painter.setPen(pen);

    painter.drawLine(x0,y0+factor/2, x0, y0+factor/2+10);
    painter.drawLine(x0,y0+factor/2+10, x5, y0+factor/2+10);
    painter.drawLine(x5,y0+factor/2+10, x5, y0+factor/2);

    char buffer[16];
    float x_length = (x5-x0)*1.0 / factor;
    sprintf(buffer, "%.4f meter", x_length);

    ui->label_17->move(x0+factor/2, y0+factor/2+14);
    ui->label_17->setText(buffer);
}
