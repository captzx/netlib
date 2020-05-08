#pragma once

#include <string>
#include <tinyxml2.h>

using namespace tinyxml2;

namespace x {

namespace tool {

class Config {
public:
	virtual bool Parse() = 0;

public:
	virtual bool LoadFile(std::string file) final {
		if (file.empty()) return false;

		_pXmlDoc = std::make_shared<XMLDocument>();

		auto result = _pXmlDoc->LoadFile(file.c_str());
		if (result) return false;

		_fileName = file;

		return Parse();
	}

	virtual const std::string& GetFileName() final { return _fileName; }
	virtual const std::shared_ptr<XMLDocument>& GetXmlDoc() final { return _pXmlDoc; }

private:
	std::shared_ptr<XMLDocument> _pXmlDoc;
	std::string _fileName;
};

} // namespace tool

} // namespace x

