#pragma once

namespace x {

namespace tool {

class NoCopyable {
protected:
	NoCopyable() {}
	~NoCopyable() {}
private:
	NoCopyable(const NoCopyable&);
	const NoCopyable& operator=(const NoCopyable&);
};

}

}