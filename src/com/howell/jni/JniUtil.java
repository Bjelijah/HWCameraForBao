package com.howell.jni;

import com.howell.camera.NetIpcamMisc;

public class JniUtil {
	static {
		System.loadLibrary("hwplay");
        System.loadLibrary("player_jni");
    }

	/*
	 * yuv 
	 */
	public static native void nativeInit(Object callbackObj);
	public static native void nativeDeinit();
	public static native void nativeRenderY();
	public static native void nativeRenderU();
	public static native void nativeRenderV();
	public static native void nativeOnSurfaceCreated();

	/*
	 * aduio
	 */
	public static native void nativeAudioInit(Object callbackObj);
	public static native void nativeAudioStop();
	public static native void nativeAudioDeinit();

	/*
	 * decode
	 */
	public static native int nativeDisplay(int isPlayBack,Object callbackObj);
	public static native void nativeQuit();
	public static native int nativeSetTime(int year,int month,int day,int hour,int minute,int second);
	public static native int nativeStartRecord();
	public static native int nativeStopRecord();
	public static native void nativeStartVMD();
	public static native void nativeStopVMD();
	public static native void nativeSetAutoColorChangeMode();
	public static native void nativeSetManualColorChangeMode();
	public static native void nativeSetBlackMode();
	public static native void nativeSetColorMode();
    public static native void nativeSetHighPower();
	public static native void nativeSetLowPower();
	public static native void nativeSetLaserIrradiateOn();
	public static native void nativeSetLaserIrradiateOff();
	public static native int nativeGetBlackWhiteMode();
	public static native int nativeGetFirstColorMode();
	public static native int nativeGetLaserIrradiateMode();	//获取红外极光照明开关状态
	public static native int nativeGetVMDMode();			//获取移动侦测报警边界值和开关状态
	public static native int nativeSetMisc(int flag,int enable_lowest_shutter ,int shutter ,int enable_uppest_agc ,int agc_upper_limit);
	public static native int nativeGetMisc(NetIpcamMisc misc);
	public static native void nativeSetIP(String ip);








}
