#ifndef ELECTRODEPAD_H
#define ELECTRODEPAD_H

#include <QObject>
#include "Patient.h"

class ElectrodePad: public QObject {

    Q_OBJECT

public:
    ElectrodePad(Patient* p=nullptr);
    void setPatient(Patient*);
    bool hasProperlyConnectedToPatient();

private:
    float compressionDepth;  // in cm or inches
    // no need to specify different types of pads (adult, child)
    Patient* patient;

};

#endif // ELECTRODEPAD_H
