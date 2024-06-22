#ifndef CAPTURE_H
#define CAPTURE_H

#include <QThread>
#include <QQueue>
#include <QString>
#include <pcap.h>
#include <WinSock2.h>
#include "Format.h"
#include "DataPackage.h"


class Capture : public QThread
{
    Q_OBJECT
public:
    Capture();
    bool setPointer(pcap_t *pointer);
    void setFlag();
    void resetFlag();
    int ethernetPackageHandle(const unsigned char *pkt_content, QString &info);
    int ipPackageHandle(const unsigned char *pkt_content, int &ipPackage);
    QString arpPackageHandle(const unsigned char *pkt_content);
    QString icmpPackageHandle(const unsigned char *pkt_content);
    int tcpPackageHandle(const unsigned char *pkt_content, QString &info, int ipPackage);
    int udpPackageHandle(const unsigned char *pkt_content, QString &info);
    QString dnsPackageHandle(const unsigned char *pkt_content);

protected:
    static QString byteToHex(unsigned char *str, int size);
    void run();

signals:
    void send(DataPackage data);

private:
    pcap_t *pointer;
    struct pcap_pkthdr *header;
    const unsigned char *pkt_data;
    time_t localTimeVersionSec;
    struct tm localTime;
    char timeString[16];
    volatile bool isDone;   // done flag
};

#endif // CAPTURE_H
