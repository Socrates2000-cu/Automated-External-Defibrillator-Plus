#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <thread>
#include <chrono>
#include "Sleeper.cpp"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent), ui(new Ui::MainWindow),
    powered(false)
{
    ui->setupUi(this);

    // initialize AED and Electrode (pad)
    theAEDPlus = new AED();
    electrode = new Electrode();
    if (ui->connectAED->isChecked()) theAEDPlus->connectElectrode(electrode);

    // disable power button and shock button at the beginning
    ui->powerButton->setEnabled(false);
    ui->deliverShock->setEnabled(false);

    // confirm the patient's age before patient initialization
    connect(ui->confirm_patient, SIGNAL(pressed()), this, SLOT(confirmInitialization()));

    // connections of Electrode connections (to AED) and attachment (to chest)
    connect(ui->connectAED, SIGNAL(toggled(bool)), this, SLOT(changeElectrodeConnection(bool)));  // change state of AED-Electrode connection
    connect(ui->connectChest, SIGNAL(toggled(bool)), this, SLOT(changePatientAttach(bool)));  // change state of AED-Patient attachment
    //connect(ui->pushButton_charge, SIGNAL(clicked()), this, SLOT(chargeBattery()));
    //connect(ui->, SIGNAL(valueChanged(double)), electrode, SLOT(setCompressionDepth(double)));

    // Power button
    //connect(ui->powerButton, &QPushButton::clicked, theAEDPlus, &AED::power);
    connect(ui->powerButton, &QPushButton::clicked, this, &MainWindow::pressPowerButton);
    connect(theAEDPlus, &AED::powerOffFromAED, this, &MainWindow::powerOff);

    // progress bar for battery
    connect(ui->battery, SIGNAL(valueChanged(int)), this, SLOT(updateBattery(int)));
    connect(theAEDPlus, &AED::updateFromAED, this, &MainWindow::setBattery);

    connect(ui->charge_battery, SIGNAL(clicked()), theAEDPlus, SLOT(chargeBattery()));

    // connections of attaching pad (step 3)
    connect(theAEDPlus, SIGNAL(attach()), this, SLOT(attachPads()));  // signal attach() -> attachPads()
    connect(ui->connectChest, SIGNAL(clicked(bool)), this, SLOT(connectedChest())); //checkbox for connected to chest

    // connections of analysis and shock delivery (step 4)
    connect(this, SIGNAL(analyze()), this, SLOT(analyzeHeartRhythm()));  // signal analyze() -> analyzeHeartRhythm()
    connect(theAEDPlus, SIGNAL(shockable()), this, SLOT(shockable()));  // AED signal shockable() -> this shockable()
    connect(ui->deliverShock, SIGNAL(released()), this, SLOT(deliverShock()));  // shock button pressed -> this deliverShock()
    connect(theAEDPlus, SIGNAL(updateNumOfShocks(int)), this, SLOT(updateNumOfShocks(int)));

    // connections of cpr (step 5)
    connect(theAEDPlus, SIGNAL(cpr()), this, SLOT());  // TODO cpr entry function
}

MainWindow::~MainWindow()
{
    delete ui;
    delete electrode;
    delete theAEDPlus;
}
bool MainWindow::isPowered(){
    return powered;
}

void MainWindow::confirmInitialization()
{
    // read UI dropboxes for patient initial state and age
    QString healthState = ui->health->currentText();
    QString age = ui->age->currentText();

    // initialize Patient
    patient = new Patient(age, healthState);

    // read UI checkboxes to decide if electrode is attached to patient
    if (ui->connectChest->isChecked()) electrode->attachPatient(patient);

    // disable the dropboxes of patient age and confirm button
    ui->age->setDisabled(true);
    ui->confirm_patient->setDisabled(true);

    // enable AED power button
    ui->powerButton->setEnabled(true);
}

void MainWindow::pressPowerButton()
{
    if (!powered){
        powerOn();
    }
    else{
        powerOff();
    }
}

void MainWindow::changeElectrodeConnection(bool connected)
{
    if (connected && theAEDPlus->getElectrode() != nullptr) return;  // already connected
    if (!connected && theAEDPlus->getElectrode() == nullptr) return;  // already disconnected
    if (connected) theAEDPlus->connectElectrode(electrode);
    else theAEDPlus->connectElectrode(nullptr);
}

void MainWindow::changePatientAttach(bool attached)
{
    if (attached && electrode->getPatient() != nullptr) return;  // already attached correctly
    if (!attached && electrode->getPatient() == nullptr) return;  // already disattached
    if (attached) electrode->attachPatient(patient);
    else electrode->attachPatient(nullptr);
}

void MainWindow::powerOn()
{
    qDebug() << "Power on!";
    theAEDPlus->powerOn();
    ui->increase->setEnabled(false);
    ui->decrease->setEnabled(false);
}

//CURRENTLY NOT IN USE
//can be called from AED or MW if we decide
void MainWindow::powerOff()
{
    //power off sequence of turning off LEDs or anything else
    qDebug() << "Power off!";

}

void MainWindow::setBattery(int v){

    ui->battery->setValue(v);
    qInfo("battery from AED is: %d", v);

}

void MainWindow::updateBattery(int v){

    v = ui->battery->value();
    theAEDPlus->setBattery(v);
    qInfo("battery is: %d", v);

}

void MainWindow::on_increase_clicked()
{
    int value = ui->battery->value();
    qInfo("value: %d", value);
    value+=1;
    ui->battery->setValue(value);
    qInfo("Increased to: %d", value);
}

void MainWindow::on_decrease_clicked()
{

    int value = ui->battery->value();
    qInfo("value: %d", value);
    value-=1;
    ui->battery->setValue(value);
    qInfo("Decreased to: %d", value);

}

void MainWindow::analyzeHeartRhythm()
{
    // turn indicator light to flash
    indicatorLightFlash(ui->indicator4);
    qInfo() << "<Voice Prompt> Don't touch patient. Analyzing.";
    nonBlockingSleep(3);

    // call AED to analyze heart rhythm
    theAEDPlus->analyzeAndDecideShock();
    displayEcgPic();  // update heart rhythm picture
    nonBlockingSleep(1);

    // turn indicator light off
    indicatorLightFlash(ui->indicator4, false); // TODO when to power this off, after shock or before shock?
}

// AED decides shockable signal -> this SLOT shockable
void MainWindow::shockable() {
    // indicator light flashes, enable button
    indicatorLightFlash(ui->deliverShock);

    ui->deliverShock->setEnabled(true);
}

//TODO: simulate sleeper
void MainWindow::deliverShock()
{
    qInfo() << "<Voice Prompt> Don't touch patient. Analyzing.";
    displayPrompt("STAND CLEAR");
    nonBlockingSleep(1);
    qInfo() << "<Voice Prompt> Shock will be delivered in three, ";
    nonBlockingSleep(1);
    qInfo() << "two, ";
    nonBlockingSleep(1);
    qInfo() << "one ... ";

    theAEDPlus->deliverShock();
    nonBlockingSleep(1);
    qInfo() << "<Voice Prompt> Shock tone beeps. Shock delivered.";
    nonBlockingSleep(1);

    ui->deliverShock->setEnabled(false); // disabled again
    indicatorLightFlash(ui->deliverShock, false);
}

void MainWindow::updateNumOfShocks(int num)
{
    ui->label_numShock_digit->setNum(num);
}

void MainWindow::indicatorLightFlash(QPushButton* indicator, bool on){
    if (indicator->objectName() == "deliverShock") {
        if (on) {
            indicator->setStyleSheet("image: none;");
            indicator->setStyleSheet("image: url(:/changed_red_deliver_shock.jpg);"
                                     "background-color: grey;"
                                     "border:none;");
        } else {
            indicator->setStyleSheet("image: none;");
            indicator->setStyleSheet("image: url(:/deliver_shock.jpg);"
                                     "background-color: grey;"
                                     "border:none;");
        }
        return;
    }

    if (on) indicator->setStyleSheet("background-color: red;border-style: solid;border-width: 1px;border-radius: 10px;max-width: 20px;max-height: 20px;min-width: 20px;min-height: 20px;");
    else indicator->setStyleSheet("background-color: grey;border-style: solid;border-width: 1px;border-radius: 10px;max-width: 20px;max-height: 20px;min-width: 20px;min-height: 20px;");
}

//for indicator 3 (part1) - attaching pad to chest
void MainWindow::attachPads()
{
    // turn indicator light to flash
    indicatorLightFlash(ui->indicator3);

    qInfo() << "<Voice Prompt> Attach defib pads to patient’s bare chest";

    //text prompt display
    displayPrompt("Attach defib pads to patient’s bare chest");

}

//for indicator 3 (part2) - attaching pad to chest
void MainWindow::connectedChest(){
    // turn indicator light off
    indicatorLightFlash(ui->indicator3, false);

    //text prompt display
    ui->textDisplay->setText("");

    emit analyze(); // trigger step 4
}

//display ecg wave when analyzing
void MainWindow::displayEcgPic(){
    clearDisplay();

    //get patient attribute
    Patient* patient = theAEDPlus->getElectrode()->getPatient();
    QString health_state = patient ->getEcgWave();

    if(health_state=="PEA"){ui->textDisplay->setIcon(QIcon(":/PEA.jpg"));}
    else if (health_state=="ASYSTOLE"){ui->textDisplay->setIcon(QIcon(":/ASYSTOLE.jpg"));}
    else if (health_state=="V_FIB"){ui->textDisplay->setIcon(QIcon(":/V_FIB.jpg"));}
    else {ui->textDisplay->setIcon(QIcon(":/V_TACH.jpg"));}

    ui->textDisplay->setFixedSize(160,90);
    ui->textDisplay->setIconSize(QSize(160,90));

}

void MainWindow::displayPrompt(QString input){
    clearDisplay();

    //these lines help QPushButton to word wrap
    auto btn = ui->textDisplay;
    auto label = new QLabel(input,btn);
    label->setWordWrap(true);
    label->setStyleSheet("border: 0px solid black;");
    auto layout1 = new QHBoxLayout(btn);
    layout1->addWidget(label,0,Qt::AlignCenter);

}

void MainWindow::clearDisplay() {
    //delete the layout we might have
    QLayout *layout = ui->textDisplay->layout();
    if (layout)
    {
        QLayoutItem * item;
        while ((item = layout->takeAt(0)) != 0)
        {
            item->widget()->hide();
            delete item->widget();
            delete item;
        }

        delete layout;
    }

    // delete icon if any
    ui->textDisplay->setIcon(QIcon());
}

void MainWindow::nonBlockingSleep(int seconds)
{
    QTime timeout = QTime::currentTime().addSecs(seconds);
    while (QTime::currentTime() < timeout)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
}
