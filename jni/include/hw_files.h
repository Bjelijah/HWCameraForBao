#ifndef hw_files_include_h
#define hw_files_include_h

#include <vector>
#include <string>
#include "hw_server.h"
#include "hw_net_base.h"

class hw_files
{
public:
	enum{
		FILE_ALL = 0,
		FILE_NORMAL = 1,
		FILE_MOTION = 2
	};
	hw_files(int slot,SYSTEMTIME beg,SYSTEMTIME end,int type);
	virtual ~hw_files();

	virtual bool refresh() = 0;
	int count();
	bool get_file(int index,rec_file_t& file);

protected:
	int m_slot;
	SYSTEMTIME m_beg;
	SYSTEMTIME m_end;
	int m_type;
	std::vector<rec_file_t> m_files;
};

class hw_net_files
	: public hw_files
	  ,public hw_net_base
{
public:
	hw_net_files(server_ref server, int slot,SYSTEMTIME beg,SYSTEMTIME end,int type = hw_files::FILE_ALL);
	virtual ~hw_net_files();
	
	bool refresh();

protected:
	bool on_protocol_come(hw_msg& msg);
	bool wait_file_finsished(int timeout = 5000);

protected:
	bool m_file_finished;
	server_ref m_server;
};

class hw_net_smart_files
	: public hw_net_files
{
public:
	hw_net_smart_files(server_ref server,int slot,SYSTEMTIME beg,SYSTEMTIME end,RECT search_rt);
	~hw_net_smart_files();

	bool refresh();

private:
	RECT m_search_rt;
};

typedef boost::shared_ptr<hw_files> file_list_ref;

#endif

