#pragma once

#include <Windows.h>
#include <string>
using namespace std;

class ConsoleControl
{
public:
	ConsoleControl();

	//与光标有关的函数
	void hideCursor();
	void showCursor();

	//与读取控制台字符有关的函数
	void readConcoleLineCharacter(COORD &, string &, int);
	void setDefault();
	void setInfomationAttribute(WORD);
	void setCharacterAttribute(WORD);
	WORD getInfomationAttribute();
	WORD getCharacterAttribute();

	//与文件操作有关的函数
	void output_TXT_File(ofstream&, CONSOLE_SCREEN_BUFFER_INFO&);
	void output_CPP_File(ofstream&, string);
	void output_H_File(ofstream&, string);
	void output_RF_File(ofstream&, CONSOLE_SCREEN_BUFFER_INFO&, string);

	~ConsoleControl();

private:
	int graph_l, graph_w;
	int the_num_of_ch;
	WORD infomationtext_color, printcharacter_color;
	WORD default_color;
};

