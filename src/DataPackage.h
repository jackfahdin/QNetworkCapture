#ifndef DATAPACKAGE_H
#define DATAPACKAGE_H

#include "Format.h"

/*
 * This class is describe the data package infomation
 * +-----+------------+
 * | type| infomation |
 * +-----+------------+
 * |  1  |    arp     |
 * +-----+------------+
 * |  2  |    icmp    |
 * +-----+------------+
 * |  3  |    tcp     |
 * +-----+------------+
 * |  4  |    udp     |
 * +-----+------------+
 * |  5  |    dns     |
 * +-----+------------+
 * |  6  |    tls     |
 * +-----+------------+
 * |  7  |    ssl     |
 * +-----+------------+
*/


class DataPackage
{
private:
    unsigned int dataLength; // data pakage length
    QString timeStamp; // timestamp of package
    QString info;      // a breif introduction of package
    int packageType;   // type

public:
    const unsigned char *pkt_content; // root pointer of package data

protected:
    /*
     * turn the byteArray to QString
    */
    static QString byteToHex(unsigned char*str,int size);
public:
    // Construction and destruction
    DataPackage();
    ~DataPackage() = default;

    // set the var
    void setDataLength(unsigned int length);                    // set the package length
    void setTimeStamp(QString timeStamp);                       // set timestamp
    void setPackageType(int type);                              // set package type
    void setPackagePointer(const unsigned char *pkt_content, int size); // set package pointer
    void setPackageInfo(QString info);                          // set package information

    // get the var
    QString getDataLength();                  // get package length
    QString getTimeStamp();                   // get timestamp
    QString getPackageType();                 // get package type
    QString getInfo();                        // get a breif package information
    QString getSource();                      // get the source address of package
    QString getDestination();                 // get the destination address of package

    // get the mac info
    QString getDesMacAddr();                  // get the destination MAC address
    QString getSrcMacAddr();                  // get the source MAC address
    QString getMacType();                     // get the type of MAC address

    // get the ip info
    QString getDesIpAddr();                   // get the destination ip address
    QString getSrcIpAddr();                   // get the source ip address
    QString getIpVersion();                   // get the ip version
    QString getIpHeaderLength();              // get the ip head length
    QString getIpTos();                       // get the ip tos
    QString getIpTotalLength();               // get the ip total package length
    QString getIpIdentification();            // get the ip identification
    QString getIpFlag();                      // get the ip flag
    QString getIpReservedBit();               // the reserved bit
    QString getIpDF();                        // Don't fragment
    QString getIpMF();                        // More fragment
    QString getIpFragmentOffset();            // get the offset of package
    QString getIpTTL();                       // get ip ttl [time to live]
    QString getIpProtocol();                  // get the ip protocol
    QString getIpCheckSum();                  // get the checksum

    // get the icmp info
    QString getIcmpType();                    // get the icmp type
    QString getIcmpCode();                    // get the icmp code
    QString getIcmpCheckSum();                // get the icmp checksum
    QString getIcmpIdentification();          // get the icmp identification
    QString getIcmpSequeue();                 // get the icmp sequence
    QString getIcmpData(int size);            // get the icmp data

    // get the arp info
    QString getArpHardwareType();             // get arp hardware type
    QString getArpProtocolType();             // get arp protocol type
    QString getArpHardwareLength();           // get arp hardware length
    QString getArpProtocolLength();           // get arp protocol length
    QString getArpOperationCode();            // get arp operation code
    QString getArpSourceEtherAddr();          // get arp source ethernet address
    QString getArpSourceIpAddr();             // get arp souce ip address
    QString getArpDestinationEtherAddr();     // get arp destination ethernet address
    QString getArpDestinationIpAddr();        // get arp destination ip address

    // get the tcp info
    QString getTcpSourcePort();               // get tcp source port
    QString getTcpDestinationPort();          // get tcp destination port
    QString getTcpSequence();                 // get tcp sequence
    QString getTcpAcknowledgment();           // get acknowlegment
    QString getTcpHeaderLength();             // get tcp head length
    QString getTcpRawHeaderLength();          // get tcp raw head length [default is 0x05]
    QString getTcpFlags();                    // get tcp flags
    QString getTcpPSH();                      // PSH flag
    QString getTcpACK();                      // ACK flag
    QString getTcpSYN();                      // SYN flag
    QString getTcpURG();                      // URG flag
    QString getTcpFIN();                      // FIN flag
    QString getTcpRST();                      // RST flag
    QString getTcpWindowSize();               // get tcp window size
    QString getTcpCheckSum();                 // get tcp checksum
    QString getTcpUrgentPointer();            // get tcp urgent pointer
    QString getTcpOperationKind(int kind);    // get tcp option kind
    int getTcpOperationRawKind(int offset);   // get tcp raw option kind

    /*
     * tcp optional parts
    */
    bool getTcpOperationMSS(int offset, unsigned short &mss);                          // kind = 2
    bool getTcpOperationWSOPT(int offset, unsigned char &shit);                         // kind = 3
    bool getTcpOperationSACKP(int offset);                                     // kind = 4
    bool getTcpOperationSACK(int offset, unsigned char &length, QVector<unsigned int>&edge);    // kind = 5
    bool getTcpOperationTSPOT(int offset, unsigned int &value, unsigned int &reply);            // kind = 8

    // get the udp info
    QString getUdpSourcePort();               // get udp source port
    QString getUdpDestinationPort();          // get udp destination port
    QString getUdpDataLength();               // get udp data length
    QString getUdpCheckSum();                 // get udp checksum

    // get the dns info
    QString getDnsTransactionId();            // get dns transaction id
    QString getDnsFlags();                    // get dns flags
    QString getDnsFlagsQR();                  // get dns flag QR
    QString getDnsFlagsOpcode();              // get dns flag operation code
    QString getDnsFlagsAA();                  // get dns flag AA
    QString getDnsFlagsTC();                  // get dns flag TC
    QString getDnsFlagsRD();                  // get dns flag RD
    QString getDnsFlagsRA();                  // get dns flag RA
    QString getDnsFlagsZ();                   // get dns flag Z [reserved]
    QString getDnsFlagsRcode();               // get dns flag Rcode
    QString getDnsQuestionNumber();           // get dns question number
    QString getDnsAnswerNumber();             // get dns answer number
    QString getDnsAuthorityNumber();          // get dns authority number
    QString getDnsAdditionalNumber();         // get dns addition number
    void getDnsQueriesDomain(QString&name, int &Type, int &Class);
    QString getDnsDomainType(int type);
    QString getDnsDomainName(int offset);
    int getDnsAnswersDomain(int offset, QString&name1, unsigned short &Type,
                            unsigned short &Class, unsigned int &ttl,
                            unsigned short &dataLength, QString &name2);

    // get the tls info
    bool getisTlsProtocol(int offset);
    void getTlsBasicInfo(int offset, unsigned char &contentType,
                         unsigned short &version, unsigned short &length);
    void getTlsClientHelloInfo(int offset,unsigned char&handShakeType,
                               int &length, unsigned short &version,
                               QString &random, unsigned char &sessionIdLength,
                               QString &sessionId, unsigned short &cipherLength,
                               QVector<unsigned short>&cipherSuit,
                               unsigned char &cmLength,
                               QVector<unsigned char>&CompressionMethod,
                               unsigned short &extensionLength);
    void getTlsServerHelloInfo(int offset, unsigned char &handShakeType,
                               int &length, unsigned short &version,
                               QString &random, unsigned char &sessionIdLength,
                               QString &sessionId,unsigned short &cipherSuit,
                               unsigned char &compressionMethod,
                               unsigned short &extensionLength);
    void getTlsServerKeyExchange(int offset, unsigned char &handShakeType,
                                 int &length, unsigned char &curveType,
                                 unsigned short &curveName, unsigned char &pubLength,
                                 QString &pubKey, unsigned short &sigAlgorithm,
                                 unsigned short &sigLength, QString &sig);
    unsigned short getTlsExtensionType(int offset);
    void getTlsHandshakeType(int offset, unsigned char &type);

    /*
     * these functions are used to parse the extension parts
     * extension parts are common in handshake parts (client hello,server hello ...)
     * there are some extension types are not included in, maybe you should refer the official API
    */
    void getTlsExtensionServerName(int offset, unsigned short &type,
                                   unsigned short &length, unsigned short &listLength,
                                   unsigned char &nameType, unsigned short&nameLength,
                                   QString &name);
    void getTlsExtensionSignatureAlgorithms(int offset, unsigned short&type,
                                            unsigned short &length,
                                            unsigned short &algorithmLength,
                                            QVector<unsigned short>&signatureAlgorithm);
    void getTlsExtensionSupportGroups(int offset, unsigned short &type,
                                      unsigned short &length,
                                      unsigned short &groupListLength,
                                      QVector<unsigned short>&group);
    void getTlsExtensionEcPointFormats(int offset, unsigned short&type,
                                       unsigned short &length,
                                       unsigned char &ecLength,
                                       QVector<unsigned char>&EC);
    void getTlsExtensionSessionTicket(int offset,unsigned short &type,
                                      unsigned short &length);
    void getTlsExtensionEncryptThenMac(int offset,unsigned short &type,
                                       unsigned short &length);
    void getTlsExtensionSupportVersions(int offset,unsigned short &type,
                                        unsigned short &length,
                                        unsigned char &supportLength,
                                        QVector<unsigned short>&supportVersion);
    void getTlsExtensionPskKeyExchangeModes(int offset,unsigned short &type,
                                            unsigned short &length,
                                            unsigned char &modeLength,
                                            QVector<unsigned char>&mode);
    void getTlsExtensionKeyShare(int offset,unsigned short &type,
                                 unsigned short &length,
                                 unsigned short &shareLength,
                                 unsigned short &group,
                                 unsigned short&exchangeLength,
                                 QString& exchange);
    void getTlsExtensionOther(int offset,unsigned short &type,
                              unsigned short &length,
                              QString &data);
    void getTlsExtensionExtendMasterSecret(int offset, unsigned short &type,
                                           unsigned short &length);
    void getTlsExtensionPadding(int offset,unsigned short &type,
                                unsigned short &length,
                                QString&data);

    /*
     * when transfer data,some types will be encoded,like using 0x01 to represent the MD5 in extension hash part
     * to visual display these types,we need to decode and analysis
     * this functions are used to do these analisis
     * however,some types may be the custom types, so we can't decode
     * also,there are some rules not be included, maybe you should refer the official API
    */
    // Parsing the encode data
    static QString getTlsHandshakeType(int type);                          // Parsing TLS handshake type
    static QString getTlsContentType(int type);                            // Parsing TLS content type
    static QString getTlsVersion(int version);                             // Parsing TLS version
    static QString getTlsHandshakeCipherSuites(unsigned short code);              // Parsing TLS cipher suite
    static QString getTlsHandshakeCompression(unsigned char code);                // Parsing TLS compression
    static QString getTlsHandshakeExtension(unsigned short type);                 // Parsing TLS extension
    static QString getTlsHandshakeExtensionECPointFormat(unsigned char type);     // Parsing TLS EC point format
    static QString getTlsHandshakeExtensionSupportGroup(unsigned short type);     // Parsing TLS support group
    static QString getTlsHadshakeExtensionSignature(unsigned char type);          // Parsing TLS signature
    static QString getTlsHadshakeExtensionHash(unsigned char type);               // Parsing TLS hash
};

#endif // DATAPACKAGE_H
