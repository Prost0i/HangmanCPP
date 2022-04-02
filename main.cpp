#include "hangman_status.h"
#include "console.h"

#include <string>

void main_loop(Console *console) {
	std::string frame;
	frame.reserve(200);

	Console::Key key_type = Console::Key::Unknown;
	HangmanStatus hstatus;
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
				case 'a':
				case 'b':
					hstatus.increase_counter();
					break;
				case 'q':
					run = false;
					break;
			}
		}

		frame.append(hstatus.return_state());

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
