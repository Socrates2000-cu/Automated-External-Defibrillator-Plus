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
    electrode->setCompressionDepth(ui->lineEdit->text().toFloat());

    //for test --> turn indicator1 into red (now not working because the button is disabled at beginning)
    //connect(ui->deliverShock, SIGNAL(released()), this, SLOT(testButPressed()));

    ui->deliverShock->setEnabled(false); // disabled by default
    connect(ui->powerButton, SIGNAL(toggled(bool)), this, SLOT(pressPowerButton(bool)));

    // connections of signal-slots cycle
    connect(this, SIGNAL(analyze()), this, SLOT(analyzeHeartRhythm()));  // signal analyze() -> analyzeHeartRhythm()
    connect(theAEDPlus, SIGNAL(shockable()), this, SLOT(shockable()));  // AED signal shockable() -> this shockable()
    connect(ui->deliverShock, SIGNAL(released()), this, SLOT(deliverShock()));  // shock button pressed -> this deliverShock()

    connect(ui->testCPR, SIGNAL(released()), this, SLOT(testDeliverCPR()));
    connect(theAEDPlus, SIGNAL(CPRFeedback(QString, float)), this, SLOT(CPRFeedback(QString, float))); // AED signal CPRFeedback(QString, float) -> this CPRFeedback(QString, float)
    connect(ui->lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(updateCPRDepth(const QString &)));
    connect(theAEDPlus, SIGNAL(waitForGuiChange(int)), this, SLOT(waitForGuiChange(int)));
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
    else indicator->setStyleSheet("background-color: red;border-style: solid;border-width: 1px;border-radius: 10px;max-width: 20px;max-height: 20px;min-width: 20px;min-height: 20px;");
    //TODO else turn it off style
}

void MainWindow::testButPressed(){
    //Sleeper::sleep(5);
    //for test --> turn indicator1 into red if press deliverShock
    ui->indicator1->setStyleSheet("background-color: red;border-style: solid;border-width: 1px;border-radius: 10px;max-width: 20px;max-height: 20px;min-width: 20px;min-height: 20px;");

}

void MainWindow::testDeliverCPR()
{
    qDebug() << "Starting CPR test";
    theAEDPlus->deliverCPR();
    qDebug() << "finished CPR test";
}

void MainWindow::CPRFeedback(QString feedBack, float cprDepth)
{
    qDebug() << " updating display for cpr feedback";
    //TODO: use function to change it
    ui->textEdit->setText(feedBack);
    QString depth = "Depth: " + QString::number(cprDepth) + " cm";

    //set compresssion depth info
    QTextCursor cursor = ui->textEdit_4->textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cursor.insertText(depth);

    qDebug() << " finished updating display for cpr feedback";
}

void MainWindow::updateCPRDepth(const QString &text)
{
    try {
        float depth = text.toFloat();
        theAEDPlus->getElectrode()->setCompressionDepth(depth);
        eventLoop.quit();
    }  catch (const std::exception& e) {
        qDebug() << "exception occured while convertin cpr depth into float";
    }

}

void MainWindow::waitForGuiChange(int milliseconds)
{
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);

    connect(timer, &QTimer::timeout, &eventLoop, [=](){qDebug() << "timer out!"; eventLoop.quit();});
    timer->start(milliseconds);
    eventLoop.exec();

    delete timer;
}
