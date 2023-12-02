#include "ElectrodePad.h"

ElectrodePad::ElectrodePad(Patient* p) : patient(p) {

}

void ElectrodePad::setPatient(Patient* p) {
    patient = p;
}

bool ElectrodePad::hasProperlyConnectedToPatient() {
    return patient != nullptr;
}
