#include "hangman_status.h"
#include "secret_word.h"
#include "console.h"

void main_loop(Console *console) {
	std::string frame;
	frame.reserve(200);

	Console::Key key_type = Console::Key::Unknown;
	char key_char = 0;

	HangmanStatus hstatus;
	SecretWord sword("hello");

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
			if(!sword.guess_char(key_char)) {
				hstatus.increase_counter();
			}
		}

		frame.append(hstatus.return_state());
		frame.append("\r\n\r\n");
		frame.append(sword.return_word_status());
		frame.append("\r\n\r\n");
		frame.append(sword.not_guessed_chars_status());
		frame.append("\r\n\r\n");

		console->clear();
		console->write(frame.c_str());

		if (sword.is_all_chars_are_guessed()) {
			run = false;
		}
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
