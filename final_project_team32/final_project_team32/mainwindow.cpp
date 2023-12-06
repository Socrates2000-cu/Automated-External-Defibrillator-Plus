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

    //for test --> turn indictaor1 into red
    connect(ui->deliverShock,SIGNAL(released()),this,SLOT(testButPressed()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::testButPressed(){
    //Sleeper::sleep(5);
    //for test --> turn indicator1 into red if press deliverShock
    ui->indicator1->setStyleSheet("background-color: red;border-style: solid;border-width: 1px;border-radius: 10px;max-width: 20px;max-height: 20px;min-width: 20px;min-height: 20px;");

}
