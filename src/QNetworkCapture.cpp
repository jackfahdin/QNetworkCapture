#include "QNetworkCapture.h"
#include "./ui_QNetworkCapture.h"

QNetworkCapture::QNetworkCapture(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QNetworkCapture)
{
    ui->setupUi(this);
    showNetworkCard();

    static bool index = false;
    connect(ui->actionrun_and_stop, &QAction::triggered, this, [=]() {
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

void QNetworkCapture::on_comboBox_currentIndexChanged(int index)
{
    int i = 0;
    if(index != 0) {
        for(device = all_device; i < index - 1; device = device->next, i++);
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
        pcap_freealldevs(all_device);
        device = nullptr;
        return -1;
    } else {
        if(pcap_datalink(pointer) != DLT_EN10MB) {
            pcap_close(pointer);
            pcap_freealldevs(all_device);
            device = nullptr;
            pointer = nullptr;
            return -1;
        }

        statusBar()->showMessage(device->name);
    }
    return 0;
}
