#include "ISocketFactory.hpp"
#include "ServerSocket.hpp"

class SocketFactory : public ISocketFactory
{

	public:
	~SocketFactory()    {}
	ISocketServer*      create_server_socket();
};
