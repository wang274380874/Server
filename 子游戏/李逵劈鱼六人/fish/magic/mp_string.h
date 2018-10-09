//-----------------------------------------------------------------------------
// File: mp_string.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_STRING
#define MAGIC_STRING

#include <stdio.h>

#pragma warning ( disable : 4996)

class MP_String
{
protected:
	char* theString;		// eng: pointer to string
							// rus: указатель на строку

	int stringLen;			// eng: string length
							// rus: длина строки
   
public:
	// eng: Constructors
	// rus: Консрукторы
	MP_String();
	MP_String(char s);
	MP_String (const char*);
	MP_String(const MP_String&);

	// eng: Destructor
	// rus: Деструктор
	virtual ~MP_String(void);

	MP_String& operator=(const MP_String&);
	MP_String& operator=(char* s);
	bool operator==(const MP_String&);
	bool operator!=(const MP_String&);
	bool operator>(const MP_String&);
	bool operator<(const MP_String&);
	char& operator[](int n);
	MP_String operator+(const MP_String& so);
	MP_String operator+(const char c);
	MP_String& operator+=(const MP_String& so);
	MP_String& operator+=(const char c);

	// eng: Loading string from stream
	// rus: Чтение строки из потока
	void LoadFromStream(FILE* stream);

	// eng: Save string to stream
	// rus: Запись строки в поток
	void SaveToStream(FILE* stream);

	// eng: Save string to stream as text
	// rus: Запись строки в поток в виде текста
	void SaveToStreamByText(FILE* stream);

	#ifndef MP_STRING_LITE
	// eng: Reversing
	// rus: Реверсирование
	MP_String& operator~(void);
	#endif

	// eng: String length
	// rus: Длина строки
	int length(void);

	#ifndef MP_STRING_LITE
	// eng: Transformation to capital letters
	// rus: Преобразование в прописные буквы
	MP_String& toUpper(void);

	// eng: Transformation to small letters
	// rus: Преобразование в строчные буквы
	MP_String& toLower(void);
	#endif

	// eng: Find symbol in string
	// rus: Поиск символа в строке
	int find(char c);

	// eng: Find substring in string
	// rus: Поиск подстроки в строке
	int find(const char* subs);
	int find(const MP_String& so);

	// eng: String pointer
	// rus: Адрес строки
	const char* c_str();

	// eng: Returns substring using specified position and length
	// rus: Получение подстроки из данной строки с заданной позиции и известным числом символов
	MP_String substr(int pos,int plen);

	// eng: Left substring
	// rus: Левая подстрока
	MP_String left(int len);

	// eng: Rigth substring
	// rus: Правая подстроки
	MP_String right(int len);

	bool empty();
};

class MP_StringW
{
protected:
	wchar_t* theString;		// eng: pointer to string
							// rus: указатель на строку

	int stringLen;			// eng: string length
							// rus: длина строки
   
public:
	// eng: Constructors
	// rus: Консрукторы
	MP_StringW();
	MP_StringW(char s);
	MP_StringW(const char* s);
	MP_StringW(const wchar_t*);
	MP_StringW(const MP_StringW&);

	// eng: Destructor
	// rus: Деструктор
	virtual ~MP_StringW(void);

	MP_StringW& operator=(const MP_StringW&);
	MP_StringW& operator=(const wchar_t* s);
	MP_StringW& operator=(const char* s);
	bool operator==(const MP_StringW&);
	bool operator!=(const MP_StringW&);
	bool operator>(const MP_StringW&);
	bool operator<(const MP_StringW&);
	MP_StringW operator+(const MP_StringW& so);
	MP_StringW& operator+=(const MP_StringW& so);

	// eng: Loading string from stream
	// rus: Чтение строки из потока
	void LoadFromStream(FILE* stream);

	// eng: Save string to stream
	// rus: Запись строки в поток
	void SaveToStream(FILE* stream);

	#ifndef MP_STRING_LITE
	// eng: Reversing
	// rus: Реверсирование
	MP_StringW& operator~(void);
	#endif
	
	// eng: String length
	// rus: Длина строки
	int length(void);

	#ifndef MP_STRING_LITE
	// eng: Transformation to capital letters
	// rus: Преобразование в прописные буквы
	MP_StringW& toUpper(void);

	// eng: Transformation to small letters
	// rus: Преобразование в строчные буквы
	MP_StringW& toLower(void);
	#endif

	// eng: Find substring in string
	// rus: Поиск подстроки в строке
	int find(const wchar_t* subs);
	int find(const MP_StringW& so);

	// eng: String pointer
	// rus: Адрес строки
	const wchar_t* w_str();

	bool empty();
};


#endif
