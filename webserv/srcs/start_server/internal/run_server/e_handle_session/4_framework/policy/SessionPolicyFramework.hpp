#include "ISessionPolicy.hpp"

class SessionPolicyFramework : public ISessionPolicy
{
	public :
		SessionPolicyFramework(const t_server_rules &server_rules);
		~SessionPolicyFramework();
		bool				is_this_session_expired(const s_session_data &session);
		int					get_max_sessions();
		bool				are_sessions_secure_only();
		const std::string	get_session_cookie_name();

	private :
		int					session_timeout_in_minute;
		std::string			session_cookie_name;
		bool				session_secure_only;
		int					max_sessions;
};
