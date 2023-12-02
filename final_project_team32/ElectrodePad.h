#ifndef ELECTRODEPAD_H
#define ELECTRODEPAD_H

#include <QObject>

class ElectrodePad: public QObject {

    Q_OBJECT

public:
    ElectrodePad();

private:
    float compressionDepth;  // in cm or inches
    // no need to specify different types of pads (adult, child)
};

#endif // ELECTRODEPAD_H
