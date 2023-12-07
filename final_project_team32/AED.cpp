#include "AED.h"
#include <QDebug>

AED::AED() : batteryLevel(100), numOfShocks(0), shockAmount(0) {
    electrode = new Electrode();
    elapsedTime.start(); //start elapsed timer
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
        shockable();
    } else if (ecgWave == "PEA" || ecgWave == "ASYSTOLE") {
        qDebug() << "No shock advised.";
    }
}

void AED::deliverShock()
{
    qDebug() << "Shock at" << shockAmount << "J delivered.";
    ++numOfShocks;
}

void AED::deliverCPR()
{
    qDebug() << "Delivering CPR feedback ";
//    QElapsedTimer timer;
    QString feedBack = "";

    qDebug() << "current cpr depth: " << electrode->getCompressionDepth();
    int d = analayzeCPRDepth(electrode->getCompressionDepth());
    if (d == 0) {
        //display good cpr
        feedBack = "Good CPR!";
    } else if (d < 0) {
        //display push harder
        feedBack = "Push harder";
    } else {
        feedBack = "Push slower";
    }

    CPRFeedback(feedBack, electrode->getCompressionDepth());

    if (d != 0) {

        //wait before getting new updateCPRDepth
//        wait(5);
        waitForGuiChange(5000);

        qDebug() << "updated cpr depth: " << electrode->getCompressionDepth();
        //get updated value
        d = analayzeCPRDepth(electrode->getCompressionDepth());
    }
    //2 breathes


    qDebug() << "Delivered CPR feedback ";
}

int AED::analayzeCPRDepth(float d)
{
    float minDepth = 5.08; //cm
    float maxDepth = 6.04;

    QString age = electrode->getPatient()->getAgeStage();
    qDebug() << "age: " << age;
    if (age != "adult") {
        minDepth = 4.99;
        maxDepth = 5.04;
    }

    if (d < minDepth) {
        return -1;
    } else if (d > maxDepth) {
        return 1;
    }else {
        return 0;
    }

}

void AED::wait(int sec)
{
    qint64 waitTimeInMs = sec * 1000;

    QElapsedTimer timer;
    timer.start();
    qDebug() << "started waiting for " << sec << " secs";
    while (timer.elapsed() < waitTimeInMs);
    qDebug() << "finished wating" ;
}
