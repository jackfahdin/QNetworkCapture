#ifndef FORMAT_H
#define FORMAT_H

#include <QItemDelegate>
#include <QStyleOptionViewItem>

#define ARP  "ARP"
#define TCP  "TCP"
#define UDP  "UDP"
#define ICMP "ICMP"
#define DNS  "DNS"
#define TLS  "TLS"
#define SSL  "SSL"


// Ethernet protocol format
/*
+-------------------+-----------------+------+
|       6 byte      |     6 byte      |2 byte|
+-------------------+-----------------+------+
|destination address|  source address | type |
+-------------------+-----------------+------+
*/
typedef struct ethernetHeader {         // 14 byte
    unsigned char ethernetDesHost[6];   // destination addr [6 byte]
    unsigned char ethernetSrcHost[6];   // source addr [6 byte]
    unsigned short ethernetType[2];     // type [2 byte]
} ETHERNETHEADER;


// Ipv4 header
/*
+-------+-----------+---------------+-------------------------+
| 4 bit |   4 bit   |    8 bit      |          16 bit         |
+-------+-----------+---------------+-------------------------+
|version|head length|  TOS/DS_byte  |        total length     |
+-------------------+--+---+---+----+-+-+-+-------------------+
|          identification           |R|D|M|    offset         |
+-------------------+---------------+-+-+-+-------------------+
|       ttl         |     protocal  |         checksum        |
+-------------------+---------------+-------------------------+
|                   source ip address                         |
+-------------------------------------------------------------+
|                 destination ip address                      |
+-------------------------------------------------------------+
*/
typedef struct IpHeader {                   // 20 byte
    unsigned char versionHeaderLength;      // version [4 bit] and length of header [4 bit]
    unsigned char TOS;                      // TOS/DS_byte [1 byte]
    unsigned short totalLength;              // ip package total length [2 byte]
    unsigned short identification;          // identification [2 byte]
    unsigned short flagOffset;              // flag [3 bit] and offset [13 bit]
    unsigned char ttl;                      // TTL [1 byte]
    unsigned char protocol;                 // protocal [1 byte]
    unsigned short checkSum;                // checksum [2 byte]
    unsigned int srcAddr;                   // source address [4 byte]
    unsigned int desAdder;                  // destination address [4 byte]
} IPHEADER;


// Tcp header
/*
+----------------------+---------------------+
|         16 bit       |       16 bit        |
+----------------------+---------------------+
|      source port     |  destination port   |
+----------------------+---------------------+
|              sequence number               |
+----------------------+---------------------+
|                 ack number                 |
+----+---------+-------+---------------------+
|head| reserve | flags |     window size     |
+----+---------+-------+---------------------+
|     checksum         |   urgent pointer    |
+----------------------+---------------------+
*/
typedef struct tcpHeader {    // 20 byte
    unsigned short srcPort;         // source port [2 byte]
    unsigned short desPort;         // destination [2 byte]
    unsigned int sequence;           // sequence number [4 byte]
    unsigned int ack;                // Confirm serial number [4 byte]
    unsigned char headerLength;     // header length [4 bit]
    unsigned char flags;             // flags [6 bit]
    unsigned short windowSize;      // size of window [2 byte]
    unsigned short checkSum;         // checksum [2 byte]
    unsigned short urgent;           // urgent pointer [2 byte]
}TCPHEADER;


// Udp header
/*
+---------------------+---------------------+
|        16 bit       |        16 bit       |
+---------------------+---------------------+
|    source port      |   destination port  |
+---------------------+---------------------+
| data package length |       checksum      |
+---------------------+---------------------+
*/
typedef struct udpHeader { // 8 byte
    unsigned short srcPort;      // source port [2 byte]
    unsigned short desPort;      // destination port [2 byte]
    unsigned short dataLength;   // data length [2 byte]
    unsigned short checkSum;      // checksum [2 byte]
} UDPHEADER;


// Icmp header
/*
+---------------------+---------------------+
|  1 byte  |  1 byte  |        2 byte       |
+---------------------+---------------------+
|   type   |   code   |       checksum      |
+---------------------+---------------------+
|    identification   |       sequence      |
+---------------------+---------------------+
|                  option                   |
+-------------------------------------------+
*/
typedef struct icmpHeader {         // at least 8 byte
    unsigned char type;                    // type [1 byte]
    unsigned char code;                    // code [1 byte]
    unsigned short checkSum;               // checksum [2 byte]
    unsigned short identification;         // identification [2 byte]
    unsigned short sequence;               // sequence [2 byte]
} ICMPHEADER;


//Arp
/*
|<--------  ARP header  ------------>|
+------+--------+-----+------+-------+----------+---------+---------------+--------------+
|2 byte| 2 byte |1byte| 1byte|2 byte |  6 byte  | 4 byte  |     6 byte    |     4 byte   |
+------+--------+-----+------+-------+----------+---------+---------------+--------------+
| type |protocol|e_len|ip_len|op_type|source mac|source ip|destination mac|destination ip|
+------+--------+-----+------+-------+----------+---------+---------------+--------------+
*/
typedef struct arpHeader {   // 28 byte
    unsigned short hardwareType;   // hardware type [2 byte]
    unsigned short protocolType;   // protocol [2 byte]
    unsigned char macLength;       // MAC address length [1 byte]
    unsigned char ipLength;        // IP address length [1 byte]
    unsigned short opCode;         // operation code [2 byte]

    unsigned char srcEthAddr[6];  // source ether address [6 byte]
    unsigned char srcIpAddr[4];   // source ip address [4 byte]
    unsigned char desEthAddr[6];  // destination ether address [6 byte]
    unsigned char desIpAddr[4];   // destination ip address [4 byte]

} ARPHEADER;


// dns
/*
+--------------------------+---------------------------+
|           16 bit         |1b|4bit|1b|1b|1b|1b|3b|4bit|
+--------------------------+--+----+--+--+--+--+--+----+
|      identification      |QR| OP |AA|TC|RD|RA|..|Resp|
+--------------------------+--+----+--+--+--+--+--+----+
|         Question         |       Answer RRs          |
+--------------------------+---------------------------+
|     Authority RRs        |      Additional RRs       |
+--------------------------+---------------------------+
*/
typedef struct dnsHeader {  // 12 byte
    unsigned short identification; // Identification [2 byte]
    unsigned short flags;          // Flags [total 2 byte]
    unsigned short question;       // Question Number [2 byte]
    unsigned short answer;         // Answer RRs [2 byte]
    unsigned short authority;      // Authority RRs [2 byte]
    unsigned short additional;     // Additional RRs [2 byte]
} DNSHEADER;

// dns question
typedef struct dnsQuestion {
    // char* name;          // Non-fixed
    unsigned short queryType;     // 2 byte
    unsigned short queryClass;    // 2 byte
} DNSQUESITON;

typedef struct dnsAnswer {
    // char* name          // Non-fixed
    unsigned short answerType;   // 2 byte
    unsigned short answerClass;  // 2 byte
    unsigned int TTL;             // 4 byte
    unsigned short dataLength;    // 2 byte
    //char* name           // Non-fixed
} DNSANSWER;

#endif // FORMAT_H
