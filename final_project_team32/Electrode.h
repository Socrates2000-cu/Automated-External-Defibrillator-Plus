#ifndef ELECTRODE_H
#define ELECTRODE_H

#include <QObject>
#include "Patient.h"

class Electrode: public QObject {

    Q_OBJECT

public:
    Electrode();

    double getCompressionDepth();
    Patient* getPatient();

    void attachPatient(Patient*);
    bool hasProperlyConnectedToPatient();
//    void setCompressionDepth(float d);

public slots:
    void setCompressionDepth(double depth);

private:
    double compressionDepth;  // in cm
    // no need to specify different types of pads (adult, child)
    Patient* patient;

};

#endif // ELECTRODE_H
