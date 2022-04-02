#include "console.h"

#include <unistd.h>

#ifdef _WIN32 // Windows

Win32Console::Win32Console() :
	win32_console_handle_(CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL,
				CONSOLE_TEXTMODE_BUFFER, NULL)) {
		SetConsoleActiveScreenBuffer(win32_console_handle_);

		toggle_cursor(false);
	}

Win32Console::~Win32Console() {
	toggle_cursor(true);
	CloseHandle(win32_console_handle_);
}

size_t Win32Console::write(const char *str) {
	const COORD zero_coord = { 0, 0 };
	DWORD bytes_written = 0;
	WriteConsoleOutputCharacter(win32_console_handle_, str,
			std::strlen(str), zero_coord, &bytes_written);

	return bytes_written;
}

void Win32Console::toggle_cursor(bool show_flag) {
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursor_info;
	GetConsoleCursorInfo(out, &cursor_info);
	cursor_info.bVisible = show_flag;
	SetConsoleCursorInfo(out, &cursor_info);
}

// Windows

#elif __unix__ // Unix

UnixConsole::UnixConsole() {
	tcgetattr(STDIN_FILENO, &orig_termios_);

	struct termios raw = orig_termios_;
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	::write(STDOUT_FILENO, "\033[?1049h", 9);
	::write(STDOUT_FILENO, "\033[?25l", 6);
}

UnixConsole::~UnixConsole() {
	::write(STDOUT_FILENO, "\033[?1049l", 8);
	::write(STDOUT_FILENO, "\033[?25h", 6);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios_);
}

size_t UnixConsole::write(const char *str) {
	size_t bytes_written = 0;
	clear();
	bytes_written += ::write(STDOUT_FILENO, str, std::strlen(str));

	return bytes_written;
}

void UnixConsole::clear() {
	::write(STDOUT_FILENO, "\033[H", 3);
}

UnixConsole::KeyValue UnixConsole::get_key() {
	char c = 0;
	read(STDIN_FILENO, &c, 1);

	if (c >= 'a' && c <= 'z') {
		return std::make_pair(Key::Char, c);
	}

	if (c == '\033') {
		char seq[2] = {0};

		read(STDIN_FILENO, &seq[0], 1);
		read(STDIN_FILENO, &seq[1], 1);

		if (seq[0] == '\0') {
			return std::make_pair(Key::Escape, 0);
		}
	}

	return std::make_pair(Key::Unknown, 0);
}

// Unix

#else
#error "Unknown operating system!"
#endif
