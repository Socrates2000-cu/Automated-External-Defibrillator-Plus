#ifndef PATIENT_H
#define PATIENT_H

#include <QObject>
#include <QThread>
#include <QMutex>

class Patient: public QObject {

    Q_OBJECT

public:
    Patient(QString ageStage, QString ecgWave);
    ~Patient();

    QString getAgeStage();
    QString getEcgWave();

public slots:
    void setEcgWave(QString newEcgWave);

private:
    std::unique_ptr<QThread> thread;
    QMutex mutex;

    QString ageStage;  // adult, child, infant
    QString ecgWave;  // V_TACH, V_FIB, PEA, ASYSTOLE
};

#endif // PATIENT_H
