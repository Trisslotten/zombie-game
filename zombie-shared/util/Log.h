#pragma once

#include <iostream>

/// https://stackoverflow.com/questions/49332013/adding-a-new-line-after-stdostream-output-without-explicitly-calling-it
class StreamDelegate {
public:
	StreamDelegate(std::ostream& os) : m_stream(os) {}
	~StreamDelegate() { m_stream << "\n"; }
	template <typename T>
	StreamDelegate& operator<<(T&& val) {
		m_stream << std::forward<T>(val);
		return *this;
	}
	StreamDelegate& operator=(const StreamDelegate&) = delete;
	StreamDelegate(const StreamDelegate&) = delete;
private:
	std::ostream& m_stream;
};

class Stream {
public:
	Stream(std::ostream& os) : m_stream(os) {}
	template <typename T>
	StreamDelegate operator<<(T&& val) {
		m_stream << std::forward<T>(val);
		return StreamDelegate(m_stream);
	}
private:
	std::ostream& m_stream;
};

namespace Log {
	Stream Error();
	Stream Debug();
	Stream Info();

	void LogDebug(bool should_log);
	void LogError(bool should_log);
	void LogInfo(bool should_log);
}
