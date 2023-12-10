#include "AED.h"
#include <QDebug>
#include <QThread>

AED::AED(int batteryLevel) : powered(false), batteryLevel(batteryLevel),
    numOfShocks(0), shockAmount(0) {
    elapsedTime.start(); //start elapsed timer
    electrode = nullptr;
}

AED::~AED() {
}

bool AED::isPowered() {
    return powered;
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
            powered = true;
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
    powered = false;
    emit powerOffFromAED();
}


bool AED::selfTest(){
    qInfo("Self test init");

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
    return batteryLevel >= 10;
}

int AED::getBattery(){
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

    int newBattery = getBattery()-b;
    updateBattery(newBattery);

}

void AED::analyzeAndDecideShock()
{
    if (electrode == nullptr) {
        qDebug() << "Warning! The electrode is not connected to AED.";
        return;
    }

    Patient* patient = electrode->getPatient();
    if (patient == nullptr) {
        qDebug() << "Warning! The electrode pad is not attached to the patient's chest correctly.";
        return;
    }

    QString age = patient->getAgeStage();
    QString ecgWave = patient->getEcgWave();
    consumeBattery(5);

    if (ecgWave == "V_TACH" || ecgWave == "V_FIB") {  // shockable
        if (age == "Adult") {
            if (numOfShocks == 0) shockAmount = 120;
            else if (numOfShocks == 1) shockAmount = 150;
            else shockAmount = 200;
        } else if (age == "Child" || age == "Infant") {
            if (numOfShocks == 0) shockAmount = 50;
            else if (numOfShocks == 1) shockAmount = 70;
            else shockAmount = 85;
        }
        qInfo() << ecgWave << "detected. Shock is advised.";
        shockable();  // signal -> MainWindow shockable() enable shock delivery button
    } else if (ecgWave == "PEA" || ecgWave == "ASYSTOLE") {
        qInfo() << ecgWave << "detected. No shock advised.";
        nonShockable();
    }
}

void AED::deliverShock()
{
    qDebug() << "Shock at" << shockAmount << "J delivered.";
    ++numOfShocks;
    consumeBattery(20);

    updateNumOfShocks(numOfShocks); // reflect num of shocks in display

}

void AED::deliverCPR()
{
    qDebug() << " Started Delivering CPR ";

    QElapsedTimer timer;
    timer.start();

    qint64 cprTime = 2 * 60 * 1000/4; //2 minutes
    int c = 0;
    while(timer.elapsed() < cprTime) {
        qDebug() << "starting compressions";
        doCompressions(5);
        qDebug() << "finished compressions";
        waitForGuiChange(5000);

        //2 breathes
        CPRFeedback("Give one breath", 0);
        waitForGuiChange(5000);
        CPRFeedback("Give another breath", 0);
        waitForGuiChange(5000);

        qDebug() << "\n\n";
        c++;
    }
    CPRFeedback("Stop CPR", 0);
    waitForGuiChange(1000);
    qDebug() << " did " << c << " cycle of compressions";

    qDebug() << "Finished Delivering CPR ";
}

/* age stage - depth inch - cm
 * Adult     -    2-2.4   - 5.08-6.09
 * Child     -      2     - 5.08
 * Infant    -     1.5    - 3.81
 */
int AED::analayzeCPRDepth(double d)
{
    double minDepth = 5.08;
    double maxDepth = 6.09;

    QString age = electrode->getPatient()->getAgeStage();
    qDebug() << "age: " << age;
    if (age == "Child") {
        minDepth = 5.03;
        maxDepth = 5.13;
    } else if (age == "Infant") {
        minDepth = 3.76;
        maxDepth = 3.86;
    }

    if (d < minDepth) {
        return -1;
    } else if (d > maxDepth) {
        return 1;
    }else {
        return 0;
    }

}

void AED::doCompressions(int numberOfCompressions)
{
    QString feedBack = "";

    qDebug() << "current cpr depth: " << electrode->getCompressionDepth();
    int d = analayzeCPRDepth(electrode->getCompressionDepth());

    int cpr = 0;
    for (; cpr < numberOfCompressions; ++cpr) {
        if (d == 0) {
            //display good cpr
            feedBack = "Good CPR!";
        } else if (d < 0) {
            //display push harder
            feedBack = "Push harder";
        } else {
            feedBack = "Push slower";
        }

        qDebug() << "current cpr depth: " << QString::number(cpr) << " " << electrode->getCompressionDepth();
        CPRFeedback(feedBack + " " + QString::number(cpr), electrode->getCompressionDepth());
        waitForGuiChange(2000);

        if (d != 0) {

            //wait before getting new updateCPRDepth
            waitForGuiChange(10000);

            qDebug() << "updated cpr depth: " << electrode->getCompressionDepth();
        }
        d = analayzeCPRDepth(electrode->getCompressionDepth());
    }
    qDebug() << "Delivered " << cpr << " compressions";

}
