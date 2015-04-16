#ifndef hw_head_include_h
#define hw_head_include_h
#include "his_net_socket.h"
#include <boost/noncopyable.hpp>

#define MAX_MSG_LEN (4096)
class hw_msg
{
public:

	hw_msg();

	~hw_msg();

	int head_len();
	char* head();

	int body_len();
	char* body();

	void set_body(const char* body,int len);

	char* msg();
	int msg_len();
	
private:
	char m_buf[MAX_MSG_LEN];
};

#endif

