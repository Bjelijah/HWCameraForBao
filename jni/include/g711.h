#ifndef _g711_include_h
#define _g711_include_h

int ulaw2linear(unsigned char	u_val);
int alaw2linear(unsigned char	u_val);
unsigned char linear2ulaw(int pcm_val);
int ulaw2linear(unsigned char	u_val);

unsigned char linear2alaw(int pcm_val);
#endif