#include "stdafx.h"
#include "ChucNang.h"
#include <windows.h>

ChucNang::ChucNang()
{

}

void ChucNang::GotoXY(int x, int y)
{
	HANDLE hConsoleOutput;
	COORD Cursor_an_Pos = { x - 1,y - 1 };
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hConsoleOutput, Cursor_an_Pos);
}

void ChucNang::TextColor(int x)
{
	HANDLE mau;
	mau = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(mau, x);
}

void ChucNang::Xoa(int x, int y)
{
	TextColor(1);
	GotoXY(x, y);
	printf(" ");
}

void ChucNang::Deletec()
{
	CONSOLE_CURSOR_INFO Info;
	Info.bVisible = FALSE;
	Info.dwSize = 20;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Info);
}

string ChucNang::ConvertInt(int number)
{
	if (number == 0)
		return "0";
	string temp = "";
	string returnvalue = "";
	while (number>0)
	{
		temp += number % 10 + 48;
		number /= 10;
	}
	for (int i = 0; i<temp.length(); i++)
		returnvalue += temp[temp.length() - i - 1];
	return returnvalue;
}
