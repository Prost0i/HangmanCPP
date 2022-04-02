#include "hangman_status.h"

#include "hangman_frames.h"

HangmanStatus::HangmanStatus(): frames_(hangman_frames), counter_(0) {}

int HangmanStatus::increase_counter() {
	if (counter_ < 6) {
		return ++counter_;
	}

	return counter_;
}

const char *HangmanStatus::return_state() {
	return frames_[counter_];
}
