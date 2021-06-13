#include "stdafx.h"
#include "VeKhung.h"
#include "ChucNang.h"
#include <iostream>
VeKhung::VeKhung()
{
}
VeKhung::VeKhung(int x, int y, int dai, int rong, string chuoi)
{
	this->x = x;
	this->y = y;
	this->ngang = dai;
	this->cao = rong;
	this->chuoi = chuoi;
}

void VeKhung::Ve()
{
	ChucNang cn = ChucNang();
	cn.GotoXY(this->x, this->y);

	printf("%c", 218);
	for (int i = 1; i < this->ngang - 1; i++) printf("%c", 196);
	printf("%c", 191);

	for (int i = 1; i < this->cao - 1; i++)
	{
		cn.GotoXY(this->x, this->y + i);
		printf("%c", 179);
		for (int j = 1; j < ngang - 1; j++)printf(" ");
		printf("%c", 179);
	}

	cn.GotoXY(this->x, this->y + this->cao - 1);

	printf("%c", 192);
	for (int i = 1; i < this->ngang - 1; i++) printf("%c", 196);
	printf("%c", 217);

	InChuoi();
}

void VeKhung::InChuoi()
{
	ChucNang cn = ChucNang();
	cn.GotoXY(this->x + 1, this->y + 1);

	int dem = 1;
	int viTri = 1;
	while ((viTri - 1)<this->chuoi.length())
	{
		cout << chuoi[viTri - 1];
		if (viTri % (this->ngang - 2) == 0)
		{
			dem++;
			cn.GotoXY(this->x + 1, this->y + dem);
		}
		viTri++;
	}

}

void VeKhung::DoiMauKhung(int mau)
{
	this->mau = mau;
	ChucNang cn = ChucNang();
	cn.TextColor(mau);
	this->Ve();
}

void VeKhung::XoaToanBo()
{
	ChucNang cn = ChucNang();
	cn.TextColor(this->mau);
	cn.GotoXY(this->x, this->y);
	for (int i = 0; i < this->cao; i++)
	{
		for (int j = 0; j < this->ngang; j++)
		{
			cn.GotoXY(this->x + i, this->y + j);
			printf(" ");
		}
	}

}

void VeKhung::SetChuoi(string chuoi)
{
	this->chuoi = chuoi;
	this->XoaToanBo();
	this->Ve();
}

void VeKhung::MauMacDinh()
{
	ChucNang cn = ChucNang();
	cn.TextColor(15);
	this->Ve();
	//cn.TextColor(15);
	//this->InChuoi();
}
