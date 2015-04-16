package com.howell.camera;
/**
 * @author 霍之昊 
 *
 * 类说明
 */
public class NetIpcamMisc {
	public int flag;	/* 具体设置哪项，由位表示,以下各项按顺序从0开始 */
	public int enable_lowest_shutter; //设1 最低快门
	public int shutter;
	public int enable_uppest_agc;	//设1 最高增益
	public int agc_upper_limit;
	@Override
	public String toString() {
		return "NetIpcamMisc [flag=" + flag + ", enable_lowest_shutter="
				+ enable_lowest_shutter + ", shutter=" + shutter
				+ ", enable_uppest_agc=" + enable_uppest_agc
				+ ", agc_upper_limit=" + agc_upper_limit + "]";
	}
	
	
}
