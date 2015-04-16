#ifndef his_net_socket
#define his_net_socket

#include <arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<string.h>
#include<fcntl.h>
#include <net/if.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<signal.h>
#include<errno.h>
#include <semaphore.h>
#include<unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

/*protocol type*/ /*message*/ 
#define HW_PROTOCOL_MESSAGE 0xff000000

/*login*/
#define HW_PROTOCOL_LOGIN  0xff000001
#define HW_PROTOCOL_LOGOUT 0xff000002

/*video param*/
#define HW_PROTOCOL_GET_VIDEO_COLOR 0xff000003
#define HW_PROTOCOL_SET_VIDEO_COLOR 0xff000004

/*osd*/
#define HW_PROTOCOL_GET_OSD_DATE  0xff000005
#define HW_PROTOCOL_SET_OSD_DATE  0xff000006

/*channel*/
#define HW_PROTOCOL_GET_CHANNEL_NAME  0xff000007
#define HW_PROTOCOL_SET_CHANNEL_NAME  0xff000008

/*dsp*/
#define HW_PROTOCOL_GET_WINDOW_NUMBER 0xff000009
#define HW_PROTOCOL_GET_DECODE_NUMBER 0xff00000A
#define HW_PROTOCOL_GET_DSP_NUMBER    0xff00000B

/*motion*/
#define HW_PROTOCOL_GET_MOTION_RECORD 0xff00000C
#define HW_PROTOCOL_GET_MOTION_SET    0xff00000D
#define HW_PROTOCOL_SET_MOTION_SET    0xff00000E

/*com*/
#define HW_PROTOCOL_GET_COM_SET       0xff00000F
#define HW_PROTOCOL_SET_COM_SET       0xff000010

/*system*/
#define HW_PROTOCOL_REBOOT            0xff000011
#define HW_PROTOCOL_SHUTDOWN          0xff000012
#define HW_PROTOCOL_SYNC_TIME         0xff000013

/*user*/
#define HW_PROTOCOL_GET_USER          0xff000014
#define HW_PROTOCOL_ADD_USER          0xff000015
#define HW_PROTOCOL_DEL_USER          0xff000016
#define HW_PROTOCOL_UPDATE_USER       0xff000017

/*work sheet info*/
#define HW_PROTOCOL_GET_WORKSHEET     0xff000018
#define HW_PROTOCOL_SET_WORKSHEET     0xff000019

/*rec*/
#define HW_PROTOCOL_START_RECORD      0xff00001A
#define HW_PROTOCOL_STOP_RECORD       0xff00001B

/*direction control*/
#define HW_PROTOCOL_COM_CONTROL       0xff00001C

/*net state*/
#define HW_PROTOCOL_NET_STAT          0xff00001D

/*real video*/
#define HW_PROTOCOL_GET_REAL_VIDEO    0xff00001E
#define HW_PROTOCOL_STOP_REAL_VIDEO   0xff00001F

/*record video*/
#define HW_PROTOCOL_GET_RECORD_FILE   0xff000020
#define HW_PROTOCOL_DOWNLOAD_FILE     0xff000021
#define HW_PROTOCOL_GET_DOWNLOAD_POS  0xff000022
#define HW_PROTOCOL_GET_RECORD_VIDEO  0xff000023
#define HW_PROTOCOL_STOP_RECORD_VIDEO 0xff000024

/*mask*/
#define HW_PROTOCOL_GET_MASK          0xff000025
#define HW_PROTOCOL_SET_MASK          0xff000026

/*quality*/
#define HW_PROTOCOL_GET_QUALITY       0xff000027
#define HW_PROTOCOL_SET_QUALITY       0xff000028

/*alarm ----not use now*/
#define HW_PROTOCOL_HARD_ALARM        0xff000029
#define HW_PROTOCOL_MOTION_ALARM      0xff00002A
#define HW_PROTOCOL_VIDEO_LOST_ALARM  0xff00002B

/*sound*/
#define HW_PROTOCOL_OPEN_SOUND        0xff00002C
#define HW_PROTOCOL_CLOSE_SOUND       0xff00002D

/*restore config*/
#define HW_PROTOCOL_RESTORE_CONFIG    0xff00002E

/*upate*/
#define HW_PROTOCOL_UPDATE_CONFIG     0xff00002F
#define HW_PROTOCOL_GET_UPDATE_CONFIG 0xff000030

/*hard disk*/
#define HW_PROTOCOL_GET_HARD_DISK_NUMBER 0xff000031
#define HW_PROTOCOL_GET_HARD_DISK_STATE   0xff000032

/*video state*/
#define HW_PROTOCOL_GET_VIDEO_STATE      0xff000033

/*log*/
#define HW_PROTOCOL_GET_LOG              0xff000034

/*dvr_version*/
#define HW_PROTOCOL_GET_DVR_VERION       0xff000035

/*client port*/
#define HW_PROTOCOL_REMOTE_PORT          0xff000036

/*force I FRAME*/
#define HW_PROTOCO_FORCE_IFRAME          0xff000037

/*video standard*/
#define HW_PROTOCOL_GET_VIDEO_STANDARD   0xff000038

/*ptz control*/
#define HW_PROTOCOL_PTZ_CONTROL          0xff000041

/*stream type*/
#define HW_PROTOCOL_GET_STREAM_TYPE       0xff000042
#define HW_PROTOCOL_SET_STREAM_TYPE       0xff000043

/*register alarm to server*/
#define HW_PROTOCOL_REGISTER_ALARM        0xff000044

/*net version*/
#define HW_PROTOCOL_GET_NET_VERSION       0xff000045

/*server info*/
#define HW_PROTOCOL_GET_SERVER_INFO		  0xff000046

/***************		lzs		*******************/
/*system time*/
#define HW_PROTOCOL_GET_SYSTEMTIME			0xff000047
#define HW_PROTOCOL_SET_SYSTEMTIME			0xff000048

/*network setting*/
#define HW_PROTOCOL_GET_NETWORKSETTING		0xff000049
#define HW_PROTOCOL_SET_NETWORKSETTING		0xff00004A


/*	smart search	*/
#define HW_PROTOCOL_GET_SMARTSEARCHFILE		0xff00004B


/* center information*/
#define HW_PROTOCOL_GET_CENTER_INFO         0xff00004C
#define HW_PROTOCOL_SET_CENTER_INFO         0xff00004D

/* alarm state */
#define HW_PROTOCOL_GET_ALARM_STATE			0xff00004E

/* device configuration */
#define HW_PROTOCOL_GET_DEVICE_CONFIG		0xff00004F

/* upgrade */
#define HW_PROTOCOL_UPGRADE_REQUEST			0xff000050
#define HW_PROTOCOL_SEND_UFHEADER			0xff000051
#define HW_PROTOCOL_SEND_UPGRADEFILE		0xff000052
#define HW_PROTOCOL_GET_UPGRADESTATE		0xff000053

/*work sheet setting*/
#define HW_PROTOCOL_GET_USEWSHEETNO			0xff000054
#define	HW_PROTOCOL_SET_USEWSHEETNO			0xff000055

/* ptz set*/
#define HW_PROTOCOL_GET_PTZ_SET				0xff000056
#define HW_PROTOCOL_SET_PTZ_SET				0xff000057

/* ip cam set*/
#define HW_PROTOCOL_IPCAM_GET_CHANNEL_SET	0xff000058
#define HW_PROTOCOL_IPCAM_SET_CHANNEL_SET	0xff000059

#define HW_PROTOCOL_IPCAM_GET_CHANNEL_STATUS 0xff00005A

#define HW_PROTOCOL_IPCAM_GET_FEATURE		0xff00005B
#define HW_PROTOCOL_IPCAM_SET_FEATURE		0xff00005C

/* get channel type */
#define HW_PROTOCOL_GET_CHANNEL_TYPE		0xff00005D

/* get record file format type */
#define HW_PROTOCOL_GET_RECFILE_INFO		0xff00005E

/*rs232*/
#define HW_PROTOCOL_GET_RS232				0xff000063
#define HW_PROTOCOL_SET_RS232				0xff000064

/*sub video*/
#define HW_PROTOCOL_GET_SUB_REAL_VIDEO      0xff000067
#define HW_PROTOCOL_STOP_SUB_REAL_VIDEO     0xff000068

#define  HW_PROTOCOL_START_VOICE       0xff000069
#define  HW_PROTOCOL_STOP_VOICE		   0xff00006A
#define  HW_PROTOCOL_VOICE_DATA        0xff00006B

#define  HW_PROTOCOL_GET_SUB_CHANNEL_SET 0xff00006C
#define  HW_PROTOCOL_SET_SUB_CHANNEL_SET 0xff00006D

#define  HW_PROTOCOL_SERIALSEND 0xff00006E
#define  HW_PROTOCOL_SERIALRECV 0xff00006F

#define  HW_PROTOCOL_GET_MOTION_ROWCOLS 0xff000070
#define  HW_PROTOCOL_GET_MOTIONEX_SET 0xff000071
#define  HW_PROTOCOL_SET_MOTIONEX_SET 0xff000072

#define HW_PROTOCOL_GET_SMARTSEARCHFILEEX	0xff000073

//get video head
#define HW_PROTOCOL_GET_VIDEO_HEAD 0xff000080

//UDP
#define HW_PROTOCOL_START_UDP_LIVE_STREAM 0xff000081
#define HW_PROTOCOL_STOP_UDP_LIVE_STREAM 0xff000082
#define HW_PROTOCOL_UDP_HEART_BEAT 0xff000083

//encode
#define HW_PROTOCOL_ENCODE_GET		0xff000087
#define HW_PROTOCOL_ENCODE_SET		0xff000088

//channel count
#define HW_PROTOCOL_GET_CHANNEL_COUNT 0xff002047
#define HW_PROTOCOL_SET_CHANNEL_COUNT 0xff002048

#define HW_EXTEND_PROTOCOL_STILL_CAPTURE 0xff001035

#define HW_EXTEND_PROTOCOL_MANUAL_RECORD 0xff001036
#define HW_EXTEND_PROTOCOL_RECORD_TYPE 0xff001037

#define HW_EXTEND_PROTOCOL_GET_IPCAM_MISC 0xff001047
#define HW_EXTEND_PROTOCOL_SET_IPCAM_MISC 0xff001048

#define HW_EXTEND_PROTOCOL_SET_BLACK_WHITE			0xff00104b
#define HW_EXTEND_PROTOCOL_GET_BLACK_WHITE			0xff00104c


#define HW_EXTEND_PROTOCOL_GET_GPIO			0xff00107b
#define HW_EXTEND_PROTOCOL_SET_GPIO			0xff00107c

/*head*/
struct protocolHead
{
	unsigned int proType;/*protocol type : 0xff000001-0xffffffff*/
	unsigned int proVersion;/*protocol version: current version is 0*/
	unsigned int dataLen;/*data length: server will receive/sed datalen data after received/send head*/
	unsigned int proMinVersion;/*protocol sub version*/
	unsigned int errornum;
	unsigned int reserved[30];/*reserved*/
};
#define CURRENT_NET_VERSION 1
#define CURRENT_NET_SUB_VERSION 0

#define INVALID_SOCKET (-1)

class his_net_do
{
public:
	static bool net_init();
	static void net_release();
	static bool connect_to_server(int s,const char *ip,short port,int timeout = 2);
	static void disconnect(int s);
	static bool send_protocol(int s, int protocol,char* buf,int len,int err = 0);
	static bool recv_protocol(int s, int protocol, char *result, int *ret_len);
	static bool send_packet(int s,char* buf,int len);
	static bool recv_packet(int s,char* buf,int len);
};

#endif
