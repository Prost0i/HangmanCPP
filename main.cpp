#include "hangman_frames.h"
#include "secret_word.h"
#include "console.h"

#include <thread>
#include <chrono>

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

const int ATTEMPTS_COUNT = 6;
static_assert(ATTEMPTS_COUNT+1 == ARRAY_SIZE(hangman_frames));

void main_loop(Console *console) {
	std::string frame;
	frame.reserve(200);

	Console::Key key_type = Console::Key::Unknown;
	char key_char = 0;

	SecretWord sword("hello");
	int attempts = 0;

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
			if(!sword.guess_char(key_char) && attempts < 6) {
				++attempts;
			}
		}

		frame.append(hangman_frames[attempts]);
		frame.append("\r\n\r\n");
		frame.append(sword.return_word_status());
		frame.append("\r\n\r\n");
		frame.append(sword.not_guessed_chars_status());
		frame.append("\r\n\r\n");

		console->clear();
		console->write(frame.c_str());

		if (sword.is_all_chars_are_guessed()) {
			frame.append("\r\nYou win!!!\r\n");
			console->clear();
			console->write(frame.c_str());

			run = false;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		if (attempts >= ATTEMPTS_COUNT) {
			frame.append("\r\nYou lose!!!\r\n");
			console->clear();
			console->write(frame.c_str());

			run = false;
			std::this_thread::sleep_for(std::chrono::seconds(1));
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
