#include "IRequestHandler.hpp"
#include <map>
#include <string>
#include <stdexcept>
#include "BaseHandler.hpp"

class RouterHandler : public IRequestHandler
{
	public :
		std::string handle( const t_parsed_request &req,
							const t_server_rules & server_rules,
							SessionHandlerUC &session_handler);
		std::vector<std::string>	get_file_content();
		int get_is_big_body();
		RouterHandler();
		~RouterHandler();
	private :
		std::map <std::string, BaseHandler*> handlers;
};