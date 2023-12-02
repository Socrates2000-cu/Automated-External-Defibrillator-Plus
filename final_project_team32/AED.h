#ifndef AED_H
#define AED_H

#include <QObject>
#include <QElapsedTimer>
#include "defs.h"
#include "ElectrodePad.h"

class AED: public QObject {

    Q_OBJECT

public:
    AED();
    ~AED();


public slots:

signals:

private:
    ElectrodePad* electrodePad;
    int numOfShocks;  // the number of shocks delivered
    int shockAmount; //the amount of shock in Joul to be delivered
    QElapsedTimer elapsedTime;  // the elapsed time in seconds
    VoicePrompt textPrompt;  // current voice prompt / text prompt displayed
    bool isShockAdvised();
    void charge();

};

#endif // AED_H
