#ifndef hw_stream_include_h
#define hw_stream_include_h

#include "his_net_socket.h"
#include <string>
#include <list>
#include <boost/noncopyable.hpp>
#include "hw_server.h"
#include "stream_observer.h"
#include "stream_type.h"
#include "stream_buf.h"
#include <boost/thread.hpp> 

class stream;
class video_decode;
class audio_decode;

class video_head
{
public:
	video_head();

	~video_head();
		
	const char* data();

	int len();

	void assign(const char* buf,int len);
	
public:
	char m_buf[40];
};

class net_live_stream
	:public stream_post
	,public boost::noncopyable
{
public:
	explicit net_live_stream(server_ref server,int slot,bool is_sub);
	virtual ~net_live_stream();

	virtual bool start() = 0;
	virtual void stop() = 0;
	bool is_start();
	server_ref server();
	int slot();
	bool is_sub();
	const char* head_data();
	int  head_len();
	
protected:
	void assign_head(const char* buf,int len);
	bool get_local_ip(int s,std::vector<std::string>& ips);

protected:
	bool m_start;		
	server_ref m_server;
	int m_slot;
	bool m_is_sub;
	int m_s;
	video_head m_head;
};

class net_tcp_live_stream 
	: public net_live_stream
{
public:
	net_tcp_live_stream (server_ref server,int slot,bool is_sub);
	virtual ~net_tcp_live_stream ();

	virtual bool start();
	virtual void stop();

protected:
	void on_recv_thread();

protected:
	boost::thread m_thread;
};

class net_udp_live_stream
	:public net_live_stream
{
public:
	net_udp_live_stream(server_ref server,int slot,bool is_sub);
	virtual ~net_udp_live_stream();

	virtual bool start();
	virtual void stop();

protected:
	void on_get_udp_data();

protected:
	int m_port;	
	std::string m_ip;
	time_t m_last_heart_beat_time;
	boost::thread m_data_thread;

protected:
	static bool bind_port(int s,std::string ip,int port);
	static int g_port;
	static boost::mutex g_port_mu;
};

class net_file_stream 
	:public stream_post
	,public boost::noncopyable
{
protected:
	typedef struct  
	{		
		int beg_tm;
		int end_tm;
		int file_len;
		int record_type;		
	}time_label;
	
public:
	net_file_stream (server_ref server,int slot,SYSTEMTIME beg,SYSTEMTIME end);
	virtual	~net_file_stream();
	
	int file_size();
	SYSTEMTIME beg();
	SYSTEMTIME end();
	virtual bool start();
	virtual void stop();
	bool is_start();
	const char* head_data();
	int  head_len();

protected:	
	void assign_head(const char* buf,int len);
	void on_get_stream();

protected:
	int m_s;
	bool m_start;		
	server_ref m_server;
	int m_slot;
	SYSTEMTIME m_beg;
	SYSTEMTIME m_end;
	time_label m_label;
	boost::thread m_thread;
	video_head m_head;
};

typedef boost::shared_ptr<net_live_stream> net_live_stream_ref;
typedef boost::shared_ptr<net_file_stream> net_file_stream_ref;

#endif
