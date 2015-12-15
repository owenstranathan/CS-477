
#include "stdafx.h"

#include "server.h"
#include <cstdio>
#include <string>
#include <algorithm>


namespace async
{
	std::string base_dir = "C:\\server\\";
	void socket_handler(cs477::net::socket sock)
	{

		// Read an http request
		auto f = cs477::net::read_http_request_async(sock).then([sock](auto f)
		{
			auto rq = f.get();
			
			// Get the path of the file requested
			std::string path = rq.url;
			// because windows
			std::replace(path.begin(), path.end(), '/', '\\'); // replace all '/' to '\\'
			path = base_dir + path;
			try {
				cs477::read_file_async(path.c_str()).then([sock](auto f) {
					std::string body = f.get();
					cs477::net::write_http_response_async(sock, make_response(200, body, "text/html"));
				});
			}
			catch (...) {
				cs477::net::write_http_response_async(sock, make_response(404, "File not found", "text/html"));
			}
			return 0;
		});
	}


	void run(const sockaddr_in &addr)
	{
		auto host = std::make_shared<cs477::net::acceptor>();
		host->listen(addr);
		printf("Wubalubadubdub!");
		printf("Go to localhost:8080/ in microsoft edge, \nyou will get a 404 since there are not files on the server");
		for (int i = 0; i < 32; i++)
		{
			host->accept_async(socket_handler);
		}

		// Just wait forever.
		cs477::promise<void> p;
		auto f = p.get_future();
		f.wait();
	}
}
