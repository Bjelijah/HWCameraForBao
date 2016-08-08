#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <pthread.h>
#include <semaphore.h>

#include "hwplay/stream_type.h"
#include "hwplay/play_def.h"
#include "net_sdk.h"

#include "com_howell_jni_JniUtil.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "jni.cc", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "jni.cc", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "jni.cc", __VA_ARGS__))



struct YV12glDisplay
{
	char * y;
	char * u;
	char * v;
	// unsigned long long time;
	int width;
	int height;
	//int inited;
	int enable;
	//  int is_catch_picture;
	//  char path[50];

	/* multi thread */
	int method_ready;
	JavaVM * jvm;
	JNIEnv * env;
	jmethodID mid,mStopCircle;
	jobject obj;
	pthread_mutex_t lock;
	unsigned long long first_time;
	//sem_t over_sem;
	//sem_t over_ret_sem;
};

static struct YV12glDisplay self;


void yuv12gl_set_enable(int enable)
{
	self.enable = enable;
	self.method_ready = 0;
}

void yv12gl_display(const unsigned char * y, const unsigned char *u,const unsigned char *v, int width, int height, unsigned long long time)
{
	if (!self.enable) return;

	if(self.jvm->AttachCurrentThread( &self.env, NULL) != JNI_OK) {
		LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
		return;
	}
	if (!self.method_ready) {

		jclass cls = self.env->GetObjectClass(self.obj);
		if (cls == NULL) {
			LOGE("FindClass() Error.....");
			goto error;
		}
		self.mid = self.env->GetMethodID( cls, "requestRender", "()V");
		if (self.mid == NULL ) {
			LOGE("GetMethodID() Error.....");
			goto error;
		}
		self.method_ready=1;
	}
	pthread_mutex_lock(&self.lock);
	if (width!=self.width || height!=self.height) {
		self.y = (char *)realloc(self.y,width*height);
		self.u = (char *)realloc(self.u,width*height/4);
		self.v = (char *)realloc(self.v,width*height/4);
		self.width = width;
		self.height = height;
	}
	memcpy(self.y,y,width*height);
	memcpy(self.u,u,width*height/4);
	memcpy(self.v,v,width*height/4);
	pthread_mutex_unlock(&self.lock);
	self.env->CallVoidMethod( self.obj, self.mid, NULL);
	if (self.jvm->DetachCurrentThread() != JNI_OK) {
		LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
	}
	return;
	error:
	if (self.jvm->DetachCurrentThread() != JNI_OK) {
		LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
	}
	return;
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeInit
(JNIEnv *env, jclass, jobject obj){
	env->GetJavaVM(&self.jvm);
	self.obj = env->NewGlobalRef(obj);
	pthread_mutex_init(&self.lock,NULL);
	//sem_init(&self.over_sem,0,0);
	//sem_init(&self.over_ret_sem,0,0);
	self.width = 352;
	self.height = 288;
	self.y = (char *)malloc(self.width*self.height);
	self.u = (char *)malloc(self.width*self.height/4);
	self.v = (char *)malloc(self.width*self.height/4);
	memset(self.y,0,self.width*self.height);
	memset(self.u,128,self.width*self.height/4);
	memset(self.v,128,self.width*self.height/4);
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeOnSurfaceCreated
(JNIEnv *, jclass){
	self.enable=1;
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeRenderY
(JNIEnv *, jclass){
	pthread_mutex_lock(&self.lock);
	if (self.y == NULL) {
		char value[4] = {0,0,0,0};
		glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,2,2,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,value);
	}
	else {
		//LOGI("render y");
		glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,self.width,self.height,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,self.y);
	}
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeRenderU
(JNIEnv *, jclass){
	if (self.u == NULL) {
		char value[] = {128};
		glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,1,1,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,value);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,self.width/2,self.height/2,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,self.u);
	}
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeRenderV
(JNIEnv *, jclass){
	if (self.v==NULL) {
		char value[] = {128};
		glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,1,1,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,value);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,self.width/2,self.height/2,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,self.v);
	}
	pthread_mutex_unlock(&self.lock);
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeDeinit
(JNIEnv *, jclass){
	self.method_ready = 0;
	free(self.y);
	free(self.u);
	free(self.v);
}

struct AudioPlay
{
	/* multi thread */
	int method_ready;
	JavaVM * jvm;
	JNIEnv * env;
	jmethodID mid;
	jobject obj;
	jfieldID data_length_id;
	jbyteArray data_array;
	int data_array_len;

	int stop;
	sem_t over_audio_sem;
	sem_t over_audio_ret_sem;
};
static struct AudioPlay audioSelf;

void audio_stop()
{
	audioSelf.stop=1;
}

void audio_play(const char* buf,int len,int au_sample,int au_channel,int au_bits)
{
	if (audioSelf.stop) return;
	if (audioSelf.jvm->AttachCurrentThread( &audioSelf.env, NULL) != JNI_OK) {
		LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
		return;
	}
	if (!audioSelf.method_ready) {
		jclass cls;
		cls = audioSelf.env->GetObjectClass(audioSelf.obj);
		if (cls == NULL) {
			LOGE("FindClass() Error.....");
			goto error;
		}
		audioSelf.mid = audioSelf.env->GetMethodID( cls, "audioWrite", "()V");
		if (audioSelf.mid == NULL) {
			LOGE("GetMethodID() Error.....");
			goto error;
		}
		audioSelf.method_ready=1;
	}
	audioSelf.env->SetIntField(audioSelf.obj,audioSelf.data_length_id,len);
	if (len<=audioSelf.data_array_len) {
		audioSelf.env->SetByteArrayRegion(audioSelf.data_array,0,len,(const signed char *)buf);
		audioSelf.env->CallVoidMethod( audioSelf.obj, audioSelf.mid, NULL);

	}
	if (audioSelf.jvm->DetachCurrentThread() != JNI_OK) {
		LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
	}
	return;
	error:
	if (audioSelf.jvm->DetachCurrentThread() != JNI_OK) {
		LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
	}
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeAudioInit
  (JNIEnv *env, jclass, jobject obj){
	  env->GetJavaVM(&audioSelf.jvm);

	  //����ֱ�Ӹ�ֵ(g_obj = obj)
	  audioSelf.obj = env->NewGlobalRef(obj);

	  jclass clz = env->GetObjectClass( obj);
	  audioSelf.data_length_id = env->GetFieldID(clz, "mAudioDataLength", "I");

	  jfieldID id = env->GetFieldID(clz,"mAudioData","[B");

	  jbyteArray data_array_local_ref = (jbyteArray)env->GetObjectField(obj,id);
	  int data_array_len_local_ref = env->GetArrayLength(data_array_local_ref);
	  jbyteArray data_array_global_ref =(jbyteArray)env->NewGlobalRef(data_array_local_ref);

	  audioSelf.data_array = data_array_global_ref;
	  audioSelf.data_array_len = data_array_len_local_ref;
	  //self.data_array = (*env)->GetObjectField(env,obj,id);
	  //self.data_array_len =(*env)->GetArrayLength(env,self.data_array);

	  sem_init(&audioSelf.over_audio_sem,0,0);
	  sem_init(&audioSelf.over_audio_ret_sem,0,0);

	  audioSelf.method_ready = 0;
	  audioSelf.stop = 0;
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeAudioStop
  (JNIEnv *, jclass){
	  audio_stop();
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeAudioDeinit
  (JNIEnv *, jclass){
	//TODO
}

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
		if(res->jvm->AttachCurrentThread( &res->env, NULL) != JNI_OK) {
			LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
			return;
		}
		__android_log_print(ANDROID_LOG_INFO, "on_alarm_stream_fun", "2222");
		jclass cls = res->env->GetObjectClass(res->obj);
		if (cls == NULL) {
			LOGE("FindClass() Error.....");
			goto error;
		}
		__android_log_print(ANDROID_LOG_INFO, "on_alarm_stream_fun", "3333");
		res->mid = res->env->GetMethodID( cls, "makeAlarmSound", "()V");
		if (res->mid == NULL ) {
			LOGE("GetMethodID() Error.....");
			goto error;
		}
		__android_log_print(ANDROID_LOG_INFO, "on_alarm_stream_fun", "4444");
		res->env->CallVoidMethod( res->obj, res->mid, NULL);
		__android_log_print(ANDROID_LOG_INFO, "on_alarm_stream_fun", "555");
		if (res->jvm->DetachCurrentThread() != JNI_OK) {
			LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
		}
		return;

		error:
		if (res->jvm->DetachCurrentThread() != JNI_OK) {
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

static void on_source_callback(PLAY_HANDLE handle,
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
	if (type == 0) {
		audio_play(buf,len,au_sample,au_channel,au_bits);
	}else if(type ==1){
		unsigned char* y = (unsigned char *)buf;
		unsigned char* u = y+w*h;
		unsigned char* v = u+w*h/4;
		yv12gl_display(y,u,v,w,h,timestamp);
	}
}

static void on_audio_callback(PLAY_HANDLE handle,
		const char* buf,//数据缓存,如果是视频，则为YV12数据，如果是音频则为pcm数据
		int len,//数据长度,如果为视频则应该等于w * h * 3 / 2
		unsigned long timestamp,//时标,单位为毫秒
		long user){
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
	//	hwplay_register_yuv_callback_ex(ph,on_yuv_callback_ex,0);
	//	hwplay_register_audio_callback(ph,on_audio_callback,0);
	hwplay_register_source_data_callback(ph,on_source_callback,0);
	hwplay_play(ph);
	return ph;
}

int create_resource(int is_playback,JNIEnv *env, jobject obj)
{
	/* make sure init once */
	__android_log_print(ANDROID_LOG_INFO, "!!!", "create_resource %d",is_playback);
	res = (struct StreamResource *)calloc(1,sizeof(*res));
	if (res == NULL) return -1;
	res->is_playback = is_playback;
	res->play_handle = init_play_handle(is_playback);
	env->GetJavaVM(&res->jvm);
	res->obj = env->NewGlobalRef(obj);
	return res->play_handle;
}

JNIEXPORT jint JNICALL Java_com_howell_jni_JniUtil_nativeDisplay
(JNIEnv *env, jclass, jint is_playback, jobject obj){
	return create_resource(is_playback,env,obj);
}


JNIEXPORT jint JNICALL Java_com_howell_jni_JniUtil_nativeStartRecord
(JNIEnv *, jclass){
	int ret = hwnet_start_record(res->user_handle,0);
	__android_log_print(ANDROID_LOG_INFO, "startRecord", "startRecord:%d",ret);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_howell_jni_JniUtil_nativeStopRecord
(JNIEnv *, jclass){
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

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeStartVMD
(JNIEnv *, jclass){
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
	memset(&motion_cfg,0,sizeof(motion_cfg_t));
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

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeStopVMD
(JNIEnv *, jclass){
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

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeQuit
(JNIEnv *, jclass){
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





JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeSetAutoColorChangeMode
(JNIEnv *, jclass){
	net_blackwhite_t bw;
	bw.control_mode = 1;
	bw.sense = 1;
	//bw.blackwhite
	bw.slot = 0;
	int ret = hwnet_set_blackwhite(res->user_handle,&bw);
	__android_log_print(ANDROID_LOG_INFO, "JNI", "setAutoColorChangeMode is:%d",ret);
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeSetManualColorChangeMode
(JNIEnv *, jclass){
	net_blackwhite_t bw;
	bw.control_mode = 0;
	bw.blackwhite = 1;
	//bw.blackwhite
	bw.sense = 1;
	bw.slot = 0;
	int ret = hwnet_set_blackwhite(res->user_handle,&bw);
	__android_log_print(ANDROID_LOG_INFO, "JNI", "setManualColorChangeMode is:%d",ret);
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeSetBlackMode
(JNIEnv *, jclass){
	net_blackwhite_t bw;
	bw.control_mode = 0;
	bw.blackwhite = 1;
	//bw.blackwhite
	bw.sense = 1;
	bw.slot = 0;
	int ret = hwnet_set_blackwhite(res->user_handle,&bw);
	__android_log_print(ANDROID_LOG_INFO, "JNI", "setBlackMode is:%d",ret);
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeSetColorMode
(JNIEnv *, jclass){
	net_blackwhite_t bw;
	bw.control_mode = 0;
	bw.blackwhite = 0;
	//bw.blackwhite
	bw.slot = 0;
	int ret = hwnet_set_blackwhite(res->user_handle,&bw);
	__android_log_print(ANDROID_LOG_INFO, "JNI", "setColorMode is:%d",ret);
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeSetHighPower
(JNIEnv *, jclass){
	int ret35 = hwnet_set_gpio(res->user_handle,35,1);
	__android_log_print(ANDROID_LOG_INFO, "setHighPower", "ret35:%d",ret35);
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeSetLowPower
(JNIEnv *, jclass){
	int ret13 = hwnet_set_gpio(res->user_handle,13,1);
	__android_log_print(ANDROID_LOG_INFO, "setHighPower", "ret13:%d ",ret13);
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeSetLaserIrradiateOn
(JNIEnv *, jclass){
	int ret13 = hwnet_set_gpio(res->user_handle,13,1);
	__android_log_print(ANDROID_LOG_INFO, "setHighPower", "ret13:%d",ret13);
}

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeSetLaserIrradiateOff
(JNIEnv *, jclass){
	int ret13 = hwnet_set_gpio(res->user_handle,13,0);
	__android_log_print(ANDROID_LOG_INFO, "setHighPower", "ret13:%d ",ret13);
}

void setFirstColorMode(int color){
	res->color_mode = color;
}

JNIEXPORT jint JNICALL Java_com_howell_jni_JniUtil_nativeGetBlackWhiteMode
(JNIEnv *, jclass){
	net_blackwhite_t bw;
	int ret = hwnet_get_blackwhite(res->user_handle,&bw);
	if(ret == 0){
		__android_log_print(ANDROID_LOG_INFO, "settings", "hwnet_get_blackwhite fail");
	}
	__android_log_print(ANDROID_LOG_INFO, "settings", "bw.control_mode %d,bw.blackwhite %d",bw.control_mode,bw.blackwhite);
	setFirstColorMode(bw.blackwhite);//0-彩色 1-黑白 在手动模式下起作用
	return bw.control_mode;//0手动 1自动
}

JNIEXPORT jint JNICALL Java_com_howell_jni_JniUtil_nativeGetFirstColorMode
(JNIEnv *, jclass){
	return res->color_mode;//0-��ɫ 1-�ڰ�
}

JNIEXPORT jint JNICALL Java_com_howell_jni_JniUtil_nativeGetLaserIrradiateMode
(JNIEnv *, jclass){
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

JNIEXPORT jint JNICALL Java_com_howell_jni_JniUtil_nativeGetVMDMode
(JNIEnv *, jclass){
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
	return -1;
}

JNIEXPORT jint JNICALL Java_com_howell_jni_JniUtil_nativeSetTime
  (JNIEnv *, jclass, jint year, jint month, jint day, jint hour, jint minute, jint second){
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

JNIEXPORT jint JNICALL Java_com_howell_jni_JniUtil_nativeGetMisc
  (JNIEnv *env, jclass, jobject classobj){
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
	jclass objectClass = env->GetObjectClass(classobj);
	__android_log_print(ANDROID_LOG_INFO, "getMisc", "2");
	jfieldID enable_lowest_shutter = env->GetFieldID(objectClass, "enable_lowest_shutter", "I");
	jfieldID shutter = env->GetFieldID(objectClass, "shutter", "I");
	jfieldID enable_uppest_agc = env->GetFieldID(objectClass, "enable_uppest_agc", "I");
	jfieldID agc_upper_limit = env->GetFieldID(objectClass, "agc_upper_limit", "I");
	__android_log_print(ANDROID_LOG_INFO, "getMisc", "3");
	env->SetIntField(classobj, enable_lowest_shutter, ipcam_misc.enable_lowest_shutter);
	env->SetIntField(classobj, shutter, ipcam_misc.shutter);
	env->SetIntField(classobj, enable_uppest_agc, ipcam_misc.enable_uppest_agc);
	env->SetIntField(classobj, agc_upper_limit, ipcam_misc.agc_upper_limit);
	__android_log_print(ANDROID_LOG_INFO, "getMisc", "4");
	return ret;
}

JNIEXPORT jint JNICALL Java_com_howell_jni_JniUtil_nativeSetMisc
  (JNIEnv *, jclass, jint flag, jint enable_lowest_shutter, jint shutter, jint enable_uppest_agc, jint agc_upper_limit){
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

JNIEXPORT void JNICALL Java_com_howell_jni_JniUtil_nativeSetIP
  (JNIEnv *env, jclass, jstring str){
	LOGE("jstring str=%s",str);
	if(str==NULL){
		return ;
	}
	const char* ip = env-> GetStringUTFChars(str,NULL);
	strcpy(g_ip,ip);
	LOGI("ip=%s",g_ip);
	env->ReleaseStringUTFChars(str,ip);
}








