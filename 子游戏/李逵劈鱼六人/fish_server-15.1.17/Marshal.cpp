#include "stdafx.h"
#include "Marshal.h"

Marshal::Marshal(FILE* file) {
	m_lpFile = file;
}

Marshal::~Marshal() {
	m_lpFile = NULL;
}

int Marshal::loadInt() {
	int result = 0;
	fread(&result, sizeof(int), 1, m_lpFile);
	return result;
}
LONGLONG Marshal::loadLong() {
	LONGLONG result = 0x0000000000000000;
	fread(&result, sizeof(LONGLONG), 1, m_lpFile);
	return result;
}
float Marshal::loadFloat() {
	float result = 0.0f;
	fread(&result, sizeof(float), 1, m_lpFile);
	return result;
}
double Marshal::loadDouble() {
	double result = 0.0;
	fread(&result, sizeof(double), 1, m_lpFile);
	return result;
}
int Marshal::loadString(TCHAR* ret) {
	BYTE len = 0;
	fread(&len, sizeof(BYTE), 1, m_lpFile);
	ret[len] = 0;
	fread(ret, sizeof(TCHAR), len, m_lpFile);
	return len;
}
BYTE Marshal::loadByte() {
	BYTE result = 0;
	fread(&result, sizeof(BYTE), 1, m_lpFile);
	return result;
}
void Marshal::loadBytes(int length, BYTE* ret) {
	fread(ret, sizeof(BYTE), length, m_lpFile);
}