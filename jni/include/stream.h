#ifndef stream_include_h
#define stream_include_h
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "frame_filt.h"
#include <string>

//获取数据抽象类
class stream_capture
	:public boost::noncopyable
{
public:
	stream_capture(frame_filt_ref frame_filt)
		:m_capturing(false),m_pausing(false) ,m_frame_filt(frame_filt)
	{		

	}

	virtual ~stream_capture()
	{		

	}

	//开始捕获
	virtual bool start_capture() = 0	;

	//停止捕获
	virtual void stop_capture() = 0;

	virtual bool is_start_capture()
	{
		return m_capturing;
	}

	//暂停/继续捕获
	virtual bool pause_capture(bool bpause) 
	{
		return false;
	}

	//是否暂停捕获
	virtual bool is_pausing()
	{
		return m_pausing;
	}

	//定位到指定位置捕获
	virtual bool locate_to(unsigned int pos )
	{
		return false;
	}

	/*获取一帧stream,上层必须不断调用,否则stream buf有可能溢出*/
	virtual bool get_one_frame_stream(char* out,unsigned int* out_len,stream_head* out_head)
	{
			if(is_start_capture())
			{
				return m_frame_filt->get_one_frame_stream(out,out_len,out_head);
			}
			return false;
	}

	/*输入数据，可以内部调用，也可以外部调用*/
	virtual bool input_stream(const char* buf,int len)
	{
		if(is_start_capture())
		{
			return m_frame_filt->input_stream(buf,len);
		}
		return false;
	}	

	/*获取当前数据长度*/
	virtual bool get_stream_len(unsigned int& len)
	{
		if(is_start_capture())
		{
			len =  m_frame_filt->get_stream_len();
			return true;
		}		
		return false;
	}

	/*获取可用空间*/
	virtual bool get_stream_free(unsigned int& len)
	{
		if(is_start_capture())
		{
			len = m_frame_filt->get_free_len();
			return true;
		}
		return false;
	}

	/*清空数据*/
	virtual void clear_stream()
	{
		m_frame_filt->clear_stream();
	}

	/*获取filt*/
	virtual bool get_frame_filt(frame_filt_ref& frame_filt)
	{
		frame_filt = m_frame_filt;
		return true;
	}

	/*获取路径*/
	virtual bool get_stream_path(std::string& path)
	{
		return false;
	}

protected:	
	frame_filt_ref m_frame_filt;
	bool m_capturing;
	bool m_pausing;
};

typedef boost::shared_ptr<stream_capture> stream_ref;

#endif