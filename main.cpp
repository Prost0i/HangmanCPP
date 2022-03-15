#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#endif

#include <iostream>
#include <chrono>
#include <thread>
#include <cstring>

class Screen {
	public:
		virtual size_t write(const char *str) = 0;
};

#ifdef _WIN32
class Win32Screen : public Screen {
	public:
		Win32Screen():
			win32_console_handle_(CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL,
						CONSOLE_TEXTMODE_BUFFER, NULL)) {
				SetConsoleActiveScreenBuffer(win32_console_handle_);

				toggle_cursor(false);
			}

		~Win32Screen() {
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
class UnixScreen : public Screen {
	public:
		UnixScreen() {
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

		~UnixScreen() {
			::write(STDOUT_FILENO, "\033[?1049l", 8);
			::write(STDOUT_FILENO, "\033[?25h", 6);
			tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios_);
		}

		size_t write(const char *str) override {
			::write(STDOUT_FILENO, "\033[H", 3);
			::write(STDOUT_FILENO, str, std::strlen(str));

			return 0;
		}

	private:
		struct termios orig_termios_;
};
#endif

void main_loop(Screen *screen) {
	screen->write("Hello, world");
	std::this_thread::sleep_for(std::chrono::seconds(1));
	screen->write("World, hello");
	std::this_thread::sleep_for(std::chrono::seconds(1));
	screen->write("Hello, hello");
	std::this_thread::sleep_for(std::chrono::seconds(1));
	screen->write("World, world");
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main() {
#ifdef _WIN32
	Win32Screen screen = Win32Screen();
#else
	UnixScreen screen = UnixScreen();
#endif
	main_loop(&screen);

	return 0;
}
