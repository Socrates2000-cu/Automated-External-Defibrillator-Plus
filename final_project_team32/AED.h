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

    int getBattery();
    void setBattery(int b);
    Electrode* getElectrode();
    void connectElectrode(Electrode*);

    void powerOn();
    void powerOff();
    bool hasBattery();
    void consumeBattery(int);
    void updateBattery(int b);
    bool selfTest();	
    // features in the cycle
    void analyzeAndDecideShock();

public slots:
    void deliverShock();
    void deliverCPR();
    void chargeBattery();


    void updateDisplayTime();


signals:
    void selfTestResult(bool);
    void powerOffFromAED();
    void updateFromAED(int b);
    void checkElectrode();
     void indicatorSig1On();
    void indicatorSig1Off();
    void indicatorSig2On();
    void indicatorSig2Off();
    void attach();
    void shockable();
    void nonShockable();
    void CPRFeedback(QString feedBack, float cprDepth);
    void waitForGuiChange(int);
    void updateNumOfShocks(int);

    //for display elapsed time and num of shock
    void updateDisplay(QString a, int numOfShocks);
    void displayPrompt(QString input);
    void resetCPRdepth();

private:
    
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
