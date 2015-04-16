#ifndef wave_encode_include_h
#define wave_encode_include_h
#include "g711.h"

class wave_encode
{
public:
	static void encode_to_g711u(const unsigned char* src,int src_len,unsigned char* dst,int dst_len)
	{
		if(src_len != dst_len * 2) return;
		short* buf = (short*)src;

		for(int i = 0; i < dst_len; i++)
		{
			dst[i] = linear2ulaw(buf[i]);
		}
	}

	static void encode_to_g711a(const unsigned char* src,int src_len,unsigned char* dst,int dst_len)
	{
		if(src_len != dst_len * 2) return;
		short* buf = (short*)src;
		
		for(int i = 0; i < dst_len; i++)
		{
			dst[i] = linear2alaw(buf[i]);
		}
	}
};

#endif