#ifndef ELECTRODE_H
#define ELECTRODE_H

#include <QObject>
#include "Patient.h"

class Electrode: public QObject {

    Q_OBJECT

public:
    Electrode();
    ~Electrode();

    Patient* getPatient();
    double getCompressionDepth();

    void attachPatient(Patient*);

public slots:
    void setCompressionDepth(double depth);

private:
    std::unique_ptr<QThread> thread;
    QMutex mutex;

    Patient* patient;
    double compressionDepth;  // in cm


};

#endif // ELECTRODE_H
