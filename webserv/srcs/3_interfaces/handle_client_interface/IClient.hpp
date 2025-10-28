#pragma once
#include <string>
#include <ctime>

class IClient
{
	public :
	virtual ~IClient() {};
	virtual int		get_fd() const = 0;

	virtual void	appendToReadBuffer(const std::string &data) = 0;
	virtual const std::string &getReadBuffer() const = 0;


	virtual	bool	delimiter_found_in_readbuffer() = 0;
	virtual void	extract_header() = 0;
	virtual void	flush_header() = 0;
	virtual void	flush_body(int body_lenght) = 0;


	virtual std::string	extract_body(int body_lenght) = 0;
	virtual std::string	&get_request() = 0;

	virtual void	update_last_activity() = 0;
	virtual bool	is_timed_out(std::time_t now, int timeout) = 0;
};