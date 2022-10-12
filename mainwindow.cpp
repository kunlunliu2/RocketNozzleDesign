#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rocket.h"
#include <QMessageBox>
#include <QDebug>
#include <QPixmap>
#include <QPainterPath>
#include <string>
#include <iostream>

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
}


void MainWindow::on_comboBox_activated(int index)
{
    parameterupdates();
    update();
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

    sprintf(buffer, "Thrust (KN):\t%.4f", Rt.thrust/1000);
    ui->label_9->setText(buffer);

    sprintf(buffer, "ISP:\t%.4f", Rt.Isp);
    ui->label_10->setText(buffer);

    sprintf(buffer, "AreaChamber (M^2):\t%.4f", Rt.areaChamber);
    ui->label_11->setText(buffer);

    sprintf(buffer, "ExpansionRatio:\t%.4f", Rt.expansionRatio);
    ui->label_12->setText(buffer);

    sprintf(buffer, "ExitMach:\t%.4f", Rt.machExit);
    ui->label_13->setText(buffer);

    sprintf(buffer, "ExitVelocity (m/s):\t%.4f", Rt.velocityExit);
    ui->label_14->setText(buffer);

    sprintf(buffer, "MassFlowRate (kg/s):\t%.4f", Rt.MFR);
    ui->label_15->setText(buffer);

    r0 = sqrt(Rt.areaThrust /3.1415926);
    rc = sqrt(Rt.areaChamber / 3.1415926);
    ye = re;
}

void MainWindow::paintEvent(QPaintEvent *event){

    QPainter painter(this);
    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::blue);
    painter.setPen(pen);

    int x0 = 300;
    int y0 = 300;
    float r1 = 0.8;

    int x1 = x0;
    int y1 = y0 + r0*(1+r1)*factor;
    float dr1 = r0*r1*factor;
    float theta0 = -30;
    float theta1 = 30;

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

    float r2 = 0.02*factor;
    x3 = x2 + r2;
    y3 = y2 + r2 * qTan(theta1*3.1415926/180);

    painter.drawLine(x2,y2,x3,y3);

    r2 = 0.2*factor;
    int x4 = x3 + r2*qSin(theta1*3.1415926/180);
    int y4 = y3 - r2*qCos(theta1*3.1415926/180);
    float theta2 = 20;
    x2 = x3;
    y2 = y3;
    for (int i = theta1; i < theta2; i--){
         theta = i*3.1415926/180;
         x3 = x4 - r2*sin(theta);
         y3 = y4 + r2*cos(theta);
         if(x3 != x2){
             painter.drawLine(x2,y2,x3,y3);
             x2 = x3;
             y2 = y3;
         }
    }

    float theta3 = 5;
    int y5 = y0 + re*factor;
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

    std::cout << y1-dr1 <<"\t" << x6 << "\t" << y6 << "\t" << y7 << std::endl;

    for (int i = x5; i>x3; i=i-2){
        y7 = 2*y0-((a*(i-x3)+b)*(i-x3)+c);
        painter.drawLine(x6,y6,i,y7);
        x6 = i;
        y6 = y7;
    }

    x2 = x6;
    y2 = y6;
    r2 = 0.2*factor;
    for (int i = theta2; i > theta1; i++){
         theta = i*3.1415926/180;
         x3 = x4 - r2*sin(theta);
         y3 = y4 + r2*cos(theta);
         y3 = 2 * y0 - y3;
         if(x3 != x2){
             painter.drawLine(x2,y2,x3,y3);
             x2 = x3;
             y2 = y3;
         }
    }

    r2 = 0.02*factor;
    x3 = x2 - r2;
    y3 = y2 + r2 * qTan(theta1*3.1415926/180);
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


}
