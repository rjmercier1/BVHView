#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void
MainWindow::setText(QString s)
{
    ui->centralWidget->loadFile(s);
    ui->statusBar->showMessage(s);
}

void
MainWindow::openSelected()
{
    QString fileName = QFileDialog::getOpenFileName(this,
             tr("Open BVH File"), "", tr("BVH Files (*.bvh)"));
    if (fileName != "")
        ui->centralWidget->loadFile(fileName);
}

void
MainWindow::quitSelected()
{

}

MainWindow::~MainWindow()
{
    delete ui;
}
