#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <setupapi.h>
#include <Cfgmgr32.h>
#include <Regstr.h>

#include <QDebug>
#include <QMainWindow>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->setSortingEnabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
