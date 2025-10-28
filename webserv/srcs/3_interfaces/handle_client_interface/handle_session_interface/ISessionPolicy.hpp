#pragma once 
#include "session_data.hpp"

class ISessionPolicy
{
	public :
		virtual bool	is_this_session_expired(const s_session_data &session) = 0;
		virtual int		get_max_sessions() = 0;
		virtual bool	are_sessions_secure_only() = 0; //seems useless for now
		virtual const std::string get_session_cookie_name() = 0;
		virtual			~ISessionPolicy() {};
};