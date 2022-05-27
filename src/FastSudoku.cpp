// Code By YYYCZ

#include "FastSudoku.h"
#include "SudokuHeader.h"
#include <iostream>
#include <cmath>

namespace YYYCZ {
	/// <summary>
	/// check if the sudoku is already init
	/// </summary>
	/// <returns>if is init</returns>
	inline bool FastSudoku::__CheckIfInit() const
	{
		return !(formDim == 0 || formLength == 0 || formSize == 0 ||
			formDim * formDim != formLength || formLength * formLength != formSize ||
			form.empty() || form.size() != formSize);
	}

	/// <summary>
	/// do operate and record in the records
	/// </summary>
	/// <param name="op">the operate</param>
	inline void FastSudoku::doOperate(const SudokuOperate& op)
	{
		switch (op.type)
		{
		case inputForm:
			if (markChange) this->changePlace.insert(op.p2);
			form[op.p2] = op.p1;
			++finishCount;
			break;
		case eraseMapper:
			mapper[op.p2].erase(op.p1);
			break;
		case moveRank:
			rank[op.p1 - 1].insert(op.p2);
			rank[op.p1].erase(op.p2);
			break;
		case clearRank:
			rank[0].insert(op.p2);
			rank[op.p1].erase(op.p2);
			break;
		}
		if (recordMode)
			records.push_back(op);
	}

	/// <summary>
	/// undo and return if undo successfully
	/// </summary>
	inline void FastSudoku::undoOperate()
	{
		SudokuOperate& op = records.back();
		switch (op.type)
		{
		case inputForm:
			if (markChange) this->changePlace.insert(op.p2);
			form[op.p2] = 0;
			--finishCount;
			break;
		case eraseMapper:
			mapper[op.p2].insert(op.p1);
			break;
		case moveRank:
			rank[op.p1].insert(op.p2);
			rank[op.p1 - 1].erase(op.p2);
			break;
		case clearRank:
			rank[op.p1].insert(op.p2);
			rank[0].erase(op.p2);
			break;
		}
		records.pop_back();
	}

	/// <summary>
	/// get the coordinate to the index
	/// </summary>
	/// <param name="i">i</param>
	/// <param name="j">j</param>
	/// <returns>index</returns>
	inline FastSudoku::index_t FastSudoku::translate(save_t i, save_t j) const
	{
		return i * formLength + j;
	}

	/// <summary>
	/// get the coordinate to the index
	/// </summary>
	/// <param name="coordinate">coordinate</param>
	/// <returns>index</returns>
	inline FastSudoku::index_t FastSudoku::translate(const std::pair<save_t, save_t>& coordinate) const
	{
		return this->translate(coordinate.first, coordinate.second);
	}

	/// <summary>
	/// get the index to the coordinate
	/// </summary>
	/// <param name="index">index</param>
	/// <returns>coordinate</returns>
	inline std::pair<FastSudoku::save_t, FastSudoku::save_t> FastSudoku::translate(index_t index) const
	{
		return std::pair<save_t, save_t>(index / formLength, index % formLength);
	}

	/// <summary>
	/// get the block of the coordinate
	/// </summary>
	/// <param name="i">i</param>
	/// <param name="j">j</param>
	/// <returns>which block</returns>
	inline FastSudoku::save_t FastSudoku::getBlock(save_t i, save_t j) const
	{
		return formDim * (i / formDim) + (j / formDim);
	}

	/// <summary>
	/// get the block of the coordinate
	/// </summary>
	/// <param name="coordinate">coordinate</param>
	/// <returns>which block</returns>
	inline FastSudoku::save_t FastSudoku::getBlock(const std::pair<save_t, save_t>& coordinate) const
	{
		return this->getBlock(coordinate.first, coordinate.second);
	}

	/// <summary>
	/// get the coordinate of the ith of the block
	/// </summary>
	/// <param name="block">which block</param>
	/// <param name="i">the ith of the block</param>
	/// <returns>coordinate</returns>
	inline std::pair<FastSudoku::save_t, FastSudoku::save_t> FastSudoku::blockAt(save_t block, save_t i) const
	{
		return std::pair<save_t, save_t>(formDim * (block / formDim) + i / formDim,
			formDim * (block % formDim) + i % formDim);
	}

	/// <summary>
	/// the summary function of local erase operate
	/// </summary>
	/// <param name="k">k</param>
	/// <param name="number">number</param>
	/// <param name="translator">translator</param>
	/// <returns>check if ok?</returns>
	bool FastSudoku::__eraseLocalNumber(save_t k, save_t number, std::function<index_t(save_t, save_t)> translator)
	{
		SudokuOperate op1{ eraseMapper, number, 0 }, op2{ moveRank, 0, 0 };
		for (save_t i = 0; i < formLength; ++i) {
			index_t index = translator(k, i);
			if (mapper[index].find(number) != mapper[index].end()) {
				op1.p2 = index;
				op2.p1 = mapper[index].size();
				op2.p2 = index;
				doOperate(op1);
				doOperate(op2);

				// check empty if ok
				if (mapper[index].empty() && form[index] == 0) {
					return false;
				}
			}
		}
		return true;
	}

	/// <summary>
	/// erase one number in one row
	/// </summary>
	/// <param name="row">row</param>
	/// <param name="number">number</param>
	/// <returns>check if ok?</returns>
	inline bool FastSudoku::eraseRowNumber(save_t row, save_t number)
	{
		return __eraseLocalNumber(row, number, [&](save_t r, save_t i) -> index_t {
			return translate(r, i);
			});
	}

	/// <summary>
	/// erase one number in one column
	/// </summary>
	/// <param name="column">column</param>
	/// <param name="number">number</param>
	/// <returns>check if ok?</returns>
	inline bool FastSudoku::eraseColumnNumber(save_t column, save_t number)
	{
		return __eraseLocalNumber(column, number, [&](save_t c, save_t i) -> index_t {
			return translate(i, c);
			});
	}

	/// <summary>
	/// erase one number in one block
	/// </summary>
	/// <param name="block">block</param>
	/// <param name="number">number</param>
	/// <returns>check if ok?</returns>
	inline bool FastSudoku::eraseBlockNumber(save_t block, save_t number)
	{
		return __eraseLocalNumber(block, number, [&](save_t b, save_t i) -> index_t {
			return translate(blockAt(b, i));
			});
	}

	/// <summary>
	/// erase all number in one place
	/// </summary>
	/// <param name="index"></param>
	inline void FastSudoku::eraseAllNumberOfOnePlace(index_t index)
	{
		SudokuOperate op1{ eraseMapper, 0, index }, op2{ clearRank, (save_t)mapper[index].size(), index };
		doOperate(op2);
		while (!mapper[index].empty()) {
			op1.p1 = *mapper[index].begin();
			doOperate(op1);
		}
	}

	/// <summary>
	/// erase the number from one place
	/// </summary>
	/// <param name="index">index</param>
	/// <param name="number">number</param>
	/// <returns>check if ok?</returns>
	inline bool FastSudoku::eraseNumberFromPlace(save_t number, index_t index)
	{
		auto coor = translate(index);
		auto block = getBlock(coor);
		eraseAllNumberOfOnePlace(index);
		if (!eraseRowNumber(coor.first, number)) return false;
		if (!eraseColumnNumber(coor.second, number)) return false;
		if (!eraseBlockNumber(block, number)) return false;
		return true;
	}

	/// <summary>
	/// put one number into one place on the form
	/// </summary>
	/// <param name="number">the number be put</param>
	/// <param name="index">index</param>
	/// <returns>check if ok?</returns>
	inline bool FastSudoku::putNumberIntoForm(save_t number, index_t index)
	{
		doOperate({ inputForm, number, index });
		return eraseNumberFromPlace(number, index);
	}

	/// <summary>
	/// find a place with minimum choices
	/// </summary>
	/// <returns>the index. if no index, return formLength</returns>
	inline FastSudoku::index_t FastSudoku::findMinimumChoicesPlace() const
	{
		for (save_t i = 1; i <= formLength; ++i) {
			if (!this->rank[i].empty()) {
				return *this->rank[i].begin();
			}
		}
		return formSize;
	}

	/// <summary>
	/// go to the dead end, need to flash back
	/// </summary>
	/// <return>if still alive</return>
	inline bool FastSudoku::flashBack()
	{
		while (true) {
			if (records.empty()) {
				return false;
			}
			else if (records.back().type == archivePoint) {
				records.pop_back();
				break;
			}
			undoOperate();
		}
		return true;
	}

	/// <summary>
	/// save the point
	/// </summary>
	inline void FastSudoku::save()
	{
		if (recordMode) {
			records.push_back(SudokuOperate{ archivePoint });
		}
	}

	/// <summary>
	/// init the sudoku form
	/// </summary>
	/// <param name="form">sudoku form</param>
	/// <param name="formDim">the dimension of the form</param>
	void FastSudoku::init(const save_t* form, const index_t formDim)
	{
		// reset but not clear settings
		auto tpTMP = tpBegin;
		this->reset(false);
		tpBegin = tpTMP;

		// don't record the operate when init
		recordMode = false;

		// get the base parameters
		this->formDim = formDim;
		this->formLength = this->formDim * this->formDim;
		this->formSize = this->formLength * this->formLength;
		this->finishCount = 0;

		// init the form
		this->form.assign(form, form + formSize);

		// init the mapper
		auto mapperInit = std::unordered_set<save_t>();
		for (index_t i = 1; i <= formLength; ++i) {
			mapperInit.insert(i);
		}
		this->mapper.resize(formSize, mapperInit);

		// init the rank
		this->rank.resize(1 + formLength, std::unordered_set<index_t>());
		for (index_t i = 1; i <= formSize; ++i) {
			this->rank.back().insert(i);
		}

		// do some base operates without recording
		index_t index = 0;
		for (auto& i : this->form) {
			if (0 < i && formLength >= i) {
				++finishCount;
				original.insert(index);
				eraseNumberFromPlace(i, index);
			}
			else {
				i = 0;
			}
			++index;
		}
	}

	/// <summary>
	/// init the sudoku form
	/// </summary>
	/// <param name="form">sudoku form</param>
	inline void FastSudoku::init(const std::vector<save_t>& form)
	{
		this->init(&form[0], (index_t)sqrt(sqrt(form.size())));
	}

	/// <summary>
	/// solve the soduku
	/// </summary>
	/// <returns>if solve successfully</returns>
	bool FastSudoku::solve()
	{
		// iteration times + 1
		++iterationTimes;

		// need to record to get back when solving failed
		recordMode = true;

		index_t minIndex = formSize;
		while (true) {
			// finish!
			if (finishCount == formSize) {
				return true;
			}

			// get the place with minimum choices
			minIndex = findMinimumChoicesPlace();

			// no number can be filled
			if (minIndex == formSize) {
				return false;
			}

			// one choice
			if (mapper[minIndex].size() == 1) {
				if (!putNumberIntoForm(*mapper[minIndex].begin(), minIndex)) {
					return false;
				}
			}
			else {
				break;
			}
		}

		// try fill...
		std::vector<save_t> choices(mapper[minIndex].begin(), mapper[minIndex].end());
		for (auto i : choices) {
			save();
			if (putNumberIntoForm(i, minIndex) && solve()) {
				return true;
			}
			else {
				flashBack();
			}
		}

		return false;
	}

	/// <summary>
	/// get the original places
	/// </summary>
	/// <returns>original places</returns>
	const decltype(FastSudoku::original)& FastSudoku::getOrigin() const
	{
		return original;
	}

	/// <summary>
	/// get the result of solving sudoku
	/// </summary>
	/// <returns>solve. if no solve, return empty vector</returns>
	const decltype(FastSudoku::form)& FastSudoku::getResult() const
	{
		return form;
	}

	/// <summary>
	/// get the records
	/// </summary>
	/// <returns>records</returns>
	const decltype(FastSudoku::records)& FastSudoku::getRecords() const
	{
		return records;
	}

	/// <summary>
	/// get the times of iteration
	/// </summary>
	/// <returns>iteration times</returns>
	FastSudoku::index_t FastSudoku::getIterationTimes() const
	{
		return iterationTimes;
	}

	/// <summary>
	/// reset the fastSudoku
	/// </summary>
	/// <param name="clearSettings">if to clear all settings</param>
	/// <returns>FastSudoku</returns>
	FastSudoku& FastSudoku::reset(bool clearSettings)
	{
		this->formDim = 0;
		this->formSize = 0;
		this->formLength = 0;
		this->finishCount = 0;
		this->iterationTimes = 0;
		this->form.clear();
		this->mapper.clear();
		this->rank.clear();
		this->original.clear();
		this->records.clear();
		this->changePlace.clear();
		this->tpBegin = decltype(this->tpBegin)();
		this->tpEnd = decltype(this->tpEnd)();
		if (clearSettings) {
			this->recordMode = false;
			this->markChange = false;
			this->showAfterFill = false;
			this->timingMode = 0;
		}
		return *this;
	}

	/// <summary>
	/// print the form
	/// </summary>
	/// <param name="selected">the item be selected</param>
	/// <returns>FastSudoku</returns>
	FastSudoku& FastSudoku::printForm(const index_t* selected)
	{
		if (!__CheckIfInit()) {
			std::cout << "No solve." << std::endl;
			return *this;
		}

		for (save_t i = 0; i < formLength; ++i) {
			for (save_t j = 0; j < formLength; ++j) {
				index_t index = translate(i, j);
				if (selected && index == *selected) {
					std::cout << "\033[44m";
				}
				std::cout << ' ';
				if (changePlace.find(index) != changePlace.end()) {
					if (form[index] == 0) {
						std::cout << "\033[31m" << 'X';
					}
					else {
						std::cout << "\033[31m" << form[index];
					}
				}
				else if (original.find(index) != original.end()) {
					std::cout << "\033[32m" << form[index];
				}
				else if (form[index] != 0) {
					std::cout << "\033[33m" << form[index];
				}
				else {
					std::cout << ' ';
				}
				std::cout << " \033[0m";
				if (j + 1 != formLength) {
					if ((j + 1) % formDim == 0) {
						std::cout << "\033[34m|\033[0m";
					}
					else {
						std::cout << "|";
					}
				}
			}
			std::cout << std::endl;
			if (i + 1 != formLength) {
				if ((i + 1) % formDim == 0) {
					std::cout << "\033[34m";
					for (save_t j = 0; j < formLength; ++j) {
						std::cout << "---";
						if (j + 1 != formLength) {
							std::cout << '+';
						}
					}
					std::cout << "\033[0m";
				}
				else {
					for (save_t j = 0; j < formLength; ++j) {
						std::cout << "---";
						if (j + 1 != formLength) {
							if ((j + 1) % formDim == 0) {
								std::cout << "\033[34m+\033[0m";
							}
							else {
								std::cout << '+';
							}
						}
					}
				}
				std::cout << std::endl;
			}
		}
		changePlace.clear();
		return *this;
	}

	/// <summary>
	/// print the form
	/// </summary>
	/// <param name="row">the row of selected item</param>
	/// <param name="column">the column of selected item</param>
	/// <returns>FastSudoku</returns>
	FastSudoku& FastSudoku::printForm(const save_t row, const save_t column)
	{
		const index_t index = translate(row, column);
		return printForm(&index);
	}

	/// <summary>
	/// replay by records
	/// </summary>
	/// <returns>FastSudoku</returns>
	/// <param name="stepPlay">if play step by step</param>
	FastSudoku& FastSudoku::replay(bool stepPlay)
	{
		if (!__CheckIfInit()) {
			std::cout << "No solve." << std::endl;
			return *this;
		}

		// to wait user press
		auto waitPress = [&]() {
			if (stepPlay) {
				std::cout << "Press any key to continue ...";
				getKeyPressed();
				std::cout << "\r                             " << std::endl;
			}
			else {
				std::cout << std::endl;
			}
		};

		// recover the form
		auto recordsCopy = records;
		while (flashBack());

		// show original
		std::cout << "Original form:" << std::endl;
		printForm();
		waitPress();

		// start replay
		for (auto& i : recordsCopy) {
			if (i.type == inputForm) {
				auto coor = translate(i.p2);
				std::cout << "Fill " << i.p1 << " into ("
					<< (coor.first + 1) << ',' << (coor.second + 1) << ')' << std::endl;
				putNumberIntoForm(i.p1, i.p2);
				changePlace.insert(i.p2);
				printForm();
				waitPress();
			}
		}
		records.swap(recordsCopy);

		// show finish
		std::cout << "Final form:" << std::endl;
		printForm();
		waitPress();

		return *this;
	}

	/// <summary>
	/// set the change mark
	/// </summary>
	/// <param name="flag">new flag</param>
	/// <returns>FastSudoku</returns>
	FastSudoku& FastSudoku::setMarkChange(bool flag)
	{
		markChange = flag;
		return *this;
	}

	/// <summary>
	/// init, solve and get the result.
	/// if no solve, return empty vector
	/// </summary>
	/// <param name="form">the sudoku form</param>
	/// <returns>FastSudoku</returns>
	FastSudoku& FastSudoku::solve(const std::vector<save_t>& form)
	{
		if (timingMode == 1 || timingMode == 3) {
			tpBegin = clock_t::now();
		}
		if (&this->form == &form) {
			auto formCopy = form;
			init(formCopy);
		}
		else {
			init(form);
		}
		if (timingMode == 1) {
			tpEnd = clock_t::now();
		}
		else if (timingMode == 2) {
			tpBegin = clock_t::now();
		}
		if (!solve()) this->form.clear();
		if (timingMode == 2 || timingMode == 3) {
			tpEnd = clock_t::now();
		}
		return *this;
	}

	/// <summary>
	/// load the sudoku form for play
	/// </summary>
	/// <param name="form">sudoku form</param>
	/// <returns>FastSudoku</returns>
	FastSudoku& FastSudoku::load(const std::vector<save_t>& form)
	{
		if (&this->form == &form) {
			auto formCopy = form;
			init(formCopy);
		}
		else {
			this->init(form);
		}
		return *this;
	}

	/// <summary>
	/// set the show after fill
	/// </summary>
	/// <param name="flag">new flag</param>
	/// <returns>FastSudoku</returns>
	FastSudoku& FastSudoku::setShowAfterFill(bool flag)
	{
		this->showAfterFill = flag;
		return *this;
	}

	/// <summary>
	/// get the tip to fill
	/// </summary>
	/// <returns>number and coordinate. if no choices, return (-1, (-1,-1))</returns>
	std::pair<FastSudoku::save_t, std::pair<FastSudoku::save_t, FastSudoku::save_t>> FastSudoku::tip() const
	{
		index_t minIndex = findMinimumChoicesPlace();
		if (minIndex == formSize || mapper[minIndex].empty()) return { -1, {-1,-1} };
		auto coor = translate(minIndex);
		return { *mapper[minIndex].begin(), coor };
	}

	/// <summary>
	/// get the legal numbers (can be filled) of one place
	/// </summary>
	/// <param name="row">row of place</param>
	/// <param name="column">column of place</param>
	/// <returns>the vector of legal numbers</returns>
	std::unordered_set<FastSudoku::save_t> FastSudoku::getLegalNumbersAt(save_t row, save_t column) const
	{
		index_t index = this->translate(row, column);
		if (index >= formSize) return std::unordered_set<save_t>();
		return mapper[index];
	}

	/// <summary>
	/// check if the sudoku is already init
	/// </summary>
	/// <returns>if is init</returns>
	bool FastSudoku::checkInit() const
	{
		return __CheckIfInit();
	}

	/// <summary>
	/// try to fill one number in one place
	/// </summary>
	/// <param name="number">number be filled</param>
	/// <param name="row">row of place</param>
	/// <param name="column">column of place</param>
	/// <returns>if fill successfully</returns>
	bool FastSudoku::tryFill(save_t number, save_t row, save_t column)
	{
		recordMode = true;
		index_t index = this->translate(row, column);
		if (index >= formSize || mapper[index].find(number) == mapper[index].end()) {
			return false;
		}
		save();
		putNumberIntoForm(number, index);
		if (showAfterFill) printForm();
		return true;
	}

	/// <summary>
	/// undo the last operate. need to open the record mode
	/// </summary>
	/// <returns>if undo</returns>
	bool FastSudoku::undo()
	{
		bool res = flashBack();
		if (showAfterFill) printForm();
		return res;
	}

	/// <summary>
	/// erase a number in a place
	/// </summary>
	/// <param name="row">row of number</param>
	/// <param name="column">column of number</param>
	/// <returns>FastSudoku</returns>
	FastSudoku& FastSudoku::erase(save_t row, save_t column) {
		// check if could be erase
		index_t index = translate(row, column);
		if (index >= formSize || form[index] == 0 || original.count(index) != 0)
			return *this;

		// flash back until find this number
		bool mc = markChange;
		markChange = false;
		std::vector<SudokuOperate> backup;
		while (!records.empty()) {
			if (records.back().type == inputForm) {
				if (records.back().p2 == index) {
					flashBack();
					break;
				}
				else {
					backup.push_back(records.back());
				}
			}
			undoOperate();
		}

		// push others
		recordMode = true;
		for (auto itr = backup.rbegin(); itr != backup.rend(); ++itr) {
			save();
			putNumberIntoForm(itr->p1, itr->p2);
		}

		// mark change
		markChange = mc;
		if (markChange) {
			changePlace.insert(index);
		}

		return *this;
	}

	/// <summary>
	/// restart the game
	/// </summary>
	/// <returns>FastSudoku</returns>
	FastSudoku& FastSudoku::restart()
	{
		while (flashBack());
		return *this;
	}

	/// <summary>
	/// judge if the game is finish
	/// </summary>
	/// <returns>if is finish</returns>
	bool FastSudoku::isFinish() const
	{
		return finishCount == formSize;
	}

	/// <summary>
	/// solve the rest part by program
	/// </summary>
	/// <returns>if finish</returns>
	bool FastSudoku::automaticSolve()
	{
		if (!__CheckIfInit()) {
			return false;
		}
		iterationTimes = 0;
		return solve();
	}

	/// <summary>
	/// generate a sudoku (may not unique solve)
	/// </summary>
	/// <param name="numberCount"> number count at least</param>
	/// <param name="dim">dimension of sudoku</param>
	/// <returns>if generate successfully</returns>
	bool FastSudoku::generate(index_t numberCount, save_t dim, int maxGenerateTimes)
	{
		// bad parameters
		if (dim == 0 || maxGenerateTimes-- == 0) {
			reset();
			return false;
		}

		// set random seed
		srand(time(nullptr) * (1 + maxGenerateTimes));

		// load a empty form
		std::vector<save_t> emptyForm;
		emptyForm.resize(dim * dim * dim * dim, 0);
		if (numberCount >= emptyForm.size())
			numberCount = emptyForm.size() - 1;
		init(emptyForm);

		// set max random fill
		index_t needToAdd = 2 * dim * dim;
		if (numberCount > needToAdd) {
			std::swap(needToAdd, numberCount);
		}
		else {
			needToAdd = 0;
		}

		// random fill number
		int failTimes = 0;
		std::unordered_set<index_t> filled;
		while (finishCount < numberCount) {
			index_t index;
			do {
				index = rand() % emptyForm.size();
			} while (filled.count(index) != 0);
			filled.insert(index);

			if (mapper[index].size() == 0) {
				++failTimes;
				if (failTimes > 3)
					return generate(numberCount, dim, maxGenerateTimes);
				continue;
			}

			failTimes = 0;
			save_t ri = rand() % mapper[index].size();
			for (const auto& i : mapper[index]) {
				if (ri-- == 0) {
					putNumberIntoForm(i, index);
					break;
				}
			}
		}

		// backup
		if (!needToAdd)
			emptyForm = this->form;
		records.clear();

		// try solve
		if (solve()) {
			if (needToAdd) {
				while (finishCount > needToAdd) {
					index_t index = rand() % this->form.size();
					if (this->form[index]) {
						this->form[index] = 0;
						--finishCount;
					}
				}
				emptyForm = this->form;
			}
			init(emptyForm);
			return true;
		}

		return generate(numberCount, dim, maxGenerateTimes);
	}

	/// <summary>
	/// set the mode of timing
	/// </summary>
	/// <param name="mode">
	/// the mode of timing, can be the condition as follows: 
	/// 0. don't timing;
	/// 1. timing init time;
	/// 2. timing solve time;
	/// 3. timing init and solve time;
	/// 4. timing start when set this mode, and end when call this function.
	/// </param>
	/// <returns>FastSudoku</returns>
	FastSudoku& FastSudoku::setTimingMode(char mode)
	{
		if (mode < 0 || mode > 4) mode = 0;
		if (mode == TimingMode_TimingNow && this->timingMode != TimingMode_TimingNow) {
			tpBegin = clock_t::now();
		}
		if (this->timingMode == TimingMode_TimingNow) {
			tpEnd = clock_t::now();
		}
		this->timingMode = mode;
		return *this;
	}

	/// <summary>
	/// get the mode of timing
	/// </summary>
	/// <returns>mode of timing</returns>
	char FastSudoku::getTimingMode() const
	{
		return timingMode;
	}

	/// <summary>
	/// get the duration of timing
	/// </summary>
	/// <returns>duration(in second)</returns>
	double FastSudoku::getDuration() const
	{
		return double((tpEnd - tpBegin).count()) * clock_t::period::num / clock_t::period::den;
	}

	/// <summary>
	/// get the dimension of form
	/// </summary>
	/// <returns>dimension</returns>
	FastSudoku::index_t FastSudoku::getFormDimension() const
	{
		return this->formDim;
	}

	/// <summary>
	/// get the length of form
	/// </summary>
	/// <returns>length</returns>
	FastSudoku::index_t FastSudoku::getFormLength() const
	{
		return this->formLength;
	}

	/// <summary>
	/// get the size of form
	/// </summary>
	/// <returns>size</returns>
	FastSudoku::index_t FastSudoku::getFormSize() const
	{
		return this->formSize;
	}
}