#include "QNetworkCapture.h"

#include <QApplication>
#include <InfoVersion.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QNetworkCapture w;
    w.setWindowTitle(QString("%1-v%2 (Jackfahdin)")
                         .arg(APPNAME, APPLICATION_VERSION));
    w.show();
    return a.exec();
}
