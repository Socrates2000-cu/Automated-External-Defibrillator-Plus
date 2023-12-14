#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

public slots:
    void passSelfTest();
    void batteryLow();
    void lossConnection();
    void handleLossAttach();

    void shockable();
    void nonShockable();
    void CPRFeedback(QString feedBack, float cprDepth);
    void finishCPR();
    void indicatorLightFlash(QPushButton* indicator, bool on = true);  // false if turn the light off
    void displayPrompt(QString input);
    void updateBatteryUI();

signals:
    void attach();
    void analyze();
    void cpr();

private slots:
    void pressPowerButton();
    void changeElectrodeConnection(bool);
    void changePatientAttach(bool);
    void confirmInitialization();

    // batteries
    void on_increase_clicked();
    void on_decrease_clicked();

    void attachPads();
    void analyzeHeartRhythm();
    void deliverShock();
    void deliverCPR();

    void updateDisplay();

private:
    void powerOn();
    void powerOff();
    void checkResponse();
    void callEmergency();

    void displayEcgPic();
    void clearDisplay();
    void nonBlockingSleep(int seconds);
    void resetAdmin();

    Ui::MainWindow *ui;
    QTimer timer;  // updates AED state
    AED* theAEDPlus;
    Electrode* electrode;
    Patient* patient;

};

#endif // MAINWINDOW_H
