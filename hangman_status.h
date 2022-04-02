#ifndef HANGMAN_STATUS_H__
#define HANGMAN_STATUS_H__

class HangmanStatus {
	public:
		HangmanStatus();

		const char *return_state();
		int increase_counter();
	private:
		const char * const *frames_;
		int counter_;
};

#endif // HANGMAN_STATUS_H__
