package com.howell.camera;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.SoundPool;
import android.opengl.GLSurfaceView;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.Spinner;

import com.howell.action.NetAction;
import com.howell.jni.JniUtil;
import com.howell.play.YV12Renderer;

public class HWCameraActivity extends Activity implements Callback,OnClickListener{
	/** Called when the activity is first created. */
	private GLSurfaceView mGlView;
	private boolean mPausing;
	private boolean showSurfaceSet;

	private ScrollView mSurfaceSet;
	private LinearLayout mLayoutChangeColor,mLayoutChangePower;
	private CheckBox mColorChange,mLaserSet,mLasetPower,mRecord,mMoveDetect,mScale;
	private Spinner spShutter;
	private ArrayAdapter<String> shutterAdapter,gainAdapter;  
	private String[] shutterArray,gainArray;
	private NetIpcamMisc misc ;
	//	private boolean spShutterFirstSelectFlag;
	private CheckBox cbShutter,cbGain;

	private static AudioTrack mAudioTrack;
	private byte[] mAudioData;
	private int mAudioDataLength;

	private SoundPool sp;  
	private int soundId;

	private ProgressDialog pd;
	boolean isShowDialog;

	private ClearAlarmNumThread thread;
	private AlarmNumManager alarmManager;

	//	static {
	//		System.loadLibrary("hwplay");
	//        System.loadLibrary("player_jni");
	//    }

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		mGlView = (GLSurfaceView)findViewById(R.id.glsurface_view);
		mGlView.setEGLContextClientVersion(2);
		mGlView.setRenderer(new YV12Renderer(this,mGlView));
		mGlView.getHolder().addCallback((Callback) this);
		mGlView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

		mPausing = false;
		showSurfaceSet = false;
		//		spShutterFirstSelectFlag = false;
		setIP(NetAction.getInstance().getServiceIp(this));
		alarmManager = new AlarmNumManager();

		mSurfaceSet = (ScrollView)findViewById(R.id.scrollView);
		mGlView.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				if(showSurfaceSet){
					mSurfaceSet.setVisibility(View.GONE);
					showSurfaceSet = false;
				}else{
					mSurfaceSet.setVisibility(View.VISIBLE);
					showSurfaceSet = true;
				}
			}
		});

		mLayoutChangeColor = (LinearLayout)findViewById(R.id.ll_color_change);
		mLayoutChangePower = (LinearLayout)findViewById(R.id.ll_change_power);

		mColorChange = (CheckBox)findViewById(R.id.cb_color_change);
		mColorChange.setOnClickListener(this);
		mScale = (CheckBox)findViewById(R.id.cb_scale);
		mScale.setOnClickListener(this);
		mLaserSet = (CheckBox)findViewById(R.id.cb_laser_set);
		mLaserSet.setOnClickListener(this);
		mLasetPower = (CheckBox)findViewById(R.id.cb_laser_power);
		mLasetPower.setOnClickListener(this);
		mMoveDetect = (CheckBox)findViewById(R.id.cb_move_detect);
		mMoveDetect.setOnClickListener(this);
		cbShutter = (CheckBox)findViewById(R.id.cb_shutter);
		cbShutter.setOnClickListener(this);
		cbGain = (CheckBox)findViewById(R.id.cb_gain);
		cbGain.setOnClickListener(this);
		spShutter = (Spinner)findViewById(R.id.spinner_shutter);
		shutterArray = getResources().getStringArray(R.array.shutter_arry);
		shutterAdapter = new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item,shutterArray);
		shutterAdapter.setDropDownViewResource(R.layout.spinner_item); 
		spShutter.setAdapter(shutterAdapter);  
		//进止程序启动时spinner调用OnItemSelectedListener
		//spShutter.setSelection(0, true);
		spShutter.setOnItemSelectedListener(new Spinner.OnItemSelectedListener(){  
			@Override  
			public void onItemSelected(AdapterView<?> arg0, View arg1,  
					int arg2, long arg3) {  
				//	        	System.out.println("spShutterFirstSelectFlag:"+spShutterFirstSelectFlag);
				//	        	if(!spShutterFirstSelectFlag){
				//	        		spShutterFirstSelectFlag = true;
				//	        		return;
				//	        	}
				System.out.println("OnItemSelectedListener"+"你选择了："+ shutterArray[(int)arg2]);
				//	            Toast.makeText(getApplicationContext(),   
				//	                    "你选择了："+ shutterArray[(int)arg2], 1).show();  
				switch((int)arg2){
				case 0:;break;
				case 1:setMisc(1 << 1, 1 ,1,0,0);break;
				case 2:setMisc(1 << 1, 1 ,3,0,0);break;
				case 3:setMisc(1 << 1, 1 ,5,0,0);break;
				case 4:setMisc(1 << 1, 1 ,7,0,0);break;
				}
				//arg0.setVisibility(View.VISIBLE);  
			}  
			@Override  
			public void onNothingSelected(AdapterView<?> arg0) { 

			}             
		});  

		sp = new SoundPool(1,AudioManager.STREAM_MUSIC, 0);  
		soundId = sp.load(this, R.raw.alarm, 1);
		System.out.println("soundId:"+soundId);

		audioInit();
		new AsyncTask<Void, Integer, Void>(){
			int ret;
			@Override
			protected Void doInBackground(Void... arg0) {
				// TODO Auto-generated method stub

				ret = display(0);
				return null;
			}
			@SuppressLint("NewApi")
			protected void onPostExecute(Void result) {
				if(ret >= 0){
					//OK
					new AsyncTask<Void, Integer, Void>(){

						@Override
						protected Void doInBackground(Void... arg0) {
							// TODO Auto-generated method stub
							getSettings();
							return null;
						}

						protected void onPostExecute(Void result) {
							setCameraCheckBoxValue();
						};

					}.execute();

				}else{
					if(!isDestroyed()){
						Dialog alertDialog = new AlertDialog.Builder(HWCameraActivity.this).   
								setTitle("登录失败").   
								setMessage("登录失败，请重新登录").   
								setIcon(R.drawable.expander_ic_minimized).   
								setPositiveButton("确定", new DialogInterface.OnClickListener() {   
									@Override   
									public void onClick(DialogInterface dialog, int which) {   
										// TODO Auto-generated method stub  
										finish();
									}   
								}).   
								create();   
						alertDialog.show();  
					}
				}
			};
		}.execute();
	}

	private int getYear(String phoneTime){
		return Integer.valueOf(phoneTime.substring(0, 4));
	}
	private int getMonth(String phoneTime){
		return Integer.valueOf(phoneTime.substring(4, 6));
	}
	private int getDay(String phoneTime){
		return Integer.valueOf(phoneTime.substring(6, 8));
	}
	private int getHour(String phoneTime){
		return Integer.valueOf(phoneTime.substring(8, 10));
	}
	private int getMinute(String phoneTime){
		return Integer.valueOf(phoneTime.substring(10, 12));
	}
	private int getSecond(String phoneTime){
		return Integer.valueOf(phoneTime.substring(12, 14));
	}

	private int BWModeRet,firstColorRet,laserRet,VMDRet;
	//获取摄像机各属性值
	private void getSettings(){
		BWModeRet = getBlackWhiteMode();
		firstColorRet = getFirstColorMode();
		laserRet = getLaserIrradiateMode();
		VMDRet = getVMDMode();
		misc = new NetIpcamMisc();
		getMisc(misc);
	}

	private void setCameraCheckBoxValue(){
		//---------设置黑白模式设置-------------
		if(BWModeRet == 0){					//手动调节
			Log.i("set", "手动调节黑白模式");
			if(firstColorRet == 0){			//彩色
				Log.i("set", "彩色模式");
				mColorChange.setChecked(true);
			}else if(firstColorRet == 1){	//黑白
				Log.i("set", "黑白模式");
				mColorChange.setChecked(false);
			}
		}else if(BWModeRet == 1){			//自动调节
			Log.i("set", "自动调节黑白模式");
			mColorChange.setChecked(true);	//彩色
		}
		//---------获取gpio状态 设置红外激光状态-------------
		if(laserRet == 1){
			Log.i("set", "红外激光打开 低功率");
			mLaserSet.setChecked(true);		//红外激光打开
			mLasetPower.setChecked(false);	//红外激光低功率
			mLasetPower.setEnabled(true);
			mLayoutChangePower.setVisibility(View.VISIBLE);
		}else if(laserRet == 2){
			Log.i("set", "红外激光打开 高功率");
			mLaserSet.setChecked(true);		//红外激光打开
			mLasetPower.setChecked(true);	//红外激光高功率
			mLasetPower.setEnabled(true);
			mLayoutChangePower.setVisibility(View.VISIBLE);
		}else if(laserRet == 3){			
			Log.i("set", "红外激光关闭");
			mLaserSet.setChecked(false);	//红外激光关闭
			mLasetPower.setEnabled(false);
			mLayoutChangePower.setVisibility(View.INVISIBLE);
		}
		//---------设置移动侦测状态-------------
		if(VMDRet == 1){
			Log.i("set", "移动侦测打开");
			mMoveDetect.setChecked(true);	//移动侦测打开
		}else if(VMDRet == 0){
			Log.i("set", "移动侦测关闭");
			mMoveDetect.setChecked(false);	//移动侦测关闭
		}
		//---------设置最低增益 慢速快门状态-------------
		if(misc.enable_lowest_shutter == 0){//慢速快门为自动状态
			Log.i("set", "慢速快门自动状态");
			spShutter.setVisibility(View.INVISIBLE);
			cbShutter.setChecked(false);
		}else{								//慢速快门为手动状态
			Log.i("set", "慢速快门自动状态");
			spShutter.setVisibility(View.VISIBLE);
			Log.i("set", "慢速快门shutter："+misc.shutter);
			switch (misc.shutter) {
			case 1:	//1/2 sec
				spShutter.setSelection(1, true);
				break;
			case 3:	//1/4 sec
				spShutter.setSelection(2, true);
				break;
			case 5:	//1/7 sec
				spShutter.setSelection(3, true);
				break;
			case 7:	//1/12.5 sec
				spShutter.setSelection(4, true);
				break;
			default:
				spShutter.setVisibility(View.INVISIBLE);
				cbShutter.setChecked(false);
				break;
			}
			cbShutter.setChecked(true);
		}
		if(misc.enable_uppest_agc == 0){	//增益调节为自动状态
			Log.i("set", "增益调节自动状态");
			cbGain.setChecked(false);
		}else{								//增益调节为手动状态
			Log.i("set", "增益调节手动状态");
			Log.i("set", "慢速快门agc_upper_limit："+misc.agc_upper_limit);
			if(misc.agc_upper_limit <= 20 && misc.agc_upper_limit >= 5){
				Log.i("set", "慢速快门-低");
				cbGain.setChecked(false);
			}else if(misc.agc_upper_limit > 20 && misc.agc_upper_limit <= 36){
				Log.i("set", "慢速快门-高");
				cbGain.setChecked(true);
			}
		}
	}

	//设置默认属性值
	private void setCheckBoxValue(){
		//开机默认彩色
		mColorChange.setChecked(true);
		//开机默认辅助调焦正常
		mScale.setChecked(false);
		//红外极光关闭
		mLaserSet.setChecked(false);
		//入侵警报关闭
		mMoveDetect.setChecked(false);
		//增益调节设低
		cbGain.setChecked(false);
		cbShutter.setChecked(false);
		spShutter.setVisibility(View.INVISIBLE);
	}

	private void settings(){
		//开机默认彩色
		setColorMode();
		//红外极光关闭
		setLaserIrradiateOff();
		//获取报警边界row,col
		getVMDMode();
		//入侵警报关闭
		stopVMD();
		//增益调节设低
		setMisc(1 << 8, 0 , 0 , 1 , 20);
	}

	/**
	 * 
	 * @param number 循环次数
	 */
	private void playSound(int number) {
		AudioManager am = (AudioManager) getSystemService(this.AUDIO_SERVICE);// 实例化
		float audioMaxVolum = am.getStreamMaxVolume(AudioManager.STREAM_MUSIC);// 音效最大值
		float audioCurrentVolum = am.getStreamVolume(AudioManager.STREAM_MUSIC);
		float audioRatio = audioCurrentVolum / audioMaxVolum;
		sp.play(soundId, 
				audioRatio,// 左声道音量
				audioRatio,// 右声道音量
				1, // 优先级
				number,// 循环播放次数
				1);// 回放速度，该值在0.5-2.0之间 1为正常速度
	}

	private int display(int isPlayBack){
		return JniUtil.nativeDisplay(isPlayBack, this);
	}
	public static void quit(){
		JniUtil.nativeQuit();
	}
	public void nativeAudioInit(){
		JniUtil.nativeAudioInit(this);
	}
	private int setTime(int year,int month,int day,int hour,int minute,int second){
		return JniUtil.nativeSetTime(year, month, day, hour, minute, second);
	}
	private int startRecord(){
		return JniUtil.nativeStartRecord();
	}
	public static int stopRecord(){
		return JniUtil.nativeStopRecord();
	}
	private void startVMD(){
		JniUtil.nativeStartVMD();
	}
	private void stopVMD(){
		JniUtil.nativeStopVMD();
	}
	private void setAutoColorChangeMode(){
		JniUtil.nativeSetAutoColorChangeMode();
	}
	private void setManualColorChangeMode(){
		JniUtil.nativeSetManualColorChangeMode();
	}
	private void setBlackMode(){
		JniUtil.nativeSetBlackMode();
	}
	private void setColorMode(){
		JniUtil.nativeSetColorMode();
	}
	private void setHighPower(){
		JniUtil.nativeSetHighPower();
	}
	private void setLowPower(){
		JniUtil.nativeSetLowPower();
	}
	private void setLaserIrradiateOn(){
		JniUtil.nativeSetLaserIrradiateOn();
	}
	private void setLaserIrradiateOff(){
		JniUtil.nativeSetLaserIrradiateOff();
	}
	private int getBlackWhiteMode(){
		return JniUtil.nativeGetBlackWhiteMode();
	}
	private int getFirstColorMode(){
		return JniUtil.nativeGetFirstColorMode();
	}
	private int getLaserIrradiateMode(){
		return JniUtil.nativeGetLaserIrradiateMode();
	}	//获取红外极光照明开关状态
	private  int getVMDMode(){
		return JniUtil.nativeGetVMDMode();
	}			//获取移动侦测报警边界值和开关状态
	private int setMisc(int flag,int enable_lowest_shutter ,int shutter ,int enable_uppest_agc ,int agc_upper_limit){
		return JniUtil.nativeSetMisc(flag, enable_lowest_shutter, shutter, enable_uppest_agc, agc_upper_limit);
	}
	private int getMisc(NetIpcamMisc misc){
		return JniUtil.nativeGetMisc(misc);
	}
	private void setIP(String ip){
		JniUtil.nativeSetIP(ip);
	}
	private void makeAlarmSound(){
		System.out.println("AlarmNum = "+alarmManager.getAlarmNum());
		if(alarmManager.getAlarmNum() == 0){
			try {
				System.out.println("11111");
				if(thread != null){
					System.out.println("22222");
					thread.join();
				}
				System.out.println("333333");
				thread = null;
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		System.out.println("444444");
		if(thread == null){
			System.out.println("55555");
			thread = new ClearAlarmNumThread();
			thread.start();
		}
		alarmManager.addAlarmNum();
		System.out.println("AlarmNum ++ = "+alarmManager.getAlarmNum());
		System.out.println("ALARM!!!!!!!!!!!!!");
		if(alarmManager.getAlarmNum() <= 3){
			playSound(0);
		}

	}

	class ClearAlarmNumThread extends Thread{
		@Override
		public void run() {
			// TODO Auto-generated method stub
			super.run();
			System.out.println("thread1111");
			try {
				Thread.sleep(10 * 1000); //30秒
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			System.out.println("thread2222");
			if(alarmManager != null)
				alarmManager.clearAlarmNum();
		}
	}

	class AlarmNumManager{
		private int alarmNum;
		public AlarmNumManager() {
			// TODO Auto-generated constructor stub
			alarmNum = 0;
		}

		public synchronized int getAlarmNum() {
			return alarmNum;
		}

		public synchronized void addAlarmNum() {
			this.alarmNum = alarmNum + 1;
		}

		public synchronized void clearAlarmNum(){
			alarmNum = 0;
		}
	}

	class VMDThread extends Thread{
		private boolean flag;
		public VMDThread(boolean flag) {
			// TODO Auto-generated constructor stub
			this.flag = flag;
		}
		@Override
		public void run() {
			// TODO Auto-generated method stub
			super.run();
			if(flag){
				startVMD();
			}else{
				stopVMD();
			}

		}
	}

	private void audioInit() {
		// TODO Auto-generated method stub
		int buffer_size = AudioTrack.getMinBufferSize(8000, AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT);
		mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, 8000, AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT, buffer_size*8, AudioTrack.MODE_STREAM);
		mAudioData = new byte[buffer_size*8];

		nativeAudioInit();

		//Log.d("play","audio buffer size"+buffer_size);
		mAudioTrack.play();
	}

	public static void audioStop(){
		//mAudioTrack.flush();
		//mAudioTrack.pause();
		mAudioTrack.stop();
		mAudioTrack.release();
	}

	public void audioWrite() {
		//		Log.d("audio","audio data len: "+mAudioDataLength);
		//		for (int i=0; i<10; i++) {
		//			Log.d("audio","data "+i+" is "+mAudioData[i]);
		//		}
		mAudioTrack.write(mAudioData,0,mAudioDataLength);
	}


	@Override
	protected void onPause() {
		Log.e("", "onPause");
		mPausing = true;
		this.mGlView.onPause();
		super.onPause();
	}

	public class MyTask extends AsyncTask<Void, Integer, Void> {

		@Override
		protected Void doInBackground(Void... params) {
			// TODO Auto-generated method stub
			System.out.println("call doInBackground");
			try{
				quit();
				audioStop();
				YV12Renderer.nativeDeinit();
				finish();
			}catch (Exception e) {
				// TODO: handle exception
			}
			return null;
		}
	}

	@Override
	protected void onDestroy() {
		Log.e("", "onDestroy");
		super.onDestroy();
		System.runFinalization();
		sp.release();
		MyTask mTask = new MyTask();
		mTask.execute();
	}

	@Override
	protected void onResume() {
		Log.e("", "onResume");
		mPausing = false;
		mGlView.onResume();
		super.onResume();
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		// TODO Auto-generated method stub

	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		// TODO Auto-generated method stub

	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		// TODO Auto-generated method stub

	}

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		switch (v.getId()) {
		//彩色黑白切换
		case R.id.cb_color_change :
			if(mColorChange.isChecked()){
				setColorMode();
			}else{
				setBlackMode();
			}
			break;
			//辅助调焦
		case R.id.cb_scale:
			ViewGroup.LayoutParams lp = (ViewGroup.LayoutParams) mGlView.getLayoutParams();
			if(mScale.isChecked()){
				lp.height = PhoneConfig.getPhoneHeight(HWCameraActivity.this) * 2;
				lp.width = PhoneConfig.getPhoneWidth(HWCameraActivity.this) * 2;
			}else{
				lp.height = PhoneConfig.getPhoneHeight(HWCameraActivity.this);
				lp.width = PhoneConfig.getPhoneWidth(HWCameraActivity.this);
			}
			mGlView.setLayoutParams(lp);
			break;
			//红外激光开关
		case R.id.cb_laser_set:
			if(mLaserSet.isChecked()){
				mLasetPower.setEnabled(true);
				mLayoutChangePower.setVisibility(View.VISIBLE);
				setLaserIrradiateOn();
				setLowPower();
				mColorChange.setChecked(false);
				setBlackMode();
			}else{
				mLasetPower.setChecked(false);
				mLasetPower.setEnabled(false);
				mLayoutChangePower.setVisibility(View.INVISIBLE);
				setLaserIrradiateOff();

				mColorChange.setChecked(true);
				setColorMode();
			}
			break;
			//红外极光强度
		case R.id.cb_laser_power:
			if(mLasetPower.isChecked()){
				setHighPower();
			}else{
				setLowPower();
			}
			break;
			//入侵报警开关
		case R.id.cb_move_detect:
			if(mMoveDetect.isChecked()){
				VMDThread thread = new VMDThread(true);
				thread.start();
			}else{
				VMDThread thread = new VMDThread(false);
				thread.start();
			}
			break;
			//增益调节
		case R.id.cb_gain:
			if(cbGain.isChecked()){
				setMisc(1 << 8, 0 , 0 , 1 , 36);
			}else{
				setMisc(1 << 8, 0 , 0 , 1 , 20);
				//setMisc(1 << 8, 0 , 0 , 0 , 0);
			}
			break;
		case R.id.cb_shutter:
			if(cbShutter.isChecked()){
				spShutter.setVisibility(View.VISIBLE);
				spShutter.performClick();
				spShutter.setSelection(0,true);
				cbGain.setChecked(true);
				setMisc(1 << 8, 0 , 0 , 1 , 36);
			}else{
				spShutter.setVisibility(View.INVISIBLE);
				setMisc(1 << 1, 0 , 0 , 0 , 0);
				cbGain.setChecked(false);
				setMisc(1 << 8, 0 , 0 , 1 , 20);
			}
			break;
			//		case R.id.cb_gain:
			//			if(cbGain.isChecked()){
			//				spGain.setEnabled(true);
			//			}else{
			//				spGain.setEnabled(false);
			//				setMisc(1 << 8, 0 , 0 , 0 , 0);
			//			}
			//			break;
		default:
			break;
		}
	}
}