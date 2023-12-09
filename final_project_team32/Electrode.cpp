#include "Electrode.h"

Electrode::Electrode() : compressionDepth(0) {

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
