#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // initialize AED and Electrode (pad)
    int initBatteryLevel = ui->battery->value();
    theAEDPlus = new AED(initBatteryLevel);
    patient = nullptr;
    electrode = new Electrode();
    if (ui->connectAED->isChecked()) theAEDPlus->connectElectrode(electrode);
    electrode->setCompressionDepth(ui->depth->text().toDouble());

    // confirm the patient's age before patient initialization
    connect(ui->confirm_patient, SIGNAL(pressed()), this, SLOT(confirmInitialization()));

    // connections of Electrode connections (to AED) and attachment (to chest)
    connect(ui->connectAED, SIGNAL(toggled(bool)), this, SLOT(changeElectrodeConnection(bool)));  // change state of AED-Electrode connection
    connect(ui->connectChest, SIGNAL(toggled(bool)), this, SLOT(changePatientAttach(bool)));  // change state of AED-Patient attachment

    // Power button
    connect(ui->powerButton, &QPushButton::clicked, this, &MainWindow::pressPowerButton);

    // Safety features and self test
    connect(theAEDPlus, &AED::passSelfTest, this, &MainWindow::passSelfTest);
    connect(theAEDPlus, &AED::batteryLow, this, &MainWindow::batteryLow);
    connect(theAEDPlus, &AED::lossConnection, this, &MainWindow::lossConnection);
    connect(theAEDPlus, &AED::lossAttach, this, &MainWindow::handleLossAttach);

    // progress bar for battery
    connect(theAEDPlus, &AED::updateBatteryUI, this, &MainWindow::updateBatteryUI);
    connect(ui->charge_battery, SIGNAL(clicked()), theAEDPlus, SLOT(chargeBattery()));

    // connections of attaching pad (step 3)
    connect(this, SIGNAL(attach()), this, SLOT(attachPads()));  // signal attach() -> attachPads()

    // connections of analysis and shock delivery (step 4)
    connect(this, SIGNAL(analyze()), this, SLOT(analyzeHeartRhythm()));  // signal analyze() -> analyzeHeartRhythm()
    connect(theAEDPlus, SIGNAL(shockable()), this, SLOT(shockable()));  // AED signal shockable() -> shockable()
    connect(theAEDPlus, SIGNAL(nonShockable()), this, SLOT(nonShockable()));  // AED signal nonShockable() -> nonShockable()
    connect(ui->deliverShock, SIGNAL(clicked()), this, SLOT(deliverShock()));  // shock button pressed -> deliverShock()

    // delivering CPR (step 5)
    connect(this, SIGNAL(cpr()), this, SLOT(deliverCPR()));  // cpr entry function
    connect(theAEDPlus, SIGNAL(CPRFeedback(QString, float)), this, SLOT(CPRFeedback(QString, float))); // AED signal CPRFeedback(QString, float) -> this CPRFeedback(QString, float)
    connect(ui->depth, SIGNAL(valueChanged(double)), electrode, SLOT(setCompressionDepth(double)));
    connect(theAEDPlus, SIGNAL(finishCPR()), this, SLOT(finishCPR()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete electrode;
    delete theAEDPlus;
    if (patient != nullptr) delete patient;
}

void MainWindow::confirmInitialization()
{
    // read UI dropboxes for patient initial state and age
    QString healthState = ui->health->currentText();
    QString age = ui->age->currentText();

    // initialize Patient
    if (patient != nullptr) { // free previous patient
        delete patient;
        electrode->attachPatient(nullptr);
    }
    patient = new Patient(age, healthState);

    // read UI checkboxes to decide if electrode is attached to patient
    if (ui->connectChest->isChecked()) electrode->attachPatient(patient);

    // disable the dropboxes of patient age and confirm button
    ui->age->setDisabled(true);
    ui->confirm_patient->setDisabled(true);

    // enable AED power button
    ui->powerButton->setEnabled(true);

    // each time comboBox changes, update patient's health state
    connect(ui->health, SIGNAL(currentTextChanged(QString)), patient, SLOT(setEcgWave(QString)));
}

void MainWindow::pressPowerButton()
{
    if (theAEDPlus->isPowered()) powerOff();
    else powerOn();
}

void MainWindow::powerOn()
{
    displayPrompt("POWERING UP AED...");
    QTimer::singleShot(1000, [this](){
        displayPrompt(QString("RUNNING SELF TEST"));
        QMetaObject::invokeMethod(theAEDPlus, "selfTest");
    });
}

void MainWindow::powerOff()
{
    displayPrompt("SHUTTING DOWN AED...");

    indicatorLightFlash(ui->indicator1, false);
    indicatorLightFlash(ui->indicator2, false);
    indicatorLightFlash(ui->indicator3, false);
    indicatorLightFlash(ui->indicator4, false);
    indicatorLightFlash(ui->indicator5, false);
    indicatorLightFlash(ui->deliverShock, false);
    ui->selfTest->setStyleSheet("image: url(:/self_test_not_ok.jpg);" "background-color: grey;" "border: none;");

    clearDisplay();
    resetAdmin();

    timer.stop();
    theAEDPlus->powerOff();  // call directly to force power off
}

void MainWindow::resetAdmin()
{
    // disable power button and shock button
    ui->powerButton->setDisabled(true);
    ui->deliverShock->setDisabled(true);

    // enable patient age initialization
    ui->age->setEnabled(true);
    ui->confirm_patient->setEnabled(true);

    // uncheck electrode and pads
    ui->connectAED->setChecked(false);
    ui->connectChest->setChecked(false);
    ui->depth->setValue(0.0);
    ui->charge_battery->setEnabled(true);

    ui->shocks->setText("Shock:  0");
    ui->elapsedtime->setText("E.T.:  00:00");
    ui->depthdisplay->setText("Dep: 0cm");
}

void MainWindow::passSelfTest(){
    ui->increase->setDisabled(true);
    ui->decrease->setDisabled(true);
    ui->charge_battery->setDisabled(true);  // cannot change battery when powered on
    displayPrompt("UNIT OK");
    ui->selfTest->setStyleSheet("image: url(:/self_test_ok.jpg);" "background-color: grey;" "border: none;");

    // start timer
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateDisplay()));
    timer.start(1000);

    // step 1
    QTimer::singleShot(2000, [this](){
        checkResponse();
    });
}

void MainWindow::updateDisplay()
{
    int secs = theAEDPlus->getElapsedSec() / 1000;
    int mins = (secs / 60) % 60;
    int hours = (secs / 3600);
    secs = secs % 60;
    QString et = QString("%1:%2:%3").arg(hours, 2, 10, QLatin1Char('0'))
            .arg(mins, 2, 10, QLatin1Char('0'))
            .arg(secs, 2, 10, QLatin1Char('0')) ;

    int numShocks  = theAEDPlus->getNumShocks();

    ui->elapsedtime->setText("E.T.: " + et.mid(3,5));// only need substring 00:00:00
    QString numShockText = QString::fromStdString(std::to_string(numShocks));
    ui->shocks->setText("Shocks: " + numShockText);
    QString depthText = QString::fromStdString(std::to_string(ui->depth->value()));
    ui->depthdisplay->setText("Dep: " + depthText.mid(0,3) + "cm");
}

void MainWindow::batteryLow()
{
    displayPrompt(QString("UNIT FAILED."));
    displayPrompt(QString("CHANGE BATTERIES."));
    qInfo("BATTERY LOW. RECHARGE OR CHANGE BATTERIES NOW TO USE.");
    QTimer::singleShot(2000, [this](){
        powerOff();
    });
}

void MainWindow::lossConnection()
{
    displayPrompt(QString("UNIT FAILED."));
    displayPrompt(QString("CONNECT ELECTRODE AND RESTART AED."));
    QTimer::singleShot(2000, [this](){
        powerOff();
    });
}

void MainWindow::handleLossAttach() {
    qDebug() << "Warning! The electrode pad is not attached to the patient's chest correctly.";
    if (theAEDPlus->getState() >= 3) emit attach();
}

void MainWindow::checkResponse() {
    if (!theAEDPlus->isPowered()) return;

    indicatorLightFlash(ui->indicator1, true);
    displayPrompt("STAY CALM");
    displayPrompt("CHECK RESPONSIVENESS.");
    theAEDPlus->setState(1);

    QTimer::singleShot(2000, [this](){
        callEmergency();
    });
}

void MainWindow::callEmergency() {
    if (!theAEDPlus->isPowered()) return;

    indicatorLightFlash(ui->indicator1, false);
    indicatorLightFlash(ui->indicator2, true);
    displayPrompt("CALLING EMERGENCY SERVICES!");
    theAEDPlus->setState(2);

    QTimer::singleShot(3000, [this](){
        emit attach();  // trigger attach (not function call stack)
    });
}

void MainWindow::changeElectrodeConnection(bool connected)
{
    if (connected) theAEDPlus->connectElectrode(electrode);
    else theAEDPlus->connectElectrode(nullptr);
}

void MainWindow::changePatientAttach(bool attached)
{
    if (attached) {
        electrode->attachPatient(patient);
        if (theAEDPlus->getState() == 3) emit analyze();
    } else {
        electrode->attachPatient(nullptr);
    }
}

void MainWindow::updateBatteryUI(){
    int battery = theAEDPlus->getBattery();
    ui->battery->setValue(battery);
}

void MainWindow::on_increase_clicked()
{
    int battery = theAEDPlus->getBattery();
    theAEDPlus->setBattery(battery + 1);
}

void MainWindow::on_decrease_clicked()
{
    int battery = theAEDPlus->getBattery();
    theAEDPlus->setBattery(battery - 1);
}

// attaching pad to chest
void MainWindow::attachPads()
{
    // safety - check electrode connection
    if (!theAEDPlus->isPowered() || !theAEDPlus->connectCheck()) return;

    indicatorLightFlash(ui->indicator2, false);
    indicatorLightFlash(ui->indicator3);
    displayPrompt("Attach defib pads to patient’s bare chest");
    theAEDPlus->setState(3);

    QTimer::singleShot(2000, [this](){
        // if already attached, trigger analyze
        if (theAEDPlus->getElectrode() != nullptr && theAEDPlus->getElectrode()->getPatient() != nullptr) {
            emit analyze();
        }
    });
}

void MainWindow::analyzeHeartRhythm()
{
    // safety - check 1 battery, 2 electrode connection, 3 pads attached
    if (!(theAEDPlus->isPowered()
          && theAEDPlus->batteryCheck(5)
          && theAEDPlus->connectCheck()
          && theAEDPlus->attachCheck())) return;

    indicatorLightFlash(ui->indicator3, false);
    indicatorLightFlash(ui->indicator4);
    displayPrompt("Don't touch patient. Analyzing.");

    QTimer::singleShot(3000, [this](){
        displayEcgPic();  // update heart rhythm picture
        indicatorLightFlash(ui->indicator4, false);  // turn indicator light off

        // call AED to analyze heart rhythm
        QMetaObject::invokeMethod(theAEDPlus, "analyzeAndDecideShock");
    });
}

// AED decides shockable signal -> this SLOT shockable
void MainWindow::shockable() {
    indicatorLightFlash(ui->deliverShock);
    ui->deliverShock->setEnabled(true);
}

void MainWindow::nonShockable() {
    emit cpr();
}

void MainWindow::deliverShock()
{
    // safety - check 1 battery, 2 electrode connection, 3 pads attached
    if (!(theAEDPlus->isPowered()
          && theAEDPlus->batteryCheck(20)
          && theAEDPlus->connectCheck()
          && theAEDPlus->attachCheck())) return;

    displayPrompt("STAND CLEAR");
    nonBlockingSleep(1);
    qInfo() << "<Voice Prompt> Shock will be delivered in three, ";
    nonBlockingSleep(1);
    qInfo() << "two, ";
    nonBlockingSleep(1);
    qInfo() << "one ... ";

    QMetaObject::invokeMethod(theAEDPlus, "deliverShock");
    nonBlockingSleep(1);
    qInfo() << "<Voice Prompt> Shock tone beeps. Shock delivered.";
    nonBlockingSleep(1);

    ui->deliverShock->setEnabled(false); // disabled again
    indicatorLightFlash(ui->deliverShock, false);

    emit cpr();
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

//display ecg wave when analyzing
void MainWindow::displayEcgPic(){
    clearDisplay();

    //get patient attribute
    Patient* conPatient = theAEDPlus->getElectrode()->getPatient();
    if (conPatient == nullptr) return;
    QString health_state = conPatient ->getEcgWave();

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

    // print the voice prompt to console
    qInfo() << "<Voice Prompt>" << input;

}

void MainWindow::deliverCPR()
{
    // safety - check 1 battery, 2 electrode connection, 3 pads attached
    if (!(theAEDPlus->isPowered()
          && theAEDPlus->batteryCheck(5)
          && theAEDPlus->connectCheck()
          && theAEDPlus->attachCheck())) return;

    indicatorLightFlash(ui->indicator5, true);
    QMetaObject::invokeMethod(theAEDPlus, "deliverCPR");
}

void MainWindow::finishCPR()
{
    indicatorLightFlash(ui->indicator5, false);

    QTimer::singleShot(1000, [this](){
        emit analyze();  // go back to analyze
    });
}

void MainWindow::CPRFeedback(QString feedBack, float cprDepth)
{
    displayPrompt(feedBack);
    QString depth = "Depth: " + QString::number(cprDepth) + " cm";
}

void MainWindow::clearDisplay() {
    // delete the layout we might have
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
