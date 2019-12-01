#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

#include "avscanner.h"
#include "detailswindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnCancel_clicked();
    void handleInfoTextChanged(int a, int b, int c, int d, int e);

    void on_btnShowDetails_clicked();

private:
    Ui::MainWindow *ui;
    AVScanner *myavs;
    DetailsWindow *dw;
    bool scanFinished = false;

protected:
    void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
