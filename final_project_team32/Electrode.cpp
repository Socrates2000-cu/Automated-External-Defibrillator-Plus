#include "Electrode.h"
#include <QDebug>

Electrode::Electrode() : compressionDepth(0) {
    thread.reset(new QThread);
    moveToThread(thread.get());
    thread->start();

    patient = nullptr;
}

Electrode::~Electrode() {
    thread->quit();
    thread->wait();
}

double Electrode::getCompressionDepth()
{
    return compressionDepth;
}

Patient* Electrode::getPatient() {
    return patient;
}

void Electrode::setCompressionDepth(double depth)
{
    mutex.lock();
    compressionDepth = depth;
    mutex.unlock();
}

void Electrode::attachPatient(Patient* patient) {
    mutex.lock();
    this->patient = patient;
    mutex.unlock();
}
