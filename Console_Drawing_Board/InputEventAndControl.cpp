#include "InputEventAndControl.h"

#define LEFT 0
#define RIGHT 1

InputEventAndControl::InputEventAndControl()
{
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);

	OutputCharacter[0] = 'o', OutputCharacter[1] = 'O';
	crHome.X = 0, crHome.Y = 0;
	bCaps = bShift = false;
	bCtrl_L = bCtrl_R = bAlt_R = bAlt_L = false;
}

void InputEventAndControl::KeyTest()
{
	ReadConsoleInput(hConsoleInput, &inputRecord, 1, &dwNumberOfEventsRead);

	switch (inputRecord.EventType)
	{
		case MOUSE_EVENT:
			crPos = inputRecord.Event.MouseEvent.dwMousePosition;

			PrintMessage(ALL);

			switchMouseButtonStateEvent();
			break;
		case KEY_EVENT:
			if (inputRecord.Event.KeyEvent.bKeyDown)
			{
				testControlKeyState();
				switchKeyButtonDownStateEvent();
				PrintMessage(ALL);
			}
			else
			{
				switchKeyButtonUpStateEvent();
			}
			break;
	}
}

void InputEventAndControl::PrintMessage(Massage massage_type)
{
	CONSOLE_SCREEN_BUFFER_INFO tempbuf;
	GetConsoleScreenBufferInfo(hConsoleOutput, &tempbuf);
	COORD start;
	start.Y = 0;
	switch (massage_type)
	{
		case ALL:
			start.X = 0;
			gotoxy(start);
			CCI.setInfomationAttribute(0);
			cout << "[Cursor Position] X: " << setw(3) << inputRecord.Event.MouseEvent.dwMousePosition.X << "  Y: " << setw(3) << inputRecord.Event.MouseEvent.dwMousePosition.Y;
			cout << "  [Now Character] Left: ";
			CCI.setCharacterAttribute(0);
			cout << OutputCharacter[LEFT];
			CCI.setInfomationAttribute(0);
			cout << " | Right: ";
			CCI.setCharacterAttribute(0);
			cout << OutputCharacter[RIGHT];
			gotoxy(tempbuf.dwCursorPosition);
			break;
		case CURSORPOSITION:
			start.X = 18;
			gotoxy(start);
			CCI.setInfomationAttribute(0);
			cout << "X: " << setw(3) << inputRecord.Event.MouseEvent.dwMousePosition.X << "  Y: " << setw(3) << inputRecord.Event.MouseEvent.dwMousePosition.Y;
			gotoxy(tempbuf.dwCursorPosition);
			break;
		case CHARACTER:
			start.X = 50;
			gotoxy(start);
			CCI.setInfomationAttribute(0);
			cout << "Left: ";
			CCI.setCharacterAttribute(0);
			cout << OutputCharacter[LEFT];
			CCI.setInfomationAttribute(0);
			cout << " | Right: ";
			CCI.setCharacterAttribute(0);
			cout << OutputCharacter[RIGHT];
			gotoxy(tempbuf.dwCursorPosition);
			break;
	}
}

InputEventAndControl::~InputEventAndControl()
{
	CloseHandle(hConsoleInput);
	CloseHandle(hConsoleOutput);
}

void InputEventAndControl::switchMouseButtonStateEvent()
{
	if (inputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
	{
		if (inputRecord.Event.MouseEvent.dwEventFlags == DOUBLE_CLICK)		// 左键双击 撤销改坐标下输出
		{
			tip();
			FillConsoleOutputCharacter(hConsoleOutput, 0, 1, inputRecord.Event.MouseEvent.dwMousePosition, &dwNumberOfEventsRead);
			return;
		}
	}
	else if (inputRecord.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
	{
		if (inputRecord.Event.MouseEvent.dwEventFlags == DOUBLE_CLICK)		// 右键双击 清除所有输出
		{
			FillConsoleOutputCharacter(hConsoleOutput, ' ', 1, { 75, 0 }, &dwNumberOfEventsRead);
			saveFile("_Distory", ".");
			CONSOLE_SCREEN_BUFFER_INFO bInfo;
			GetConsoleScreenBufferInfo(hConsoleOutput, &bInfo);
			COORD cls = { 104, 0 };

			FillConsoleOutputCharacter(hConsoleOutput, 0, bInfo.dwSize.X * bInfo.dwSize.Y, cls, &dwNumberOfEventsRead);
			return;
		}
	}

	switch (inputRecord.Event.MouseEvent.dwButtonState)
	{
		case FROM_LEFT_1ST_BUTTON_PRESSED:			// 左键 输出左字符			
			tip();
			FillConsoleOutputAttribute(hConsoleOutput, CCI.getCharacterAttribute(), 1, inputRecord.Event.MouseEvent.dwMousePosition, &dwNumberOfEventsRead);
			FillConsoleOutputCharacter(hConsoleOutput, OutputCharacter[LEFT], 1, inputRecord.Event.MouseEvent.dwMousePosition, &dwNumberOfEventsRead);
			break;
		case RIGHTMOST_BUTTON_PRESSED:				// 右键 输出右字符
			tip();
			FillConsoleOutputAttribute(hConsoleOutput, CCI.getCharacterAttribute(), 1, inputRecord.Event.MouseEvent.dwMousePosition, &dwNumberOfEventsRead);
			FillConsoleOutputCharacter(hConsoleOutput, OutputCharacter[RIGHT], 1, inputRecord.Event.MouseEvent.dwMousePosition, &dwNumberOfEventsRead);
			break;
		case FROM_LEFT_2ND_BUTTON_PRESSED:
			{
				tip();
				COORD firstPos = inputRecord.Event.MouseEvent.dwMousePosition;
				FillConsoleOutputAttribute(hConsoleOutput, FOREGROUND_INTENSITY | FOREGROUND_RED, 1, inputRecord.Event.MouseEvent.dwMousePosition, &dwNumberOfEventsRead);
				FillConsoleOutputCharacter(hConsoleOutput, '.', 1, firstPos, &dwNumberOfEventsRead);
				clearBoard_NextActionTest(firstPos);
			}
		default:
			break;
	}
}

void InputEventAndControl::switchKeyButtonUpStateEvent()
{
	if (!( inputRecord.Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED ))		// 左CTRL弹起
		bCtrl_L = false;
	if (!( inputRecord.Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED ))		// 右CTRL弹起
		bCtrl_R = false;
	if (!( inputRecord.Event.KeyEvent.dwControlKeyState &  LEFT_ALT_PRESSED ))		// 左ALT弹起
		bAlt_L = false;
	if (!( inputRecord.Event.KeyEvent.dwControlKeyState &  RIGHT_ALT_PRESSED ))		// 右ALT弹起
		bAlt_R = false;
	if (!( inputRecord.Event.KeyEvent.dwControlKeyState &  SHIFT_PRESSED ))			// SHIFT弹起
		bShift = false;

}

void InputEventAndControl::switchKeyButtonDownStateEvent()
{
	if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)	// 按ESC退出并保存缓存文件
	{
		saveFile("_CacheFile", ".");
		exit(EXIT_SUCCESS);
	}
	else if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_DELETE)
	{
		COORD firstPos = crPos;
		FillConsoleOutputAttribute(hConsoleOutput, FOREGROUND_INTENSITY | FOREGROUND_RED, 1, firstPos, &dwNumberOfEventsRead);
		FillConsoleOutputCharacter(hConsoleOutput, '.', 1, firstPos, &dwNumberOfEventsRead);
		clearBoard_NextActionTest(firstPos);
	}

	if (!bShift && !bCtrl_L && !bCtrl_R && !bAlt_L && !bAlt_R)
	{
		if (isPrintableCharacter(inputRecord.Event.KeyEvent.wVirtualKeyCode))
			changeCharacter(inputRecord.Event.KeyEvent.wVirtualKeyCode);
	}
	else
		testHotKey();

}

void InputEventAndControl::testControlKeyState()
{
	DWORD dwState = inputRecord.Event.KeyEvent.dwControlKeyState;

	bCaps = dwState & CAPSLOCK_ON ? true : false;
	bShift = dwState & SHIFT_PRESSED ? true : false;

	bCtrl_L = dwState & LEFT_CTRL_PRESSED ? true : false;
	bCtrl_R = dwState & RIGHT_CTRL_PRESSED ? true : false;
	bAlt_L = dwState & LEFT_ALT_PRESSED ? true : false;
	bAlt_R = dwState & RIGHT_ALT_PRESSED ? true : false;
}

void InputEventAndControl::testHotKey()
{
	if (bAlt_L)
	{
		string filename;
		switch (inputRecord.Event.KeyEvent.wVirtualKeyCode)
		{
			case 'C':
				saveFile("_CacheFile", ".");
				break;
			case 'S':
				print_and_getFileInfomation(filename, ".txt");
				if (!filename.empty())
					saveFile(move(filename), ".txt");
				break;
			case 'R':
				print_and_getFileInfomation(filename, ".rf");
				if (!filename.empty())
					saveFile(move(filename), ".rf");
				break;
		}
	}
	else if (bAlt_R)
	{
		string filename;
		switch (inputRecord.Event.KeyEvent.wVirtualKeyCode)
		{
			case 'P':
				print_and_getFileInfomation(filename, ".h .cpp .rf");
				if (!filename.empty())
				{
					saveFile(move(filename), ".rf");
					saveFile(move(filename), ".h");
					saveFile(move(filename), ".cpp");
				}
				break;
		}
	}
	else if (bCtrl_L)
	{
		switch (inputRecord.Event.KeyEvent.wVirtualKeyCode)
		{
			case '1':
				if (bShift)
					changeColor(0x08, 'i');
				else
					changeColor(0x08, 'c');
				break;
			case '2':
				if (bShift)
					changeColor(0x09, 'i');
				else
					changeColor(0x09, 'c');
				break;
			case '3':
				if (bShift)
					changeColor(0x0a, 'i');
				else
					changeColor(0x0a, 'c');
				break;
			case '4':
				if (bShift)
					changeColor(0x0b, 'i');
				else
					changeColor(0x0b, 'c');
				break;
			case '5':
				if (bShift)
					changeColor(0x0c, 'i');
				else
					changeColor(0x0c, 'c');
				break;
			case '6':
				if (bShift)
					changeColor(0x0d, 'i');
				else
					changeColor(0x0d, 'c');
				break;
			case '7':
				if (bShift)
					changeColor(0x0e, 'i');
				else
					changeColor(0x0e, 'c');
				break;
			case '8':
				if (bShift)
					changeColor(0x0f, 'i');
				else
					changeColor(0x0f, 'c');
				break;
			case VK_F1:
				if (bShift)
					changeColor(0x07, 'i');
				else
					changeColor(0x07, 'c');
				break;
			case VK_F2:
				if (bShift)
					changeColor(0x06, 'i');
				else
					changeColor(0x06, 'c');
				break;
			case VK_F3:
				if (bShift)
					changeColor(0x05, 'i');
				else
					changeColor(0x05, 'c');
				break;
			case VK_F4:
				if (bShift)
					changeColor(0x04, 'i');
				else
					changeColor(0x04, 'c');
				break;
			case VK_F5:
				if (bShift)
					changeColor(0x03, 'i');
				else
					changeColor(0x03, 'c');
				break;
			case VK_F6:
				if (bShift)
					changeColor(0x02, 'i');
				else
					changeColor(0x02, 'c');
				break;
			case VK_F7:
				if (bShift)
					changeColor(0x01, 'i');
				else
					changeColor(0x01, 'c');
				break;
			case VK_OEM_3:
				if (bShift)
					CCI.setDefault();
				break;
		}
	}
}

bool InputEventAndControl::isPrintableCharacter(WORD &wVirtualKeyCode)
{
	if (wVirtualKeyCode == 32)
		return true;
	else if (wVirtualKeyCode >= 48 && wVirtualKeyCode <= 57)
		return true;
	else if (wVirtualKeyCode >= 65 && wVirtualKeyCode <= 90)
		return true;
	else if (wVirtualKeyCode >= 96 && wVirtualKeyCode <= 105)
		return true;
	else if (wVirtualKeyCode >= 106 && wVirtualKeyCode <= 111 && wVirtualKeyCode != 108)
		return true;
	else if (wVirtualKeyCode >= 186 && wVirtualKeyCode <= 192)
		return true;
	else if (wVirtualKeyCode >= 219 && wVirtualKeyCode <= 222)
		return true;
	return false;
}

void InputEventAndControl::changeCharacter(WORD &wVirtualKeyCode)
{
	char ch = 'a' - 'A';

	if (wVirtualKeyCode >= 'A' && wVirtualKeyCode <= 'Z')
		OutputCharacter[LEFT] = ch + wVirtualKeyCode, OutputCharacter[RIGHT] = wVirtualKeyCode;
	else if (wVirtualKeyCode >= 96 && wVirtualKeyCode <= 105)
		OutputCharacter[LEFT] = OutputCharacter[RIGHT] = wVirtualKeyCode - '0';
	else if (wVirtualKeyCode >= 106 && wVirtualKeyCode <= 111 && wVirtualKeyCode != 108)
		OutputCharacter[LEFT] = OutputCharacter[RIGHT] = wVirtualKeyCode - 64;
	else if (wVirtualKeyCode == 32)
		OutputCharacter[LEFT] = OutputCharacter[RIGHT] = 0;
	else
	{
		if (!bCaps)
		{
			downCharacter(wVirtualKeyCode);
		}
		else
		{
			upCharacter(wVirtualKeyCode);
		}
	}
}

void InputEventAndControl::upCharacter(WORD &wVirtualKeyCode)
{
	switch (wVirtualKeyCode)
	{
		case '1': OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '!'; break;
		case '2': OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '@'; break;
		case '3': OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '#'; break;
		case '4': OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '$'; break;
		case '5': OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '%'; break;
		case '6': OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '^'; break;
		case '7': OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '&'; break;
		case '8': OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '*'; break;
		case '9': OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '('; break;
		case '0': OutputCharacter[LEFT] = OutputCharacter[RIGHT] = ')'; break;

		case 186: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = ':'; break;
		case 187: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '+'; break;
		case 188: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '<'; break;
		case 189: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '_'; break;
		case 190: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '>'; break;
		case 191: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '?'; break;
		case 192: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '~'; break;

		case 219: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '{'; break;
		case 220: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '|'; break;
		case 221: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '}'; break;
		case 222: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '"'; break;
	}
}

void InputEventAndControl::downCharacter(WORD &wVirtualKeyCode)
{
	if (wVirtualKeyCode >= '0' && wVirtualKeyCode <= '9')
		OutputCharacter[LEFT] = OutputCharacter[RIGHT] = wVirtualKeyCode;
	else
		switch (wVirtualKeyCode)
		{
			case 186: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = ';'; break;
			case 187: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '='; break;
			case 188: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = ','; break;
			case 189: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '-'; break;
			case 190: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '.'; break;
			case 191: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '/'; break;
			case 192: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '`'; break;

			case 219: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '['; break;
			case 220: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '\\'; break;
			case 221: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = ']'; break;
			case 222: OutputCharacter[LEFT] = OutputCharacter[RIGHT] = '\''; break;
		}
}

void InputEventAndControl::gotoxy(COORD Pos)
{
	SetConsoleCursorPosition(hConsoleOutput, Pos);
}

void InputEventAndControl::gotoxy(short x, short y)
{
	COORD Pos = { x,y };
	gotoxy(Pos);
}

void InputEventAndControl::changeColor(WORD color, char info)
{
	switch (info)
	{
		case 'c':
			CCI.setCharacterAttribute(color);
			PrintMessage(CHARACTER);
			break;
		case 'i':
			CCI.setInfomationAttribute(color);
			PrintMessage(ALL);
			break;
	}
}

COORD & InputEventAndControl::setClearBeginPoint(COORD &first, COORD &last)
{
	if (first.X < last.X)
	{
		if (first.Y > last.Y)
			first.Y -= abs(first.Y - last.Y);
		return first;
	}
	else
	{
		if (first.Y < last.Y)
			last.Y -= abs(first.Y - last.Y);
		return last;
	}
}

void InputEventAndControl::clearBoard_NextActionTest(COORD firstPos)
{
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD inputRec;
	DWORD res;
	COORD crPos;
	while (1)
	{
		ReadConsoleInput(hIn, &inputRec, 1, &res);
		if (inputRec.EventType == MOUSE_EVENT)
		{
			crPos = inputRec.Event.MouseEvent.dwMousePosition;
			gotoxy(crHome);
			PrintMessage(CURSORPOSITION);

			if (inputRec.Event.MouseEvent.dwButtonState == FROM_LEFT_2ND_BUTTON_PRESSED)	// 按下中键 执行删除
			{
				COORD lastPos = crPos;
				int clearbuf_x = abs(firstPos.X - lastPos.X) + 1;
				int clearbuf_y = abs(firstPos.Y - lastPos.Y) + 1;

				COORD begin = setClearBeginPoint(firstPos, lastPos);
				for (int i = 0; i < clearbuf_y; i++, begin.Y++)
					FillConsoleOutputCharacter(hConsoleOutput, 0, clearbuf_x, begin, &res);
				break;
			}
			else if (inputRec.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED
					 || inputRec.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)	// 按下左右键 放弃删除
			{
				FillConsoleOutputCharacter(hConsoleOutput, 0, 1, firstPos, &dwNumberOfEventsRead);
				break;
			}
		}
		else if (inputRec.EventType == KEY_EVENT && inputRec.Event.KeyEvent.bKeyDown)
		{
			if (inputRec.Event.KeyEvent.wVirtualKeyCode == VK_DELETE)	// 按下delete 执行删除
			{
				COORD lastPos = crPos;
				int clearbuf_x = abs(firstPos.X - lastPos.X) + 1;
				int clearbuf_y = abs(firstPos.Y - lastPos.Y) + 1;

				COORD begin = setClearBeginPoint(firstPos, lastPos);
				for (int i = 0; i < clearbuf_y; i++, begin.Y++)
					FillConsoleOutputCharacter(hConsoleOutput, 0, clearbuf_x, begin, &res);
				break;
			}
			else break;	// 按下其它键 放弃删除
		}
	}
}

void InputEventAndControl::saveFile(string &&filename, string &&filetype)
{
	CONSOLE_SCREEN_BUFFER_INFO buf;
	GetConsoleScreenBufferInfo(hConsoleOutput, &buf);
	string FileName = filename + filetype;
	ofstream outfile(FileName, ios::out);

	if (!outfile)
	{
		string Wrong = "SAVE WRONG!!! PLEASE CHECK YOUR FILE NAME.(Press any keys to continue)";
		gotoxy({ 0,1 });
		cout << Wrong;
		_getch();
		FillConsoleOutputCharacter(hConsoleOutput, 0, Wrong.size(), { 0,1 }, &dwNumberOfEventsRead);
		gotoxy(crPos);
		return;
	}

	FillConsoleOutputCharacter(hConsoleOutput, ' ', 1, { 75, 0 }, &dwNumberOfEventsRead);
	switch (filetype[1])
	{
		case 'c':	CCI.output_CPP_File(outfile, filename);		break;
		case 't':	CCI.output_TXT_File(outfile, buf);			break;
		case 'h':	CCI.output_H_File(outfile, filename);		break;
		case 'r':	CCI.output_RF_File(outfile, buf, filename);	break;
		default:	CCI.output_TXT_File(outfile, buf);			break;
	}
}

void InputEventAndControl::print_and_getFileInfomation(string &filename, string &&filetype)
{
	COORD start = { 0,1 };
	DWORD rec;
	bool cancel = false;
	gotoxy(start);

	CCI.setInfomationAttribute(0);
	CCI.showCursor();
	cout << "File Name: " << filetype;

	for (short i = 0;;)
	{
		gotoxy(11 + i, 1);

		int ch = _getch();
		if (isprint(ch) && filename.size() <= 30)
		{
			if (ch == '/' || ch == '\\' || ch == ':' || ch == '*' || ch == '?' || ch == '"' || ch == '<' || ch == '>' || ch == '|')
				continue;

			char record_ch = ch;
			filename.push_back(record_ch);
			printFileType(filename, filetype);
			i++;
		}
		else if (ch == '\b')
		{
			if (i > 0)
			{
				switch (filetype[1])
				{
					case 'h':
						FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), 0, 3, { 11 + 3 * i + 8,1 }, &rec);
						break;
					case 't':
						FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), 0, 1, { 11 + i + 3,1 }, &rec);
						break;
					case 'r':
						FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), 0, 1, { 11 + i + 2,1 }, &rec);
						break;
				}

				filename.pop_back();
				printFileType(filename, filetype);
				i--;
			}
		}
		else if (ch == '\r')
			break;
		else if (ch == 27)
		{
			cancel = true;
			break;
		}
	}

	filetype += "File Name: ";
	CCI.hideCursor();
	if (filetype[1] == 'h')
		FillConsoleOutputCharacter(hConsoleOutput, 0, 3 * filename.size() + filetype.size(), start, &dwNumberOfEventsRead);
	else
		FillConsoleOutputCharacter(hConsoleOutput, 0, filename.size() + filetype.size(), start, &dwNumberOfEventsRead);

	if (cancel)
		filename.clear();

	gotoxy(crPos);
}

void InputEventAndControl::printFileType(string &filename, string &filetype)
{
	gotoxy(11, 1);

	switch (filetype[1])
	{
		case 'h':
			cout << filename << filetype.substr(0, 3) << filename << filetype.substr(3, 5) << filename << filetype.substr(8, 3);
			break;
		case 't': case 'r':
			cout << filename << filetype;
			break;
	}
}

void InputEventAndControl::tip()
{
	FillConsoleOutputAttribute(hConsoleOutput, FOREGROUND_INTENSITY | FOREGROUND_RED, 1, { 75, 0 }, &dwNumberOfEventsRead);
	FillConsoleOutputCharacter(hConsoleOutput, '*', 1, { 75, 0 }, &dwNumberOfEventsRead);
}