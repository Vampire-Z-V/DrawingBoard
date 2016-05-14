#pragma once

#include <Windows.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cmath>
#include <conio.h>
using namespace std;

#include "ConsoleControl.h"

enum Massage
{
	ALL,
	CURSORPOSITION,
	CHARACTER,
};

class InputEventAndControl
{
public:
	InputEventAndControl();

	void KeyTest();									//输入检测
	void saveFile(string&&, string&&);
	void PrintMessage(Massage);						//输出坐标和字符信息

	~InputEventAndControl();
private:
	HANDLE			hConsoleOutput;					//获取标准输出句柄
	HANDLE			hConsoleInput;					//获取标准输入句柄
	DWORD			dwNumberOfEventsRead;			//记录事件数量
	INPUT_RECORD	inputRecord;					//输入记录

	char			OutputCharacter[2];				//记录鼠标左右键的字符
	COORD			crHome;							//坐标{0，0}
	COORD			crPos;							//记录光标位置
	bool			bCaps;							//大写锁定键是否开启
	bool			bShift;							//Shift是否被按下
	bool			bCtrl_L, bCtrl_R;				//Ctrl是否被按下
	bool			bAlt_L, bAlt_R;					//Alt是否被按下

	ConsoleControl CCI;

	//鼠标和键盘事件
	void switchMouseButtonStateEvent();
	void switchKeyButtonUpStateEvent();
	void switchKeyButtonDownStateEvent();
	void testControlKeyState();
	void testHotKey();

	//与输出字符有关的函数
	bool isPrintableCharacter(WORD&);
	void changeCharacter(WORD&);
	void upCharacter(WORD&);
	void downCharacter(WORD&);
	void gotoxy(COORD);
	void gotoxy(short, short);
	void changeColor(WORD, char);

	//与清屏有关的函数
	COORD &setClearBeginPoint(COORD&, COORD&);
	void clearBoard_NextActionTest(COORD);

	//与文件操作有关的函数
	void print_and_getFileInfomation(string&, string&&);
	void printFileType(string&, string&);
	void tip();
};

