#pragma once

#include <iostream>

class CECPListener {
 public:
        CECPListener(std::istream& input, std::ostream& output);
	virtual ~CECPListener();

 protected:
        std::istream& input_;
        std::ostream& output_;
};
