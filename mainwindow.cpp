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
    profileGenerator();
    update();
}


void MainWindow::on_comboBox_activated(int index)
{
    parameterupdates();
    profileGenerator();
    update();
}


void MainWindow::parameterupdates(){

    Rocket Rt;

    int i = ui->comboBox->currentIndex();
    if (i == 0){
        Rt.MassWeight = 21.6;
        Rt.Gamma = 1.219;
        Rt.temperatureChamber = 3550;
    }
    else if (i == 1){
        Rt.MassWeight = 13.1;
        Rt.Gamma = 1.2;
        Rt.temperatureChamber = 3550;
    }
    else{
        Rt.MassWeight = 19.9;
        Rt.Gamma = 1.206;
        Rt.temperatureChamber = 3450;
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
    Gamma = Rt.Gamma;
    Tt = Rt.temperatureTotal;
    Cp = Rt.Cp;

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

    r0 = qSqrt(Rt.areaThrust /3.1415926);
    rc = qSqrt(Rt.areaChamber / 3.1415926);
    Cstar = Rt.machChamber*Rt.residualTime*qSqrt(Rt.Gamma*Rt.R*Rt.temperatureChamber)*2.0;
    ER = Rt.expansionRatio;
    MFR = Rt.MFR;
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
    float PM1 = 1;
    float A1=r0;
    float T1=Tt/(Gamma+1)*2;
    float a2=qSqrt(2.0*Cp*(Tt-T1));

    outStream << x2 << "," << y2 << "," << 1.0 << "," << Tt << "," << a2 << "\n";;

    for (int i = theta0; i < theta1; i++){
        theta = i*3.1415926/180;
        x3 = x1 + dr1*sin(theta);
        y3 = y1 - dr1*cos(theta);
        if(x3 != x2){
            x2 = x3;
            y2 = y3;
            if(x3>0){
                Px.append(x3);
                Py.append(y3);
                while(A1<y3){
                    PM1 = PM1 + 0.0005;
                    A1 = (PM1*PM1*(Gamma-1)*0.5+1)*2/(Gamma+1);
                    A1 = qPow(A1, (Gamma+1)/(Gamma-1)*0.5);
                    A1 = qSqrt(A1 / PM1) * r0;
                }

                PM.append(PM1);
                T1=Tt/(1+PM1*PM1*(Gamma-1)*0.5);
                Pt1.append(T1);
                a2 = qSqrt(2.0*Cp*(Tt-T1));
                Pu.append(a2);
                outStream << x3 << "," << y3 << "," << PM1 << "," << T1 << "," << a2 << "\n";
            }
        }
    }

    x3 = x2 + r2;
    y3 = y2 + r2 * qTan(theta1*3.1415926/180);

    Px.append(x3);
    Py.append(y3);
    while(A1<y3){
        PM1 = PM1 + 0.0005;
        A1 = (PM1*PM1*(Gamma-1)*0.5+1)*2/(Gamma+1);
        A1 = qPow(A1, (Gamma+1)/(Gamma-1)*0.5);
        A1 = qSqrt(A1 / PM1) * r0;
    }

    PM.append(PM1);
    T1=Tt/(1+PM1*PM1*(Gamma-1)*0.5);
    Pt1.append(T1);
    a2 = qSqrt(2.0*Cp*(Tt-T1));
    Pu.append(a2);
    outStream << x3 << "," << y3 << "," << PM1 << "," << T1 << "," << a2 << "\n";

    float x4 = x3 + r3*qSin(theta1*3.1415926/180);
    float y4 = y3 - r3*qCos(theta1*3.1415926/180);

    x2 = x3;
    y2 = y3;
    for (int i = theta1; i < theta2; i--){
         theta = i*3.1415926/180;
         x3 = x4 - r3*qSin(theta);
         y3 = y4 + r3*qCos(theta);
         if(x3 != x2){
             x2 = x3;
             y2 = y3;
             Px.append(x3);
             Py.append(y3);
             while(A1<y3){
                 PM1 = PM1 + 0.0005;
                 A1 = (PM1*PM1*(Gamma-1)*0.5+1)*2/(Gamma+1);
                 A1 = qPow(A1, (Gamma+1)/(Gamma-1)*0.5);
                 A1 = qSqrt(A1 / PM1) * r0;
             }

             PM.append(PM1);
             T1=Tt/(1+PM1*PM1*(Gamma-1)*0.5);
             Pt1.append(T1);
             a2 = qSqrt(2.0*Cp*(Tt-T1));
             Pu.append(a2);
             outStream << x3 << "," << y3 << "," << PM1 << "," << T1 << "," << a2 << "\n";

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
    if(x5>2.5){
        factor = 200;}
    else if(x5>4){
        factor = 100;
    }

    int N = 100;
    for (int i = 1; i<N+1; i++){
        x7 = x3+(x5-x3)*i/N;
        y7 = ((a*(x7-x3)+b)*(x7-x3)+c);
        Px.append(x7);
        Py.append(y7);
        while(A1<y7){
            PM1 = PM1 + 0.0005;
            A1 = (PM1*PM1*(Gamma-1)*0.5+1)*2/(Gamma+1);
            A1 = qPow(A1, (Gamma+1)/(Gamma-1)*0.5);
            A1 = qSqrt(A1 / PM1) * r0;
        }

        PM.append(PM1);
        T1=Tt/(1+PM1*PM1*(Gamma-1)*0.5);
        Pt1.append(T1);
        a2 = qSqrt(2.0*Cp*(Tt-T1));
        Pu.append(a2);
        outStream << x7 << "," << y7 << "," << PM1 << "," << T1 << "," << a2 << "\n";
    }

    /*
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
    lc = Cstar;
    outStream << x3+l << "," << y3+l << "\n";
    outStream << x3-l << "," << y3 << "\n";
    outStream << x3-lc << "," << y3 << "\n";
    outStream << x3-lc << "," << -y3 << "\n";
    outStream << x3-l << "," << -y3 << "\n";
    outStream << x3+l << "," << -y3-l << "\n";
    outStream << x2 << "," << -y2 << "\n";
*/

    outStream << MFR << "," << Gamma << "," << Tt << "," << Cp << "," << ER << "\n";
    outputFile.close();

}

void MainWindow::paintEvent(QPaintEvent *event){

    QPainter painter(this);
    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::blue);
    painter.setPen(pen);

    int x0 = 300;
    int y0 = 300;

    if(ER<25){
        factor = 300;
    }
    else if(ER < 50){
        factor = 200;
        x0 = 250;
    }
    else{
        factor = 150;
        x0 = 200;
    }

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

//    std::cout << y1-dr1 <<"\t" << x6 << "\t" << y6 << "\t" << y7 << "\t" << Cstar << std::endl;

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
    lc = Cstar;

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

void MainWindow::on_pushButton_2_clicked()
{
    ht = new HeatTransfer(this);
    ht->show();
}

