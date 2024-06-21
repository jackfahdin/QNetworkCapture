#ifndef QNETWORKCAPTURE_H
#define QNETWORKCAPTURE_H

#include <QMainWindow>
#include <pcap.h>

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

    void showNetworkCard();
    int capture();

private slots:
    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::QNetworkCapture *ui;
    pcap_if_t *all_device;
    pcap_if_t *device;
    pcap_t *pointer;
    char errbuf[PCAP_ERRBUF_SIZE];
};
#endif // QNETWORKCAPTURE_H
