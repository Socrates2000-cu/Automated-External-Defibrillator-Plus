#include <QDebug>
#include "AED.h"

AED::AED(int batteryLevel) :
    powered(false), state(0), batteryLevel(batteryLevel), numOfShocks(0), shockAmount(0)
{
    thread.reset(new QThread);
    moveToThread(thread.get());
    thread->start();

    electrode = nullptr;
}

AED::~AED() {
    thread->quit();
    thread->wait();
}

int AED::getNumShocks()
{
    return numOfShocks;
}

void AED::reSetNumShocks()
{
    numOfShocks=0;
}

int AED::getState()
{
    return state;
}

void AED::setState(int state)
{
    if (state < 0 || state > 5) return;
    mutex.lock();
    this->state = state;
    mutex.unlock();
}

bool AED::isPowered()
{
    return powered;
}

void AED::powerOn(){
    mutex.lock();
    powered = true;
    mutex.unlock();

    // this timer is for elapsed time, so that it will show 'real-time'
    elapsedTime.start();
}

int AED::getElapsedSec()
{
    return elapsedTime.elapsed();
}

void AED::powerOff(){
    mutex.lock();
    powered = false;
    numOfShocks = 0;
    mutex.unlock();
}

void AED::selfTest(){
    if (hasBattery(20) && (electrode != nullptr)) {
        powerOn();
        emit passSelfTest();
    } else {
        if (!hasBattery(20)) emit batteryLow();
        else emit lossConnection();
    }
}

// if false returned in MW, poweroff
bool AED::batteryCheck(int minBattery) {
    if (batteryLevel < minBattery) {
        batteryLow();
        return false;
    }
    return true;
}

// if false returned in MW, poweroff
bool AED::connectCheck() {
    if (electrode == nullptr) {
        lossConnection();
        return false;
    }
    return true;
}

bool AED::attachCheck() {
    if (electrode == nullptr || electrode->getPatient() == nullptr) {
        lossAttach();
        return false;
    }
    return true;
}

void AED::connectElectrode(Electrode* electrode){
    mutex.lock();
    this->electrode = electrode;
    mutex.unlock();
}

Electrode* AED::getElectrode(){
    return electrode;
}

bool AED::hasBattery(int minBattery){
    return batteryLevel >= minBattery;
}

int AED::getBattery(){
    return batteryLevel;
}

void AED::setBattery(int b){
    mutex.lock();
    batteryLevel = b;
    mutex.unlock();
    emit updateBatteryUI();
}

void AED::chargeBattery() {
    setBattery(100);
}

void AED::consumeBattery(int b){
    int newBattery = batteryLevel - b;
    setBattery(newBattery);
}

void AED::analyzeAndDecideShock()
{
    // safety check
    if (!(powered && batteryCheck(5) && connectCheck() && attachCheck())) return;

    Patient* patient = electrode->getPatient();
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
        QThread::sleep(2);
        qInfo() << ecgWave << "detected. Shock is advised.";
        emit shockable();
    } else if (ecgWave == "PEA" || ecgWave == "ASYSTOLE") {
        QThread::sleep(2);
        qInfo() << ecgWave << "detected. No shock advised.";
        emit nonShockable();
    }
}

void AED::deliverShock()
{
    // safety check
    if (!(powered && batteryCheck(20) && connectCheck() && attachCheck())) return;

    qInfo() << "Shock at" << shockAmount << "J delivered.";
    ++numOfShocks;
    consumeBattery(20);
}

void AED::deliverCPR()
{
    // safety check
    if (!(powered && connectCheck() && attachCheck())) return;

    QElapsedTimer cprTimer;
    cprTimer.start();
    qint64 cprTime = 2 * 60 * 1000/4; // 2 minutes / 4 = 30 sec for simulation
    int numCycle = 0;

    while (cprTimer.elapsed() < cprTime) {
        QString feedBack = "";

        // safety - check electrode connection, pads attached
        if (!(powered && connectCheck() && attachCheck())) return;

        // 5 compressions
        int num = 0;
        for (; num < 5; ++num) {
            // safety - check electrode connection, pads attached
            if (!(powered && connectCheck() && attachCheck())) return;

            double depth = electrode->getCompressionDepth();
            int result = analayzeCPRDepth(depth);
            if (result == 0) feedBack = "Good CPR!";
            else if (result < 0) feedBack = "Push harder";
            else feedBack = "Push slower";

            CPRFeedback(feedBack + " " + QString::number(num+1), depth);
            QThread::sleep(3);

            if (result != 0) {
                // wait before getting new updateCPRDepth
                QThread::sleep(5);
            }
        }

        QThread::sleep(3);

        // 2 breathes
        if (!(powered && connectCheck() && attachCheck())) return;
        CPRFeedback("Give one breath", 0);
        QThread::sleep(3);

        if (!(powered && connectCheck() && attachCheck())) return;
        CPRFeedback("Give another breath", 0);
        QThread::sleep(3);
        ++numCycle;
    }

    if (!(powered && connectCheck() && attachCheck())) return;

    CPRFeedback("Stop CPR", 0);
    QThread::sleep(1);

    emit finishCPR();
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
    } else {
        return 0;
    }

}
