#include "SudokuHeader.h"

#ifdef _WIN32

#include <Windows.h>
#include <iostream>
#include <conio.h>

int YYYCZ::judgeDirection(int key)
{
	if (key == 224) {
		key = getKeyPressed();
		switch (key) {
		case 72: return 0; // up
		case 80: return 1; // down
		case 75: return 2; // left
		case 77: return 3; // right
		}
	}
	return -1;
}

void YYYCZ::relocateConsole()
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0,0 });
}

void YYYCZ::clearConsole()
{
	system("cls");
}

void YYYCZ::setConsoleCursor(bool visiable)
{
	CONSOLE_CURSOR_INFO info{ 100, visiable ? TRUE : FALSE };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

bool YYYCZ::isKeyPressed()
{
	return _kbhit();
}

int YYYCZ::getKeyPressed()
{
	return _getch();
}

void YYYCZ::wait(unsigned int ms)
{
	Sleep(ms);
}

#endif // _WIN32

#ifdef __linux

#include <iostream>
#include <unistd.h>
#include <termio.h>
#include <fcntl.h> 

int YYYCZ::judgeDirection(int key)
{
	if(key == '\033'){
		if(isKeyPressed()){
			int x1 = getKeyPressed();
			int x2 = getKeyPressed();
			switch (x2)
			{
			case 'A': return 0;
			case 'B': return 1;
			case 'D': return 2;
			case 'C': return 3;
			}
		}
	}
	return -1;
}

void YYYCZ::relocateConsole()
{
	std::cout << "\033[0;0H";
}

void YYYCZ::clearConsole()
{
	system("clear");
}

void YYYCZ::setConsoleCursor(bool visiable)
{
	std::cout << (visiable ? "\033[?25h" : "\033[?25l");
}

bool YYYCZ::isKeyPressed()
{
	struct termios oldt, newt;  
	int ch;  
	int oldf;  
	tcgetattr(STDIN_FILENO, &oldt);  
	newt = oldt;  
	newt.c_lflag &= ~(ICANON | ECHO);  
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);  
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);  
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK); 
	ch = getchar();  
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  
	fcntl(STDIN_FILENO, F_SETFL, oldf);  
	if(ch != EOF)  
	{  
		ungetc(ch, stdin);  
		return true;  
	}  
	return false; 
}

int YYYCZ::getKeyPressed()
{
	struct termios tm, tm_old;
	int fd = 0, ch;

	if (tcgetattr(fd, &tm) < 0) {
		return -1;
	}

	tm_old = tm;
	cfmakeraw(&tm);
	if (tcsetattr(fd, TCSANOW, &tm) < 0) {
		return -1;
	}

	ch = getchar();
	if (tcsetattr(fd, TCSANOW, &tm_old) < 0) {
		return -1;
	}

	std::cout << "    \r";
	return ch;
}

void YYYCZ::wait(unsigned int ms)
{
	usleep(ms * 1000);
}

#endif // __linux