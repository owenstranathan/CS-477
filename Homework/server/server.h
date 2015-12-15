
#pragma once

#include <thread>

#include "../../external/include/cs477.h"
#include "../../external/include/queue.h"
#include "../../external/include/data.h"
#include "../../external/include/net.h"
#include "../../external/include/http.h"
#include "../../external/include/file.h"



cs477::net::http_response make_response(int status, const std::string& content, const std::string &contentType);

namespace async
{
	void run(const sockaddr_in &addr);
}

