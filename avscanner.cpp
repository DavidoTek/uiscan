#include "avscanner.h"

AVScanner::AVScanner()
{
    clamProcess = new QProcess;
}

AVScanner *AVScanner::instance = nullptr;
AVScanner *AVScanner::getInstance() {
    if(!instance) {
        instance = new AVScanner();
    }
    return instance;
}

// setArguments sets the arguments for AVScanner
void AVScanner::setArguments(QStringList a)
{
    args = a;
}

// arguments returns the arguments for AVScanner
QStringList AVScanner::arguments()
{
    return args;
}

// parseArguments will parse the arguments (files to scan, flags)
void AVScanner::parseArguments()
{
    for(QString arg : arguments()) {
        if(arg.startsWith("-")) {  // is flag
            if(arg.startsWith("-d")) {              // -d : use clamdscan instead of clamscan
                clamApp = "clamdscan";
            } else if(arg.startsWith("-r")) {       // -r : run as root / administrator
                if (platform() == "linux") {
                    clamPath.prepend("pkexec ");
                }
            } else if(arg.startsWith("-l")) {       // -l : log file (log of clam[d]scan) - multiple logfiles possible
                QString file(arg);
                file.remove("-l");
                addLogFile(file);
            } else if(arg.startsWith("-c")) {       // -c : clamav path
                QString path(arg);
                path.remove("-c");
                clamPath.append(path);
            }
        } else {
            appendPath(arg);
        }
    }
}

void AVScanner::setPlatform(QString p)
{
    pf = p;
}

QString AVScanner::platform()
{
    return pf;
}

void AVScanner::appendPath(QString p)
{
    scanPaths.append(parsePath(p));
}

QStringList AVScanner::paths()
{
    return scanPaths;
}

// parsePath will parse the given file path to an format that clamav will accept. file:///C:/bla/bla.txt -> C:\\bla\\bla.txt
QString AVScanner::parsePath(QString p)
{
    p = p.remove("file:");
    if (platform() == "windows") {
        p = p.remove("///");
        p = p.replace('/' , '\\');
    } else {
        p = p.remove("//");
    }
    return p;
}

// addLogFile adds a path to the log file
void AVScanner::addLogFile(QString path)
{
    logFiles.append(path);
}

// log will log to all log files
void AVScanner::log(QString text)
{
    for (QString file : logFiles) {
        QFile f(file);
        if (f.open(QIODevice::ReadWrite | QIODevice::Append)) {
            QTextStream s(&f);
            s << text;
            f.close();
        } else {
            qDebug() << "Error writing to log file " << file;
        }
    }
}

// scan will scan all files
void AVScanner::scan()
{
    qDebug() << "SCAN STARTING";
    QStringList clamArguments;
    clamArguments.append("-r"); // Recursive scanning (Scan subdirectories)
    //clamArguments.append("-z"); // Continue scanning a file after a match for additional matches
    for (QString p : scanPaths) {
        clamArguments.append(p);
    }

    QProcess::connect(clamProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(handleClamOutput()));
    //QProcess::connect(clamProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, SLOT(handleScanFinished(int exitCode, QProcess::ExitStatus exitStatus)));
    connect(clamProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus){ handleScanFinished(exitCode, exitStatus); });

    clamProcess->start(clamPath + clamApp, clamArguments);
    qDebug() << "SCAN STARTED";
}

// stop will stop/kill the scan process
void AVScanner::stop()
{
    clamProcess->kill();
}

// handleClamOutput will be called whenever clamProcess has output.
void AVScanner::handleClamOutput()
{
    QString output = clamProcess->readAllStandardOutput();
    log(output);
    output = output.remove('\r');
    QStringList buffer = output.split('\n');

    for(QString line : buffer) {
        for (QString t : errorTypes) {  // error
            if(line.endsWith(t, Qt::CaseInsensitive)) {
                scannedFiles++;
                errorfiles.append(line);
            }
        }
        for (QString t : okTypes) { // ok
            if(line.endsWith(t, Qt::CaseInsensitive)) {
                scannedFiles++;
            }
        }
        for (QString t : excludedTypes) {   // excluded
            if(line.endsWith(t, Qt::CaseInsensitive)) {
                scannedFiles++;
                skippedfiles.append(line);
            }
        }
        if(line.endsWith("FOUND", Qt::CaseInsensitive)) {   // virus found
                scannedFiles++;
                infectedfiles.append(line);
        }
    }

    changeLabels(scannedFiles, infectedfiles.length(), fixedfiles.length(), errorfiles.length(), skippedfiles.length());
}

// getClamProcess return a pointer to clamProcess.
QProcess *AVScanner::getClamProcess()
{
    return clamProcess;
}


// handleScanFinished will be called when the QProcess is finished.
void AVScanner::handleScanFinished(int exitCode, QProcess::ExitStatus exitStatus)
{

}

// getInfectedFiles returns all infected files as a QStringList. TODO: also return skipped + error files
QStringList AVScanner::getInfectedFiles()
{
    return infectedfiles;
}

void AVScanner::changeLabels(int a, int b, int c, int d, int e)
{
    emit labelsChanged(a, b, c, d, e);
}

void AVScanner::fixFile(QString f)
{
    fixedfiles.append(f);
    infectedfiles.removeAll(f);
    log("DELETEDFILE " + f + "\n");
    changeLabels(scannedFiles, infectedfiles.length(), fixedfiles.length(), errorfiles.length(), skippedfiles.length());
}
