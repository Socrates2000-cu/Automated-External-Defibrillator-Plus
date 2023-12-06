#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QTimer>
#include <QPushButton>
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

private:
    void printVoicePromptToDisplay(QString prompt);

    void powerOn();  // start the cycle after power on
    void powerOff();
    void indicatorLightFlash(QPushButton* indicator, bool on = true);  // false if turn the light off
    void displayPrompt(QString input);
    Ui::MainWindow *ui;
    AED* theAEDPlus;
    Electrode* electrode;

private slots:
    void analyzeHeartRhythm();  // step 4
    void deliverShock();  // step 4
    //void electrodeConnected();
    //void checkBattery();
    void setBattery(int v);
    void updateBattery(int v);
    void on_increase_clicked();
    void on_decrease_clicked();


    void testButPressed();

    void attachPads();  //step3
    void connectedChest(); //step3
    void setEcgpic();


signals:
    void analyze();
};

#endif // MAINWINDOW_H
