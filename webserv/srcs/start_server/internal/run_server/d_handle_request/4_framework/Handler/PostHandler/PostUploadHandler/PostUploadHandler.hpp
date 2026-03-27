#include "BaseHandler.hpp"

class PostUploadHandler : public BaseHandler
{
	public :
		PostUploadHandler(){};
		~PostUploadHandler(){};
		std::string handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler);

	private :
		void		verify_mandatory_field(const t_parsed_request &req);
		std::string build_http_response(int status_code, const std::string &body, const std::string &error_page_filepath, const std::string &filepath);
		int 		check_file_status(const std::string &file_path, const t_server_rules &server_rules);
		bool 		upload_file(const std::string &upload_folder, const std::string &file_name);
		std::string get_filename_from_path(const std::string &path);
};