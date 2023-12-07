#include "Electrode.h"
#include <QDebug>

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

void Electrode::setCompressionDepth(float d) {compressionDepth = d;}
float Electrode::getCompressionDepth()
{
    qDebug() << "Setting Electrode's compressionDepth";
    return compressionDepth;
}
