#ifndef CONSOLE_H__
#define CONSOLE_H__

#include <iostream>

#include <termios.h>
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

#endif // CONSOLE_H__

