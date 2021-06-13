#pragma once
#include <stdio.h>
#include <conio.h>
#include <iostream>
using namespace std;
class ChucNang
{
public:
	ChucNang();
public:
	void GotoXY(int x, int y);
	void TextColor(int x);
	void Xoa(int x, int y);
	void Deletec();
	string ConvertInt(int number);
};