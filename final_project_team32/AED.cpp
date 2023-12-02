#include "AED.h"


AED::AED() : numOfShocks(0) {
    electrodePad = new ElectrodePad();
    elapsedTime.start(); //start elapsed timer
}

AED::~AED() {
//    delete electrodePad;
}

bool AED::isShockAdvised() {
   return false; //TODO
}

void AED::charge() {

}

