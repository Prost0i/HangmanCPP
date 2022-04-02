#include "secret_word.h"

SecretWord::SecretWord(const char *word) : word_(word) {}

bool SecretWord::guess_char(char input_char)  {
	if (!char_in_guessed_chars(input_char)) {
		if (char_in_word_chars(input_char)) {
			guessed_chars_.push_back(input_char);
			return true;
		}
	}

	if (!char_in_not_guessed_chars(input_char) && !char_in_guessed_chars(input_char)) {
		not_guessed_chars_.push_back(input_char);
		return false;
	}

	return true;
}

bool SecretWord::is_all_chars_are_guessed() {
	for (const char c: word_) {
		if (!char_in_guessed_chars(c)) {
			return false;
		}
	}

	return true;
}

std::string SecretWord::not_guessed_chars_status() {
	std::string result;

	for (size_t i = 0; i < not_guessed_chars_.size(); ++i) {
		char c = not_guessed_chars_[i];
		result += c;

		if (i+1 != not_guessed_chars_.size()) {
			result += ", ";
		}
	}

	return result;
}

std::string SecretWord::return_word_status() {
	std::string result;

	for (size_t i = 0; i < word_.length(); ++i) {
		char c = word_[i];
		if (char_in_guessed_chars(c)) {
			result += c;
		} else {
			result += '_';
		}

		if (i+1 != word_.length()) {
			result += ' ';
		}
	}

	return result;
}

bool SecretWord::char_in_word_chars(char input_char) {
	const auto &x = word_;

	return std::find(x.begin(), x.end(), input_char) != std::end(x);
}


bool SecretWord::char_in_guessed_chars(char input_char) {
	const auto &x = guessed_chars_;

	return std::find(x.begin(), x.end(), input_char) != std::end(x);
}

bool SecretWord::char_in_not_guessed_chars(char input_char) {
	const auto &x = not_guessed_chars_;

	return std::find(x.begin(), x.end(), input_char) != std::end(x);
}
