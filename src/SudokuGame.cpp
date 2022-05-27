// Code by YYYCZ

#include "SudokuGame.h"
#include "SudokuHeader.h"
#include <iostream>
#include <iomanip>

namespace YYYCZ {
	void SudokuGame::printTime()
	{
		if (solver.getTimingMode() == solver.TimingMode_TimingNow) {
			solver.setTimingMode(solver.TimingMode_TimingNow);
		}
		std::cout << "Timing: \033[031m"
			<< std::fixed << std::setprecision(3) << solver.getDuration()
			<< "\033[0m s.\n";
	}

	void SudokuGame::printForm()
	{
		solver.printForm(row, column);
	}

	void SudokuGame::printFill()
	{
		std::cout << "\033[32mLegal Numbers\033[0m\n  ";
		auto legalNumbers = solver.getLegalNumbersAt(row, column);
		for (save_t i = 1; i <= solver.getFormLength(); ++i) {
			if (legalNumbers.count(i) == 0) {
				std::cout << "\033[30m\033[1m" << i;
			}
			else {
				std::cout << "\033[33m\033[1m" << i;
			}
			if (i != solver.getFormLength()) {
				std::cout << "\033[0m, ";
			}
		}
		std::cout << "\n\n"
			"\033[033mTip\033[0m: 1. Press \033[033mL\033[0m again to back to main UI;\n"
			"     2. \033[33m\033[1mHighlight\033[0m numbers is legal;\n"
			"     3. \033[30m\033[1mOther\033[0m numbers is illegal."
			<< std::endl;
	}

	void SudokuGame::printHelp()
	{
		static const char* helpMsg =
			"\033[032mGame Help\033[0m\n"
			"  1. Press \033[033mUp\033[0m, \033[033mDown\033[0m, \033[033mLeft\033[0m, \033[033mRight\033[0m to move cursor;\n"
			"  2. Press \033[033mNumber\033[0m to fill a number into this place;\n"
			"  3. Press \033[033mL\033[0m to see which numbers can be filled in this place;\n"
			"  4. Press \033[033mU\033[0m to undo;\n"
			"  5. Press \033[033mE\033[0m to erase number in this place;\n"
			"  6. Press \033[033mR\033[0m to restart;\n"
			"  7. Press \033[033mH\033[0m to see the suggest to fill number;\n"
			"  8. Press \033[033mA\033[0m to see answer;\n"
			"  9. Press \033[033mEsc\033[0m to exit;\n"
			"  10. If the console draw incorrectly, please enlarge the window and press \033[33mL\033[0m twice.";
		std::cout << helpMsg << std::endl;
	}

	void SudokuGame::printResult()
	{
		result.printForm();
	}

	void SudokuGame::printTip()
	{
		auto t = solver.tip();
		std::cout << "\033[032mSuggest\033[0m\n";
		if (t.first != -1) {
			std::cout << "  Fill \033[033m" << t.first << "\033[0m into row \033[033m" << t.second.first + 1
				<< "\033[0m column \033[033m" << t.second.second + 1 << "\033[0m" << std::endl;
		}
		else {
			std::cout << "  \033[031mTry to erase number!\033[0m                " << std::endl;
		}
		std::cout << "\n"
			"\033[033mTip\033[0m: Press \033[033mH\033[0m again to back to main UI.\n" << std::endl;
	}

	/// <summary>
	/// set the state
	/// </summary>
	/// <param name="newState">new state</param>
	inline void SudokuGame::setState(int newState)
	{
		if (newState >= 0 && newState < screens.size()) {
			*const_cast<int*>(&this->state) = newState;
		}
		clearConsole();
	}

	/// <summary>
	/// draw frame
	/// </summary>
	void SudokuGame::draw()
	{
		// relocate cursor in the console
		relocateConsole();

		// draw the screen
		for (auto& p : this->screens[state]) {
			if (p.isFunction) {
				auto itr = this->printFunctions.find(p.content);
				if (itr != this->printFunctions.end()) {
					itr->second();
				}
			}
			else {
				std::cout << p.content;
			}
		}
	}

	/// <summary>
	/// when player move on one direction
	/// </summary>
	/// <param name="direction">the direction</param>
	inline void SudokuGame::move(char direction)
	{
		const auto dAdd = [&](save_t& i) {
			if (++i >= solver.getFormLength()) i = 0;
		};
		const auto dSub = [&](save_t& i) {
			if (i-- == 0) i = solver.getFormLength() - 1;
		};

		switch (direction) {
		case 0: dSub(row); break; // up
		case 1: dAdd(row); break; // down
		case 2: dSub(column); break; // left
		case 3: dAdd(column); break; // right
		}
	}

	/// <summary>
	/// fill a number in this place
	/// </summary>
	/// <param name="number">number filled</param>
	inline void SudokuGame::fillNumber(save_t number)
	{
		if (solver.tryFill(number, row, column)) {
			if (solver.isFinish()) {
				solver.setTimingMode(solver.TimingMode_NotTiming);
				setState(stateFinish);
			}
			else {
				solver.setTimingMode(solver.TimingMode_TimingNow);
			}
		}
	}

	/// <summary>
	/// the main thread of game
	/// </summary>
	void SudokuGame::mainThread()
	{
		int key;
		while (true) {
			if (isKeyPressed()) {
				key = getKeyPressed();

				if (state == stateFinish) {
					break;
				}

				// direction control
				int direction = judgeDirection(key);
				if(direction != -1){
					move(direction);
				}
				// fill number
				else if (key >= '1' && key <= '9') {
					fillNumber(key - '0');
				}
				// see legal
				else if (key == 'L' || key == 'l') {
					setState(state == stateFill ? stateHelp : stateFill);
				}
				// undo
				else if (key == 'U' || key == 'u') {
					solver.undo();
				}
				// erase
				else if (key == 'E' || key == 'e' || key == '0') {
					solver.erase(row, column);
				}
				// restart
				else if (key == 'R' || key == 'r') {
					solver.restart();
				}
				// tip
				else if (key == 'H' || key == 'h') {
					setState(state == stateTip ? stateHelp : stateTip);
				}
				// answer
				else if (key == 'A' || key == 'a') {
					setState(state == stateResult ? stateHelp : stateResult);
				}
				// esc
				else if (key == 27) {
					break;
				}
			}
			// update console
			draw();
			// yield the thread
			wait(duration);
		}
	}

	/// <summary>
	/// game start, have fun!
	/// </summary>
	/// <param name="form">sudoku form</param>
	void SudokuGame::gameStart(const std::vector<save_t>& form)
	{
		std::cout << "\033[0m";

		// only support 4*4 and 9*9 sudoku
		if (form.size() != 16 && form.size() != 81) return;

		// load sudoku
		solver.reset();
		solver.load(form);
		if (!solver.checkInit()) return;
		solver.setMarkChange(false);

		// get the result
		result.solve(form);
		if (result.getResult().empty()) return;

		// hide cursor
		setConsoleCursor(false);

		// set the initial point
		row = column = 0;

		// set the state and start game main thread
		setState(stateHelp);
		mainThread();

		// show cursor
		setConsoleCursor(true);
	}

	/// <summary>
	/// game start, have fun!
	/// </summary>
	void SudokuGame::gameStart()
	{
		clearConsole();
		std::cout << "\033[0mWelcome to play \033[34mYYYCZ\033[0m's sudoku game!\n"
			"Press any \033[33mnumber\033[0m to select game mode:\n"
			"  \033[33m1\033[0m. 2x2 Sudoku - Easy\n"
			"  \033[33m2\033[0m. 2x2 Sudoku - Medium\n"
			"  \033[33m3\033[0m. 3x3 Sudoku - Easy\n"
			"  \033[33m4\033[0m. 3x3 Sudoku - Medium\n"
			"  \033[33m5\033[0m. 3x3 Sudoku - Difficult\n"
			"  \033[33m6\033[0m. 3x3 Sudoku - Hard\n"
			"  \033[33m7\033[0m. 3x3 Sudoku - Expert\n"
			"  \033[33m8\033[0m. 3x3 Sudoku - Hell\n"
			"  \033[33m9\033[0m. 3x3 Sudoku - God\n"
			"  \033[33m0\033[0m. 3x3 Sudoku - Custom\n\n"
			"Note: If the console draw incorrectly, please enlarge the window and press \033[33mL\033[0m twice.\n";

		while (true) {
			if (isKeyPressed()) {
				int key = getKeyPressed();

				switch (key) {
				case '1':
					while (!result.generate(8, 2));
					gameStart(result.getResult());
					return;
				case '2':
					while (!result.generate(6, 2));
					gameStart(result.getResult());
					return;
				case '3':
					while (!result.generate(40));
					gameStart(result.getResult());
					return;
				case '4':
					while (!result.generate(35));
					gameStart(result.getResult());
					return;
				case '5':
					while (!result.generate(31));
					gameStart(result.getResult());
					return;
				case '6':
					while (!result.generate(27));
					gameStart(result.getResult());
					return;
				case '7':
					while (!result.generate(24));
					gameStart(result.getResult());
					return;
				case '8':
					while (!result.generate(21));
					gameStart(result.getResult());
					return;
				case '9':
					while (!result.generate(18));
					gameStart(result.getResult());
					return;
				case '0': {
					std::vector<FastSudoku::save_t> sudoku;

					clearConsole();
					std::cout << "Please input the sudoku form. Use 0 to represent empty space, such as:\n\033[33m"
						"7 4 0 6 3 0 0 0 0\n"
						"0 0 0 0 0 0 0 0 0\n"
						"0 0 5 2 0 0 0 3 0\n"
						"6 0 3 0 0 5 0 4 0\n"
						"0 1 0 0 6 0 0 8 0\n"
						"0 8 0 7 0 0 2 0 3\n"
						"0 7 0 0 0 2 9 0 0\n"
						"0 0 0 0 0 0 0 0 0\n"
						"0 0 0 0 4 3 0 1 6\033[0m\n\n";

					sudoku.resize(81, 0);
					for (int i = 0; i < 81; ++i)
						std::cin >> sudoku[i];

					gameStart(sudoku);
					return;
				}
				}
			}
			wait(duration);
		}
	}

	/// <summary>
	/// set the duration
	/// </summary>
	/// <param name="duration">duration</param>
	void SudokuGame::setDuration(const int duration)
	{
		*const_cast<int*>(&this->duration) = duration > 0 ? duration : this->duration;
	}
}