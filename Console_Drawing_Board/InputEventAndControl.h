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

	void KeyTest();									//������
	void saveFile(string&&, string&&);
	void PrintMessage(Massage);						//���������ַ���Ϣ

	~InputEventAndControl();
private:
	HANDLE			hConsoleOutput;					//��ȡ��׼������
	HANDLE			hConsoleInput;					//��ȡ��׼������
	DWORD			dwNumberOfEventsRead;			//��¼�¼�����
	INPUT_RECORD	inputRecord;					//�����¼

	char			OutputCharacter[2];				//��¼������Ҽ����ַ�
	COORD			crHome;							//����{0��0}
	COORD			crPos;							//��¼���λ��
	bool			bCaps;							//��д�������Ƿ���
	bool			bShift;							//Shift�Ƿ񱻰���
	bool			bCtrl_L, bCtrl_R;				//Ctrl�Ƿ񱻰���
	bool			bAlt_L, bAlt_R;					//Alt�Ƿ񱻰���

	ConsoleControl CCI;

	//���ͼ����¼�
	void switchMouseButtonStateEvent();
	void switchKeyButtonUpStateEvent();
	void switchKeyButtonDownStateEvent();
	void testControlKeyState();
	void testHotKey();

	//������ַ��йصĺ���
	bool isPrintableCharacter(WORD&);
	void changeCharacter(WORD&);
	void upCharacter(WORD&);
	void downCharacter(WORD&);
	void gotoxy(COORD);
	void gotoxy(short, short);
	void changeColor(WORD, char);

	//�������йصĺ���
	COORD &setClearBeginPoint(COORD&, COORD&);
	void clearBoard_NextActionTest(COORD);

	//���ļ������йصĺ���
	void print_and_getFileInfomation(string&, string&&);
	void printFileType(string&, string&);
	void tip();
};

