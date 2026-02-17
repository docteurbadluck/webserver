#include "parsed_request.hpp"
#include "IRequestParser.hpp"
#include <string>
#include <map>
#include <cstring>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cctype>

class RequestParser : public IRequestParser
{
	public :
		RequestParser();
		~RequestParser();
		t_parsed_request	parse_txt_request();
		void				init_request_txt(const std::string &req);

	private :
		void				attribute_field_from_txt();
		void				read_request_line();
		void				read_header_lines();
		void				trim(std::string &s);
		t_parsed_request	parsed_request;
		std::string			request_txt;

		typedef void (RequestParser::*HeaderHandler)(const std::string& value);
		// Handlers pour chaque header utile
		void handle_host(const std::string& value);
		void handle_content_length(const std::string& value);
		void handle_content_type(const std::string& value);
		void handle_connection(const std::string& value);
		void handle_cookies(const std::string& value);
		void init_header_field_map();
		void http_version_initialiser(std::istringstream &first_line);
		void parse_first_line(std::string line);
		void	attribute_value(std::string &line);
		void handle_last_cookie(const std::string &value, const size_t &start);

		std::map<std::string, HeaderHandler> header_field_map;
};
