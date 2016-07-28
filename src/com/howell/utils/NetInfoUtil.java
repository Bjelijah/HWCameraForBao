package com.howell.utils;


import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;

import com.howell.camera.R;

import android.content.Context;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.util.Log;

public class NetInfoUtil {

	public static String intToIp(int i) {       

		return (i & 0xFF ) + "." +       
				((i >> 8 ) & 0xFF) + "." +       
				((i >> 16 ) & 0xFF) + "." +       
				( i >> 24 & 0xFF) ;  
	}   


	public static int getWifiIp(Context context){

		WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
		if(!wifiManager.isWifiEnabled()){
			wifiManager.setWifiEnabled(true);//open wifi
		}

		WifiInfo wifiInfo = wifiManager.getConnectionInfo();
		return wifiInfo.getIpAddress();	
	} 






	public static String getLocalIpAddress(){  
		try  
		{  
			for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements();)  
			{  
				NetworkInterface intf = en.nextElement();  
				for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements();)  
				{  
					InetAddress inetAddress = enumIpAddr.nextElement();  
					if (!inetAddress.isLoopbackAddress())  
					{  
						return inetAddress.getHostAddress().toString();  
					}  
				}  
			}  
		}  
		catch (SocketException ex)  
		{  
			Log.e("WifiPreference IpAddress", ex.toString());  
		}  
		return null;  
	}  



}
