#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QTimer>
#include <QPushButton>
#include <QEventLoop>
#include "AED.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void pressPowerButton();

public slots:
    void shockable();
    void CPRFeedback(QString feedBack, float cprDepth);
    void waitForGuiChange(int milliseconds);

    void updateNumOfShocks(int num);

private:

    void powerOn();  // start the cycle after power on
    //void powerOff();
    bool isPowered();
    void indicatorLightFlash(QPushButton* indicator, bool on = true);  // false if turn the light off
    void displayPrompt(QString input);
    void displayEcgPic();
    void clearDisplay();
    void nonBlockingSleep(int seconds);

    Ui::MainWindow *ui;
    bool powered;

    AED* theAEDPlus;
    Electrode* electrode;
    Patient* patient;
    QEventLoop eventLoop;

private slots:

    void changeElectrodeConnection(bool);
    void changePatientAttach(bool);
    void confirmInitialization();
    void powerOff();
    void analyzeHeartRhythm();  // step 4
    void deliverShock();  // step 4

    void setBattery(int v);
    void updateBattery(int v);
    void on_increase_clicked();
    void on_decrease_clicked();

    void attachPads();  //step3
    void connectedChest(); //step3
    void deliverCPR();
    void updateCPRDepth(const QString &text);


signals:
    void analyze();  // step 3 -> step 4
};

#endif // MAINWINDOW_H
