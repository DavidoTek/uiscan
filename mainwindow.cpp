#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lblTitle->setText(tr("Scanner - Starting..."));

    myavs = AVScanner::getInstance();   // get AVScanner Singleton

    // platform dependent stuff: bring window to front etc.
    if(myavs->platform() == "windows") {
        this->activateWindow();
    } else if (myavs->platform() == "darwin") {
        this->raise();
    }

    myavs->parseArguments();

    // set window title. "no files specified" if no files are specified. 1 file: show file name, more than 1 file: show name of first file and "..."
    if(myavs->paths().length() <= 1) {
        setWindowTitle(tr("uiscan - no files specified!"));
        ui->lblTitle->setText(tr("Scanner - no files specified!"));
        ui->btnCancel->setText(tr("CLOSE"));
        scanFinished = true;
    } else if (myavs->paths().length() == 2) {
        setWindowTitle("uiscan - " + myavs->paths()[1]);
    } else {
        setWindowTitle("uiscan - " + myavs->paths()[1] + " ...");
    }

    // connect myavs>labelsChanged to handleInfoTextChanged | Labels get updated (Viruses scanned, detected etc.)
    connect(myavs, SIGNAL(labelsChanged(int, int, int, int, int)), this, SLOT(handleInfoTextChanged(int, int, int, int, int)));

    // change the cancel button to a close button if clamprocess is finished.
    connect(myavs->getClamProcess(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus){
        scanFinished = true;
        ui->btnCancel->setText(tr("CLOSE"));
        ui->lblTitle->setText(tr("Scanner - DONE!"));
        ui->btnShowDetails->setEnabled(true);
    });

    if(!scanFinished) {
        myavs->scan();  // Start clamscan
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnCancel_clicked()
{
    if(scanFinished) {
        close();
    } else{
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Cancel scan?"));
        msgBox.setText(tr("Do you really want to cancel the scan?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        if(msgBox.exec() == QMessageBox::Yes) {
            myavs->stop();
            ui->btnCancel->setText(tr("CLOSE"));
            scanFinished = true;
        }
    }
}

// handleInfoTextChanged SIGNAL will be called when the labels are supposed to change.
void MainWindow::handleInfoTextChanged(int a, int b, int c, int d, int e)
{
    ui->txtScannedFiles->setText(QString::number(a));
    ui->txtInfectedFiles->setText(QString::number(b));
    ui->txtFixedFiles->setText(QString::number(c));
    ui->txtErrorFiles->setText(QString::number(d));
    ui->txtSkippedFiles->setText(QString::number(e));
    if(a > 0) {
        ui->lblTitle->setText(tr("Scanner - Scanning..."));
    }
}

void MainWindow::on_btnShowDetails_clicked()
{
    dw = new DetailsWindow;
    dw->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(scanFinished) {
        event->accept();
    } else{
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Cancel scan?"));
        msgBox.setText(tr("Do you really want to cancel the scan?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        if(msgBox.exec() == QMessageBox::Yes) {
            myavs->stop();
            event->accept();
        } else {
            event->ignore();
        }
    }
}
