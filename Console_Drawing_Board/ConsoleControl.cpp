#include "ConsoleControl.h"
#include <fstream>
#include <iostream>
using namespace std;

ConsoleControl::ConsoleControl()
{
	graph_l = graph_w = 0;
	the_num_of_ch = 0;
	infomationtext_color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	printcharacter_color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	default_color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
}

void ConsoleControl::hideCursor()
{
	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 }; // {光标大小百分比 ，光标是否可见}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void ConsoleControl::showCursor()
{
	CONSOLE_CURSOR_INFO cursor_info = { 20, 1 }; // {光标大小百分比 ，光标是否可见}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void ConsoleControl::readConcoleLineCharacter(COORD &crPos, string &record, int size)
{
	char *buf = new char[size + 1];
	memset(buf, 0, size + 1);
	DWORD rec = 0;
	ReadConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), buf, size, crPos, &rec);
	buf[size] = 0;
	record = buf;
	delete[] buf;
}

void ConsoleControl::setDefault()
{
	infomationtext_color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	printcharacter_color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
}

void ConsoleControl::setInfomationAttribute(WORD infomation)
{
	if (infomation != 0)
		infomationtext_color = infomation;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), infomationtext_color);
}

void ConsoleControl::setCharacterAttribute(WORD character)
{
	if (character != 0)
		printcharacter_color = character;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), printcharacter_color);
}

WORD ConsoleControl::getInfomationAttribute()
{
	return infomationtext_color;
}

WORD ConsoleControl::getCharacterAttribute()
{
	return printcharacter_color;
}

void ConsoleControl::output_TXT_File(ofstream &outfile, CONSOLE_SCREEN_BUFFER_INFO &consolebuf)
{
	string record;
	COORD start = { 0,0 };

	for (int i = 0; i < consolebuf.dwMaximumWindowSize.Y; i++)
	{
		readConcoleLineCharacter(start, record, consolebuf.dwMaximumWindowSize.X);
		outfile << record << endl;
		start.Y++;
	}
}

void ConsoleControl::output_CPP_File(ofstream &outfile, string filename)
{
	outfile
		<< "#include \"" << filename << ".h\"\n"
		<< "#include <fstream>\n"
		<< "using namespace std;\n"
		<< "\n"
		<< filename << "::" << filename << "()\n"
		<< "{\n"
		<< "	ifstream getgraph(\"" << filename << ".rf\", ios::in);\n"
		<< "	ifstream getcolor(\"" << filename << ".color\", ios::in | ios::binary);\n"
		<< "\n"
		<< "	centerPos.X = " << ( graph_l - 1 ) / 2 << ", centerPos.Y = " << ( graph_w - 1 ) / 2 << ";\n"
		<< "	graph.size.X = " << graph_l << ", graph.size.Y = " << graph_w << ";\n"
		<< "	graph.the_num_of_ch = " << the_num_of_ch << ";\n"
		<< "	is_display = false;\n"
		<< "\n"
		<< "	char ch_buf;\n"
		<< "	WORD color_buf;\n"
		<< "	int count = 0;\n"
		<< "	for (int i = 0; i < graph.size.Y; i++)\n"
		<< "	{\n"
		<< "		for (int j = 0; j < graph.size.X; j++)\n"
		<< "		{\n"
		<< "			ch_buf = getgraph.get();\n"
		<< "			getcolor.read(reinterpret_cast<char*>( &color_buf ), sizeof(WORD));\n"
		<< "			if (ch_buf !=' ')\n"
		<< "			{\n"
		<< "				graph.data[count].ch = ch_buf;\n"
		<< "				graph.data[count].color = color_buf;\n"
		<< "				graph.data[count].chPos.X = j, graph.data[count].chPos.Y = i;\n"
		<< "				prev[count].ch = prev[count].color = 0;\n"
		<< "				prev[count].chPos = {-1, -1 };\n"
		<< "				count++;\n"
		<< "			}\n"
		<< "		}\n"
		<< "		getgraph.get();\n"
		<< "	}\n"
		<< "}\n"
		<< "\n"
		<< "COORD " << filename << "::getCenterPos()\n"
		<< "{\n"
		<< "	return centerPos;\n"
		<< "}\n"
		<< "\n"
		<< "void "<<filename<<"::displayGraph(COORD & cenPos)\n"
		<< "{\n"
		<< "	int offset_x = cenPos.X - centerPos.X;\n"
		<< "	int offset_y = cenPos.Y - centerPos.Y;\n"
		<< "	displayPos = cenPos;\n"
		<< "	is_display = true;\n"
		<< "\n"
		<< "	CONSOLE_SCREEN_BUFFER_INFO buf;\n"
		<< "	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buf);\n"
		<< "	DWORD rec;\n"
		<< "\n"
		<< "	for (int count = 0; count < graph.the_num_of_ch; count++)\n"
		<< "	{\n"
		<< "		if (graph.data[count].chPos.X + offset_x >= 0 && graph.data[count].chPos.Y + offset_y >= 0)\n"
		<< "		{\n"
		<< "			COORD drawPos = { graph.data[count].chPos.X + offset_x , graph.data[count].chPos.Y + offset_y };\n"
		<< "			ReadConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), &prev[count].color, 1, drawPos, &rec);\n"
		<< "			ReadConsoleOutputCharacterA(GetStdHandle(STD_OUTPUT_HANDLE), &prev[count].ch, 1, drawPos, &rec);\n"
		<< "			prev[count].chPos = drawPos;\n"
		<< "			FillConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), graph.data[count].color, 1, drawPos, &rec);\n"
		<< "			FillConsoleOutputCharacterA(GetStdHandle(STD_OUTPUT_HANDLE), graph.data[count].ch, 1, drawPos, &rec);\n"
		<< "		}\n"
		<< "	}\n"
		<< "\n"
		<< "	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), buf.dwCursorPosition);\n"
		<< "}\n"
		<< "\n"
		<< "void " << filename << "::hideGraph()\n"
		<< "{\n"
		<< "	if (!is_display)\n"
		<< "		return;\n"
		<< "\n"
		<< "	int offset_x = displayPos.X - centerPos.X;\n"
		<< "	int offset_y = displayPos.Y - centerPos.Y;\n"
		<< "	CONSOLE_SCREEN_BUFFER_INFO buf;\n"
		<< "	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buf);\n"
		<< "	DWORD rec;\n"
		<< "\n"
		<< "	for (int count = 0; count < graph.the_num_of_ch; count++)\n"
		<< "	{\n"
		<< "		if (prev[count].chPos.X != -1 && prev[count].chPos.Y != -1)\n"
		<< "		{\n"
		<< "			COORD hidePos = prev[count].chPos;\n"
		<< "			FillConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), prev[count].color, 1, hidePos, &rec);\n"
		<< "			FillConsoleOutputCharacterA(GetStdHandle(STD_OUTPUT_HANDLE), prev[count].ch, 1, hidePos, &rec);\n"
		<< "			prev[count].ch = prev[count].color = 0;\n"
		<< "			prev[count].chPos = { -1, -1 };\n"
		<< "		}\n"
		<< "	}\n"
		<< "\n"
		<< "	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), buf.dwCursorPosition);\n"
		<< "}\n"
		<< "\n"
		<< filename << "::~" << filename << "(){}\n";
	
}

void ConsoleControl::output_H_File(ofstream &outfile, string filename)
{
	outfile
		<< "#pragma once\n"
		<< "#include <Windows.h>\n"
		<< "\n"
		<< "class " << filename << endl
		<< "{\n"
		<< "public:\n"
		<< "	" << filename << "();\n"
		<< "\n"
		<< "	COORD getCenterPos();\n"
		<< "	void displayGraph(COORD &cenPos);\n"
		<< "	void hideGraph();\n"
		<< "\n"
		<< "	~" << filename << "();\n"
		<< "private:\n"
		<< "	COORD centerPos;\n"
		<< "	COORD displayPos;\n"
		<< "	bool is_display;\n"
		<< "\n"
		<< "	struct record\n"
		<< "	{\n"
		<< "		COORD chPos;\n"
		<< "		char ch;\n"
		<< "		WORD color;\n"
		<< "	} prev["<< the_num_of_ch << "];\n"
		<< "\n"
		<< "	struct Graph\n"
		<< "	{\n"
		<< "		COORD size;\n"
		<< "		int the_num_of_ch;\n"
		<< "		record data[" << the_num_of_ch << "];\n"
		<< "	} graph;\n"
		<< "};\n";
}

void ConsoleControl::output_RF_File(ofstream &outfile, CONSOLE_SCREEN_BUFFER_INFO &consolebuf, string filename)
{
	string record;
	COORD cutStart = { 0,1 };
	ofstream colorfile(filename + ".color", ios::out | ios::binary);

	int L = consolebuf.dwMaximumWindowSize.X, R = 0,
		U = consolebuf.dwMaximumWindowSize.Y, D = 0;
	
	the_num_of_ch = 0;
	graph_l = graph_w = 0;

	for (int i = 1; i < consolebuf.dwMaximumWindowSize.Y; i++)
	{
		readConcoleLineCharacter(cutStart, record, consolebuf.dwMaximumWindowSize.X);
		for (int j = 0; j < consolebuf.dwMaximumWindowSize.X; j++)
		{
			if (record[j] != ' ' && record[j] != 0)
			{
				if (j < L)	L = j;
				if (j > R)	R = j;
				if (i < U)	U = i;
				if (i > D)	D = i;
				the_num_of_ch++;
			}
		}
		cutStart.Y++;
	}

	graph_l = R - L + 1, graph_w = D - U + 1;
	WORD *record_color = new WORD[graph_l];
	DWORD rec;
	COORD writeStart = { L,U };
	for (int i = 0; i < graph_w; i++)
	{
		readConcoleLineCharacter(writeStart, record, graph_l);
		ReadConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), record_color, graph_l, writeStart, &rec);
		
		outfile << record << endl;
		for (short i = 0; i < graph_l; i++)
			colorfile.write(reinterpret_cast<char*>( &record_color[i] ), sizeof(WORD));

		writeStart.Y++;
	}
	delete[] record_color;
}

ConsoleControl::~ConsoleControl()
{
}
