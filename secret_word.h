#ifndef SECRET_WORD_H__
#define SECRET_WORD_H__

#include <string>
#include <vector>
#include <algorithm>

class SecretWord {
	public:
		SecretWord(const char *word);
		bool guess_char(char input_char);
		bool is_all_chars_are_guessed();
		std::string not_guessed_chars_status();
		std::string return_word_status();


	private:
		bool char_in_word_chars(char input_char);
		bool char_in_guessed_chars(char input_char);
		bool char_in_not_guessed_chars(char input_char);

		std::string word_;
		std::vector<char> guessed_chars_;
		std::vector<char> not_guessed_chars_;
};

#endif // SECRET_WORD_H__
