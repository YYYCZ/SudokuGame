// Code by YYYCZ

namespace YYYCZ {
	int judgeDirection(int key); // judge if is direction control and return 0,1,2,3 (up,down,left,right) or -1(not direction)
	void relocateConsole(); // relocate cursor in the console
	void clearConsole(); // clear the console
	void setConsoleCursor(bool visiable); // set the visibility of console's cursor
	bool isKeyPressed(); // judge if some keys be pressed
	int getKeyPressed(); // get the key be pressed
	void wait(unsigned int ms); // wait x milliseconds
}