#include <jni.h>
#include <pthread.h>
#include <android/log.h>
#include <stdio.h>
#include <string.h> 
#include "hwplay/stream_type.h"
#include "hwplay/play_def.h"

#include "net_sdk.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "yv12", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "yv12", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "yv12", __VA_ARGS__))

struct StreamResource
{
	JavaVM * jvm;
	JNIEnv * env;
	jobject obj;
	jmethodID mid;
	PLAY_HANDLE play_handle;
	LIVE_STREAM_HANDLE live_stream_handle;
	USER_HANDLE user_handle;
	ALARM_STREAM_HANDLE alarm_stream_handle;
	int is_playback;
	int media_head_len;
	int row,col;
	int color_mode;
	int year,month,day,hour,minute,second;
};
static struct StreamResource * res = NULL;
static char g_ip[64]="";
void on_live_stream_fun(LIVE_STREAM_HANDLE handle,int stream_type,const char* buf,int len,long userdata){
	//__android_log_print(ANDROID_LOG_INFO, "jni", "-------------stream_type %d-",stream_type);
	int ret = hwplay_input_data(res->play_handle, buf ,len);
	if(ret == 0)
	{
		// hwplay_get_stream_buf_len(
		__android_log_print(ANDROID_LOG_INFO, "jni", "hwplay_input_data error");
	}

	int buf_len;
	ret = hwplay_get_stream_buf_remain(res->play_handle,&buf_len);
	if(ret == 1)
	{		 
		//__android_log_print(ANDROID_LOG_INFO, "jni", "buf_len %d",buf_len);
	}

}

void on_alarm_stream_fun(ALARM_STREAM_HANDLE handle,int alarm_type,const char* buf,int len,long userdata){
	__android_log_print(ANDROID_LOG_INFO, "jni", "alarm_type %d",alarm_type);
	if(alarm_type == HW_ALARM_MOTIONEX){
		__android_log_print(ANDROID_LOG_INFO, "on_alarm_stream_fun", "111");
		if((*res->jvm)->AttachCurrentThread(res->jvm, &res->env, NULL) != JNI_OK) {
			LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
			return;
		}
		__android_log_print(ANDROID_LOG_INFO, "on_alarm_stream_fun", "2222");
		jclass cls = (*res->env)->GetObjectClass(res->env,res->obj);
		if (cls == NULL) {
			LOGE("FindClass() Error.....");
			goto error;
		}
		__android_log_print(ANDROID_LOG_INFO, "on_alarm_stream_fun", "3333");
		res->mid = (*res->env)->GetMethodID(res->env, cls, "makeAlarmSound", "()V");
		if (res->mid == NULL ) {
			LOGE("GetMethodID() Error.....");
			goto error;
		}
		__android_log_print(ANDROID_LOG_INFO, "on_alarm_stream_fun", "4444");
		(*res->env)->CallVoidMethod(res->env, res->obj, res->mid, NULL);
		__android_log_print(ANDROID_LOG_INFO, "on_alarm_stream_fun", "555");
		if ((*res->jvm)->DetachCurrentThread(res->jvm) != JNI_OK) {
			LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
		}
		return;

		error:
		if ((*res->jvm)->DetachCurrentThread(res->jvm) != JNI_OK) {
			LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
		}
		return;
	}
}

static void on_yuv_callback_ex(PLAY_HANDLE handle,
		const unsigned char* y,
		const unsigned char* u,
		const unsigned char* v,
		int y_stride,
		int uv_stride,
		int width,
		int height,
		unsigned long long time,
		long user)
{	
	//__android_log_print(ANDROID_LOG_INFO, "jni", "start decode  time: %llu",time);
	//sdl_display_input_data(y,u,v,width,height,time);

	yv12gl_display(y,u,v,width,height,time);
}

on_source_callback(PLAY_HANDLE handle,
		int type,//0-��Ƶ,1-��Ƶ
		const char* buf,//��ݻ���,�������Ƶ����ΪYV12��ݣ��������Ƶ��Ϊpcm���
		int len,//��ݳ���,���Ϊ��Ƶ��Ӧ�õ���w * h * 3 / 2
		unsigned long timestamp,//ʱ��,��λΪ����
		long sys_tm,//osd ʱ��(1970�����ڵ�UTCʱ��)
		int w,//��Ƶ��,��Ƶ�����Ч
		int h,//��Ƶ��,��Ƶ�����Ч
		int framerate,//��Ƶ֡��,��Ƶ�����Ч
		int au_sample,//��Ƶ������,��Ƶ�����Ч
		int au_channel,//��Ƶͨ����,��Ƶ�����Ч
		int au_bits,//��Ƶλ��,��Ƶ�����Ч
		long user)
{
	//__android_log_print(ANDROID_LOG_INFO, "JNI", "type:%d len:%d %lu\n",type,len,timestamp);

	if (type == 0) {
		audio_play(buf,len,au_sample,au_channel,au_bits);
	}
	/*else if (type == 1) {
    native_catch_picture(res->play_handle);
  }
  __android_log_print(ANDROID_LOG_INFO, "JNI", "type0 over");*/
}

on_audio_callback(PLAY_HANDLE handle,
		const char* buf,//数据缓存,如果是视频，则为YV12数据，如果是音频则为pcm数据
		int len,//数据长度,如果为视频则应该等于w * h * 3 / 2
		unsigned long timestamp,//时标,单位为毫秒
		long user){
	//__android_log_print(ANDROID_LOG_INFO, "audio", "on_audio_callback timestamp: %lu ",timestamp);

	//if(res->is_exit == 1) return;
	audio_play(buf,len,0,0,0);

}

PLAY_HANDLE init_play_handle(int is_playback){
	__android_log_print(ANDROID_LOG_INFO, "jni", "start init ph palyback: %d",is_playback);
	//hwplay_init(1,352,288);
	hwplay_init(1,0,0);
	__android_log_print(ANDROID_LOG_INFO, "jni", "0");

	int ret = hwnet_init(5888);

	/* 192.168.42.2 */

	if(strcmp(g_ip,"")==0){
		LOGE("g_ip = null");
		return -1;
	}

	res->user_handle = hwnet_login(g_ip,5198,"admin","12345");
	if(res->user_handle == -1){ 
		__android_log_print(ANDROID_LOG_INFO, "jni", "user_handle fail");
		return -1;
	}
	__android_log_print(ANDROID_LOG_INFO, "jni", "user_handle: %d",res->user_handle);
	res->live_stream_handle = hwnet_get_live_stream(res->user_handle,0,1
			,1,on_live_stream_fun,0);
	__android_log_print(ANDROID_LOG_INFO, "jni", "live_stream_handle: %d",res->live_stream_handle);

	RECT area ;
	//memset(&area,0,sizeof(area));
	//area.right = 177;
	//area.bottom = 144;
	/*HW_MEDIAINFO media_head;
	memset(&media_head,0,sizeof(media_head));
	media_head.media_fourcc = HW_MEDIA_TAG;
	media_head.au_channel = 1;
	media_head.au_sample = 8;
	media_head.au_bits = 16;
	media_head.adec_code = ADEC_AAC;
	media_head.vdec_code = VDEC_H264;*/
	//__android_log_print(ANDROID_LOG_INFO, "JNI", "media_head finish");

	HW_MEDIAINFO media_head;
	memset(&media_head,0,sizeof(media_head));
	__android_log_print(ANDROID_LOG_INFO, "jni", "1");
	//int media_head_len = 0;
	int ret2 = hwnet_get_live_stream_head(res->live_stream_handle,(char*)&media_head,1024,&res->media_head_len);
	__android_log_print(ANDROID_LOG_INFO, "jni", "ret2 :%d",ret2);
	__android_log_print(ANDROID_LOG_INFO, "jni", "is_playback :%d",is_playback);
	LOGE("0x%x",media_head.vdec_code);
	res->alarm_stream_handle = hwnet_get_alarm_stream(res->user_handle,on_alarm_stream_fun,0);
	__android_log_print(ANDROID_LOG_INFO, "jni", "alarm_stream_handle :%d",res->alarm_stream_handle);

	PLAY_HANDLE  ph = hwplay_open_stream((char*)&media_head,sizeof(media_head),1024*1024,is_playback,area);
	ret = hwplay_open_sound(ph);
	hwplay_set_max_framenum_in_buf(ph,is_playback?25:5);
	//__android_log_print(ANDROID_LOG_INFO, "JNI", "media_head.media_fourcc is:%d",media_head.media_fourcc);
	__android_log_print(ANDROID_LOG_INFO, "JNI", "ph is:%d",ph);
	//resource->play_handle = ph;
	hwplay_register_yuv_callback_ex(ph,on_yuv_callback_ex,0);
	//hwplay_register_source_data_callback(ph,on_source_callback,0);
	hwplay_register_audio_callback(ph,on_audio_callback,0);
	//	__android_log_print(ANDROID_LOG_INFO, "JNI", "true");
	//else
	//	__android_log_print(ANDROID_LOG_INFO, "JNI", "false");
	hwplay_play(ph);

	return ph;
}

int create_resource(int is_playback,JNIEnv *env, jobject obj)
{
	/* make sure init once */
	__android_log_print(ANDROID_LOG_INFO, "!!!", "create_resource %d",is_playback);
	res = (struct StreamResource *)calloc(1,sizeof(*res));
	if (res == NULL) return 0;
	res->is_playback = is_playback;
	res->play_handle = init_play_handle(is_playback);
	(*env)->GetJavaVM(env,&res->jvm);
	res->obj = (*env)->NewGlobalRef(env,obj);
	return res->play_handle;
}


JNIEXPORT int JNICALL Java_com_howell_camera_HWCameraActivity_display
(JNIEnv *env, jobject obj, jint is_playback){
	return create_resource(is_playback,env,obj);
}

//�Զ����ںڰ�ģʽ
JNIEXPORT void JNICALL Java_com_howell_camera_HWCameraActivity_setAutoColorChangeMode
(JNIEnv *env, jclass cls){

	net_blackwhite_t bw;
	bw.control_mode = 1;
	bw.sense = 1;
	//bw.blackwhite  
	bw.slot = 0;
	int ret = hwnet_set_blackwhite(res->user_handle,&bw);
	__android_log_print(ANDROID_LOG_INFO, "JNI", "setAutoColorChangeMode is:%d",ret);
}

//�ֶ����ںڰ�ģʽ
JNIEXPORT void JNICALL Java_com_howell_camera_HWCameraActivity_setManualColorChangeMode
(JNIEnv *env, jclass cls){

	net_blackwhite_t bw;
	bw.control_mode = 0;
	bw.blackwhite = 1;
	//bw.blackwhite  
	bw.sense = 1;
	bw.slot = 0;
	int ret = hwnet_set_blackwhite(res->user_handle,&bw);
	__android_log_print(ANDROID_LOG_INFO, "JNI", "setManualColorChangeMode is:%d",ret);
}

//�ڰ�ģʽ
JNIEXPORT void JNICALL Java_com_howell_camera_HWCameraActivity_setBlackMode
(JNIEnv *env, jclass cls){
	net_blackwhite_t bw;
	bw.control_mode = 0;
	bw.blackwhite = 1;
	//bw.blackwhite  
	bw.sense = 1;
	bw.slot = 0;
	int ret = hwnet_set_blackwhite(res->user_handle,&bw);
	__android_log_print(ANDROID_LOG_INFO, "JNI", "setBlackMode is:%d",ret);
}

//��ɫģʽ
JNIEXPORT void JNICALL Java_com_howell_camera_HWCameraActivity_setColorMode
(JNIEnv *env, jclass cls){
	net_blackwhite_t bw;
	bw.control_mode = 0;
	bw.blackwhite = 0;
	//bw.blackwhite  
	bw.slot = 0;
	int ret = hwnet_set_blackwhite(res->user_handle,&bw);
	__android_log_print(ANDROID_LOG_INFO, "JNI", "setColorMode is:%d",ret);
}

//���ø߹���
JNIEXPORT void JNICALL Java_com_howell_camera_HWCameraActivity_setHighPower
(JNIEnv *env, jclass cls){
	//int ret13 = hwnet_get_gpio(res->user_handle,13,0);
	int ret35 = hwnet_set_gpio(res->user_handle,35,1);
	__android_log_print(ANDROID_LOG_INFO, "setHighPower", "ret35:%d",ret35);
}

//���õ͹���
JNIEXPORT void JNICALL Java_com_howell_camera_HWCameraActivity_setLowPower
(JNIEnv *env, jclass cls){
	int ret13 = hwnet_set_gpio(res->user_handle,13,1);
	//int ret35 = hwnet_get_gpio(res->user_handle,35,0);
	__android_log_print(ANDROID_LOG_INFO, "setHighPower", "ret13:%d ",ret13);
}

//���ú��⼤���
JNIEXPORT void JNICALL Java_com_howell_camera_HWCameraActivity_setLaserIrradiateOn
(JNIEnv *env, jclass cls){
	//��Ĭ�ϵ͹���
	int ret13 = hwnet_set_gpio(res->user_handle,13,1);
	//int ret35 = hwnet_get_gpio(res->user_handle,35,0);
	__android_log_print(ANDROID_LOG_INFO, "setHighPower", "ret13:%d",ret13);
}

//���ú��⼤��ر�
JNIEXPORT void JNICALL Java_com_howell_camera_HWCameraActivity_setLaserIrradiateOff
(JNIEnv *env, jclass cls){
	int ret13 = hwnet_set_gpio(res->user_handle,13,0);
	//int ret35 = hwnet_set_gpio(res->user_handle,35,0);
	__android_log_print(ANDROID_LOG_INFO, "setHighPower", "ret13:%d ",ret13);
}

//��¼��
JNIEXPORT int JNICALL Java_com_howell_camera_HWCameraActivity_startRecord
(JNIEnv *env, jclass cls){

	int ret = hwnet_start_record(res->user_handle,0);
	__android_log_print(ANDROID_LOG_INFO, "startRecord", "startRecord:%d",ret);
	return ret;
}

//�ر�¼��
JNIEXPORT int JNICALL Java_com_howell_camera_HWCameraActivity_stopRecord
(JNIEnv *env, jclass cls){

	int ret = hwnet_stop_record(res->user_handle,0);
	__android_log_print(ANDROID_LOG_INFO, "stopRecord","stopRecord:%d", ret);
	return ret;
}

int is_motion_set(motion_cfg_t* cfg,int x,int y,int cols)
{
	int *data = (int*)cfg->data;
	int int_num_in_one_row = 1 + (cols - 1) / 32;
	return data[y * int_num_in_one_row + x / 32] & (1 << (x % 32));
}

void motion_set(motion_cfg_t* cfg,int x,int y,int cols)
{
	int * data = (int*)cfg->data;
	int int_num_in_one_row = 1 + (cols - 1) / 32;
	data[y * int_num_in_one_row + x / 32] |= (1 << (x % 32));
}


//�ƶ�����
JNIEXPORT void JNICALL Java_com_howell_camera_HWCameraActivity_startVMD
(JNIEnv *env, jclass cls){
	//row = 18; col = 32; lev= 2��
	/*�����ƶ�������
	 * handle:					hwnet_login()���صľ��
	 * motion_cfg:				
	 * 	--slot:					������ͨ����(��0��ʼ)
	 * 	--lev:					������0-6,0-���,5���,6�ر�
	 * 	--rec_delay;			���¼����Ҫ¼�����ʱ�䡣0-6,�ֱ��Ӧ 10��,20�룬30�룬1�֣�2�֣�6�֣�10��
	 * 	--data;					��18��long��ɣ���ݷ��ص�row,col���ж�
	 * return:					1:�ɹ�	0:����

	 */
	//int row =9,col =11;
	motion_cfg_t motion_cfg;
	__android_log_print(ANDROID_LOG_INFO, "startVMD","row:%d col:%d",res->row, res->col);
	int i,j ;
	for(i = 0; i < res->row; i++)
	{
		for(j = 0; j < res->col; j++)
		{
			motion_set(&motion_cfg,j,i,res->col);
		}
	}
	__android_log_print(ANDROID_LOG_INFO, "startVMD","row:%d col:%d",res->row, res->col);
	motion_cfg.slot = 0;
	motion_cfg.lev = 5;
	motion_cfg.rec_delay = 0;
	int ret = hwnet_set_motion_cfg(res->user_handle,&motion_cfg);
	__android_log_print(ANDROID_LOG_INFO, "startVMD","startVMD:%d", ret);
}

//�ƶ����ر�
JNIEXPORT void JNICALL Java_com_howell_camera_HWCameraActivity_stopVMD
(JNIEnv *env, jclass cls){
	int row = 18 ,col = 32;
	motion_cfg_t motion_cfg;
	int i,j ;
	/*for(i = 0; i < res->row; i++)
	 {
		for(j = 0; j < res->col; j++)
		{
			motion_set(&motion_cfg,i,j,res->col);
		}
	 }*/
	memset(&motion_cfg,0,sizeof(motion_cfg));
	motion_cfg.slot = 0;
	motion_cfg.lev = 2;
	motion_cfg.rec_delay = 0;
	int ret = hwnet_set_motion_cfg(res->user_handle,&motion_cfg);
	__android_log_print(ANDROID_LOG_INFO, "stopVMD","stopVMD:%d", ret);

}

//�˳�
JNIEXPORT void JNICALL Java_com_howell_camera_HWCameraActivity_quit
(JNIEnv *env, jclass cls){
	__android_log_print(ANDROID_LOG_INFO, "quit", "1");
	int ret = hwnet_close_live_stream(res->live_stream_handle);
	__android_log_print(ANDROID_LOG_INFO, "quit", "2");
	if(ret == 0){
		__android_log_print(ANDROID_LOG_INFO, "quit", "close live stream fail");
	}
	ret = hwnet_close_alarm_stream(res->alarm_stream_handle);
	if(ret == 0){
		__android_log_print(ANDROID_LOG_INFO, "quit", "close alarm stream fail");
	}
	__android_log_print(ANDROID_LOG_INFO, "quit", "3");
	ret = hwnet_logout(res->user_handle);
	__android_log_print(ANDROID_LOG_INFO, "quit", "4");
	if(ret == 0){
		__android_log_print(ANDROID_LOG_INFO, "quit", "logout fail");
	}
	__android_log_print(ANDROID_LOG_INFO, "quit", "5");
	hwplay_stop(res->play_handle);
	__android_log_print(ANDROID_LOG_INFO, "quit", "6");
	hwnet_release();
	__android_log_print(ANDROID_LOG_INFO, "quit", "7");
	free(res);
}

//����ʼʱ�ж��Ƿ�ڰ�ģʽ
JNIEXPORT int JNICALL Java_com_howell_camera_HWCameraActivity_getBlackWhiteMode
(JNIEnv *env, jclass cls){

	net_blackwhite_t bw;
	int ret = hwnet_get_blackwhite(res->user_handle,&bw);
	if(ret == 0){
		__android_log_print(ANDROID_LOG_INFO, "settings", "hwnet_get_blackwhite fail");
	}
	__android_log_print(ANDROID_LOG_INFO, "settings", "bw.control_mode %d,bw.blackwhite %d",bw.control_mode,bw.blackwhite);
	setFirstColorMode(bw.blackwhite);//0-彩色 1-黑白 在手动模式下起作用
	return bw.control_mode;//0手动 1自动
}

void setFirstColorMode(int color){
	res->color_mode = color;
}

JNIEXPORT int JNICALL Java_com_howell_camera_HWCameraActivity_getFirstColorMode
(JNIEnv *env, jclass cls){
	return res->color_mode;//0-��ɫ 1-�ڰ�
}

//����ʼʱ�жϺ��⼤���Ƿ���
JNIEXPORT int JNICALL Java_com_howell_camera_HWCameraActivity_getLaserIrradiateMode
(JNIEnv *env, jclass cls){

	int value;
	hwnet_get_gpio(res->user_handle,13,&value);
	__android_log_print(ANDROID_LOG_INFO, "settings", "value 13 %d",value);
	if(value != 1){
		hwnet_get_gpio(res->user_handle,35,&value);
		__android_log_print(ANDROID_LOG_INFO, "settings", "value 35 %d",value);
		if(value != 1){
			return 3;//�ر�
		}else{
			return 2;//�߹���
		}
	}else {
		return 1;//�͹���
	}
}

//����ʼʱ�ж��ƶ�����Ƿ���
JNIEXPORT int JNICALL Java_com_howell_camera_HWCameraActivity_getVMDMode
(JNIEnv *env, jclass cls){

	motion_cfg_t motion_cfg;
	memset(&motion_cfg,0,sizeof(motion_cfg));
	int i,j;
	__android_log_print(ANDROID_LOG_INFO, "settings", "res->user_handle %d",res->user_handle);
	int ret = hwnet_get_motion_cfg(res->user_handle,&res->row,&res->col,&motion_cfg);
	__android_log_print(ANDROID_LOG_INFO, "settings", "ret %d res->row:%d res->col:%d",ret,res->row,res->col);
	if(ret)
	{
		for(i = 0; i < res->row; i++)
		{
			for(j = 0; j < res->col; j++)
			{
				if(is_motion_set(&motion_cfg,j,i,res->col))
				{
					//�������������ƶ����
					return 1;//�ƶ���⿪��
				}
				else
				{	
					//������δ�����ƶ����

				}
			}
		}
		return 0;//�ƶ����ر�
	}
}

//����ʼʱ����ʱ��
JNIEXPORT int JNICALL Java_com_howell_camera_HWCameraActivity_setTime
(JNIEnv *env, jclass cls,jint year,jint month,jint day,jint hour,jint minute,jint second){

	SYSTEMTIME systm;
	systm.wYear = year;
	systm.wMonth = month;
	systm.wDay = day;

	systm.wHour = hour;
	systm.wMinute = minute;
	systm.wSecond = second;

	systm.wDayofWeek = 0;
	systm.wMilliseconds = 0;

	int ret = hwnet_set_systime(res->user_handle,&systm);
	return ret;
}

JNIEXPORT int JNICALL Java_com_howell_camera_HWCameraActivity_getMisc
(JNIEnv *env, jclass cls ,jobject classobj){
	net_ipcam_misc_t ipcam_misc;
	int ret = hwnet_ipc_get_misc(res->user_handle,&ipcam_misc);
	__android_log_print(ANDROID_LOG_INFO, "getMisc", "ret %d",ret);
	__android_log_print(ANDROID_LOG_INFO, "getMisc", "flag:%d,is_flip:%d,enable_lowest_shutter:%d,enable_uppest_agc:%d,agc_upper_limit:%d	"
			,ipcam_misc.flag
			,ipcam_misc.enable_lowest_shutter
			,ipcam_misc.shutter
			,ipcam_misc.enable_uppest_agc
			,ipcam_misc.agc_upper_limit);
	__android_log_print(ANDROID_LOG_INFO, "getMisc", "1");
	jclass objectClass = (*env)->GetObjectClass(env,classobj);
	__android_log_print(ANDROID_LOG_INFO, "getMisc", "2");
	jfieldID enable_lowest_shutter = (*env)->GetFieldID(env,objectClass, "enable_lowest_shutter", "I");
	jfieldID shutter = (*env)->GetFieldID(env,objectClass, "shutter", "I");
	jfieldID enable_uppest_agc = (*env)->GetFieldID(env,objectClass, "enable_uppest_agc", "I");
	jfieldID agc_upper_limit = (*env)->GetFieldID(env,objectClass, "agc_upper_limit", "I");
	__android_log_print(ANDROID_LOG_INFO, "getMisc", "3");
	(*env)->SetIntField(env,classobj, enable_lowest_shutter, ipcam_misc.enable_lowest_shutter);
	(*env)->SetIntField(env,classobj, shutter, ipcam_misc.shutter);
	(*env)->SetIntField(env,classobj, enable_uppest_agc, ipcam_misc.enable_uppest_agc);
	(*env)->SetIntField(env,classobj, agc_upper_limit, ipcam_misc.agc_upper_limit);
	__android_log_print(ANDROID_LOG_INFO, "getMisc", "4");
	return ret;
}

JNIEXPORT int JNICALL Java_com_howell_camera_HWCameraActivity_setMisc
(JNIEnv *env, jclass cls ,jint flag, jint enable_lowest_shutter , jint shutter ,jint enable_uppest_agc ,jint agc_upper_limit){
	net_ipcam_misc_t ipcam_misc;
	memset(&ipcam_misc,0,sizeof(ipcam_misc));
	ipcam_misc.flag = flag;
	if(flag == 1 << 1){
		ipcam_misc.enable_lowest_shutter = enable_lowest_shutter;
		ipcam_misc.shutter = shutter;
	}else if (flag == 1 << 8){
		ipcam_misc.enable_uppest_agc = enable_uppest_agc;
		ipcam_misc.agc_upper_limit = agc_upper_limit;
	}
	int ret = hwnet_ipc_set_misc(res->user_handle,&ipcam_misc);
	return ret;
}


JNIEXPORT int JNICALL Java_com_howell_camera_HWCameraActivity_setIP
(JNIEnv *env, jclass cls,jstring str){
	LOGE("jstring str=%s",str);
	if(str==NULL){
		return -1;
	}
	const char* ip = (*env)-> GetStringUTFChars(env,str,NULL);
	strcpy(g_ip,ip);
	LOGI("ip=%s",g_ip);
	(*env)->ReleaseStringUTFChars(env,str,ip);
	return 1;
}

