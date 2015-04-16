LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := hwplay
LOCAL_SRC_FILES := libhwplay.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := hwnet
LOCAL_SRC_FILES := libhwnet.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := player_jni
# Add your application source files here...
LOCAL_SRC_FILES := yv12gl_jni.c decode_jni.c audio_jni.c
LOCAL_SHARED_LIBRARIES := hwplay hwnet
LOCAL_LDFLAGS := -LF:/Android/android-ndk-r8e/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi-v7a
LOCAL_LDLIBS := -llog -lgnustl_static -lGLESv2 -lz -ldl -lgcc
#	-L$(NDK_PLATFORMS_ROOT)/$(TARGET_PLATFORM)/arch-arm/usr/lib -L$(LOCAL_PATH) -lz -ldl -lgcc 
include $(BUILD_SHARED_LIBRARY)


