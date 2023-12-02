#ifndef PATIENT_H
#define PATIENT_H

#include <QObject>
#include "defs.h"

class Patient: public QObject {

    Q_OBJECT

public:
    Patient();
    ECGWaveform getEcgWave();

private:
    AgeStage ageStage;
    ECGWaveform ecgWave;
};

#endif // PATIENT_H
