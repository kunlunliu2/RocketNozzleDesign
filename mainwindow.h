#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    virtual void paintEvent(QPaintEvent *event);
    void parameterupdates();
    void profileGenerator();

    float r0=0.1265;        // the thrust area radius.
    float re=0.5;           // the radius of exit.
    float rc=0.2213;        // the radius of combustion chamber
    float lc=0.5;           // the length of combustion chamber

    int factor = 300;
    float r1 = 1.0;         // the ratio of thrust curve radius to the thrust area radius
    float r2 = 0.02;        // the extension length of thrust curve along the the x axis, where the film cooling hole located.
    float r3 = 0.2;         // the circle which adjusts the diverge angle from theta1 to theta2
    float theta0 = -45;     // thrust span from theta0 to theta1 with radius r1*r0.
    float theta1 = 35;
    float theta2 = 25;
    float theta3 = 5;       // expansion angle of nozzle exit.

    QVector<float> parameters;

private slots:
    void on_pushButton_clicked();

    void on_comboBox_activated(int index);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
