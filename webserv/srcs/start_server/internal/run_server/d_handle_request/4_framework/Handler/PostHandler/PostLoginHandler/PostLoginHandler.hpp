#include "BaseHandler.hpp"

class PostLoginHandler : public BaseHandler
{
	public :
		PostLoginHandler(){};
		~PostLoginHandler(){};
		std::string handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler);

	private :
		void		verify_mandatory_field(const t_parsed_request &req);
		std::string build_http_response(int status_code, const std::string &body, const std::string &error_page_filepath, const std::string &filepath);
		int 		update_session_from_body(const std::string &body);
		std::map<std::string,std::string> parse_urlencoded_body(const std::string &body);
		std::map<std::string,std::string> parse_json_body(const std::string &body);
};



