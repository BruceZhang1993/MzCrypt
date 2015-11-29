#ifndef CRYPTOWORKER_H
#define CRYPTOWORKER_H

#include <QObject>
#include <QRunnable>

class CryptoWorker : public QObject, public QRunnable
{
    Q_OBJECT

public:
    CryptoWorker(QString, QString, int);
public:
    void run();

private:
    QString input;
    QString output;
    int decrypting;

signals:
    void signalWorkFinished(int status, const QString& message);
};

#endif // CRYPTOWORKER_H
