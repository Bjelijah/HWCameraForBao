#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <jni.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <pthread.h>
#include <semaphore.h>

#include "hwplay/stream_type.h"
#include "hwplay/play_def.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "yv12", __VA_ARGS__))   
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "yv12", __VA_ARGS__))   
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "yv12", __VA_ARGS__))  


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

void yv12gl_display(const unsigned char * y, const unsigned char *u, unsigned char *v, int width, int height, unsigned long long time)
{
//LOGE("display"); 
//__android_log_print(ANDROID_LOG_INFO, "yv12gl_display", "time: %llu",time);
  if (!self.enable) return;

  if((*self.jvm)->AttachCurrentThread(self.jvm, &self.env, NULL) != JNI_OK) {   
      LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);   
      return;
  }

  /* get JAVA method first */
  if (!self.method_ready) {
	  //LOGE("111111111"); 
   
    jclass cls = (*self.env)->GetObjectClass(self.env,self.obj);
	//self.clz = (*self.env)->FindClass(self.env, "com/howell/webcam/player/YV12Renderer");
    if (cls == NULL) {   
      LOGE("FindClass() Error.....");   
      goto error;   
    }
    //�ٻ�����еķ���   
    self.mid = (*self.env)->GetMethodID(self.env, cls, "requestRender", "()V");
	//self.mStopCircle = (*self.env)->GetMethodID(self.env, cls, "stopCircle", "()V");
    if (self.mid == NULL ) {   
      LOGE("GetMethodID() Error.....");   
      goto error;   
    }
    self.method_ready=1;
  } 

	//(*self.env)->CallVoidMethod(self.env,self.obj,self.mStopCircle,NULL);

 // LOGE("22222222"); 
  
  /*
  if (sem_trywait(&self.over_sem)==0) {
	  if (self.method_ready)
	  {
		  
	  }
	  sem_post(&self.over_ret_sem);
	  self.enable=0;
	  return;
  }
  */
  //LOGE("33333333"); 
  pthread_mutex_lock(&self.lock);
  if (width!=self.width || height!=self.height) {
    self.y = realloc(self.y,width*height);
    self.u = realloc(self.u,width*height/4);
    self.v = realloc(self.v,width*height/4);
    self.width = width;
    self.height = height;
  }
  memcpy(self.y,y,width*height);
  memcpy(self.u,u,width*height/4);
  memcpy(self.v,v,width*height/4);
  pthread_mutex_unlock(&self.lock);

//LOGE("4444444"); 
  /* notify the JAVA */
  (*self.env)->CallVoidMethod(self.env, self.obj, self.mid, NULL);
//LOGE("555555555"); 

 if ((*self.jvm)->DetachCurrentThread(self.jvm) != JNI_OK) {   
				LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);   
		   }   
  return;

 error:
  if ((*self.jvm)->DetachCurrentThread(self.jvm) != JNI_OK) {   
    LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);   
  }   
  return;
}

JNIEXPORT void JNICALL Java_com_howell_play_YV12Renderer_nativeInit
(JNIEnv *env, jobject obj)
{
	//self = malloc(sizeof(YV12glDisplay));
	//memset(&self,0,sizeof(YV12glDisplay));
  (*env)->GetJavaVM(env,&self.jvm);   
  //����ֱ�Ӹ�ֵ(g_obj = obj)   
  self.obj = (*env)->NewGlobalRef(env,obj);
  pthread_mutex_init(&self.lock,NULL);
  //sem_init(&self.over_sem,0,0);
  //sem_init(&self.over_ret_sem,0,0);
  self.width = 352;
  self.height = 288;
  self.y = malloc(self.width*self.height);
  self.u = malloc(self.width*self.height/4);
  self.v = malloc(self.width*self.height/4);
  memset(self.y,0,self.width*self.height);
  memset(self.u,128,self.width*self.height/4);
  memset(self.v,128,self.width*self.height/4);
  //self.time = 0;
}

JNIEXPORT void JNICALL JNICALL Java_com_howell_play_YV12Renderer_nativeOnSurfaceCreated
(JNIEnv *env, jobject obj)
{
  //self.inited=1;
  self.enable=1;
}

JNIEXPORT void JNICALL JNICALL Java_com_howell_play_YV12Renderer_nativeRenderY
(JNIEnv *env, jobject obj)
{
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

JNIEXPORT void JNICALL JNICALL Java_com_howell_play_YV12Renderer_nativeRenderU
(JNIEnv *env, jobject obj)
{
  if (self.u == NULL) {
    char value[] = {128};
    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,1,1,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,value);
  }
  else {
    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,self.width/2,self.height/2,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,self.u);
  }
}

JNIEXPORT void JNICALL JNICALL Java_com_howell_play_YV12Renderer_nativeRenderV
(JNIEnv *env, jobject obj)
{
  if (self.v==NULL) {
    char value[] = {128};
    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,1,1,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,value);
  }
  else {
    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,self.width/2,self.height/2,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,self.v);
  }
  pthread_mutex_unlock(&self.lock);
}

JNIEXPORT void JNICALL Java_com_howell_play_YV12Renderer_nativeDeinit
(JNIEnv *env, jobject obj)
{
  /* TODO */
  self.method_ready = 0;
  free(self.y);
  free(self.u);
  free(self.v);
}
