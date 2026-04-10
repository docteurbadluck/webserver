#include "SessionHandlerUC.hpp"
#include "ISessionStorage.hpp"
#include "ISessionPolicy.hpp"
#include "parsed_request.hpp"
#include <iostream>

void	SessionHandlerUC::add_session_from_request(t_parsed_request &new_request)
{
	find_session_id_in_request(new_request);
	if (new_request.were_cookies_sent)
	{
		add_session(transform_request_into_session(new_request));
	}
}

void	SessionHandlerUC::find_session_id_in_request(t_parsed_request &new_request)
{
	std::string value_session_id;

	for (std::vector<std::string>::const_iterator it = new_request.cookies.begin();
		it != new_request.cookies.end(); ++it)
	{
		if (it->find(this->policy.get_session_cookie_name().c_str()) == 0)
		{
			new_request.were_cookies_sent = true;
			std::size_t pos = (*it).find('=');
			if (pos != std::string::npos)
				new_request.session_id = (*it).substr(pos + 1);
		}
	}
}

void SessionHandlerUC::add_session(t_session_data session_data)
{
	if (this->storage.compte_current_session() < this->policy.get_max_sessions())
	{
		this->storage.store_session(session_data);
	}
	else
	{
		std::cout << "Too many concurent session, session not added" << std::endl;
	}
}

t_session_data SessionHandlerUC::transform_request_into_session(t_parsed_request &new_request)
{
	t_session_data session;

	session.session_id = new_request.session_id;
	session.creation_time = time(NULL); 
	session.last_activity = session.creation_time;
	session.is_authentificated = false;
	session.username = "no_username";
	return session;
}
