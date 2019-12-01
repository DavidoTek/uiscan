#ifndef AVSCANNER_H
#define AVSCANNER_H
#include <QString>
#include <QProcess>
#include <QDebug>
#include <QObject>
#include <QMessageBox>
#include <QFile>

class AVScanner : public QObject
{
    Q_OBJECT
private:
    AVScanner();
    static AVScanner *instance;

    QProcess *clamProcess;
    QString clamPath = "";
    QString clamApp = "clamscan";

    QString parsePath(QString p);
    QStringList scanPaths;

    QStringList args;
    QString pf;

    QStringList logFiles;
    void addLogFile(QString path);
    void log(QString text);

    QStringList errorTypes = {"Zip module failure", "RAR module failure", "Encrypted.RAR", "Encrypted.Zip", "Input/Output error", "Files number limit exceeded", "handler error", "Oversized.Zip"};
    QStringList okTypes = {"OK", "Empty file", "Broken.Executable"};
    QStringList excludedTypes = {"Excluded", "Symbolic Link"};

    int scannedFiles = 0;
    QStringList infectedfiles;
    QStringList fixedfiles;
    QStringList errorfiles;
    QStringList skippedfiles;

    void changeLabels(int a, int b, int c, int d, int e);

private slots:
    void handleClamOutput();
    void handleScanFinished(int exitCode, QProcess::ExitStatus exitStatus);

public:
    static AVScanner *getInstance();

    void setArguments(QStringList a);
    QStringList arguments();
    void parseArguments();

    void setPlatform(QString p);
    QString platform();

    void appendPath(QString p);
    QStringList paths();

    QProcess *getClamProcess();

    QStringList getInfectedFiles();

    void scan();
    void stop();
    void fixFile(QString f);

signals:
    void labelsChanged(int a, int b, int c, int d, int e);
};

#endif // AVSCANNER_H
