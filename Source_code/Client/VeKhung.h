#pragma once
//#include "VeKhung.cpp"
#include <stdio.h>
#include <string>

using namespace std;

class VeKhung
{
private:
	int x;
	int y;
	int ngang;
	int cao;
	int mau = 2;
	string chuoi;
public:
	VeKhung();
	VeKhung(int x, int y, int dai, int rong, string chuoi);
public:
	void Ve();
	void InChuoi();
	void DoiMauKhung(int mau);
	void XoaToanBo();
	void SetChuoi(string chuoi);
	void MauMacDinh();
};