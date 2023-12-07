#ifndef AED_H
#define AED_H

#include <QObject>
#include <QElapsedTimer>
#include "Electrode.h"

class AED: public QObject {

    Q_OBJECT

public:
    AED();
    ~AED();

    Electrode* getElectrode();

    // features in the cycle
    void analyzeAndDecideShock();


public slots:
    void deliverShock();
    void deliverCPR();

signals:
    void shockable();
    void CPRFeedback(QString feedBack, float cprDepth);
    void waitForGuiChange(int);


private:

    bool isShockAdvised();
    void charge();

    int batteryLevel;
    int numOfShocks;  // the number of shocks delivered
    int shockAmount; //the amount of shock in Joul to be delivered (prev?)
    QElapsedTimer elapsedTime;  // the elapsed time in seconds

    Electrode* electrode;
    int analayzeCPRDepth(float);
    void wait(int sec);
};

#endif // AED_H
