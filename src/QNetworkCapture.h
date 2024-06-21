#ifndef QNETWORKCAPTURE_H
#define QNETWORKCAPTURE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class QNetworkCapture;
}
QT_END_NAMESPACE

class QNetworkCapture : public QMainWindow
{
    Q_OBJECT

public:
    QNetworkCapture(QWidget *parent = nullptr);
    ~QNetworkCapture();

private:
    Ui::QNetworkCapture *ui;
};
#endif // QNETWORKCAPTURE_H
