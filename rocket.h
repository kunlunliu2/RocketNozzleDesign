#ifndef ROCKET_H
#define ROCKET_H

#include <QtCore/qmath.h>
#include "mainwindow.h"

class Rocket : MainWindow
{

public:
    float Cp;
    float Gamma;
    float MassWeight;
    float R;
    float MFR;
    float pressureAmbiant;

    float temperatureChamber;
    float pressureChamber;
    float machChamber;
    float areaChamber;
    float pressureRatio;

    float temperatureExit;
    float pressureExit;
    float machExit;
    float areaExit;

    float temperatureTotal;
    float pressureTotal;

    float velocityExit;
    float areaThrust;
    float Isp;
    float thrust;
    float expansionRatio;
    float residualTime;

    Rocket(){
        MassWeight = 21.4;
        Gamma = 1.21;
        temperatureChamber = 3600;
        pressureAmbiant = 101000;
        machChamber = 0.2;
        pressureExit = pressureAmbiant * 0.9;
        pressureRatio = 100;
        areaExit = 3.1415926/4;
        residualTime = 0.001;
        pressureChamber = pressureExit*pressureRatio;
    }

    void parameterUpdate(){
        R = 8314.0/MassWeight;
        Cp = R * Gamma / (Gamma - 1.0);
        temperatureTotal = temperatureChamber *(1+ (Gamma-1)*0.5*machChamber*machChamber);
        pressureTotal=pressureChamber*(1+Gamma*machChamber*machChamber);
        velocityExit = calExitVelocity();
        machExit = calMachExit();
        temperatureExit = temperatureTotal / (1 + (Gamma-1)/2*machExit*machExit );
        expansionRatio = (machExit*machExit*(Gamma-1)/2+1.0)/(Gamma+1)*2.0;
        expansionRatio = qPow(expansionRatio, (Gamma+1)/(Gamma-1)*0.5);
        expansionRatio = expansionRatio / machExit;
        areaThrust = areaExit / expansionRatio;

        areaChamber = (machChamber*machChamber*(Gamma-1)/2+1.0)/(Gamma+1)*2.0;
        areaChamber = qPow(areaChamber, (Gamma+1)/(Gamma-1)*0.5);
        areaChamber = areaChamber / machChamber * areaThrust;

        MFR = pressureExit/R/temperatureExit*velocityExit*areaExit;
        MFR = pressureChamber*machChamber*areaChamber*qSqrt(Gamma/R/temperatureChamber);
        thrust = MFR*velocityExit + (pressureExit - pressureAmbiant)*areaExit;
        Isp = thrust / MFR / 9.81;
    }

    float calExitVelocity(){
        float t = qPow( pressureExit/pressureTotal, (Gamma-1)/Gamma );
        t = 2*Cp*temperatureTotal*(1-t);
        t = qSqrt(t);
        return t;
    }

    float calMachExit(){
        float t = Gamma*R*temperatureTotal / velocityExit / velocityExit - (Gamma-1)*0.5;
        t = qSqrt(1.0/t);
        return t;
    }

private:

};

#endif // ROCKET_H
