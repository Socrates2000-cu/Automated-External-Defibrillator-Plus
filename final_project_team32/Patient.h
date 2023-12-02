#ifndef PATIENT_H
#define PATIENT_H

#include <QObject>

class Patient: public QObject {

    Q_OBJECT

public:
    Patient(QString ageStage = "adult", QString ecgWave = "V_TACH");
// V_TACH,V_FIB,PEA,ASYSTOLE
    QString getAgeStage();
    QString getEcgWave();

private:
    QString ageStage;
    QString ecgWave;
};

#endif // PATIENT_H
