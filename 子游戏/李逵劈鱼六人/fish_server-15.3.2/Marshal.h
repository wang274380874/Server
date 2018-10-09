#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <tchar.h>

#pragma once
#ifndef _C_MASHAL_H_

class Marshal {
private :
	FILE* m_lpFile;
public :
	Marshal(FILE* file);
	~Marshal();
	void dump(int value);
	void dump(LONGLONG value);
	void dump(float value);
	void dump(double value);
	void dump(TCHAR* value);
	void dump(BYTE value);
	void dump(int length, BYTE* value);
	int loadInt();
	LONGLONG loadLong();
	float loadFloat();
	double loadDouble();
	int loadString(TCHAR* ret);
	BYTE loadByte();
	void loadBytes(int length, BYTE* ret);
};
#endif