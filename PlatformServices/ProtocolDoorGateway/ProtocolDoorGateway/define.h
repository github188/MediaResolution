#ifndef DEFINE_H
#define DEFINE_H

#define PROTOCOL_ID_LENGTH 20
#define PROTOCOL_VERSION 0x0808
#define PROTOCOL_MAX_LENGTH 20 * 1024 - 80

#define PROTOCOL_TERM_STORE  2
#define PROTOCOL_SERVER_STORE 4

#define ERR_VERSION 305
#define ERR_CONNECT 306
#define ERR_SEND 307
#define ERR_RECV 308

#define ERR_CENTER_NOT_RES 400
#define ERR_VAILD_XML 401
#define ERR_TERM_NOTEXIST 405
#define ERR_TERM_NOTRES 406

/*******************Э��ָ��*************************/
#define CMD_REGTERM 10004
#define CMD_TERMABILITY 10006
#define CMD_DEVREALTIME 10008	//������ʵʱ��Ƶ
#define CMD_DEVREALTIME_RESP 10009
#define CMD_DEVREALSTREAM 10010
#define CMD_DEVHISTROY 10014 //������ʷ��Ƶ
#define CMD_DEVHISTROY_RESP 10015
#define CMD_DEVRECORDSTREAM 10016

#endif