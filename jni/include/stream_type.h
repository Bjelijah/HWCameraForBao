#ifndef stream_type_include_h
#define stream_type_include_h

#define STREAM_AUDIO_FRAME 2
#define STREAM_I_FRAME 1
#define STREAM_P_FRAME 0
#define STREAM_MOTION_FRAME 3
#define STREAM_B_FRAME 4
#define STREAM_FOCUS_FRAME 5
#define STREAM_MJPEG_FRAME 6

#define IS_VIDEO_FRAME(n) ((n) == STREAM_I_FRAME  \
|| (n)== STREAM_P_FRAME  \
||(n)== STREAM_B_FRAME  \
||(n) == STREAM_MJPEG_FRAME)

#define IS_LOCATE_FRAME(n)((n) == STREAM_I_FRAME \
|| (n) == STREAM_MJPEG_FRAME)

typedef struct {
	long len;
	long type; //0-bbp frame,1-i frame,2-audio
	unsigned long long time_stamp;
	long tag;
	long sys_time;
	//long reserve[1];
}stream_head;

#define  HW_MEDIA_TAG 0x48574D49
//#define MAX_STREAM_LEN	(1024 * 1024)
#define MAX_ALARM_LEN	(32 * 1024)

typedef enum{
	VDEC_H264 = 			0x00,	
	ADEC_G711U = 			0x01,
	ADEC_HISG711A = 		0x02,
	VDEC_HISH264 = 			0x03,
	ADEC_HISG711U = 		0x04,
	ADEC_HISADPCM = 		0x05,
	VDEC_MJPEG = 			0x06,
	ADEC_RAW = 				0x07,
	ADEC_G711A = 			0x08,
	ADEC_HISADPCM_DIV4 = 	0x09,
	ADEC_AAC = 				0x0a,
	ADEC_G726_32 = 			0x0b,
}HW_DEC_TYPE;
typedef struct 				
{
	unsigned int    media_fourcc;			// "HKMI": 0x484B4D49 Hikvision Media Information,"HWMI":0x48574D49
	long dvr_version;
	long vdec_code;
	long adec_code; 

	unsigned char au_bits;
	unsigned char au_sample;
	unsigned char au_channel;
	unsigned char reserve;
	unsigned int    reserved[5];            // ±£¡Ù
}HW_MEDIAINFO;

#endif

