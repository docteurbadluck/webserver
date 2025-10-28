#include "SessionPolicyFramework.hpp"

SessionPolicyFramework::SessionPolicyFramework(const t_server_rules &server_rules)
{
	this->session_timeout_in_minute = server_rules.session_timeout_in_minute;
	this->session_cookie_name = server_rules.session_cookie_name;
	this->session_secure_only = server_rules.session_secure_only;
	this->max_sessions = server_rules.max_sessions;
}
SessionPolicyFramework::~SessionPolicyFramework()
{}

int	SessionPolicyFramework::get_max_sessions()
{
	return max_sessions;
}

bool SessionPolicyFramework::is_this_session_expired(const s_session_data &session)
{
	std::time_t now = std::time(NULL);
	std::time_t session_timeout = session.last_activity + (this->session_timeout_in_minute * 60);
	return (now > session_timeout);
}


bool	SessionPolicyFramework::are_sessions_secure_only()
{
	return (this->session_secure_only);
}
const std::string SessionPolicyFramework::get_session_cookie_name()
{
	return (this->session_cookie_name);
}