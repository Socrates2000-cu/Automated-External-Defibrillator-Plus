#ifndef AED_H
#define AED_H

#include <QObject>
#include <QElapsedTimer>
#include "Electrode.h"
#include "QTimer"

class AED: public QObject {

    Q_OBJECT

public:
    AED();
    ~AED();

    Electrode* getElectrode();

    // features in the cycle
    void analyzeAndDecideShock();
    void powerOn();
    void powerOff(); //may or may not use
    //bool isPowered();
    bool selfTest();

    void connectElectrode(Electrode*);

    //bool electrodeConnected();
    //void setConnection(bool state);
    bool hasBattery();
    int currBattery();
    void consumeBattery(int);
    void setBattery(int b);
    void updateBattery(int b);


public slots:
    void deliverShock();
    void deliverCPR();
    void chargeBattery();

signals:
    void powerOffFromAED();
    void updateFromAED(int b);
    void checkElectrode();
    void shockable();
    void attach();
    void CPRFeedback(QString feedBack, float cprDepth);
    void waitForGuiChange(int);
    void cpr();
    void updateNumOfShocks(int);


private:
    //bool powered;
    int batteryLevel;
    int numOfShocks;  // the number of shocks delivered
    int shockAmount; //the amount of shock in Joul to be delivered (prev?)
    QElapsedTimer elapsedTime;  // the elapsed time in seconds
    Electrode* electrode;

    int analayzeCPRDepth(double);
    void doCompressions(int numberOfCompressions);
};

#endif // AED_H
