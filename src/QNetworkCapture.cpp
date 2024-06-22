#include "QNetworkCapture.h"
#include "./ui_QNetworkCapture.h"

#include <pcap.h>
#include <WinSock2.h>
#include "Capture.h"
#include <QStringList>
#include <QColor>
#include <QMessageBox>
#include <QTreeWidgetItem>

QNetworkCapture::QNetworkCapture(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QNetworkCapture)
{
    ui->setupUi(this);
    statusBar()->showMessage("Welcome to QNetworkCapture!");

    ui->toolBar->addAction(ui->actionStartCapture);
    ui->toolBar->addAction(ui->actionClearAll);
    ui->toolBar->addAction(ui->actionUp);
    ui->toolBar->addAction(ui->actionDown);
    ui->toolBar->addAction(ui->actionTop);
    ui->toolBar->addAction(ui->actionEnd);

    showNetworkCard();

    static bool index = false;
    connect(ui->actionStartCapture, &QAction::triggered, this, [=]() {
        index = !index;
        if(index) {
            // 开始
            capture();
        } else {

        }
    });
}

QNetworkCapture::~QNetworkCapture()
{
    delete ui;
}

// todo: 这个函数记得考虑使用QT的接口代替
void QNetworkCapture::showNetworkCard()
{
    int n = pcap_findalldevs(&allDevices, errbuf);
    if(n == -1) {
        ui->comboBox->addItem("error: " + QString(errbuf));
    } else {
        ui->comboBox->clear();
        ui->comboBox->addItem("pls choose card!");
        for(device = allDevices; device != nullptr; device = device->next) {
            QString device_name = device->name;
            device_name.replace("\\Device\\", "");
            QString des = device->description;
            QString item = device_name + des;
            ui->comboBox->addItem(item);
        }
    }
}

void QNetworkCapture::on_comboBox_currentIndexChanged(int index)
{
    int i = 0;
    if(index != 0) {
        for(device = allDevices; i < index - 1; device = device->next, i++);
    }
    return;
}

int QNetworkCapture::capture()
{
    if(device) {
        pointer = pcap_open_live(device->name, 65536, 1, 1000, errbuf);
    } else {
        return -1;
    }

    if(!pointer) {
        pcap_freealldevs(allDevices);
        device = nullptr;
        return -1;
    } else {
        if(pcap_datalink(pointer) != DLT_EN10MB) {
            pcap_close(pointer);
            pcap_freealldevs(allDevices);
            device = nullptr;
            pointer = nullptr;
            return -1;
        }

        statusBar()->showMessage(device->name);
    }
    return 0;
}
