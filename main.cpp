#include "hangman_frames.h"
#include "secret_word.h"
#include "console.h"

#include <thread>
#include <chrono>
#include <random>
#include <fstream>
#include <sstream>

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

const int ATTEMPTS_COUNT = 6;
const double FPSMAX = 1000.0/15.0;
static_assert(ATTEMPTS_COUNT+1 == ARRAY_SIZE(hangman_frames));

std::string peak_random_word(const std::vector<std::string>& words) {
	std::mt19937 generator(std::random_device{}());
	std::uniform_int_distribution<std::size_t> distribution(0, words.size() - 1);

	return words[distribution(generator)];
}

void main_loop(Console *console, const std::vector<std::string>& words) {
	std::string frame;
	frame.reserve(200);

	Console::Key key_type = Console::Key::Unknown;
	char key_char = 0;

	SecretWord sword(peak_random_word(words).c_str());
	int attempts = 0;

	bool run = true;
	auto start_timer = std::chrono::high_resolution_clock::now();
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

		auto end_timer = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> delta = end_timer - start_timer;
		start_timer = end_timer;

		if (delta.count() < FPSMAX) {
			std::chrono::duration<double, std::milli> delta_ms(FPSMAX - delta.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}
	}
}

void split_words(std::vector<std::string> &result_words, std::string &text, const std::string &delimiter) {
	size_t pos = 0;
	while ((pos = text.find(delimiter)) != std::string::npos) {
		result_words.push_back(text.substr(0, pos));
		text.erase(0, pos + delimiter.length());
	}
}

std::string read_file_into_string(const std::string &path) {
	auto ss = std::ostringstream{};
	std::ifstream input_file(path);
	if (!input_file.is_open()) {
		std::cerr << "Could not open the file - '" << path << "'\n";
		exit(EXIT_FAILURE);
	}
	ss << input_file.rdbuf();
	return ss.str();
}

int main() {
	std::string words_file_contents = read_file_into_string("words.txt");
	std::vector<std::string> words;
	split_words(words, words_file_contents, "\n");

	UnixConsole console = UnixConsole();

	main_loop(&console, words);

	return 0;
}
