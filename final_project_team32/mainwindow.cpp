#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <thread>
#include <chrono>
#include "Sleeper.cpp"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    theAEDPlus = new AED();
    Patient* patient = new Patient();
    Electrode* electrode = theAEDPlus->getElectrode();
    electrode->setPatient(patient);

    //for test --> turn indicator1 into red (now not working because the button is disabled at beginning)
    //connect(ui->deliverShock, SIGNAL(released()), this, SLOT(testButPressed()));

    ui->deliverShock->setEnabled(false); // disabled by default
    connect(ui->powerButton, SIGNAL(toggled(bool)), this, SLOT(pressPowerButton(bool)));

    connect(ui->deliverShock, SIGNAL(released()), this, SLOT(deliverShock()));

    // connections of signal-slots cycle
    connect(this, SIGNAL(analyze()), this, SLOT(analyzeHeartRhythm()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pressPowerButton(bool checked)
{
    if (checked) powerOn();
    else powerOff();
}

void MainWindow::powerOn()
{
    qDebug() << "Power on!";
    analyze();   // test purpose, step 4 goes first
}

void MainWindow::powerOff()
{
    qDebug() << "Power off!";
}

void MainWindow::analyzeHeartRhythm()
{
    // turn indicator light to flash
    indicatorLightFlash(ui->indicator4);

    // voice prompt
    qDebug() << "Don't touch patient. Analyzing.";

    // call AED to analyze heart rhythm
    theAEDPlus->analyzeAndDecideShock();

    // turn indicator light off
    indicatorLightFlash(ui->indicator4, false);
}

void MainWindow::shockable() {
    // indicator light flashes, enable button
    indicatorLightFlash(ui->deliverShock);
    ui->deliverShock->setEnabled(true);
}

//TODO: simulate sleeper
void MainWindow::deliverShock()
{
    qDebug() << "Don't touch patient. Analyzing.";
    printVoicePromptToDisplay("STAND CLEAR");
    qDebug() << "Shock will be delivered in three, two, one ...";
    theAEDPlus->deliverShock();
    qDebug() << "Shock tone beeps. Shock delivered.";
}

//TODO
void MainWindow::printVoicePromptToDisplay(QString prompt)
{
    return;
}

void MainWindow::indicatorLightFlash(QPushButton* indicator, bool on){
    if (on) indicator->setStyleSheet("background-color: red;border-style: solid;border-width: 1px;border-radius: 10px;max-width: 20px;max-height: 20px;min-width: 20px;min-height: 20px;");
    else indicator->setStyleSheet("background-color: red;border-style: solid;border-width: 1px;border-radius: 10px;max-width: 20px;max-height: 20px;min-width: 20px;min-height: 20px;");
    //TODO else turn it off style
}

void MainWindow::testButPressed(){
    //Sleeper::sleep(5);
    //for test --> turn indicator1 into red if press deliverShock
    ui->indicator1->setStyleSheet("background-color: red;border-style: solid;border-width: 1px;border-radius: 10px;max-width: 20px;max-height: 20px;min-width: 20px;min-height: 20px;");

}
