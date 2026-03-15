#include <string>
#include "server_rules.hpp"
#include "SessionHandlerUC.hpp"

class IRequestParser;
class IRequestHandler;

class RequestHandlerUC
{
	public :
		explicit	                RequestHandlerUC(IRequestParser &request_parser, IRequestHandler &request_handler);
		                            ~RequestHandlerUC();

		std::string					handle_the_request(const t_server_rules &server_rules, SessionHandlerUC &session_handler,const std::string &headers, const std::string &body);
		void						init_request_txt(const std::string &req);
		t_parsed_request			parse_header();
		int							get_fd_stream();
		pid_t						get_cgi_pid();
		std::vector<std::string>	file_content;
		int							get_close_flag();

	private :
		IRequestParser		        &parser;
		IRequestHandler          	&handler;
};

