#include "DataPackage.h"
#include <QMetaType>
#include <QVector>
#include <winsock.h>

DataPackage::DataPackage()
{
    // register the DataPackage type then
    qRegisterMetaType<DataPackage>("DataPackage");
    this->timeStamp = "";
    this->dataLength = 0;
    this->packageType = 0;
    this->pkt_content = nullptr;
}

void DataPackage::setDataLength(unsigned int length)
{
    this->dataLength = length;
}

void DataPackage::setTimeStamp(QString timeStamp)
{
    this->timeStamp = timeStamp;
}

void DataPackage::setPackageType(int type){
    this->packageType = type;
}

void DataPackage::setPackagePointer(const unsigned char *pkt_content, int size)
{
    this->pkt_content = (unsigned char*)malloc(size);
    if(this->pkt_content != nullptr)
        memcpy((char*)(this->pkt_content), pkt_content,size);
    else
        this->pkt_content = nullptr;
    //  Do not use  `this->pkt_content = pkt_content;`
}

void DataPackage::setPackageInfo(QString info){
    this->info = info;
}

QString DataPackage::byteToHex(unsigned char *str, int size)
{
    QString res = "";
    for(int i = 0; i < size; i++){
        char one = str[i] >> 4;
        char two = str[i] & 0xF;

        if(one >= 0x0A)
            one = one + 0x41 - 0x0A;
        else
            one = one + 0x30;

        if(two >= 0x0A)
            two = two  + 0x41 - 0x0A;
        else
            two = two + 0x30;

        res.append(one);
        res.append(two);
    }
    return res;
}

QString DataPackage::getTimeStamp()
{
    return this->timeStamp;
}

QString DataPackage::getDataLength()
{
    return QString::number(this->dataLength);
}

QString DataPackage::getPackageType()
{
    switch (this->packageType) {
    case 1:return ARP;
    case 2:return ICMP;
    case 3:return TCP;
    case 4:return UDP;
    case 5:return DNS;
    case 6:return TLS;
    case 7:return SSL;
    // TODO ...more protocol you can add
    default:
        return "";
    }
}

QString DataPackage::getInfo()
{
    return info;
}

QString DataPackage::getSource()
{
    if(this->packageType == 1)
        return getArpSourceIpAddr();
    else
        return getSrcIpAddr();
}

QString DataPackage::getDestination()
{
    if(this->packageType == 1)
        return getArpDestinationIpAddr();
    else
        return getDesIpAddr();
}

/* Ether */
/********************** get destination ethenet address **********************/
QString DataPackage::getDesMacAddr()
{
    ETHERNETHEADER *ethernet;
    ethernet = (ETHERNETHEADER*)pkt_content;
    unsigned char *addr;
    if(ethernet) {
        addr = ethernet->ethernetDesHost;
        if(addr) {
            QString res = byteToHex(addr,1) + ":"
                          + byteToHex((addr+1),1) + ":"
                          + byteToHex((addr+2),1) + ":"
                          + byteToHex((addr+3),1) + ":"
                          + byteToHex((addr+4),1) + ":"
                          + byteToHex((addr+5),1);
            if(res == "FF:FF:FF:FF:FF:FF")
                return "FF:FF:FF:FF:FF:FF(Broadcast)";
            else
                return res;
        }
    }
    return "";
}

/********************** get source ethenet address **********************/
QString DataPackage::getSrcMacAddr()
{
    ETHERNETHEADER *ethernet;
    ethernet = (ETHERNETHEADER*)pkt_content;
    unsigned char *addr;
    if(ethernet) {
        addr = ethernet->ethernetSrcHost;
        if(addr) {
            QString res = byteToHex(addr,1) + ":"
                          + byteToHex((addr+1),1) + ":"
                          + byteToHex((addr+2),1) + ":"
                          + byteToHex((addr+3),1) + ":"
                          + byteToHex((addr+4),1) + ":"
                          + byteToHex((addr+5),1);
            if(res == "FF:FF:FF:FF:FF:FF")
                return "FF:FF:FF:FF:FF:FF(Broadcast)";
            else
                return res;
        }
    }
    return "";
}

/********************** get ethenet type **********************/
QString DataPackage::getMacType()
{
    ETHERNETHEADER *ethernet;
    ethernet = (ETHERNETHEADER*)pkt_content;
    unsigned short ethernetType = ntohs(ethernet->ethernetType);
    switch (ethernetType) {
    case 0x0800:
        return "IPv4(0x800)";
    case 0x0806:
        return "ARP(0x0806)";
    default:
        return "";
    }
}

/* ip */
/********************** get destination ip address **********************/
QString DataPackage::getDesIpAddr()
{
    IPHEADER *ip;
    ip = (IPHEADER*)(pkt_content + 14);
    sockaddr_in desAddr;
    desAddr.sin_addr.s_addr = ip->desAdder;
    return QString(inet_ntoa(desAddr.sin_addr));
}

/********************** get source ip address **********************/
QString DataPackage::getSrcIpAddr()
{
    IPHEADER *ip;
    ip = (IPHEADER*)(pkt_content + 14);
    sockaddr_in srcAddr;
    srcAddr.sin_addr.s_addr = ip->srcAddr;
    return QString(inet_ntoa(srcAddr.sin_addr));
}

/********************** get ip version **********************/
QString DataPackage::getIpVersion()
{
    IPHEADER *ip;
    ip = (IPHEADER*)(pkt_content + 14);
    return QString::number(ip->versionHeaderLength >> 4);
}

/********************** get ip header length **********************/
QString DataPackage::getIpHeaderLength()
{
    IPHEADER *ip;
    ip = (IPHEADER*)(pkt_content + 14);
    QString res = "";
    int length = ip->versionHeaderLength & 0x0F;
    if(length == 5)
        res = "20 bytes (5)";
    else
        res = QString::number(length*5) + "bytes (" + QString::number(length) + ")";

    return res;
}

/********************** get ip TOS **********************/
QString DataPackage::getIpTos()
{
    IPHEADER *ip;
    ip = (IPHEADER*)(pkt_content + 14);
    return QString::number(ntohs(ip->TOS));
}

/********************** get ip total length **********************/
QString DataPackage::getIpTotalLength()
{
    IPHEADER *ip;
    ip = (IPHEADER*)(pkt_content + 14);
    return QString::number(ntohs(ip->totalLength));
}

/********************** get ip indentification **********************/
QString DataPackage::getIpIdentification()
{
    IPHEADER *ip;
    ip = (IPHEADER*)(pkt_content + 14);
    return QString::number(ntohs(ip->identification), 16);
}

/********************** get ip flag **********************/
QString DataPackage::getIpFlag()
{
    IPHEADER *ip;
    ip = (IPHEADER*)(pkt_content + 14);
    return QString::number((ntohs(ip->flagOffset)& 0xe000) >> 8, 16);
}

/********************** get ip reverse bit **********************/
QString DataPackage::getIpReservedBit()
{
    IPHEADER *ip;
    ip = (IPHEADER*)(pkt_content + 14);
    int bit = (ntohs(ip->flagOffset) & 0x8000) >> 15;
    return QString::number(bit);
}

/********************** get ip DF flag[Don't Fragment] **********************/
QString DataPackage::getIpDF()
{
    IPHEADER *ip;
    ip = (IPHEADER*)(pkt_content + 14);
    return QString::number((ntohs(ip->flagOffset) & 0x4000) >> 14);
}

/********************** get ip MF flag[More Fragment] **********************/
QString DataPackage::getIpMF()
{
    IPHEADER *ip;
    ip = (IPHEADER*)(pkt_content + 14);
    return QString::number((ntohs(ip->flagOffset) & 0x2000) >> 13);
}

/********************** get ip Fragment Offset **********************/
QString DataPackage::getIpFragmentOffset()
{
    IPHEADER *ip;
    ip = (IPHEADER*)(pkt_content + 14);
    return QString::number(ntohs(ip->flagOffset) & 0x1FFF);
}

/********************** get ip TTL **********************/
QString DataPackage::getIpTTL()
{
    IPHEADER *ip;
    ip = (IPHEADER*)(pkt_content + 14);
    return QString::number(ip->ttl);
}

/********************** get ip protocol **********************/
QString DataPackage::getIpProtocol()
{
    IPHEADER *ip;
    ip = (IPHEADER*)(pkt_content + 14);
    int protocol = ip->protocol;
    switch (protocol) {
    case 1:
        return "ICMP (1)";
    case 6:
        return "TCP (6)";
    case 17:
        return "UDP (17)";
    default:
        return "";
    }
}

/********************** get ip checksum **********************/
QString DataPackage::getIpCheckSum()
{
    IPHEADER *ip;
    ip = (IPHEADER*)(pkt_content + 14);
    return QString::number(ntohs(ip->checkSum), 16);
}

/* icmp */
/********************** get icmp type **********************/
QString DataPackage::getIcmpType()
{
    ICMPHEADER *icmp;
    icmp = (ICMPHEADER*)(pkt_content + 14 + 20);
    return QString::number(ntohs(icmp->type));
}

/********************** get icmp code **********************/
QString DataPackage::getIcmpCode()
{
    ICMPHEADER *icmp;
    icmp = (ICMPHEADER*)(pkt_content + 14 + 20);
    return QString::number(ntohs(icmp->code));
}

/********************** get icmp checksum **********************/
QString DataPackage::getIcmpCheckSum()
{
    ICMPHEADER *icmp;
    icmp = (ICMPHEADER*)(pkt_content + 14 + 20);
    return QString::number(ntohs(icmp->checkSum), 16);
}

/********************** get icmp identification **********************/
QString DataPackage::getIcmpIdentification()
{
    ICMPHEADER *icmp;
    icmp = (ICMPHEADER*)(pkt_content + 14 + 20);
    return QString::number(ntohs(icmp->identification));
}

/********************** get icmp sequence **********************/
QString DataPackage::getIcmpSequeue()
{
    ICMPHEADER *icmp;
    icmp = (ICMPHEADER*)(pkt_content + 14 + 20);
    return QString::number(ntohs(icmp->sequence));
}

QString DataPackage::getIcmpData(int size)
{
    char*icmp;
    icmp = (char*)(pkt_content + 14 + 20 + 8);
    QString res= "";
    for(int i = 0;i < size;i++) {
        res += (*icmp);
        icmp++;
    }
    return res;
}

/* arp info */
QString DataPackage::getArpHardwareType()
{
    ARPHEADER *arp;
    arp = (ARPHEADER*)(pkt_content + 14);
    int type = ntohs(arp->hardwareType);
    QString res = "";
    if(type == 0x0001)
        res = "Ethernet(1)";
    else
        res = QString::number(type);
    return res;
}

/********************** get arp protocol type **********************/
QString DataPackage::getArpProtocolType()
{
    ARPHEADER *arp;
    arp = (ARPHEADER*)(pkt_content + 14);
    int type = ntohs(arp->protocolType);
    QString res = "";
    if(type == 0x0800)
        res = "IPv4(0x0800)";
    else
        res = QString::number(type);
    return res;
}

/********************** get hardware length **********************/
QString DataPackage::getArpHardwareLength()
{
    ARPHEADER *arp;
    arp = (ARPHEADER*)(pkt_content + 14);
    return QString::number(arp->macLength);
}

/********************** get arp protocol length **********************/
QString DataPackage::getArpProtocolLength()
{
    ARPHEADER *arp;
    arp = (ARPHEADER*)(pkt_content + 14);
    return QString::number(arp->ipLength);
}

/********************** get arp operator code **********************/
QString DataPackage::getArpOperationCode(){
    ARPHEADER *arp;
    arp = (ARPHEADER*)(pkt_content + 14);
    int code = ntohs(arp->opCode);
    QString res = "";
    if(code == 1)
        res  = "request(1)";
    else if(code == 2)
        res = "reply(2)";
    return res;
}

/********************** get arp source ethernet address **********************/
QString DataPackage::getArpSourceEtherAddr()
{
    ARPHEADER *arp;
    arp = (ARPHEADER*)(pkt_content + 14);
    unsigned char *addr;
    if(arp) {
        addr = arp->srcEthAddr;
        if(addr){
            QString res = byteToHex(addr,1) + ":"
                          + byteToHex((addr+1),1) + ":"
                          + byteToHex((addr+2),1) + ":"
                          + byteToHex((addr+3),1) + ":"
                          + byteToHex((addr+4),1) + ":"
                          + byteToHex((addr+5),1);
            return res;
        }
    }
    return "";
}

/********************** get arp destination ethernet address **********************/
QString DataPackage::getArpDestinationEtherAddr()
{
    ARPHEADER *arp;
    arp = (ARPHEADER*)(pkt_content + 14);
    unsigned char *addr;
    if(arp){
        addr = arp->desEthAddr;
        if(addr){
            QString res = byteToHex(addr,1) + ":"
                          + byteToHex((addr+1),1) + ":"
                          + byteToHex((addr+2),1) + ":"
                          + byteToHex((addr+3),1) + ":"
                          + byteToHex((addr+4),1) + ":"
                          + byteToHex((addr+5),1);
            return res;
        }
    }
    return "";
}

/********************** get arp source ip address **********************/
QString DataPackage::getArpSourceIpAddr()
{
    ARPHEADER *arp;
    arp = (ARPHEADER*)(pkt_content + 14);
    if(arp){
        unsigned char *addr = arp->srcIpAddr;
        QString srcIp = QString::number(*addr) + "."
                        + QString::number(*(addr+1)) + "."
                        + QString::number(*(addr+2)) + "."
                        + QString::number(*(addr+3));
        return srcIp;
    }
    return "";
}

/********************** get arp destination ip address **********************/
QString DataPackage::getArpDestinationIpAddr()
{
    ARPHEADER *arp;
    arp = (ARPHEADER*)(pkt_content + 14);
    if(arp){
        unsigned char*addr = arp->desIpAddr;
        QString desIp = QString::number(*addr) + "."
                        + QString::number(*(addr+1)) + "."
                        + QString::number(*(addr+2)) + "."
                        + QString::number(*(addr+3));
        return desIp;
    }
    return "";
}

/* tcp */
/********************** get tcp source port **********************/
QString DataPackage::getTcpSourcePort()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    int port = ntohs(tcp->srcPort);
    if(port == 443)
        return "https(443)";
    return QString::number(port);
}

/********************** get tcp destination port **********************/
QString DataPackage::getTcpDestinationPort()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    int port = ntohs(tcp->desPort);
    if(port == 443)
        return "https(443)";
    return QString::number(port);
}

/********************** get tcp sequence **********************/
QString DataPackage::getTcpSequence()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    return QString::number(ntohl(tcp->sequence));
}

/********************** get tcp acknowledgment **********************/
QString DataPackage::getTcpAcknowledgment()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    return QString::number(ntohl(tcp->ack));
}

/********************** get tcp header length **********************/
QString DataPackage::getTcpHeaderLength()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    int length = (tcp->headerLength >> 4);
    if(length == 5)
        return "20 bytes (5)";
    else
        return QString::number(length*4) + " bytes (" + QString::number(length) + ")";
}

QString DataPackage::getTcpRawHeaderLength()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    return QString::number(tcp->headerLength >> 4);
}

/********************** get tcp flags **********************/
QString DataPackage::getTcpFlags()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    return QString::number(tcp->flags, 16);
}

/********************** get tcp PSH **********************/
QString DataPackage::getTcpPSH()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    return QString::number(((tcp->flags) & 0x08) >> 3);
}

/********************** get tcp ACK **********************/
QString DataPackage::getTcpACK()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    return QString::number(((tcp->flags) & 0x10) >> 4);
}

/********************** get tcp SYN **********************/
QString DataPackage::getTcpSYN()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    return QString::number(((tcp->flags) & 0x02) >> 1);
}

/********************** get tcp UGR **********************/
QString DataPackage::getTcpURG()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    return QString::number(((tcp->flags) & 0x20) >> 5);
}

/********************** get tcp FIN **********************/
QString DataPackage::getTcpFIN()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    return QString::number((tcp->flags) & 0x01);
}

/********************** get tcp RST **********************/
QString DataPackage::getTcpRST()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    return QString::number(((tcp->flags) & 0x04) >> 2);
}

/********************** get tcp window size **********************/
QString DataPackage::getTcpWindowSize()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    return QString::number(ntohs(tcp->windowSize));
}

/********************** get tcp checksum **********************/
QString DataPackage::getTcpCheckSum()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    return QString::number(ntohs(tcp->checkSum), 16);
}

/********************** get tcp urgent pointer **********************/
QString DataPackage::getTcpUrgentPointer()
{
    TCPHEADER *tcp;
    tcp = (TCPHEADER*)(pkt_content + 14 + 20);
    return QString::number(ntohs(tcp->urgent));
}

QString DataPackage::getTcpOperationKind(int kind)
{
    switch(kind) {
    case 0:return "EOL";              // end of list
    case 1:return "NOP";              // no operation
    case 2:return "MSS";              // max segment
    case 3:return "WSOPT";            // window scaling factor
    case 4:return "SACK-Premitted";   // support SACK
    case 5:return "SACK";             // SACK Block
    case 8:return "TSPOT";            // Timestamps
    case 19:return "TCP-MD5";         // MD5
    case 28:return "UTP";             // User Timeout
    case 29:return "TCP-AO";          // authenticated
    }
    return "";
}

int DataPackage::getTcpOperationRawKind(int offset)
{
    unsigned char *tcp;
    tcp = (unsigned char*)(pkt_content + 14 + 20 + offset + 20);
    return *tcp;
}

bool DataPackage::getTcpOperationSACK(int offset, unsigned char &length, QVector<unsigned int>&edge)
{
    unsigned char *tcp;
    tcp = (unsigned char*)(pkt_content + 14 + 20 + offset + 20);
    if(*tcp == 5) {
        tcp++;
        length = *tcp;
        tcp++;
        unsigned int* pointer = (unsigned int*)tcp;
        for(int i = 0; i < (length - 2)/4; i++) {
            unsigned int temp = htonl(*pointer);
            edge.push_back(temp);
            pointer++;
        }
        return true;
    } else
        return false;
}

bool DataPackage::getTcpOperationMSS(int offset, unsigned short &mss)
{
    unsigned char *tcp;
    tcp = (unsigned char*)(pkt_content + 14 + 20 + offset + 20);
    if(*tcp == 2) {
        tcp++;
        if(*tcp == 4) {
            tcp++;
            unsigned short* Mss = (unsigned short*)tcp;
            mss = ntohs(*Mss);
            return true;
        }
        else
            return false;
    }
    return false;
}

bool DataPackage::getTcpOperationSACKP(int offset)
{
    unsigned char *tcp;
    tcp = (unsigned char*)(pkt_content + 14 + 20 + offset + 20);
    if(*tcp == 4)
        return true;
    else
        return false;
}

bool DataPackage::getTcpOperationWSOPT(int offset, unsigned char &shit)
{
    unsigned char *tcp;
    tcp = (unsigned char*)(pkt_content + 14 + 20 + offset + 20);
    if(*tcp == 3) {
        tcp++;
        if(*tcp == 3) {
            tcp++;
            shit = *tcp;
        } else
            return false;
    } else
        return false;
    return true;
}

bool DataPackage::getTcpOperationTSPOT(int offset, unsigned int &value, unsigned int &reply)
{
    unsigned char *tcp;
    tcp = (unsigned char*)(pkt_content + 14 + 20 + offset + 20);
    if(*tcp == 8) {
        tcp++;
        if(*tcp == 10) {
            tcp++;
            unsigned int *pointer = (unsigned int*)(tcp);
            value = ntohl(*pointer);
            pointer++;
            reply = ntohl(*pointer);
            return true;
        } else
            return false;
    }else
        return false;
}

/* udp */
/********************** get udp source port **********************/
QString DataPackage::getUdpSourcePort()
{
    UDPHEADER *udp;
    udp = (UDPHEADER*)(pkt_content + 20 + 14);
    int port = ntohs(udp->srcPort);
    if(port == 53)
        return "domain(53)";
    else
        return QString::number(port);
}

/********************** get udp destination port **********************/
QString DataPackage::getUdpDestinationPort()
{
    UDPHEADER *udp;
    udp = (UDPHEADER*)(pkt_content + 20 + 14);
    int port = ntohs(udp->desPort);
    if(port == 53)
        return "domain(53)";
    else
        return QString::number(port);
}

/********************** get udp data length **********************/
QString DataPackage::getUdpDataLength()
{
    UDPHEADER *udp;
    udp = (UDPHEADER*)(pkt_content + 20 + 14);
    return QString::number(ntohs(udp->dataLength));
}

/********************** get udp checksum **********************/
QString DataPackage::getUdpCheckSum()
{
    UDPHEADER *udp;
    udp = (UDPHEADER*)(pkt_content + 20 + 14);
    return QString::number(ntohs(udp->checkSum), 16);
}

/* dns */
/********************** get dns transaction **********************/
QString DataPackage::getDnsTransactionId()
{
    DNSHEADER *dns;
    dns = (DNSHEADER*)(pkt_content + 14 + 20 + 8);
    return QString::number(ntohs(dns->identification), 16);
}

/********************** get dns flag **********************/
QString DataPackage::getDnsFlags()
{
    DNSHEADER*dns;
    dns = (DNSHEADER*)(pkt_content + 14 + 20 + 8);
    int type = ntohs(dns->flags);
    QString info = "";
    if((type & 0xf800) == 0x0000) {
        info = "(Standard query)";
    } else if((type & 0xf800) == 0x8000) {
        info = "(Standard query response)";
    }
    return QString::number(type,16) + info;
}

/********************** get dns QR **********************/
QString DataPackage::getDnsFlagsQR()
{
    DNSHEADER *dns;
    dns = (DNSHEADER*)(pkt_content + 14 + 20 + 8);
    return QString::number((ntohs(dns->flags) & 0x8000) >> 15);
}

/********************** get dns Operation code **********************/
QString DataPackage::getDnsFlagsOpcode(){
    DNSHEADER *dns;
    dns = (DNSHEADER*)(pkt_content + 14 + 20 + 8);
    return QString::number((ntohs(dns->flags) & 0x7800) >> 11);
}

/********************** get dns AA **********************/
QString DataPackage::getDnsFlagsAA(){
    DNSHEADER *dns;
    dns = (DNSHEADER*)(pkt_content + 14 + 20 + 8);
    return QString::number((ntohs(dns->flags) & 0x0400) >> 10);
}

/********************** get dns TC **********************/
QString DataPackage::getDnsFlagsTC()
{
    DNSHEADER *dns;
    dns = (DNSHEADER*)(pkt_content + 14 + 20 + 8);
    return QString::number((ntohs(dns->flags) & 0x0200) >> 9);
}

/********************** get dns RD **********************/
QString DataPackage::getDnsFlagsRD()
{
    DNSHEADER *dns;
    dns = (DNSHEADER*)(pkt_content + 14 + 20 + 8);
    return QString::number((ntohs(dns->flags) & 0x0100) >> 8);
}

/********************** get dns RA **********************/
QString DataPackage::getDnsFlagsRA()
{
    DNSHEADER *dns;
    dns = (DNSHEADER*)(pkt_content + 14 + 20 + 8);
    return QString::number((ntohs(dns->flags) & 0x0080) >> 7);
}

/********************** get dns Z(reserved) **********************/
QString DataPackage::getDnsFlagsZ()
{
    DNSHEADER *dns;
    dns = (DNSHEADER*)(pkt_content + 14 + 20 + 8);
    return QString::number((ntohs(dns->flags) & 0x0070) >> 4);
}

/********************** get dns Response code **********************/
QString DataPackage::getDnsFlagsRcode()
{
    DNSHEADER *dns;
    dns = (DNSHEADER*)(pkt_content + 14 + 20 + 8);
    return QString::number((ntohs(dns->flags) & 0x000f));
}

/********************** get dns Question number **********************/
QString DataPackage::getDnsQuestionNumber(){
    DNSHEADER *dns;
    dns = (DNSHEADER*)(pkt_content + 14 + 20 + 8);
    return QString::number(ntohs(dns->question));
}

/********************** get dns Answer number **********************/
QString DataPackage::getDnsAnswerNumber()
{
    DNSHEADER *dns;
    dns = (DNSHEADER*)(pkt_content + 14 + 20 + 8);
    return QString::number(ntohs(dns->answer));
}

/********************** get dns Authority number **********************/
QString DataPackage::getDnsAuthorityNumber()
{
    DNSHEADER *dns;
    dns = (DNSHEADER*)(pkt_content + 14 + 20 + 8);
    return QString::number(ntohs(dns->authority));
}

/********************** get dns Additional number **********************/
QString DataPackage::getDnsAdditionalNumber()
{
    DNSHEADER *dns;
    dns = (DNSHEADER*)(pkt_content + 14 + 20 + 8);
    return QString::number(ntohs(dns->additional));
}

/********************** get dns query result **********************/
void DataPackage::getDnsQueriesDomain(QString&name,int&Type,int&Class)
{
    DNSHEADER*dns;
    dns = (DNSHEADER*)(pkt_content + 14 + 20 + 8);
    char*domain = (char*)(pkt_content + 14 + 20 + 8 + 12);
    while(*domain != 0x00) {
        if(domain && (*domain) <= 64) {
            int length = *domain;
            domain++;
            for(int k = 0;k < length;k++) {
                name += (*domain);
                domain++;
            }
            name += ".";
        } else
            break;
    }
    domain++;
    name = name.left(name.length() - 1);
    DNSQUESITON *qus = (DNSQUESITON*)(domain);
    Type = ntohs(qus->queryType);
    Class = ntohs(qus->queryClass);
}

/********************** get dns domian name **********************/
QString DataPackage::getDnsDomainName(int offset)
{
    char *dns;
    dns = (char*)(pkt_content + 14 + 20 + 8 + offset);
    QString name = "";
    while(dns && *dns != 0x00) {
        if((unsigned char)(*dns) <= 64) {
            int length = *dns;
            dns++;
            for(int k = 0; k<length; k++) {
                name += (*dns);
                dns++;
            }
            name += ".";
        }else if(((*dns) & 0xc0) == 0xc0) {
            int accOffset = (((*dns) & 0x3f) << 8);
            dns++;
            accOffset += (unsigned char)(*dns);
            name += getDnsDomainName(accOffset) + ".";
            dns++;
            break;
        }
    }
    name = name.left(name.length() - 1);
    return name;
}

/********************** get dns answer result **********************/
int DataPackage::getDnsAnswersDomain(int offset, QString &name1, unsigned short &Type,
                                     unsigned short &Class, unsigned int &ttl,
                                     unsigned short &dataLength,QString&name2)
{
    char *dns = (char*)(pkt_content + 14 + 20 + 8 + 12 + offset);
    if(((*dns) & 0xc0) == 0xc0){
        int accOffset = (((*dns) & 0x3f) << 8);
        dns++; //
        accOffset += (*dns);
        name1 = getDnsDomainName(accOffset);
        dns++; //
        DNSANSWER*answer = (DNSANSWER*)(dns);
        Type = ntohs(answer->answerType);
        Class = ntohs(answer->answerClass);
        ttl = ntohl(answer->TTL);
        dataLength = ntohs(answer->dataLength);
        dns += (2 + 2 + 4 + 2);
        if(dataLength == 4) {
            for(int i = 0;i < 4;i++) {
                name2 += QString::number((unsigned char)(*dns));
                name2 += ".";
                dns++;
            }
        } else {
            for(int k = 0;k<dataLength;k++) {
                if((unsigned char)(*dns) <= 64) {
                    int length = *dns;
                    dns++;
                    k++;
                    for(int j = 0;j < length;j++) {
                        name2 += *dns;
                        dns++;
                        k++;
                    }
                    name2 += ".";
                }else if(((*dns) & 0xc0) == 0xc0) {
                    int accOffset = (((*dns) & 0x3f) << 8);
                    dns++;
                    k++;
                    accOffset += (unsigned char)(*dns);
                    name2 += getDnsDomainName(accOffset) + ".";
                    dns++;
                    k++;
                }
            }
        }
        name2 = name2.left(name2.length() - 1);
        return dataLength + 2 + 2 + 2 + 4 + 2;

    } else {
        name1 = getDnsDomainName(offset + 12);
        DNSANSWER*answer = (DNSANSWER*)(dns + name1.size() + 2);
        Type = ntohs(answer->answerType);
        Class = ntohs(answer->answerClass);
        ttl = ntohl(answer->TTL);
        dataLength = ntohs(answer->dataLength);
        if(dataLength == 4) {
            dns += (2 + 2 + 4 + 2 + name1.size() + 1);
            for(int i = 0;i < 4;i++){
                name2 += (char)(*dns);
                dns++;
            }
        } else {
            for(int k = 0;k<dataLength;k++){
                if((unsigned char)(*dns) <= 64) {
                    int length = *dns;
                    dns++;
                    k++;
                    for(int j = 0;j < length;j++) {
                        name2 += *dns;
                        dns++;
                        k++;
                    }
                    name2 += ".";
                } else if(((*dns) & 0xc0) == 0xc0) {
                    int accOffset = (((*dns) & 0x3f) << 8);
                    dns++;
                    k++;
                    accOffset += (*dns);
                    name2 += getDnsDomainName(accOffset);
                    dns++;
                    k++;
                }
            }
        }
        name2 = name2.left(name2.length() - 1);
        return dataLength + 2 + 2 + 2 + 4 + 2 + name1.size() + 2;
    }
}

/********************** get dns domain type **********************/
QString DataPackage::getDnsDomainType(int type)
{
    switch (type) {
    case 1: return "A (Host Address)";
    case 2:return "NS";
    case 5:return "CNAME (Canonical NAME for an alias)";
    case 6:return "SOA";
    case 11:return "WSK";
    case 12:return "PTR";
    case 13:return "HINFO";
    case 15:return "MX";
    case 28:return "AAAA";
    case 252:return "AXFR";
    case 255:return "ANY";
    default:return "";
    }
}

// tls
/********************** get tls protocol to check the data is meet this format or not **********************/
bool DataPackage::getisTlsProtocol(int offset)
{
    char *ssl;
    ssl = (char*)(pkt_content + 14 + 20 + 20 + offset);
    unsigned char type = (unsigned char)(*ssl);
    if(type >= 20 && type <= 23) {
        unsigned short *point = (unsigned short*)(ssl+1);
        int version = ntohs(*point);
        if(version >= 0x0301 && version <= 0x0304)
            return true;
        else
            return false;
    }
    else
        return false;
}

/********************** get tls basic information **********************/
void DataPackage::getTlsBasicInfo(int offset, unsigned char &contentType,
                                  unsigned short &version, unsigned short &length)
{
    unsigned char *ssl;
    ssl = (unsigned char*)(pkt_content + 14 + 20 + offset);
    contentType = *ssl;
    ssl++;
    unsigned short *pointer = (unsigned short*)(ssl);
    version = ntohs(*pointer);
    pointer++;
    length = ntohs(*(pointer));
}

/********************** get tls handshake type **********************/
void DataPackage::getTlsHandshakeType(int offset, unsigned char &type)
{
    unsigned char *ssl;
    ssl = (unsigned char*)(pkt_content + 14 + 20 + offset);
    type = *ssl;
}

/********************** get tls client hello information **********************/
void DataPackage::getTlsClientHelloInfo(int offset, unsigned char &handShakeType,
                                        int &length,
                                        unsigned short &version,
                                        QString &random,
                                        unsigned char &sessionIdLength,
                                        QString &sessionId,
                                        unsigned short &cipherLength,
                                        QVector<unsigned short> &cipherSuit,
                                        unsigned char &cmLength,
                                        QVector<unsigned char>&CompressionMethod,
                                        unsigned short&extensionLength)
{
    unsigned char *ssl;
    ssl = (unsigned char*)(pkt_content + 14 + 20 + offset);
    handShakeType = *ssl;
    ssl++;
    length = (*ssl) * 4096 + (*(ssl+1)) * 256 + *(ssl + 2);
    ssl += 3;
    unsigned short* ver = (unsigned short*)(ssl);
    version = ntohs(*ver);
    ver++;
    ssl += 2;
    for(int i = 0;i < 32;i++){
        random += QString::number(*ssl,16);
        ssl++;
    }
    sessionIdLength = *ssl;
    ssl++;
    for(int k = 0;k < sessionIdLength;k++){
        sessionId += QString::number(*ssl,16);
        ssl++;
    }
    unsigned short* clen = (unsigned short*)(ssl);
    cipherLength = ntohs(*clen);
    clen++;
    for(int k = 0;k < cipherLength/2;k++){
        cipherSuit.push_back(ntohs(*clen));
        clen++;
    }
    ssl += (2 + cipherLength);
    cmLength = *ssl;
    ssl++;
    for(int k = 0;k<cmLength;k++){
        CompressionMethod.push_back(*ssl);
        ssl++;
    }
    extensionLength = (*(ssl)) * 256 + *(ssl + 1);
}

void DataPackage::getTlsServerHelloInfo(int offset, unsigned char &handShakeType,
                                        int &length, unsigned short &version,
                                        QString& random, unsigned char &sessionIdLength,
                                        QString &sessionId, unsigned short &cipherSuit,
                                        unsigned char &compressionMethod,
                                        unsigned short &extensionLength)
{
    unsigned char *ssl;
    ssl = (unsigned char*)(pkt_content + 14 + 20 + offset);
    handShakeType = *ssl;
    ssl++;
    length = (*ssl) * 4096 + (*(ssl + 1)) * 256 + *(ssl + 2);
    ssl += 3;
    unsigned short* ver = (unsigned short*)(ssl);
    version = ntohs(*ver);
    ver++;
    ssl += 2;
    for(int i = 0;i < 32;i++){
        random += QString::number(*ssl,16);
        ssl++;
    }
    sessionIdLength = *ssl;
    ssl++;
    for(int k = 0;k < sessionIdLength;k++){
        sessionId += QString::number(*ssl,16);
        ssl++;
    }
    unsigned short*point = (unsigned short*)(ssl);
    cipherSuit = ntohs(*point);
    ssl += 2;
    compressionMethod = *ssl;
    ssl++;
    extensionLength = (*ssl) * 256 + (*(ssl + 1));
}

void DataPackage::getTlsServerKeyExchange(int offset, unsigned char &handShakeType,
                                          int &length, unsigned char &curveType,
                                          unsigned short &curveName,
                                          unsigned char &pubLength,
                                          QString &pubKey,
                                          unsigned short &sigAlgorithm,
                                          unsigned short &sigLength,
                                          QString &sig)
{
    unsigned char*ssl;
    ssl = (unsigned char*)(pkt_content + 14 + 20 + offset);
    handShakeType = *ssl;
    ssl++;
    length = (*ssl) * 4096 + (*(ssl + 1)) * 256 + *(ssl + 2);
    ssl += 3;
    curveType = (*ssl);
    ssl++;
    unsigned short*point = (unsigned short*)(ssl);
    curveName = ntohs(*point);
    ssl += 2;
    pubLength = (*ssl);
    ssl++;
    for(int i = 0;i < pubLength;i++){
        pubKey += QString::number(*ssl,16);
        ssl++;
    }
    point = (unsigned short*)(ssl);
    sigAlgorithm = ntohs(*point);
    point++;
    sigLength = ntohs(*point);
    ssl += 4;
    for(int i = 0;i < sigLength;i++){
        sig += QString::number(*ssl,16);
        ssl++;
    }
}

/********************** get tls handshake type **********************/
QString DataPackage::getTlsHandshakeType(int type)
{
    switch (type) {
    case 1:return "Client Hello";
    case 2:return "Server hello";
    case 11:return "Certificate";
    case 16:return "Client Key Exchange";
    case 4:return "New Session Ticket";
    case 12:return "Server Key Exchange";
    case 14:return "Server Hello Done";

    default:return "";
    }
}

/********************** get tls content type **********************/
QString DataPackage::getTlsContentType(int type)
{
    switch (type) {
    case 20: return "Change Cipher Spec";
    case 21:return "Alert";
    case 22:return "Handshake";
    case 23:return "Application Data";
    default:return "";
    }
}

/********************** get tls version **********************/
QString DataPackage::getTlsVersion(int version)
{
    switch (version) {
    case 0x0300:return "SSL 3.0";
    case 0x0301:return "TLS 1.0";
    case 0x0302:return "TLS 1.1";
    case 0x0303:return "TLS 1.2";
    case 0x0304:return "TLS 1.3";
    default:return "Unkonwn";
    }
}

/********************** get tls handshake cipher suites **********************/
QString DataPackage::getTlsHandshakeCipherSuites(unsigned short code)
{
    switch (code) {
    case 0x00ff: return "TLS_EMPTY_RENEGOTIATION_INFO_SCSV (0x00ff)";
    case 0xc02c: return "TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 (0xc02c)";
    case 0xc030: return "TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384 (0xc030)";
    case 0x009f: return "TLS_DHE_RSA_WITH_AES_256_GCM_SHA384 (0x009f)";
    case 0xc0ad: return "TLS_ECDHE_ECDSA_WITH_AES_256_CCM (0xc0ad)";
    case 0xc09f: return "TLS_DHE_RSA_WITH_AES_256_CCM (0xc09f)";
    case 0xc024: return "TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384 (0xc024)";
    case 0xc028: return "TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384 (0xc028)";
    case 0x006b: return "TLS_DHE_RSA_WITH_AES_256_CBC_SHA256 (0x006b)";
    case 0xc00a: return "TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA (0xc00a)";
    case 0xc014: return "TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA (0xc014)";
    case 0x0039: return "TLS_DHE_RSA_WITH_AES_256_CBC_SHA (0x0039)";
    case 0xc0af: return "TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8 (0xc0af)";
    case 0xc0a3: return "TLS_DHE_RSA_WITH_AES_256_CCM_8 (0xc0a3)";
    case 0xc087: return "TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384 (0xc087)";
    case 0xc08b: return "TLS_ECDHE_RSA_WITH_CAMELLIA_256_GCM_SHA384 (0xc08b)";
    case 0xc07d: return "TLS_DHE_RSA_WITH_CAMELLIA_256_GCM_SHA384 (0xc07d)";
    case 0xc073: return "TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_CBC_SHA384 (0xc073)";
    case 0xc077: return "TLS_ECDHE_RSA_WITH_CAMELLIA_256_CBC_SHA384 (0xc077)";
    case 0x00c4: return "TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256 (0x00c4)";
    case 0x0088: return "TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA (0x0088)";
    case 0xc02b: return "TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 (0xc02b)";
    case 0xc02f: return "TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 (0xc02f)";
    case 0x009e: return "TLS_DHE_RSA_WITH_AES_128_GCM_SHA256 (0x009e)";
    case 0xc0ac: return "TLS_ECDHE_ECDSA_WITH_AES_128_CCM (0xc0ac)";
    case 0xc09e: return "TLS_DHE_RSA_WITH_AES_128_CCM (0xc09e)";
    case 0xc023: return "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 (0xc023)";
    case 0xc027: return "TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256 (0xc027)";
    case 0x0067: return "TLS_DHE_RSA_WITH_AES_128_CBC_SHA256 (0x0067)";
    case 0xc009: return "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA (0xc009)";
    case 0xc013: return "TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA (0xc013)";
    case 0x0033: return "TLS_DHE_RSA_WITH_AES_128_CBC_SHA (0x0033)";
    case 0xc0ae: return "TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8 (0xc0ae)";
    case 0xc0a2: return "TLS_DHE_RSA_WITH_AES_128_CCM_8 (0xc0a2)";
    case 0xc086: return "TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_GCM_SHA256 (0xc086)";
    case 0xc08a: return "TLS_ECDHE_RSA_WITH_CAMELLIA_128_GCM_SHA256 (0xc08a)";
    case 0xc07c: return "TLS_DHE_RSA_WITH_CAMELLIA_128_GCM_SHA256 (0xc07c)";
    case 0xc072: return "TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_CBC_SHA256 (0xc072)";
    case 0xc076: return "TLS_ECDHE_RSA_WITH_CAMELLIA_128_CBC_SHA256 (0xc076)";
    case 0x00be: return "TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256 (0x00be)";
    case 0x0045: return "TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA (0x0045)";
    case 0xc008: return "TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA (0xc008)";
    case 0xc012: return "TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA (0xc012)";
    case 0x0016: return "TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA (0x0016)";
    case 0x00ab: return "TLS_DHE_PSK_WITH_AES_256_GCM_SHA384 (0x00ab)";
    case 0xc0a7: return "TLS_DHE_PSK_WITH_AES_256_CCM (0xc0a7)";
    case 0xc038: return "TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA384 (0xc038)";
    case 0x00b3: return "TLS_DHE_PSK_WITH_AES_256_CBC_SHA384 (0x00b3)";
    case 0xc036: return "TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA (0xc036) ";
    case 0x0091: return "TLS_DHE_PSK_WITH_AES_256_CBC_SHA (0x0091)";
    case 0xc091: return "TLS_DHE_PSK_WITH_CAMELLIA_256_GCM_SHA384 (0xc091)";
    case 0xc09b: return "TLS_ECDHE_PSK_WITH_CAMELLIA_256_CBC_SHA384 (0xc09b)";
    case 0xc097: return "TLS_DHE_PSK_WITH_CAMELLIA_256_CBC_SHA384 (0xc097)";
    case 0xc0ab: return "TLS_PSK_DHE_WITH_AES_256_CCM_8 (0xc0ab)";
    case 0x00aa: return "TLS_DHE_PSK_WITH_AES_128_GCM_SHA256 (0x00aa)";
    case 0xc0a6: return "TLS_DHE_PSK_WITH_AES_128_CCM (0xc0a6)";
    case 0xc037: return "TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256 (0xc037)";
    case 0x00b2: return "TLS_DHE_PSK_WITH_AES_128_CBC_SHA256 (0x00b2)";
    case 0xc035: return "TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA (0xc035)";
    case 0x0090: return "TLS_DHE_PSK_WITH_AES_128_CBC_SHA (0x0090)";
    case 0xc090: return "TLS_DHE_PSK_WITH_CAMELLIA_128_GCM_SHA256 (0xc090)";
    case 0xc096: return "TLS_DHE_PSK_WITH_CAMELLIA_128_CBC_SHA256 (0xc096)";
    case 0xc09a: return "TLS_ECDHE_PSK_WITH_CAMELLIA_128_CBC_SHA256 (0xc09a)";
    case 0xc0aa: return "TLS_PSK_DHE_WITH_AES_128_CCM_8 (0xc0aa)";
    case 0xc034: return "TLS_ECDHE_PSK_WITH_3DES_EDE_CBC_SHA (0xc034)";
    case 0x008f: return "TLS_DHE_PSK_WITH_3DES_EDE_CBC_SHA (0x008f)";
    case 0x009d: return "TLS_RSA_WITH_AES_256_GCM_SHA384 (0x009d)";
    case 0xc09d: return "TLS_RSA_WITH_AES_256_CCM (0xc09d)";
    case 0x003d: return "TLS_RSA_WITH_AES_256_CBC_SHA256 (0x003d)";
    case 0x0035: return "TLS_RSA_WITH_AES_256_CBC_SHA (0x0035)";
    case 0xc032: return "TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384 (0xc032)";
    case 0xc02a: return "TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384 (0xc02a)";
    case 0xc00f: return "TLS_ECDH_RSA_WITH_AES_256_CBC_SHA (0xc00f)";
    case 0xc02e: return "TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384 (0xc02e)";
    case 0xc026: return "TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384 (0xc026)";
    case 0xc005: return "TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA (0xc005)";
    case 0xc0a1: return "TLS_RSA_WITH_AES_256_CCM_8 (0xc0a1)";
    case 0xc07b: return "TLS_RSA_WITH_CAMELLIA_256_GCM_SHA384 (0xc07b)";
    case 0x00c0: return "TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256 (0x00c0)";
    case 0x0084: return "TLS_RSA_WITH_CAMELLIA_256_CBC_SHA (0x0084)";
    case 0xc08d: return "TLS_ECDH_RSA_WITH_CAMELLIA_256_GCM_SHA384 (0xc08d)  ";
    case 0xc079: return "TLS_ECDH_RSA_WITH_CAMELLIA_256_CBC_SHA384 (0xc079)  ";
    case 0xc089: return "TLS_ECDH_ECDSA_WITH_CAMELLIA_256_GCM_SHA384 (0xc089)";
    case 0xc075: return "TLS_ECDH_ECDSA_WITH_CAMELLIA_256_CBC_SHA384 (0xc075)";
    case 0x009c: return "TLS_RSA_WITH_AES_128_GCM_SHA256 (0x009c)";
    case 0xc09c: return "TLS_RSA_WITH_AES_128_CCM (0xc09c)";
    case 0x003c: return "TLS_RSA_WITH_AES_128_CBC_SHA256 (0x003c)";
    case 0x002f: return "TLS_RSA_WITH_AES_128_CBC_SHA (0x002f)";
    case 0xc031: return "TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256 (0xc031)";
    case 0xc029: return "TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256 (0xc029)";
    case 0xc00e: return "TLS_ECDH_RSA_WITH_AES_128_CBC_SHA (0xc00e)";
    case 0xc02d: return "TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256 (0xc02d)";
    case 0xc025: return "TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256 (0xc025)";
    case 0xc004: return "TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA (0xc004)";
    case 0xc0a0: return "TLS_RSA_WITH_AES_128_CCM_8 (0xc0a0)";
    case 0xc07a: return "TLS_RSA_WITH_CAMELLIA_128_GCM_SHA256 (0xc07a)";
    case 0x00ba: return "TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256 (0x00ba)";
    case 0x0041: return "TLS_RSA_WITH_CAMELLIA_128_CBC_SHA (0x0041)";
    case 0xc08c: return "TLS_ECDH_RSA_WITH_CAMELLIA_128_GCM_SHA256 (0xc08c)";
    case 0xc078: return "TLS_ECDH_RSA_WITH_CAMELLIA_128_CBC_SHA256 (0xc078)";
    case 0xc088: return "TLS_ECDH_ECDSA_WITH_CAMELLIA_128_GCM_SHA256 (0xc088)";
    case 0xc074: return "TLS_ECDH_ECDSA_WITH_CAMELLIA_128_CBC_SHA256 (0xc074)";
    case 0x000a: return "TLS_RSA_WITH_3DES_EDE_CBC_SHA (0x000a)";
    case 0xc00d: return "TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA (0xc00d)  ";
    case 0xc003: return "TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA (0xc003)";
    case 0x00ad: return "TLS_RSA_PSK_WITH_AES_256_GCM_SHA384 (0x00ad)";
    case 0x00b7: return "TLS_RSA_PSK_WITH_AES_256_CBC_SHA384 (0x00b7)";
    case 0x0095: return "TLS_RSA_PSK_WITH_AES_256_CBC_SHA (0x0095)";
    case 0xc093: return "TLS_RSA_PSK_WITH_CAMELLIA_256_GCM_SHA384 (0xc093)";
    case 0xc099: return "TLS_RSA_PSK_WITH_CAMELLIA_256_CBC_SHA384 (0xc099)";
    case 0x00ac: return "TLS_RSA_PSK_WITH_AES_128_GCM_SHA256 (0x00ac)";
    case 0x00b6: return "TLS_RSA_PSK_WITH_AES_128_CBC_SHA256 (0x00b6)";
    case 0x0094: return "TLS_RSA_PSK_WITH_AES_128_CBC_SHA (0x0094)";
    case 0xc092: return "TLS_RSA_PSK_WITH_CAMELLIA_128_GCM_SHA256 (0xc092)";
    case 0xc098: return "TLS_RSA_PSK_WITH_CAMELLIA_128_CBC_SHA256 (0xc098)";
    case 0x0093: return "TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA (0x0093)";
    case 0x00a9: return "TLS_PSK_WITH_AES_256_GCM_SHA384 (0x00a9)";
    case 0xc0a5: return "TLS_PSK_WITH_AES_256_CCM (0xc0a5)";
    case 0x00af: return "TLS_PSK_WITH_AES_256_CBC_SHA384 (0x00af)";
    case 0x008d: return "TLS_PSK_WITH_AES_256_CBC_SHA (0x008d)";
    case 0xc08f: return "TLS_PSK_WITH_CAMELLIA_256_GCM_SHA384 (0xc08f)";
    case 0xc095: return "TLS_PSK_WITH_CAMELLIA_256_CBC_SHA384 (0xc095)";
    case 0xc0a9: return "TLS_PSK_WITH_AES_256_CCM_8 (0xc0a9)";
    case 0x00a8: return "TLS_PSK_WITH_AES_128_GCM_SHA256 (0x00a8)";
    case 0xc0a4: return "TLS_PSK_WITH_AES_128_CCM (0xc0a4)";
    case 0x00ae: return "TLS_PSK_WITH_AES_128_CBC_SHA256 (0x00ae)";
    case 0x008c: return "TLS_PSK_WITH_AES_128_CBC_SHA (0x008c)";
    case 0xc08e: return "TLS_PSK_WITH_CAMELLIA_128_GCM_SHA256 (0xc08e)";
    case 0xc094: return "TLS_PSK_WITH_CAMELLIA_128_CBC_SHA256 (0xc094)";
    case 0xc0a8: return "TLS_PSK_WITH_AES_128_CCM_8 (0xc0a8)";
    case 0x008b: return "TLS_PSK_WITH_3DES_EDE_CBC_SHA (0x008b)";
    case 0xc007: return "TLS_ECDHE_ECDSA_WITH_RC4_128_SHA (0xc007)";
    case 0xc011: return "TLS_ECDHE_RSA_WITH_RC4_128_SHA (0xc011)";
    case 0xc033: return "TLS_ECDHE_PSK_WITH_RC4_128_SHA (0xc033)";
    case 0x008e: return "TLS_DHE_PSK_WITH_RC4_128_SHA (0x008e) ";
    case 0x0005: return "TLS_RSA_WITH_RC4_128_SHA (0x0005)";
    case 0x0004: return "TLS_RSA_WITH_RC4_128_MD5 (0x0004)";
    case 0xc00c: return "TLS_ECDH_RSA_WITH_RC4_128_SHA (0xc00c)";
    case 0xc002: return "TLS_ECDH_ECDSA_WITH_RC4_128_SHA (0xc002) ";
    case 0x0092: return "TLS_RSA_PSK_WITH_RC4_128_SHA (0x0092)";
    case 0x008a: return "TLS_PSK_WITH_RC4_128_SHA (0x008a)";
    case 0x1302: return "TLS_AES_256_GCM_SHA384 (0x1302)";
    case 0x1303: return "TLS_CHACHA20_POLY1305_SHA256 (0x1303)";
    case 0x1301: return "TLS_AES_128_GCM_SHA256 (0x1301)";
    case 0xcca9: return "TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 (0xcca9)";
    case 0xcca8: return "TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 (0xcca8)";
    case 0xccaa: return "TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256 (0xccaa)";
    default:return "0x" + QString::number(code,16);
    }
}

/********************** get tls handshake compression **********************/
QString DataPackage::getTlsHandshakeCompression(unsigned char code)
{
    switch (code) {
    case 0:return "null";
    default:return "";
    }
}

QString DataPackage::getTlsHandshakeExtension(unsigned short type)
{
    switch (type) {
    case 0: return "server_name";
    case 5: return "status_request";
    case 11:return "ec_point_format";
    case 10:return "supported_groups";
    case 35:return "session_ticket";
    case 22:return "encrypt_then_mac";
    case 23:return "extended_master_secret";
    case 13:return "signature_algorithms";
    case 43:return "supported_versions";
    case 45:return "psk_key_exchange_modes";
    case 51:return "key_share";
    case 21:return "padding";
    case 18:return "signed_certificate_timestamp";
    case 39578:return "Reserved (GREASE) (39578)";
    default:return "Unknown type";
    }
}

QString DataPackage::getTlsHandshakeExtensionECPointFormat(unsigned char type)
{
    switch (type) {
    case 0:return "EC point format: uncompressed (0)";
    case 1:return "EC point format: ansiX962_compressed_prime (1)";
    case 2:return "EC point format: ansiX962_compressed_char2 (2)";
    default:return QString::number(type);
    }
}

QString DataPackage::getTlsHandshakeExtensionSupportGroup(unsigned short type)
{
    switch (type) {
    case 0x001d:return "x25519 (0x001d)";
    case 0x0017:return "secp256r1 (0x0017)";
    case 0x001e:return "x448 (0x001e)";
    case 0x0019:return "secp521r1 (0x0019)";
    case 0x0018:return "secp384r1 (0x0018)";
    case 0x001c:return "brainpoolP512r1 (0x001c)";
    case 0x001b:return "brainpoolP384r1 (0x001b)";
    case 0x0016:return "secp256k1 (0x0016)";
    case 0x001a:return "brainpoolP256r1 (0x001a)";
    case 0x0015:return "secp224r1 (0x0015)";
    case 0x0014:return "secp224k1 (0x0014)";
    case 0x0013:return "secp192r1 (0x0013)";
    case 0x0012:return "secp192k1 (0x0012)";
    default:return "0x" + QString::number(type,16);
    }
}

QString DataPackage::getTlsHadshakeExtensionHash(unsigned char type)
{
    switch (type) {
    case 4:return "SHA256";
    case 5:return "SHA384";
    case 6:return "SHA512";
    case 2:return "SHA1";
    case 3:return "SHA224";
    case 1:return "MD5";
    default:return "Unknown";
    }
}
QString DataPackage::getTlsHadshakeExtensionSignature(unsigned char type)
{
    switch (type) {
    case 1:return "RSA";
    case 2:return "DSA";
    case 3:return "ECDSA";
    default:return "Unknown";
    }
}
unsigned short DataPackage::getTlsExtensionType(int offset){
    unsigned short*ssl;
    ssl = (unsigned short*)(pkt_content + 14 + 20 + offset);
    return ntohs(*ssl);
}

void DataPackage::getTlsExtensionServerName(int offset, unsigned short &type,
                                            unsigned short &length,
                                            unsigned short &listLength,
                                            unsigned char &nameType,
                                            unsigned short &nameLength,
                                            QString &name)
{
    unsigned short*ssl;
    ssl = (unsigned short*)(pkt_content + 14 + 20 + offset);
    type = ntohs(*ssl);
    ssl++;
    length = ntohs(*ssl);
    ssl++;
    listLength = ntohs(*ssl);
    ssl++;
    unsigned char*p = (unsigned char*)ssl;
    nameType = *p;
    p++;
    nameLength = (*p) * 16 + *(p+1);
    p += 2;
    for(int i = 0;i < nameLength;i++){
        name += (char)(*p);
        p++;
    }
    return;
}

void DataPackage::getTlsExtensionKeyShare(int offset, unsigned short &type,
                                          unsigned short &length,
                                          unsigned short &shareLength,
                                          unsigned short &group,
                                          unsigned short &exchangeLength,
                                          QString &exchange)
{
    unsigned short *ssl;
    ssl = (unsigned short*)(pkt_content + 14 + 20 + offset);
    type = ntohs(*ssl);
    ssl++;
    length = ntohs(*ssl);
    ssl++;
    shareLength = ntohs(*ssl);
    ssl++;
    group = ntohs(*ssl);
    ssl++;
    exchangeLength = ntohs(*ssl);
    ssl++;
    unsigned char*point = (unsigned char*)(ssl);
    for(int i = 0;i < exchangeLength;i++){
        exchange += QString::number(*point,16);
        point++;
    }
}

void DataPackage::getTlsExtensionEcPointFormats(int offset, unsigned short &type,
                                                unsigned short &length,
                                                unsigned char &ecLength,
                                                QVector<unsigned char> &EC)
{
    unsigned short*ssl;
    ssl = (unsigned short*)(pkt_content + 14 + 20 + offset);
    type = ntohs(*ssl);
    ssl++;
    length = ntohs(*ssl);
    ssl++;
    unsigned char* point = (unsigned char*)(ssl);
    ecLength = *point;
    point++;
    for(int i = 0;i < ecLength;i++){
        EC.push_back(*point);
        point++;
    }
}

void DataPackage::getTlsExtensionOther(int offset, unsigned short &type,
                                       unsigned short &length,
                                       QString&data)
{
    unsigned short*ssl;
    ssl = (unsigned short*)(pkt_content + 14 + 20 + offset);
    type = ntohs(*ssl);
    ssl++;
    length = ntohs(*ssl);
    ssl++;
    unsigned char*point = (unsigned char*)(ssl);
    for(int i = 0;i < length;i++){
        data += QString::number(*point,16);
        point++;
    }
}

void DataPackage::getTlsExtensionSupportGroups(int offset, unsigned short &type,
                                               unsigned short &length,
                                               unsigned short &groupListLength,
                                               QVector<unsigned short> &group)
{
    unsigned short*ssl;
    ssl = (unsigned short*)(pkt_content + 14 + 20 + offset);
    type = ntohs(*ssl);
    ssl++;
    length = ntohs(*ssl);
    ssl++;
    groupListLength = ntohs(*ssl);
    ssl++;
    for(int i = 0;i < groupListLength/2;i++){
        group.push_back(ntohs(*ssl));
        ssl++;
    }
}

void DataPackage::getTlsExtensionSessionTicket(int offset, unsigned short &type,
                                               unsigned short &length)
{
    unsigned short*ssl;
    ssl = (unsigned short*)(pkt_content + 14 + 20 + offset);
    type = ntohs(*ssl);
    ssl++;
    length = ntohs(*ssl);
    ssl++;
}

void DataPackage::getTlsExtensionEncryptThenMac(int offset, unsigned short &type,
                                                unsigned short &length)
{
    unsigned short*ssl;
    ssl = (unsigned short*)(pkt_content + 14 + 20 + offset);
    type = ntohs(*ssl);
    ssl++;
    length = ntohs(*ssl);
}

void DataPackage::getTlsExtensionExtendMasterSecret(int offset, unsigned short &type,
                                                    unsigned short &length)
{
    unsigned short*ssl;
    ssl = (unsigned short*)(pkt_content + 14 + 20 + offset);
    type = ntohs(*ssl);
    ssl++;
    length = ntohs(*ssl);
}

void DataPackage::getTlsExtensionSignatureAlgorithms(int offset, unsigned short &type,
                                                     unsigned short &length,
                                                     unsigned short &algorithmLength,
                                                     QVector<unsigned short> &signatureAlgorithm)
{
    unsigned short*ssl;
    ssl = (unsigned short*)(pkt_content + 14 + 20 + offset);
    type = ntohs(*ssl);
    ssl++;
    length = ntohs(*ssl);
    ssl++;
    algorithmLength = ntohs(*ssl);
    ssl++;
    for(int i = 0;i < algorithmLength/2;i++){
        signatureAlgorithm.push_back(ntohs(*ssl));
        ssl++;
    }
}

void DataPackage::getTlsExtensionSupportVersions(int offset, unsigned short &type,
                                                 unsigned short &length,
                                                 unsigned char &supportLength,
                                                 QVector<unsigned short> &supportVersion)
{
    unsigned short*ssl;
    ssl = (unsigned short*)(pkt_content + 14 + 20 + offset);
    type = ntohs(*ssl);
    ssl++;
    length = ntohs(*ssl);
    ssl++;
    unsigned char*point = (unsigned char*)(ssl);
    supportLength = *point;
    point++;
    ssl = (unsigned short*)(point);
    for(int i = 0;i < supportLength;i++){
        supportVersion.push_back(ntohs(*ssl));
        ssl++;
    }
}

void DataPackage::getTlsExtensionPadding(int offset, unsigned short &type,
                                         unsigned short &length,
                                         QString& data)
{
    unsigned short*ssl;
    ssl = (unsigned short*)(pkt_content + 14 + 20 + offset);
    type = ntohs(*ssl);
    ssl++;
    length = ntohs(*ssl);
    ssl++;
    unsigned char*point = (unsigned char*)(ssl);
    for(int i = 0;i < length;i++){
        data += QString::number(*point,16);
        point++;
    }
}
