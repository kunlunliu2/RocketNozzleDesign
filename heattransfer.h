#ifndef HEATTRANSFER_H
#define HEATTRANSFER_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

namespace Ui {
class HeatTransfer;
}

class HeatTransfer : public QDialog
{
    Q_OBJECT

public:
    explicit HeatTransfer(QWidget *parent = nullptr);
    ~HeatTransfer();
    float KS = 52;
    float KF = 0.14;     // W/MK 0.16 for RP-1, 0.08 for liquid methane, 0.12 for liquid hydrogan
    float emis = 0.9;
    float radia = 0.051; // microW/M^2K^4
    float Pr = 0.9;      // for turbulence
    float mu = 0.000076;  // for air kg/ms
    float Thick = 0.01;
    float Te = 10;
    float MFR = 200.0;
    float Gamma = 1.2;
    float Tt = 3000;
    float Cp = 1800;
    float cpf = 27.4;        // 13.8 J/gK for liquid Hydrogan, 2.4 J/gK for liquid methane, 2.1 J/gk for RP-1
    float coolMRFRatio = 0.5;
    float coolTubeDiameter = 0.005;
    float coolTubeAreaFactor = 0.5;
    float KF1 = 0.14;

    QVector<float> Px, Py, Pt1, Pt2, Pt3, Pu, PM, ResErr, Aup, Adown;

    virtual void paintEvent(QPaintEvent *event);
    void res();
    void readData();
    float Solver();

private slots:
    void on_pushButton_clicked();

private:
    Ui::HeatTransfer *ui;
    QLineEdit *line;
    QLineEdit *line1;
    QLineEdit *line2;
    QLineEdit *line3;
    QLineEdit *line4;
    QLineEdit *line5;
};

#endif // HEATTRANSFER_H
