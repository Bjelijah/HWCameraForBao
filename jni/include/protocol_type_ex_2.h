#ifndef _protocol_type_include_h
#define _protocol_type_include_h

#include "hw_config.h"

#define	MAX_NAME_LEN			32
#define MAX_SERIALID			32
#define MAX_SLOT				16
#define MAX_ALARM_IN			16
#define MAX_ALARM_OUT			16
#define MAX_DISKNUM				8
#define MAX_DSPNUM				32

#define VERSION_TEMPEST(x) ((x) >= 20000 && (x) <=29999)
#define VERSION_DAVI(x) ((x) >= 10000 && (x) <=19999)
#define VERSION_HOWELL_LINUX(x)((x) >= 30000 && (x) <= 39999)
#define VERSION_HIS_NORMAL(x) ((x) >= 40000 && (x) <= 49999)
#define VERSION_NVR(x) ((x) >= 50000 && (x) <= 59999)
#define VERSION_IPCAM(x) ((x) >= 60000 && (x) <= 69999)
#define VERSION_MIXED_NVR(x)((x) >= 70000 && (x) <= 79999)//混和型nvr
#define VERSION_HIS_RAILWAY(x) ((x) >= 80000 && (x) <= 89999)
#define VERSION_HIS(x)(VERSION_HIS_NORMAL(x) || VERSION_HIS_RAILWAY(x))

typedef struct 
{
	char	name[MAX_NAME_LEN];				//设备名称
	char	serial[MAX_SERIALID];				//设备序列号
	int		window_count;			//窗口数
	int		alarm_in_count;			//报警输入数
	int		alarm_out_count;		//报警输出数		
	int		disk_count;				//硬盘数
	int		dsp_count;				//dsp数
	int		rs232_count;			//串口数
	char	channel_name[MAX_SLOT][32]; 	//通道名称
	char	reserve[32];
}device_info_t;

typedef struct 
{
	int		alarm_in_state[MAX_ALARM_IN];		//报警输入状态
	int		alarm_out_state[MAX_ALARM_OUT];		//报警输出状态
	int		rec_state[MAX_SLOT];					//录像状态 0-不录，1-录像
	int		signal_state[MAX_SLOT];					//视频信号状态 0-无 1-有
	int		disk_state[MAX_DISKNUM];				//硬盘状态	0-正常 1-无硬盘 2-硬盘坏 3-硬盘未格式化 4-硬盘满 
	int		motion_state[MAX_SLOT];					//移动侦测 0-没有移动侦测报警 1-有
	int 	dsp_state[MAX_DSPNUM];				//dsp状态  0-异常 1-正常
	char	reserve[32];
}alarm_state_t;

typedef struct 
{	
	int type;	//暂时未使用
	char name[32];
	char password[32];	
	int user_id;
	int reserved[32];
}log_info_t;

typedef struct 
{
	int slot_count;			//通道总数
	int server_version;		//服务器版本
	int net_version;		//网络版本
	int reserver[32];		
}server_info_t;

typedef struct
{
	int port;
	char ip[32];
	int slot;
	int is_sub;
	int reserve[32];
}udp_live_info_t;

typedef struct  
{
	int slot;
	int is_sub;
	char buf[128];
	int len;
}net_head_t;

typedef struct 
{
	int		slot;
	int		type;	//实际通道版本号
	int		reserved[32];
}channel_type_t;

typedef struct 
{
	int slot;
	SYSTEMTIME beg;
	SYSTEMTIME end;
	int type;/*0=all 1=normal file 2=mot file*/
}rec_file_t;

typedef struct
{
	int slot;
	int type;/*0=head  1=video data*/
	int len;
	char buf[2048];
}rec_data_t;

typedef struct 
{
	int		slot;
	SYSTEMTIME beg;
	SYSTEMTIME end;
	int		format;	
	int		reserved[32];
}rec_file_format_t;

typedef struct 
{
	rec_file_t file;
	int rt_count;
	RECT rt[5];
	int direction;
	int reserved[32];
}smart_search_t;

typedef struct 
{
	int slot;
	int brightness/*0-255*/;
	int contrast/*0-127*/;
	int saturation/*0-127*/;
	int hue/*0-255*/;
}video_color_t;

typedef struct 
{
	int		port;
	char	ip[32];			
	char	net_mask[32];		
	char	gate_way[32];
	int		mac_addr[6];
	char	dns[32];
	char	multi_ip[32];
	int		ppoe_enable;
	char	ppoe_user[32];
	char	ppoe_password[16];
	char	ppoe_ip[32];
	int		reserve[32];
}net_cfg_t;

typedef struct
{
	int 	slot;
	char log_name[32];
	char log_psw[32];
	char ip[32];
	char mac[32];
	int port;
	int remote_slot;
}slot_cfg_t;

typedef struct
{
	int slot;
	int encode_type;/*0-cif,1-d1,2-720p*/
	int quality_level;/*0-最好，1-次好，2-较好 3-一般*/
	int max_bps;/*当最高位为0时，0-不限，1-较小，2-大，3-较大，4-最大,CIF(2000,400,600,800,1000),D1(4000,800,1000,1400,1600),720p(8000,2000,2500,3000,4000),当最高位为1时，其他位为自定义码流大小。*/
	int vbr;/*0-cbr 1-vbr*/
	int framerate;/*0-全帧率，1-24实际帧率*/
	int reserve;
}video_quality_t;

typedef struct
{
	int channel_count;
	int reserve[64];
}channel_count_t;

typedef struct
{
	int slot;
	int show;
	int left;
	int top;
	int type;
	int show_week;
}osd_date_t;

typedef struct
{
	int slot;
	int show;
	int left;
	int top;
	char name[32];
}osd_name_t;

typedef struct
{
	int slot;
	int used;
	int encode_type;
	int quality_lev;
	int max_bps;
	int vbr;
	int framerate;

	int reserve[256];
}sub_video_quality_t;

typedef enum 
{
	IPCAM_ESHUTTER_8L_SEC=0, /* 8 sec */
	IPCAM_ESHUTTER_4L_SEC=1,	/* 4 sec */
	IPCAM_ESHUTTER_10_SEC=2, /* 1/10 sec */
	IPCAM_ESHUTTER_12P5_SEC=3, /* 1/12.5 sec */
	IPCAM_ESHUTTER_15_SEC=4,
	IPCAM_ESHUTTER_20_SEC=5,
	IPCAM_ESHUTTER_25_SEC=6,
	IPCAM_ESHUTTER_30_SEC=7,
	IPCAM_ESHUTTER_50_SEC=8,
	IPCAM_ESHUTTER_60_SEC=9,
	IPCAM_ESHUTTER_100_SEC=10,
	IPCAM_ESHUTTER_120_SEC=11,
	IPCAM_ESHUTTER_240_SEC=12,
	IPCAM_ESHUTTER_480_SEC=13,
	IPCAM_ESHUTTER_960_SEC=14,
	IPCAM_ESHUTTER_1024_SEC=15,
	IPCAM_ESHUTTER_COUNT,
}HW_IPCAM_ESHUTTER_E;
typedef enum
{
	IPCAM_AGC_42_DB=0,
	IPCAM_AGC_36_DB,
	IPCAM_AGC_30_DB,
	IPCAM_AGC_24_DB,
	IPCAM_AGC_18_DB,
	IPCAM_AGC_12_DB,
	IPCAM_AGC_6_DB,
	IPCAM_AGC_0_DB,
	IPCAM_AGC_COUNT,
}HW_IPCAM_AGC_E;

typedef struct
{
	int slot;
	int bae;
	int eshutter;
	int agcgain;
	int black_white_mode;
	int badj;
	int black_level;
	int bautowb;
	int wb_mode;
	int rgb_gain[3];
	int noise_filter;
	int sharp;
	int luma;
	int reserve[13];
}ipc_feature_t;

typedef struct
{
	int slot;
	int type;
}stream_type_t;


typedef struct 
{ 
	int type;   /*处理方式,处理方式的"或"结果,参照AlarmHandleType*/
	char alarm_out[16];/* 报警触发的输出通道,报警触发的输出,为1 表示触发该输出 */ 
	char record[16];/*联动录象,报警触发的录象通道,为1表示触发该通道 //移动侦测可以不支持*/
	char snap[16];/*联动抓图*/
}handle_t; 

#define  HW_MOTION_MAX_ROWS 128
#define  HW_MOTION_MAX_COLS 128

typedef struct
{
	int slot;
	int lev;/*0最高,5最低,6关闭*/
	int rec_delay;/*0:10s,1:20s,2:30s,3:1m,4:2m,5:6m,6:10m*/
	char data[HW_MOTION_MAX_ROWS * HW_MOTION_MAX_COLS / 8];
	handle_t handle;
	int reserved[32];
}motion_cfg_t;

//串口设备设置
typedef struct
{
	int rs232_no;
	/* 0-50, 1-75, 2-110, 3-150, 4-300, 5-600, 6-1200, 7-2400, 8-4800, 9-9600, 10-19200
	 * 11-38400, 12-57600, 13-76800, 14-115200 
	 * */
	int rate; 
	unsigned char data_bit; /* 0-5bit 1-6bit 2-7bit 3-8bit */
	unsigned char stop_bit; /* 0-1bit 1-2bit */
	unsigned char parity; /* 0-无校验 1-奇校验 2-偶校验 */
	unsigned char flow_control; /* 0-无 1-软流控 2-硬流控 */
	int work_mode; /* 工作模式: 0-云台 1-报警接收 2-透明通道 */
	int annunciator_type; /* 0-Howell 1-vista 120 ... */
	int reserved[32];
} rs232_cfg_t;

//ptz设置
#if 0
typedef enum
{
	PELCO_D =0,
	PELCO_P,
	ALEC,
	YAAN,
}PTZProtocolType;
#endif
typedef struct {
	int slot;
	int rs232_no;

	/* 0-50, 1-75, 2-110, 3-150, 4-300, 5-600, 6-1200, 7-2400, 8-4800, 9-9600, 10-19200
	 * 11-38400, 12-57600, 13-76800, 14-115200 
	 * */
	int rate; 
	unsigned char data_bit; /* 0-5bit 1-6bit 2-7bit 3-8bit */
	unsigned char stop_bit; /* 0-1bit 1-2bit */
	unsigned char parity; /* 0-无校验 1-奇校验 2-偶校验 */
	unsigned char flow_control; /* 0-无 1-软流控 2-硬流控 */

	int protocol;
	int address;
	int reserve[32];
}ptz_cfg_t;

typedef struct {
	int slot;
	int control;/*0-direct 1- len 2-AUTO zoom in 3-preset */
	int cmd/*direct:
			 7-left up
			 8-up
			 9-right up
			 4-left
			 5-stop
			 6-right
			 1-left down
			 2-down
			 3-right down
			 len:
			 1-iris open
			 2-iris close
			 3-len tele
			 4-len wide
			 5-focus far
			 6-focus near
			 7-stop

			 AUTO zoom in:
			 1-auto zoom in

			 preset:
			 1-set	
			 2-clear
			 3-go
			 */;

	int value;
	RECT rect;			//点击放大，相对于704×576
}ptz_ctrl_t;

typedef struct
{
	int enable;			/*是否启用,如果为0*/
	int type;			/*编码类型,0-H264,1-MJPEG*/
	int reso_w;			/*编码宽度,由设备验证是否正确*/
	int reso_h;			/*编码高度，由设备验证是否正确*/
	int fr;				/*编码帧率,由设备验证是否正确*/

	int reserve[32];	/*保留，必须为0*/
}encode_video_t;

typedef struct
{
	int profile;		/*0-main profile 1-base line*/
	int gop;			/*i帧间隔*/
	int bitrate_ctrl;	/*0-cbr 1-vbr*/
	int bitrate_avg;	/*cbr码率*/
	int bitrate_min;	/*vbr最小码率*/
	int bitrate_max;	/*vbr最大码率*/
	int quality;		/*图像质量*/
	int reserve[32];	/*保留，必须为0*/
}h264_t;

typedef struct
{
	int quality;		/*图像质量*/
	int reserve[32];	/*保留*/
}mjpeg_t;

typedef struct
{
	int enable;			/*是否启用*/
	int type;			/*0-G711A 1-G711U 2-AAC 3-G726*/
	int sample;			/*采样率*/
	int bit_fmt;		/*采样位宽*/
	int chn_num;		/*采样通道*/
	int bitrate;		/*编码码率*/

	int reserve[32];	/*保留，必须为0*/
}encode_audio_t;

typedef struct
{
    int type;/*0:客户端到设备 1:双向*/
    int slot;
    int reserve[31];
}voice_t;

/* 抓图 */
typedef struct {
  unsigned char slot;    /* 通道 */
  unsigned char stream;  /* 码流 */
  unsigned char quality; /* 1～100 */
  unsigned char type;/*0:jpg 1:bmp*/
  char filename[64];  /* 文件名,绝对路径 */
  char reserve[64];
}net_snap_t;

/*motion row col*/
typedef struct
{
    int slot;
    int rows;
    int cols;
    int reserved[32];
}motion_rowcol_t;

/*net manual record extend*/
typedef struct
{
    char enable;
    char slot; 
    char stream;
    char reserve[65];
}net_record_ex_t;

/* 摄像机的一些设置 */
typedef struct 
{		
    /* 各项请参考ipcam_misc_t */
	unsigned int flag;	/* 具体设置哪项，由位表示,以下各项按顺序从0开始 */
	unsigned char is_flip;		
	unsigned char enable_lowest_shutter; 
	unsigned char shutter;
	unsigned char noise_filter_auto;
	unsigned char noise_filter_level;
	unsigned char sharp_auto;
	unsigned char sharp_level;
	unsigned char envirenment; /* 0:outdoor 1:indoor */
	unsigned char enable_uppest_agc;
	unsigned char agc_upper_limit;
	unsigned char gamma;				//0~9
	unsigned char reserver[53];
}net_ipcam_misc_t;

/*黑白模式控制*/
typedef struct
{
    int control_mode;//0-手动  1-自动
    int sense;//灵敏度(1-3)在自动模式下起作用
    int blackwhite;//0-彩色 1-黑白 在手动模式下起作用
    int slot;//通道号
    char reserve[56];
}net_blackwhite_t;


/*gpio 控制*/
typedef struct
{
    int gpio;//gpio号
    int value;//0-低电位 1-高电位
    char reserve[32];
}net_gpio_ctrl_t;

#endif




