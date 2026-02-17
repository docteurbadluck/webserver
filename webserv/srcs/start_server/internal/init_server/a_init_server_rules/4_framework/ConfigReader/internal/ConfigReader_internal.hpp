    	t_server_config     new_config;
		std::string			config_content;
		unsigned long		ip_tmp;

    	void 				parse_config_line(const std::string &line);
		void				verify_pathfile_validity();
		void				verify_mandatory_fields(const t_server_config &new_config);
		t_server_config		get_config();
		t_server_config		init_configuration_struct();
		void 				init_new_config();
		
		typedef void (ConfigReader::*ConfigHandler)(const std::string&);
		std::vector<std::pair <std::string, ConfigHandler> > config_field;

		void 				init_config_field();
		void 				init_backlog(const std::string &line);
		void 				init_ip(const std::string &line);
		void 				init_port(const std::string &line);
		void 				init_error_page_filepath(const std::string &line);
		void 				init_max_size_request_body(const std::string &line);
		void 				init_list_allowed_method(const std::string &line);
		void 				init_redirection(const std::string &line);
		void				init_redirection_tmp(const std::string& value);
		void 				init_file_system_root(const std::string &line);
		void 				init_enable_directory_listing(const std::string &line);
		void 				init_default_filepath(const std::string &line);
		void				init_restrained_pathfile(const std::string &line);
		void 				init_enable_upload(const std::string &line);
		void 				init_upload_folder(const std::string &line);
		void 				init_CGI_pathfile(const std::string &line);
		void				init_client_timeout(const std::string &line);
		void				init_session_timeout(const std::string &line);
		void				init_session_cookie_name(const std::string &line);
		void				init_session_secure_only(const std::string &line);
		void				init_max_sessions(const std::string &line);

		void				verify_single_paths();
		void				verify_collection_paths();
		bool				is_valid_file(const std::string& path);
		bool				is_valid_repository(const std::string& path);
