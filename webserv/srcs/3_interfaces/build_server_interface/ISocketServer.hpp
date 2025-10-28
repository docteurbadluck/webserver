#pragma once

class ISocketServer
{
	public :
		virtual void bind_socket(unsigned long ip, int port) = 0;
		virtual void listen_socket(int backlog) = 0;
		virtual int  get_fd() const = 0;
		virtual ~ISocketServer() {};
};
