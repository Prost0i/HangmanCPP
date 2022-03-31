#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#endif

#include <iostream>
#include <chrono>
#include <thread>
#include <cstring>

#include "hangman_frames.h"

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

#ifdef _WIN32
class Win32Console : public Console {
	public:
		Win32Console():
			win32_console_handle_(CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL,
						CONSOLE_TEXTMODE_BUFFER, NULL)) {
				SetConsoleActiveScreenBuffer(win32_console_handle_);

				toggle_cursor(false);
			}

		~Win32Console() {
			toggle_cursor(true);
			CloseHandle(win32_console_handle_);
		}

		size_t write(const char *str) override {
			const COORD zero_coord = { 0, 0 };
			DWORD bytes_written = 0;
			WriteConsoleOutputCharacter(win32_console_handle_, str,
					std::strlen(str), zero_coord, &bytes_written);

			return bytes_written;
		}

	private:
		void toggle_cursor(bool show_flag) {
			HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
			CONSOLE_CURSOR_INFO cursor_info;
			GetConsoleCursorInfo(out, &cursor_info);
			cursor_info.bVisible = show_flag;
			SetConsoleCursorInfo(out, &cursor_info);
		}

		HANDLE win32_console_handle_;
};
#else
class UnixConsole : public Console {
	public:
		UnixConsole() {
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

		~UnixConsole() {
			::write(STDOUT_FILENO, "\033[?1049l", 8);
			::write(STDOUT_FILENO, "\033[?25h", 6);
			tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios_);
		}

		size_t write(const char *str) override {
			size_t bytes_written = 0;
			clear();
			bytes_written += ::write(STDOUT_FILENO, str, std::strlen(str));

			return bytes_written;
		}

		void clear() override {
			::write(STDOUT_FILENO, "\033[H", 3);
		}

		KeyValue get_key() override {
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

	private:
		struct termios orig_termios_;
};
#endif

void main_loop(Console *console) {
	std::string frame;
	frame.reserve(200);

	int hangman_frame_counter = 0;
	Console::Key key_type = Console::Key::Unknown;
	char key_char = 0;
	bool run = true;
	while (run) {
		frame.clear();
		Console::KeyValue key_value = console->get_key();
		key_type = key_value.first;
		key_char = key_value.second;

		if (key_type != Console::Key::Char) {
			if (key_type == Console::Key::Escape) {
				run = false;
			}
		} else {
			switch (key_char) {
				case 'q':
					run = false;
					break;
				case 'a':
					if (hangman_frame_counter > 0) {
						--hangman_frame_counter;
					}
					break;
				case 'b':
					if (hangman_frame_counter < 6) {
						++hangman_frame_counter;
					}
					break;

				default:
					break;
			}

		}

		frame.append(hangman_frames[hangman_frame_counter]);

		console->clear();
		console->write(frame.c_str());
	}
}

int main() {
#ifdef _WIN32
	Win32Console console = Win32Console();
#else
	UnixConsole console = UnixConsole();
#endif
	main_loop(&console);

	return 0;
}
