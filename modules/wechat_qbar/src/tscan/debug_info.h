
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>


namespace tscan{
namespace debug{

class DebugInfo
{
public:
	std::string filename_;
	
	DebugInfo(std::string filename = ""): filename_(filename) {}	
	~DebugInfo() {}

	void Reset()
	{
		filename_ = "";
	}
};

}  // tscan
}  // debug

