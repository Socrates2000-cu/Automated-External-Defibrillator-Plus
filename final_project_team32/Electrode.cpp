#include "Electrode.h"
#include <QDebug>

Electrode::Electrode() : compressionDepth(0) {
    patient = nullptr;
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
    compressionDepth = depth;
}

void Electrode::attachPatient(Patient* patient) {
    this->patient = patient;
}

bool Electrode::hasProperlyConnectedToPatient() {
    return patient != nullptr;
}

