#include "BaseHandler.hpp"

class InternalErrorHandler : public BaseHandler
{
	public :
		InternalErrorHandler(){};
		~InternalErrorHandler(){};
		std::string handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler);

	private :
		void		verify_mandatory_field(const t_parsed_request &req);
		std::string build_http_response(int status_code, const std::string &body, const std::string &error_page_filepath, const std::string &filepath);
};
