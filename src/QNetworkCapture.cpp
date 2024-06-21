#include "QNetworkCapture.h"
#include "./ui_QNetworkCapture.h"

QNetworkCapture::QNetworkCapture(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QNetworkCapture)
{
    ui->setupUi(this);
    showNetworkCard();
}

QNetworkCapture::~QNetworkCapture()
{
    delete ui;
}

// todo: 这个函数记得考虑使用QT的接口代替
void QNetworkCapture::showNetworkCard() {
    int n = pcap_findalldevs(&all_device, errbuf);
    if(n == -1) {
        ui->comboBox->addItem("error: " + QString(errbuf));
    } else {
        ui->comboBox->clear();
        ui->comboBox->addItem("pls choose card!");
        for(device = all_device; device != nullptr; device = device->next) {
            QString device_name = device->name;
            device_name.replace("\\Device\\", "");
            QString des = device->description;
            QString item = device_name + des;
            ui->comboBox->addItem(item);
        }
    }
}
