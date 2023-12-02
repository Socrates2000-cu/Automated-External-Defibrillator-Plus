#include "Patient.h"


Patient::Patient(QString ageStage, QString ecgWave) :
    ageStage(ageStage), ecgWave(ecgWave)
{

}

QString Patient::getAgeStage()
{
    return ageStage;
}

QString Patient::getEcgWave()
{
    return ecgWave;
}
