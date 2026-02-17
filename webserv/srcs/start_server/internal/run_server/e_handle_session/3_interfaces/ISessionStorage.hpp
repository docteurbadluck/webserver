#pragma once 
#include "session_data.hpp"

class ISessionPolicy;

class ISessionStorage
{
	public :
		virtual	void								store_session(t_session_data session_data) = 0;
		virtual void								delete_session(const std::string &session_id) = 0;
		virtual bool								is_valid_session(const std::string &session_id) const = 0;
		virtual t_session_data& 					get_session(const std::string &session_id) = 0;
		virtual int									compte_current_session() = 0;
		virtual void								purge_expired_sessions(ISessionPolicy &policy) = 0;
		virtual std::string									generate_session_id() const = 0;
		virtual ~ISessionStorage() {};
};