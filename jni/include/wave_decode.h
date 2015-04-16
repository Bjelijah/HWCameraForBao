#ifndef wave_decode_include_h
#define wave_decode_include_h

#include "g711.h"
class wave_decode
{
public:
	static void decode_from_g711u(const unsigned char* src,int src_len,unsigned char* dst,int dst_len)
	{
		if(dst_len < (src_len * 2)) return ;

		short* decode_buf = (short*)dst;
		for(int i = 0; i < src_len; i++)
		{
			decode_buf[i] = ulaw2linear(src[i]);
		}
	}

	static void decode_from_g711a(const unsigned char* src,int src_len,unsigned char* dst,int dst_len)
	{
		if(dst_len < (src_len * 2)) return ;
		
		short* decode_buf = (short*)dst;
		for(int i = 0; i < src_len; i++)
		{
			decode_buf[i] = alaw2linear(src[i]);
		}
	}
};

#endif