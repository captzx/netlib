#pragma once

#include <string>

class Config {
	virtual int LoadConfig(std::string) = 0;
};