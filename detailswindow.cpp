#include "detailswindow.h"
#include "ui_detailswindow.h"

DetailsWindow::DetailsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DetailsWindow)
{
    ui->setupUi(this);

    myavs = AVScanner::getInstance();   // get AVScanner Singleton
    ui->listWidget->addItems(myavs->getInfectedFiles());

}

DetailsWindow::~DetailsWindow()
{
    delete ui;
}

void DetailsWindow::on_btnOK_clicked()
{
    close();
}

void DetailsWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Delete file?"));
    msgBox.setText(tr("Do you really want to delete this file permanently?") + "\n" + item->text().split(": ")[0]);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() == QMessageBox::Yes) {
        QFile::remove(item->text().split(": ")[0]);
        myavs->fixFile(item->text());
        ui->listWidget->takeItem(ui->listWidget->row(item));
    }
}
