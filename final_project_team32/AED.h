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
    int getState();
    int getBattery();
    Electrode* getElectrode();
    int getNumShocks();
    int getElapsedSec();

    void connectElectrode(Electrode*);
    void setState(int state);

    bool hasBattery(int minBattery);
    void setBattery(int b);
    void consumeBattery(int);
    void updateBattery(int b);

    // safety checks in each step
    bool batteryCheck(int minBattery);
    bool connectCheck();
    bool attachCheck();

public slots:
    void selfTest();
    void powerOff();
    void analyzeAndDecideShock();
    void deliverShock();
    void deliverCPR();
    void chargeBattery();

signals:
    // safety - self test
    void passSelfTest();
    void batteryLow();
    void lossConnection();
    void lossAttach();

    void updateFromAED(int b);
    void checkElectrode();
    void shockable();
    void nonShockable();
    void CPRFeedback(QString feedBack, float cprDepth);
    void finishCPR();

private:
    void powerOn();
    int analayzeCPRDepth(double);
    void doCompressions(int numberOfCompressions);

    std::unique_ptr<QThread> thread;
    QMutex mutex;

    bool powered;
    int state;  // currently in which step
    int batteryLevel;
    int numOfShocks;  // the number of shocks delivered
    int shockAmount; //the amount of shock in Joul to be delivered (prev?)
    QElapsedTimer elapsedTime;  // the elapsed time in seconds
    Electrode* electrode;

};

#endif // AED_H
