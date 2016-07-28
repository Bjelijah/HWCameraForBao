package com.howell.action;

import com.howell.utils.IConst;
import com.howell.utils.NetInfoUtil;

import android.content.Context;
import android.util.Log;

public class NetAction implements IConst{
	private static NetAction mInstance=null;
	public static NetAction getInstance(){
		if (mInstance==null) {
			mInstance = new NetAction();
		}
		return mInstance;
	}
	private NetAction(){}
	
	
	public  String getServiceIp(Context context){
		int bar =NetInfoUtil.getWifiIp(context);
		String ip=null;
		if(	((bar >> 16 ) & 0xFF) ==8){
			ip = WAY_IP;
		}else if(((bar >> 16 ) & 0xFF) ==42){
			ip = CAM_IP;
		}else if(((bar >> 16 ) & 0xFF) ==128){
			ip = TEST_IP;
		}
		Log.i("123", "ip="+ip);
		return ip;
	}
}
