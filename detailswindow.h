#ifndef DETAILSWINDOW_H
#define DETAILSWINDOW_H

#include <QDialog>
#include <QListWidget>
#include <QFile>
#include "avscanner.h"

namespace Ui {
class DetailsWindow;
}

class DetailsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DetailsWindow(QWidget *parent = nullptr);
    ~DetailsWindow();

private slots:
    void on_btnOK_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::DetailsWindow *ui;
    AVScanner *myavs;
};

#endif // DETAILSWINDOW_H
