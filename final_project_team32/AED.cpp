#include "AED.h"
#include <QDebug>

AED::AED() : batteryLevel(24), numOfShocks(0), shockAmount(0) {
    //electrode = new Electrode();
    elapsedTime.start(); //start elapsed timer
}

AED::~AED() {
    //delete electrode;
}

void AED::powerOn(){

//    if(!isPowered()){
//        //turn on
//        powered = true;
//        //visual prompt
        qInfo("Starting AED...");
        //audio prompt
        //initiate self-test
        if(selfTest()){
            qInfo("STAY CALM.");
            qInfo("CHECK RESPONSIVENESS.");
            qInfo("Calling emergency services...");
            //timer
            //next signal or function initiating electrodePad
            emit attach();
        }
        else{
            //power off AED from AEDplus
            //power off MW
           QTimer::singleShot(5000, [this](){
             powerOff();
           });
        }

}
//   else{
//        //prompts for shutting off
//        qInfo("Shutting down...");
//        //turn LEDsoff
//        //indefinite sleep?
//        powered = false;
//    }
//}

void AED::powerOff(){
    //whatever we decide should happen
    emit powerOffFromAED();
}


bool AED::selfTest(){
    qInfo("Self test init");
    qDebug() << "electrode:" << getElectrode();
    if((getElectrode()!=nullptr) && hasBattery()){//add battery condition
        qInfo("UNIT OK.");
        return true;
    }

    qInfo("UNIT FAILED.");
    if(!hasBattery()){
        qInfo("CHANGE BATTERIES.");
        return false;
    }
    if(getElectrode()==nullptr){
        qInfo("Connect electrode and restart AED.");
    }
    return false;
}


void AED::connectElectrode(Electrode* electrode){
    this->electrode = electrode;
}

Electrode* AED::getElectrode(){
    return electrode;
}

bool AED::hasBattery(){

    if(currBattery()<10){
        batteryLevel=false;
    }
    else{
        batteryLevel=true;
    }
    return batteryLevel;
}

int AED::currBattery(){
   // emit checkBattery();
    return batteryLevel;
}

void AED::setBattery(int b){
    batteryLevel=b;
}

void AED::updateBattery(int b){
    emit updateFromAED(b);
}

void AED::chargeBattery() {
    updateBattery(100);
}

void AED::consumeBattery(int b){

    int newBattery = currBattery()-b;
    updateBattery(newBattery);

}
bool AED::isShockAdvised() {
    return false; //TODO
}

void AED::analyzeAndDecideShock()
{
    if (electrode == nullptr) {
        qDebug() << "Warning! The electrode is not connected to AED.";
        return;  //TODO what else should do
    }

    Patient* patient = electrode->getPatient();
    if (patient == nullptr) {
        qDebug() << "Warning! The electrode pad is not attached to the patient's chest correctly.";
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

