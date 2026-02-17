struct body_data_s
{
	int body_fd;
	std::size_t body_size;
}

class BodyInitiator
{
	void init_body_data(body_data_s &body_data, std::string path); 
}
