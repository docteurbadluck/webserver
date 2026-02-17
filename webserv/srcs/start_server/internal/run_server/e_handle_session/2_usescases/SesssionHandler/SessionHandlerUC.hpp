#pragma once
#include "session_data.hpp"
#include "parsed_request.hpp"

class ISessionStorage;
class ISessionPolicy;

class SessionHandlerUC
{
	public :
		SessionHandlerUC(ISessionStorage &storage, ISessionPolicy &policy) : storage(storage), policy(policy) {};
		void					purge_expired_sessions();
		const t_session_data	&get_session(const std::string &session_id);
		void					modif_session_data(const std::string &session_id, const std::string &field, const std::string &new_value);
		bool					is_valid_session(const std::string &session_id);
		std::string 			generate_session_id() const;
		void					add_session_from_request(t_parsed_request &new_request);

		private :
		void					delete_session(const std::string &session_id);
		void					add_session(t_session_data session_data);
		void					find_session_id_in_request(t_parsed_request &new_request);
		t_session_data 			transform_request_into_session(t_parsed_request &new_request);
		ISessionStorage 		&storage;
		ISessionPolicy  		&policy;
};
