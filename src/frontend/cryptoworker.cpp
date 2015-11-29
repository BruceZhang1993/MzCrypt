#include <QtDebug>

#include <QThread>
#include "cryptoworker.h"
#include "aescrypt.h"

/*
 *  Define externals
 */
/*extern wchar_t *optarg;
extern int     optopt;
extern int     optind;
extern int     opterr;
*/


CryptoWorker::CryptoWorker(QString input, QString output, int decrypting) : QRunnable()
{
    this->input = input;
    this->output = output;
    this->decrypting = decrypting;
}

void CryptoWorker::run()
{
    qDebug() << QString("[worker thread %1] %2 -> %3")
           .arg(reinterpret_cast<qlonglong>(QThread::currentThread()->currentThreadId()))
           .arg(input).arg(output);

    int rc;

    FILE *infp = NULL;
    FILE *outfp = NULL;

    if (( infp = _wfopen(this->input.toStdWString().c_str(), L"rb")) == NULL)
        goto close_handles_and_return;
    if ((outfp = _wfopen(this->output.toStdWString().c_str(), L"wb")) == NULL)
        goto close_handles_and_return;

    // "normally" not overflowed...
    char errorBuf[4096];

    if (this->decrypting)
    {
        // should probably test against ascii, utf-16le, and utf-16be encodings
        rc = decrypt_stream_hardcoded(infp, outfp, errorBuf);
    }
    else
    {
        rc = encrypt_stream_hardcoded(infp, outfp, errorBuf);
    }

    emit signalWorkFinished(rc, QString(errorBuf));

close_handles_and_return:
    if (infp) fclose(infp);
    if (outfp) fclose(outfp);

}
