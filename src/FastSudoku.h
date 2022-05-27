// Code By YYYCZ

#ifndef __YYYCZ_FAST_SUDOKU_H__
#define __YYYCZ_FAST_SUDOKU_H__

#include <deque>
#include <vector>
#include <chrono>
#include <functional>
#include <unordered_set>

namespace YYYCZ {
	// a tool class can fastly solve the sudoku.
	// also can be used to play sudoku!
	// NOT THREAD SAFE
	class FastSudoku {
	public:
		using save_t = int;
		using index_t = unsigned int;
		using clock_t = std::chrono::high_resolution_clock;

	private:
		constexpr static char inputForm = 0; // input the number to the sudoku form
		constexpr static char eraseMapper = 1; // erase the number from the mapper
		constexpr static char moveRank = 2; // move the item from the rank
		constexpr static char clearRank = 3; // clear the item from the rank
		constexpr static char archivePoint = 4; // do nothing, just the archive point

		// a struct to record the operate
		struct SudokuOperate {
			char type = archivePoint;
			save_t p1 = 0;
			index_t p2 = 0;
		};

		// For solve
		bool recordMode = false; // if to record operate
		index_t formDim = 0; // the dimension of the form
		index_t formSize = 0; // the size of the form
		index_t formLength = 0; // the length of the form
		index_t finishCount = 0; // the count of the finish place
		index_t iterationTimes = 0; // the times of iteration
		std::vector<save_t> form; // the form of sudoku
		std::vector<std::unordered_set<save_t>> mapper; // the number of the place can be filled
		std::vector<std::unordered_set<index_t>> rank; // help to find minimum choices' place
		std::unordered_set<index_t> original; // record the original form place where has number
		std::deque<SudokuOperate> records; // record the operates of history

		// For show
		bool markChange = false; // if record the place changed
		std::unordered_set<index_t> changePlace; // record the place changed

		// For play
		bool showAfterFill = true; // if show after fill

		// For timing
		char timingMode = 0; // the mode of timing
		decltype(clock_t::now()) tpBegin; // time point of begin
		decltype(clock_t::now()) tpEnd; // time point of end

		// Check
		bool __CheckIfInit() const;

		// Base Operate
		void doOperate(const SudokuOperate& op);
		void undoOperate();

		// Coordinate transform
		index_t translate(save_t i, save_t j) const;
		index_t translate(const std::pair<save_t, save_t>& coordinate) const;
		std::pair<save_t, save_t> translate(index_t index) const;
		save_t getBlock(save_t i, save_t j) const;
		save_t getBlock(const std::pair<save_t, save_t>& coordinate) const;
		std::pair<save_t, save_t> blockAt(save_t block, save_t i) const;

		// Local Operate
	protected:
		bool __eraseLocalNumber(save_t k, save_t number, std::function<index_t(save_t, save_t)> translator);
	private:
		bool eraseRowNumber(save_t row, save_t number);
		bool eraseColumnNumber(save_t column, save_t number);
		bool eraseBlockNumber(save_t block, save_t number);
		void eraseAllNumberOfOnePlace(index_t index);

		// Global Operate
		bool eraseNumberFromPlace(save_t number, index_t index);
		bool putNumberIntoForm(save_t number, index_t index);
		index_t findMinimumChoicesPlace() const;
		bool flashBack();
		void save();

		// Core Operate
	protected:
		void init(const save_t* form, const index_t formDim);
		void init(const std::vector<save_t>& form);
		bool solve();

	public:
		// Function For Solve
		const decltype(original)& getOrigin() const;
		const decltype(form)& getResult() const;
		const decltype(records)& getRecords() const;
		index_t getIterationTimes() const;
		FastSudoku& reset(bool clearSettings = true);
		FastSudoku& printForm(const index_t* selected = nullptr);
		FastSudoku& printForm(const save_t row, const save_t column);
		FastSudoku& replay(bool stepPlay = true);
		FastSudoku& setMarkChange(bool flag);
		FastSudoku& solve(const std::vector<save_t>& form);

		// Function For Play
		FastSudoku& load(const std::vector<save_t>& form);
		FastSudoku& setShowAfterFill(bool flag);
		std::pair<save_t, std::pair<save_t, save_t>> tip() const;
		std::unordered_set<save_t> getLegalNumbersAt(save_t row, save_t column) const;
		bool checkInit() const;
		bool tryFill(save_t number, save_t row, save_t column);
		bool undo();
		FastSudoku& erase(save_t row, save_t column);
		FastSudoku& restart();
		bool isFinish() const;
		bool automaticSolve();

		// Function For Generate
		bool generate(index_t numberCount = 20, save_t dim = 3, int maxGenerateTimes = 10);

		// Function For Timing
		static constexpr char TimingMode_NotTiming = 0;
		static constexpr char TimingMode_TimingInit = 1;
		static constexpr char TimingMode_TimingSolve = 2;
		static constexpr char TimingMode_TimingBoth = 3;
		static constexpr char TimingMode_TimingNow = 4;
		FastSudoku& setTimingMode(char mode);
		char getTimingMode() const;
		double getDuration() const;

		// Get Parameters
		index_t getFormDimension() const;
		index_t getFormLength() const;
		index_t getFormSize() const;
	};
}

#endif