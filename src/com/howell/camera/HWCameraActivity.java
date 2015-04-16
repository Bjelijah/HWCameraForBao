package com.howell.camera;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.SoundPool;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
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
import android.widget.TextView;
import android.widget.Toast;

import com.howell.play.YV12Renderer;

public class HWCameraActivity extends Activity implements Callback,OnClickListener{
    /** Called when the activity is first created. */
	private GLSurfaceView mGlView;
	private static HWCameraActivity mPlayer;
	
	private int backCount;
	private boolean mPausing;
	private boolean showSurfaceSet;
	
	private ScrollView mSurfaceSet;
	private LinearLayout mLayoutChangeColor,mLayoutChangePower;
	private CheckBox mAutoChange,mColorChange,mLaserSet,mLasetPower,mRecord,mMoveDetect,mScale;
	private TextView mTVColor,mTVWhite,mTVHighPower,mTVLowPower,mTVOff,mTVOn;
//	private TextView mSDSpace;
//	private static ProgressBar mProgressBar;
//	private Spinner sp;
	private Spinner spShutter,spGain;
	private ArrayAdapter<String> shutterAdapter,gainAdapter;  
	private String[] shutterArray,gainArray;
	private NetIpcamMisc misc ;
	private boolean spShutterFirstSelectFlag,spGainFirstSelectFlag;
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
	
	//private static final int AutoChangeSetChecked = 1;
	//private static final int AutoChangeSetUnChecked = 2;
	//private static final int LaserSetChecked = 3;
	//private static final int LaserSetUnChecked = 4;
	
	static {
		System.loadLibrary("hwplay");
        System.loadLibrary("player_jni");
    }
	
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
		
		mPlayer = this;
		backCount = 0;
		mPausing = false;
		showSurfaceSet = false;
		spShutterFirstSelectFlag = false;
		spGainFirstSelectFlag = false;
		
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
//		mSDSpace = (TextView)findViewById(R.id.sd_space);
//		mProgressBar = (ProgressBar)findViewById(R.id.progressBar);
		mTVColor = (TextView)findViewById(R.id.tv_color);
		mTVWhite = (TextView)findViewById(R.id.tv_white);
		mTVHighPower = (TextView)findViewById(R.id.tv_high_power);
		mTVLowPower = (TextView)findViewById(R.id.tv_low_power);
		mTVOn = (TextView)findViewById(R.id.tv_switch_on);
		mTVOff = (TextView)findViewById(R.id.tv_switch_off);
		
		mLayoutChangeColor = (LinearLayout)findViewById(R.id.ll_color_change);
		mLayoutChangePower = (LinearLayout)findViewById(R.id.ll_change_power);
		
		mAutoChange = (CheckBox)findViewById(R.id.cb_auto_change);
		mColorChange = (CheckBox)findViewById(R.id.cb_color_change);
		mLaserSet = (CheckBox)findViewById(R.id.cb_laser_set);
		mLasetPower = (CheckBox)findViewById(R.id.cb_laser_power);
		mMoveDetect = (CheckBox)findViewById(R.id.cb_move_detect);
		cbShutter = (CheckBox)findViewById(R.id.cb_shutter);
		cbGain = (CheckBox)findViewById(R.id.cb_gain);
		cbShutter.setOnClickListener(this);
		cbGain.setOnClickListener(this);
		spShutter = (Spinner)findViewById(R.id.spinner_shutter);
		shutterArray = getResources().getStringArray(R.array.shutter_arry);
		shutterAdapter = new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item,shutterArray);
		shutterAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item); 
		spShutter.setAdapter(shutterAdapter);  
		//进止程序启动时spinner调用OnItemSelectedListener
		//spShutter.setSelection(0, true);
		spShutter.setOnItemSelectedListener(new Spinner.OnItemSelectedListener(){  
	        @Override  
	        public void onItemSelected(AdapterView<?> arg0, View arg1,  
	                  int arg2, long arg3) {  
	        	System.out.println("spShutterFirstSelectFlag:"+spShutterFirstSelectFlag);
	        	if(!spShutterFirstSelectFlag){
	        		spShutterFirstSelectFlag = true;
	        		return;
	        	}
	        	System.out.println("OnItemSelectedListener"+"你选择了："+ shutterArray[(int)arg2]);
//	            Toast.makeText(getApplicationContext(),   
//	                    "你选择了："+ shutterArray[(int)arg2], 1).show();  
	            setMisc(1 << 1, 1 ,(int)arg2,0,0);
	            //arg0.setVisibility(View.VISIBLE);  
	        }  
	        @Override  
	        public void onNothingSelected(AdapterView<?> arg0) { 
	          	
	        }             
	    });  
		
		spGain = (Spinner)findViewById(R.id.spinner_gain);
		gainArray = new String[32];
		for(int i=0 ; i <= 31 ;i++){
			gainArray[i] = String.valueOf(i+5);
		}
		gainAdapter = new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item,gainArray);
		gainAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item); 
		spGain.setAdapter(gainAdapter);  
		//进止程序启动时spinner调用OnItemSelectedListener
		//spGain.setSelection(0, true);
		spGain.setOnItemSelectedListener(new Spinner.OnItemSelectedListener(){  
	        @Override  
	        public void onItemSelected(AdapterView<?> arg0, View arg1,  
	                  int arg2, long arg3) {  
	        	System.out.println("spGainFirstSelectFlag:"+spGainFirstSelectFlag);
	        	if(!spGainFirstSelectFlag){
	        		spGainFirstSelectFlag = true;
	        		return;
	        	}
	        	System.out.println("OnItemSelectedListener"+"你选择了："+ gainArray[(int)arg2]);
//	            Toast.makeText(getApplicationContext(),   
//	                    "你选择了："+ gainArray[(int)arg2], 1).show();  
	            setMisc(1 << 8, 0 , 0 , 1 , Integer.valueOf(gainArray[(int)arg2]));
	            //arg0.setVisibility(View.VISIBLE);  
	        }  
	        @Override  
	        public void onNothingSelected(AdapterView<?> arg0) { 
	          	
	        }             
	    });  
		
		//CheckBox:��ɫ�ڰ��ֶ��Զ��л�
		mAutoChange.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				if(mAutoChange.isChecked()){
					//--------�ֶ���ɫ�ڰ��л�--------
//					autoChange = true;
					//��ɫ�ڰ����ñ��δѡȡ�����ɰ��£���ɫ�ڰ���������
					mColorChange.setChecked(false);
					mColorChange.setEnabled(false);
					mLayoutChangeColor.setVisibility(View.INVISIBLE);
//					myHandler.sendEmptyMessage(AutoChangeSetChecked);
					setAutoColorChangeMode();
				}else{
					//--------�Զ���ɫ�ڰ��л�--------
					//ɫ�����ÿ��԰���
					mColorChange.setEnabled(true);
					mLayoutChangeColor.setVisibility(View.VISIBLE);
//					myHandler.sendEmptyMessage(AutoChangeSetUnChecked);
					setManualColorChangeMode();
				}
			}
		});
		//CheckBox:��ɫ�ڰ���ɫ�л�
		mColorChange.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				if(mColorChange.isChecked()){
					//--------��ɫģʽ--------
					setColorMode();
				}else{
					//--------�ڰ�ģʽ--------
					setBlackMode();
				}
			}
		});
		
		mLaserSet.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				if(mLaserSet.isChecked()){
					//--------���⼤�⿪��--------	
					//���⹦�ʰ�ť��ɿɰ��£��ߵ͹���������ʾ
					mLasetPower.setEnabled(true);
					mLayoutChangePower.setVisibility(View.VISIBLE);
//						myHandler.sendEmptyMessage(LaserSetChecked);
					setLaserIrradiateOn();
						
					//���ó��ֶ����ںڰ�ģʽ
					mAutoChange.setChecked(false);
					mColorChange.setChecked(false);
					setBlackMode();
//					mTVHighPower.setTextColor(getResources().getColor(R.color.red));
//					mTVLowPower.setTextColor(getResources().getColor(R.color.white));
				}else{
					//--------���⼤��ر�--------
//					laseroff = true;
					mLasetPower.setChecked(false);
					mLasetPower.setEnabled(false);
					mLayoutChangePower.setVisibility(View.INVISIBLE);
//						myHandler.sendEmptyMessage(LaserSetUnChecked);
					setLaserIrradiateOff();
//						laseroff = false;
//						mTVHighPower.setTextColor(color.darker_gray);
//						mTVLowPower.setTextColor(color.darker_gray);
				}
			}
		});
		
		mLasetPower.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				if(mLasetPower.isChecked()){
					//--------�߹���-----------
					setHighPower();
				}else{
					//--------�͹���-----------
					setLowPower();
				}
			}
		});
//			}
//		});
		//¼���Ȳ�ʵ��
//		mRecord.setEnabled(false);
		
		//CheckBox:�ƶ���⿪��
		mMoveDetect.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				if(mMoveDetect.isChecked()){
					//--------�ƶ���⿪��--------
					//startVMD();
					VMDThread thread = new VMDThread(true);
					thread.start();
				}else{
					//--------�ƶ����ر�--------
					//stopVMD();
					VMDThread thread = new VMDThread(false);
					thread.start();
				}
			}
		});
		
		mScale = (CheckBox)findViewById(R.id.cb_scale);
		mScale.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				ViewGroup.LayoutParams lp = (ViewGroup.LayoutParams) mGlView.getLayoutParams();
				if(mScale.isChecked()){
					lp.height = PhoneConfig.getPhoneHeight(HWCameraActivity.this) * 2;
				    lp.width = PhoneConfig.getPhoneWidth(HWCameraActivity.this) * 2;
				}else{
					lp.height = PhoneConfig.getPhoneHeight(HWCameraActivity.this);
				    lp.width = PhoneConfig.getPhoneWidth(HWCameraActivity.this);
				}
				 mGlView.setLayoutParams(lp);
			}
		});
		
		sp = new SoundPool(1,AudioManager.STREAM_MUSIC, 0);  
		soundId = sp.load(this, R.raw.alarm, 1);
		System.out.println("soundId:"+soundId);
        try{
	        audioInit();
	        display(0);
        }catch (Exception e) {
			// TODO: handle exception
        	MessageUtiles.postNewUIDialog(getContext(), getContext().getString(R.string.link_error), getContext().getString(R.string.ok), 1);
		}
        settings();

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
    
    private int setTimeRet,BWModeRet,firstColorRet,laserRet,VMDRet;
    private void settings(){
		//��ȡ�ڰ�ģʽ��ʼ״̬
		BWModeRet = setBlackWhiteMode();
		if(BWModeRet == 0){
		    //�ֶ��ڰ�ģʽ
		    mAutoChange.setChecked(false);
		    System.out.println("�ֶ�ģʽ");
		    //��ȡ�ڰײ�ɫ��ʼ״̬
			firstColorRet = setFirstColorMode();
			System.out.println("��ɫ�ڰ�"+firstColorRet);
			if(firstColorRet == 0){
			    //��ɫ
			    mColorChange.setChecked(true);
			    System.out.println("��ɫ");
			}else if(firstColorRet == 1){
			    //�ڰ�
			    mColorChange.setChecked(false);
			    System.out.println("�ڰ�");
			}
		}else if(BWModeRet == 1){
		    //�Զ��ڰ�ģʽ
		    mAutoChange.setChecked(true);
		    mColorChange.setEnabled(false);
		    mLayoutChangeColor.setVisibility(View.INVISIBLE);
		    System.out.println("�Զ�ģʽ");
		}
		//��ȡ���⼤��ģʽ��ʼ״̬
		laserRet = setLaserIrradiateMode();
		if(laserRet == 1){
		    //�͹���
		    mLaserSet.setChecked(true);
		    mLasetPower.setChecked(false);
		    		
		    mLasetPower.setEnabled(true);
			mLayoutChangePower.setVisibility(View.VISIBLE);
		    System.out.println("�͹���");
		}else if(laserRet == 2){
		    //�߹���
		    mLaserSet.setChecked(true);
		    mLasetPower.setChecked(true);
		    System.out.println("11111111");
		    mLasetPower.setEnabled(true);
			mLayoutChangePower.setVisibility(View.VISIBLE);
			System.out.println("222222222");
		    System.out.println("�߹���");
		}else if(laserRet == 3){
		    //�ر�
		    mLaserSet.setChecked(false);
		    mLasetPower.setEnabled(false);
			mLayoutChangePower.setVisibility(View.INVISIBLE);
			System.out.println("����ر�");
		}
		//��ȡ�ƶ�����ʼ״̬
		VMDRet = setVMDMode();
		if(VMDRet == 1){
		    //�ƶ���⿪��
		    mMoveDetect.setChecked(true);
		    System.out.println("�ƶ���⿪��");
		}else if(VMDRet == 0){
		    //�ƶ����ر�
		    mMoveDetect.setChecked(false);
		    System.out.println("�ƶ����ر�");
		}
		misc = new NetIpcamMisc();
		getMisc(misc);
		System.out.println(misc.toString());
		if(misc.enable_lowest_shutter == 0){
			spShutter.setEnabled(false);
			cbShutter.setChecked(false);
		}else{
			spShutter.setEnabled(true);
			if(misc.shutter >= 0 && misc.shutter <= 19){
				spShutter.setSelection(misc.shutter, true);
			}
			cbShutter.setChecked(true);
		}
		if(misc.enable_uppest_agc == 0){
			spGain.setEnabled(false);
			cbGain.setChecked(false);
		}else{
			spGain.setEnabled(true);
			for(int i = 0 ; i < gainArray.length ; i++){
				if(gainArray[i].equals(String.valueOf(misc.agc_upper_limit))){
					spGain.setSelection(i, true);
					break;
				}
			}
			cbGain.setChecked(true);
		}
//		isSetting =false;
    }
    
    public static Context getContext() {
        return mPlayer;
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
    
    private native void display(int isPlayBack);
    public static native void quit();
	public native void nativeAudioInit();
	private native int setTime(int year,int month,int day,int hour,int minute,int second);
	private native int startRecord();
	public static native int stopRecord();
	private native void startVMD();
	private native void stopVMD();
	private native void setAutoColorChangeMode();
	private native void setManualColorChangeMode();
	private native void setBlackMode();
	private native void setColorMode();
	private native void setHighPower();
	private native void setLowPower();
	private native void setLaserIrradiateOn();
	private native void setLaserIrradiateOff();
	private native int setBlackWhiteMode();
	private native int setFirstColorMode();
	private native int setLaserIrradiateMode();
	private native int setVMDMode();
	private native int setMisc(int flag,int enable_lowest_shutter ,int shutter ,int enable_uppest_agc ,int agc_upper_limit);
	private native int getMisc(NetIpcamMisc misc);
	
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
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		super.onKeyDown(keyCode, event);
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			Log.e("backCount", "backCount:"+backCount);
			if (backCount == 0) {
				MyTask mTask = new MyTask();
				mTask.execute();
			}
			System.out.println(backCount);
			backCount++;
		}
		return false;
	}

	@Override
	protected void onPause() {
		Log.e("", "onPause");
		mPausing = true;
		this.mGlView.onPause();
		super.onPause();
//		if (backCount == 0) {
//			MyTask mTask = new MyTask();
//			mTask.execute();
//		}
//		System.out.println(backCount);
//		backCount++;
		//finish();
	}

	@Override
	protected void onDestroy() {
		Log.e("", "onDestroy");
		super.onDestroy();
		System.runFinalization();
		sp.release();
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
		case R.id.cb_shutter:
			if(cbShutter.isChecked()){
				spShutter.setEnabled(true);
			}else{
				spShutter.setEnabled(false);
				setMisc(1 << 1, 0 , 0 , 0 , 0);
			}
			break;
		case R.id.cb_gain:
			if(cbGain.isChecked()){
				spGain.setEnabled(true);
			}else{
				spGain.setEnabled(false);
				setMisc(1 << 8, 0 , 0 , 0 , 0);
			}
			break;
		default:
			break;
		}
	}
}