#include "Electrode.h"

Electrode::Electrode(Patient* p) : compressionDepth(0), patient(p) {

}

Patient* Electrode::getPatient() {
    return patient;
}

void Electrode::setPatient(Patient* p) {
    patient = p;
}

bool Electrode::hasProperlyConnectedToPatient() {
    return patient != nullptr;
}
