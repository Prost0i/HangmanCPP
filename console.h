#ifndef CONSOLE_H__
#define CONSOLE_H__

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#elif __unix__
#include <termios.h>
#else
#error "Unknown operating system!"
#endif

#include <cstring>

class Console {
	public:
		enum class Key {
			Char,
			Escape,
			Unknown,
		};

		using KeyValue = std::pair<Console::Key, char>;

		virtual size_t write(const char *str) = 0;
		virtual void clear() = 0;
		virtual KeyValue get_key() = 0;
};

#ifdef _WIN32 // Windows

class Win32Console : public Console {
	public:
		Win32Console();
		~Win32Console();

		size_t write(const char *str) override;
	private:
		void toggle_cursor(bool show_flag);
		HANDLE win32_console_handle_;
};

// Windows

#elif __unix__ // Unix

class UnixConsole : public Console {
	public:
		UnixConsole();
		~UnixConsole();

		size_t write(const char *str) override;
		void clear() override;

		KeyValue get_key() override;
	private:
		struct termios orig_termios_;
};

// Unix

#else
#error "Unknown operating system!"
#endif

#endif // CONSOLE_H__

