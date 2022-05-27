// Code by YYYCZ

#ifndef __YYYCZ_SUDOKU_GAME_H__
#define __YYYCZ_SUDOKU_GAME_H__

#include "FastSudoku.h"
#include <string>
#include <unordered_map>

namespace YYYCZ {
	class SudokuGame {
		using save_t = FastSudoku::save_t;
		using index_t = FastSudoku::index_t;

		struct Printer {
			bool isFunction = false;
			std::string content;
		};

		void printTime();
		void printForm();
		void printFill();
		void printHelp();
		void printResult();
		void printTip();

		const std::unordered_map<std::string, std::function<void()>> printFunctions{
			{ "time", [&]() {this->printTime(); }},
			{ "form", [&]() {this->printForm(); }},
			{ "fill", [&]() {this->printFill(); }},
			{ "help", [&]() {this->printHelp(); }},
			{ "result", [&]() {this->printResult(); }},
			{ "tip", [&]() {this->printTip(); }},
		};

		const std::vector<std::vector<Printer>> screens = {
			{{true, "time"}, {false, "\n"}, {true, "form"}, {false, "\n"}, {true, "help"}},
			{{true, "time"}, {false, "\n"}, {true, "form"}, {false, "\n"}, {true, "fill"}},
			{{true, "time"}, {false, "\n"}, {true, "result"}, {false, "\n"}, {true, "help"}},
			{{true, "time"}, {false, "\n"}, {true, "result"}, {false, "\n\t\033[33mYou do it! Finished!\033[0m"}},
			{{true, "time"}, {false, "\n"}, {true, "form"}, {false, "\n"}, {true, "tip"}},
		};

		static constexpr int stateHelp = 0;
		static constexpr int stateFill = 1;
		static constexpr int stateResult = 2;
		static constexpr int stateFinish = 3;
		static constexpr int stateTip = 4;

		FastSudoku result; // the sudoku result
		FastSudoku solver; // the sudoku solver

		save_t row = 0; // the row of now selected
		save_t column = 0; // the column of now selected
		const int state = stateHelp; // now state of game
		const int duration = 5; // the duration of gameframe (ms)

	protected:
		// core process
		void draw();
		void setState(int newState);
		void move(char direction);
		void fillNumber(save_t number);

		void mainThread();

	public:
		void gameStart(const std::vector<save_t>& form); // giving sudoku
		void gameStart(); // random sudoku
		void setDuration(const int duration); // set duration

	};
}

#endif