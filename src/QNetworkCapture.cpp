#include "QNetworkCapture.h"
#include "./ui_QNetworkCapture.h"

QNetworkCapture::QNetworkCapture(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QNetworkCapture)
{
    ui->setupUi(this);
}

QNetworkCapture::~QNetworkCapture()
{
    delete ui;
}
