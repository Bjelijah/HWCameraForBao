#ifndef hw_net_base_include_h
#define hw_net_base_include_h

#include "protocol_type.h"
#include "his_net_socket.h"
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <map>
#include "stream_type.h"
#include <hw_head.h>
#include <deque>

class hw_net_base
	 :public boost::noncopyable
{
public:
	hw_net_base(const char* server_ip,
			short port,
			const char* name,
			const char* log_name,
			const char* log_psw);

	virtual ~hw_net_base() = 0;

	const char* name()
	{
		return m_name.c_str();
	}

	const char* ip()
	{
		return m_ip.c_str();
	}

	const char* log_name()
	{
		return m_log_name.c_str();
	}

	const char* log_password()
	{
		return	m_log_psw.c_str();			 
	}

	short port()
	{
		return m_port;
	}

	bool connect();

	void disconnect();

	bool is_connected();

protected:
	virtual bool on_protocol_come(hw_msg& msg);

	bool send_protocol(unsigned long type,const char* data,int len);
	bool wait_net_response(unsigned long type,hw_msg& response,int timeout = 5000);
	void on_recv_thread();

protected:
	int m_s;
	std::string m_name;
	std::string m_log_name;
	std::string m_log_psw;
	std::string m_ip;
	short m_port;
	boost::thread m_thread;

	boost::mutex m_send_mu;
	boost::mutex m_response_mu;
	std::map<unsigned long ,hw_msg> m_net_responses;
};

#endif

