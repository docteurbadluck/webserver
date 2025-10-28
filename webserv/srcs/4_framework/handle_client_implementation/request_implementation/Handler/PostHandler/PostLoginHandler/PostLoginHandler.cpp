#include "PostLoginHandler.hpp"

std::string PostLoginHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	verify_mandatory_field(req);
	init_field(req, server_rules, session_handler);
	this->set_cookie_flag = this->request.were_cookies_sent;
	if (set_cookie_flag == false)
	{
		return build_http_response(200, request.body, server_rules.error_page_filepath, "");
	}
	int status_code = update_session_from_body(request.body);
	return build_http_response(status_code, request.body, server_rules.error_page_filepath, "");
}

int	PostLoginHandler::update_session_from_body(const std::string &body)
{
	if (!this->session_handler)
		return 500;
	std::map<std::string, std::string> params;
	if (request.content_type == "application/x-www-form-urlencoded")
		params = parse_urlencoded_body(body);
	else if (request.content_type == "application/json")
		params = parse_json_body(body);
	else
		return 415;
	try
	{
		for (std::map<std::string,std::string>::iterator it = params.begin(); it != params.end(); ++it)
		{
			session_handler->modif_session_data(request.session_id, it->first, it->second);
		}
	}
	catch (...)
	{
		return 500;
	}
	return 200;
}

std::map<std::string,std::string> PostLoginHandler::parse_urlencoded_body(const std::string &body)
{
	std::map<std::string,std::string> params;
	size_t start = 0;
	while (start < body.size())
	{
		size_t eq_pos = body.find('=', start);
		if (eq_pos == std::string::npos)
			break;
		std::string key = body.substr(start, eq_pos - start);
		size_t amp_pos = body.find('&', eq_pos + 1);

		std::string value = (amp_pos == std::string::npos) ? body.substr(eq_pos + 1)
							: body.substr(eq_pos + 1, amp_pos - eq_pos - 1);

		params[key] = value;
		if (amp_pos == std::string::npos) 
			break;
		start = amp_pos + 1;
	}
	return params;
}

std::map<std::string,std::string> PostLoginHandler::parse_json_body(const std::string &body)
{
	std::map<std::string,std::string> params;
	size_t pos = 0;
	while (pos < body.size())
	{
		size_t key_start = body.find('"', pos);
		if (key_start == std::string::npos)
			break;
		size_t key_end = body.find('"', key_start + 1);
		if (key_end == std::string::npos)
			break;
		std::string key = body.substr(key_start + 1, key_end - key_start - 1);
		size_t colon = body.find(':', key_end);
		if (colon == std::string::npos)
			break;
		size_t value_start = body.find('"', colon);
		if (value_start == std::string::npos)
			break;
		size_t value_end = body.find('"', value_start + 1);
		if (value_end == std::string::npos)
			break;
		std::string value = body.substr(value_start + 1, value_end - value_start - 1);
		params[key] = value;
		pos = value_end + 1;
	}
	return params;
}

void PostLoginHandler::verify_mandatory_field(const t_parsed_request &req)
{
	if (req.methode != "POST")
		throw std::runtime_error("POST: Invalid method, expected POST");
}
