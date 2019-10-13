#pragma once
#include <set>
#include <string>
#include "../..//common/kdvtype.h"

class DemoAppData {
public:
	struct ClientInfo {
		s32 clientNode;
		std::string userName;
		/*bool operator<(const ClientInfo& clientInfo)
		{
			return this->clientNode < clientInfo.clientNode;
		}*/
	};

	DemoAppData();
private:
	std::set<ClientInfo> clientList;
};