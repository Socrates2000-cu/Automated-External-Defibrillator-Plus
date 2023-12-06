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

    //Patient* patient = new Patient();
    electrode = new Electrode();
    if(ui->checkBox_pad_aed->isChecked()) theAEDPlus->connectElectrode(electrode);


    //for test --> turn indicator1 into red (now not working because the button is disabled at beginning)
    //connect(ui->deliverShock, SIGNAL(released()), this, SLOT(testButPressed()));

    //Power button
    connect(ui->powerButton, &QPushButton::clicked, theAEDPlus, &AED::power);
    connect(ui->powerButton, &QPushButton::clicked, this, &MainWindow::pressPowerButton);

    ui->deliverShock->setEnabled(false); // disabled by default

    //progress bar for battery
    connect(ui->battery, SIGNAL(valueChanged(int)), this, SLOT(updateBattery(int)));
    connect(theAEDPlus, &AED::updateFromAED, this, &MainWindow::setBattery);

    //indicator3
    connect(ui->testButton, SIGNAL(released()), this, SLOT(attachPads())); //for test: simulate receiving the signal from the second step(i.e., indicator 2)
    connect(ui->connectChest, SIGNAL(clicked(bool)), this, SLOT(connectedChest())); //checkbox for connected to chest
    connect(ui->testButton2, SIGNAL(released()), this, SLOT(setEcgpic()));

    // connections of signal-slots cycle
    connect(this, SIGNAL(analyze()), this, SLOT(analyzeHeartRhythm()));  // signal analyze() -> analyzeHeartRhythm()
    connect(theAEDPlus, SIGNAL(shockable()), this, SLOT(shockable()));  // AED signal shockable() -> this shockable()
    connect(ui->deliverShock, SIGNAL(released()), this, SLOT(deliverShock()));  // shock button pressed -> this deliverShock()
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pressPowerButton()
{
   powerOn();
}

void MainWindow::powerOn()
{
    qDebug() << "Power on!";
    analyze();   // test purpose, step 4 goes first
}

//CURRENTLY NOT IN USE
void MainWindow::powerOff()
{
    qDebug() << "Power off!";
}

void MainWindow::setBattery(int v){

    ui->battery->setValue(v);
    qInfo("battery from AED is: %d", v);

}

//void MainWindow::electrodeConnected(){


//}

void MainWindow::updateBattery(int v){

    v = ui->battery->value();
    theAEDPlus->setBattery(v);
    qInfo("battery is: %d", v);

}

//void MainWindow::checkBattery(){

//    int v = ui->battery->value();
//    aedObj.setBattery(v);
//    qInfo("battery is: %d", v);

//}


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
    qDebug() << "is shockable";
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
    ui->deliverShock->setEnabled(false); // disabled again
}

//TODO
void MainWindow::printVoicePromptToDisplay(QString prompt)
{
    return;
}

void MainWindow::indicatorLightFlash(QPushButton* indicator, bool on){
    if (on) indicator->setStyleSheet("background-color: red;border-style: solid;border-width: 1px;border-radius: 10px;max-width: 20px;max-height: 20px;min-width: 20px;min-height: 20px;");
    else indicator->setStyleSheet("background-color: grey;border-style: solid;border-width: 1px;border-radius: 10px;max-width: 20px;max-height: 20px;min-width: 20px;min-height: 20px;");
    //TODO else turn it off style
}

//for indicator 3 (part1)
void MainWindow::attachPads()
{
    // turn indicator light to flash
    indicatorLightFlash(ui->indicator3);

    // voice prompt
    qDebug() << "Attach defib pads to patient’s bare chest";

    //text prompt display
    displayPrompt("Attach defib pads to patient’s bare chest");

}

//for indicator 3 (part2)
void MainWindow::connectedChest(){
    qDebug() << "trigger the indicator 4";
    //analyze(); //how to trigger next step?

    // turn indicator light off
    indicatorLightFlash(ui->indicator3, false);

    //text prompt display
    ui->textDisplay->setText("");
}

//display ecg wave when analyzing
void MainWindow::setEcgpic(){

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

void MainWindow::testButPressed(){
    //Sleeper::sleep(5);
    //for test --> turn indicator1 into red if press deliverShock
    ui->indicator1->setStyleSheet("background-color: red;border-style: solid;border-width: 1px;border-radius: 10px;max-width: 20px;max-height: 20px;min-width: 20px;min-height: 20px;");

}

void MainWindow::displayPrompt(QString input){

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

    //these lines help qpushbutton to word wrap
    auto btn = ui->textDisplay;
    auto label = new QLabel(input,btn);
    label->setWordWrap(true);
    label->setStyleSheet("border: 0px solid black;");
    auto layout1 = new QHBoxLayout(btn);
    layout1->addWidget(label,0,Qt::AlignCenter);

}
