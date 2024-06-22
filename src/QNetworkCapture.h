#ifndef QNETWORKCAPTURE_H
#define QNETWORKCAPTURE_H

#include <QMainWindow>
#include <pcap.h>
#include <QVector>
#include "Capture.h"
#include "ReadOnlyDelegate.h"
#include "DataPackage.h"

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

    // show network card
    void showNetworkCard();
    // capture the data package
    int capture();

public slots:
    // void handleMessage(DataPackage data);

private slots:
    void on_comboBox_currentIndexChanged(int index);
    // void on_tableWidget_cellClicked(int row, int column);
    // void on_lineEdit_returnPressed();
    // void on_lineEdit_textChanged(const QString &arg1);
    // void on_tableWidget_currentCellChanged(int currentRow, int currentColumn,
    //                                        int previousRow, int previousColumn);

private:
    Ui::QNetworkCapture *ui;
    pcap_if_t *allDevices;                 // all adapter device
    pcap_if_t *device;                      // An adapter
    pcap_t *pointer;                        // data package pointer
    ReadOnlyDelegate* readOnlyDelegate;     // readonly detegate
    int countNumber;                        // countNumber
    int rowNumber;                          // rowNumber
    QVector<DataPackage>data;               // store data
    char errbuf[PCAP_ERRBUF_SIZE];          // error buffer
    bool isStart;                           // the thread is start or not
};
#endif // QNETWORKCAPTURE_H
