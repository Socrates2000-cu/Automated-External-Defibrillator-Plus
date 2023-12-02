#include "AED.h"
#include <QDebug>

AED::AED() : batteryLevel(100), numOfShocks(0), shockAmount(0) {
    electrode = new Electrode();
    elapsedTime.start(); //start elapsed timer

    connect(this, SIGNAL(shockable()), this, SLOT(deliverShock()));
}

AED::~AED() {
    delete electrode;
}

Electrode* AED::getElectrode()
{
    return electrode;
}

bool AED::isShockAdvised() {
    return false; //TODO
}

void AED::charge() {

}

void AED::analyzeAndDecideShock()
{
    if (electrode == nullptr) {
        qDebug() << "Warning! The electrode is not connected.";
        return;  //TODO what else should do
    }

    Patient* patient = electrode->getPatient();
    if (patient == nullptr) {
        qDebug() << "Warning! The electrode is not attached.";
        return;
    }

    QString age = patient->getAgeStage();
    QString ecgWave = patient->getEcgWave();

    if (ecgWave == "V_TACH" || ecgWave == "V_FIB") {  // shockable
        if (age == "adult") {
            if (numOfShocks == 0) shockAmount = 120;
            else if (numOfShocks == 1) shockAmount = 150;
            else shockAmount = 200;
        } else if (age == "child" || age == "infant") {
            if (numOfShocks == 0) shockAmount = 50;
            else if (numOfShocks == 1) shockAmount = 70;
            else shockAmount = 85;
        }
        emit shockable();
    } else if (ecgWave == "PEA" || ecgWave == "ASYSTOLE") {
        qDebug() << "No shock advised.";
    }
}

void AED::deliverShock()
{
    qDebug() << "Shock at " << shockAmount << "J delivered.";
    ++numOfShocks;
}

