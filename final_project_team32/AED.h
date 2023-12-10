#ifndef AED_H
#define AED_H

#include <QObject>
#include <QElapsedTimer>
#include "Electrode.h"
#include "QTimer"

class AED: public QObject {

    Q_OBJECT

public:
    AED(int batteryLevel);
    ~AED();

    bool isPowered();
    int getBattery();
    Electrode* getElectrode();

    // features in the cycle
    void analyzeAndDecideShock();
    void powerOn();
    void powerOff(); //may or may not use

    bool selfTest();
    void connectElectrode(Electrode*);
    bool hasBattery();
    void consumeBattery(int);
    void setBattery(int b);
    void updateBattery(int b);

public slots:
    void deliverShock();
    void deliverCPR();
    void chargeBattery();


    void updateDisplayTime();


signals:
    void powerOffFromAED();
    void updateFromAED(int b);
    void checkElectrode();
    void attach();
    void shockable();
    void nonShockable();
    void CPRFeedback(QString feedBack, float cprDepth);
    void waitForGuiChange(int);
    void updateNumOfShocks(int);

    //for display elapsed time and num of shock
    void updateDisplay(QString a, int numOfShocks);

    void resetCPRdepth();

private:
    bool powered;
    int batteryLevel;
    int numOfShocks;  // the number of shocks delivered
    int shockAmount; //the amount of shock in Joul to be delivered (prev?)
    QElapsedTimer elapsedTime;  // the elapsed time in seconds
    Electrode* electrode;

    int analayzeCPRDepth(double);
    void doCompressions(int numberOfCompressions);


    QElapsedTimer time;
};

#endif // AED_H
