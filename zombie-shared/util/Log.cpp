#include "LogLifetime.hpp"
#include "Log.h"

#include <chrono>

namespace {
	bool c_print_debug = true;
	bool c_print_error = true;
	bool c_print_info = true;
}

class NullBuffer : public std::streambuf {
public:
	int_type overflow(int_type ch) {
		return 0;
	}
};

class Logger {
public:
	Logger() : m_null_stream{&m_null_buffer, false} {
		m_start = std::chrono::steady_clock::now();
	} 
	Stream Error() {
		if (m_print_error) {
			return Log("ERROR");
		} else {
			return Stream(m_null_stream);
		}
	}
	Stream Debug() {
		if (m_print_debug) {
			return Log("DEBUG");
		} else {
			return Stream(m_null_stream);
		}
	}
	Stream Info() {
		if (m_print_info) {
			return Log("INFO ");
		} else {
			return Stream(m_null_stream);
		}
	}

	void LogDebug(bool should_log) {
		m_print_debug = should_log;
	}
	void LogError(bool should_log) {
		m_print_error = should_log;
	}
	void LogInfo(bool should_log) {
		m_print_info = should_log;
	}
private:
	Stream Log(const char* type) {
		auto now = std::chrono::system_clock::now();
		auto elapsed = std::chrono::steady_clock::now() - m_start;
		std::cout << type << " " << elapsed.count() / 1'000'000 << "ms: ";
		return Stream(std::cout);
	}
	std::chrono::steady_clock::time_point m_start;
	NullBuffer m_null_buffer;
	std::ostream m_null_stream;

	bool m_print_debug = true;
	bool m_print_error = true;
	bool m_print_info = true;
};

namespace {
	Logger* s_instance = nullptr;
}

void Log::Create() {
	if (!s_instance) {
		s_instance = new Logger();
	}
}

void Log::Destroy() {
	if (s_instance) {
		delete s_instance;
		s_instance = nullptr;
	}
}

Stream Log::Error() {
	return s_instance->Error();
}

Stream Log::Debug() {
	return s_instance->Debug();
}

Stream Log::Info() {
	return s_instance->Info();
}

void Log::LogDebug(bool should_log) {
	return s_instance->LogDebug(should_log);
}

void Log::LogError(bool should_log) {
	return s_instance->LogError(should_log);
}

void Log::LogInfo(bool should_log) {
	return s_instance->LogInfo(should_log);
}
