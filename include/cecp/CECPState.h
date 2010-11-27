#pragma once

#include <string>

class CECPState {
public:
	CECPState();
	virtual ~CECPState();

	virtual void handleMessage(const std::string& message);
};
