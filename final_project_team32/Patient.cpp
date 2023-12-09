#include "Patient.h"


Patient::Patient(QString ageStage, QString ecgWave) :
    ageStage(ageStage), ecgWave(ecgWave)
{
    // verify inputs, otherwise use default beginning states
    if (ageStage != "adult" && ageStage != "child" && ageStage != "infant") {
            this->ageStage = "adult";
        } else {
            this->ageStage = ageStage;
        }
    if (ecgWave != "V_TACH" && ecgWave != "V_FIB" && ecgWave != "PEA" && ecgWave != "ASYSTOLE") {
            this->ecgWave = "V_TACH";
        } else {
            this->ecgWave = ecgWave;
        }

    thread.reset(new QThread);
    moveToThread(thread.get());
    thread->start();

}

Patient::~Patient()
{
    thread->quit();
    thread->wait();
}

QString Patient::getAgeStage()
{
    return ageStage;
}

QString Patient::getEcgWave()
{
    return ecgWave;
}

void Patient::setAgeStage(QString newAge)
{
    // verify input
    if (newAge != "adult" && newAge != "child" && newAge != "infant") return;

    mutex.lock();
    this->ageStage = newAge;
    mutex.unlock();
}

void Patient::setEcgWave(QString newEcgWave)
{
    // verify input
    if (newEcgWave != "V_TACH" && newEcgWave != "V_FIB"
        && newEcgWave != "PEA" && newEcgWave != "ASYSTOLE") return;

    mutex.lock();
    this->ecgWave = newEcgWave;
    mutex.unlock();
}
