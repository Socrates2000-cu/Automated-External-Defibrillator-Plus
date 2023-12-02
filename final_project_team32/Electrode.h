#ifndef ELECTRODE_H
#define ELECTRODE_H

#include <QObject>
#include "Patient.h"

class Electrode: public QObject {

    Q_OBJECT

public:
    Electrode(Patient* p=nullptr);

    Patient* getPatient();
    void setPatient(Patient*);
    bool hasProperlyConnectedToPatient();

private:
    float compressionDepth;  // in cm or inches
    // no need to specify different types of pads (adult, child)
    Patient* patient;

};

#endif // ELECTRODE_H
