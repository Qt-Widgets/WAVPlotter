#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "track.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->selectFile,SIGNAL(clicked()),SLOT(updateFile()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateFile(){
    ui->widget->setSource(QFileDialog::getOpenFileName(this,
                                        "Open WAV", "", tr("All Files (*.*)")));
}
