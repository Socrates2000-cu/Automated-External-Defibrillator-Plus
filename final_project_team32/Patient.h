#ifndef PATIENT_H
#define PATIENT_H

#include <QObject>
#include "defs.h"

class Patient: public QObject {

    Q_OBJECT

public:
    Patient();

private:
    AgeStage ageStage;
};

#endif // PATIENT_H
