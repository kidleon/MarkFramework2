/*
Copyright (C) 2017 Electronic Arts Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
	its contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
https://github.com/electronicarts/EAStdC
*/
#ifndef __CTYPE_H__
#define __CTYPE_H__


static constexpr size_t STDC_WCMAP_SIZE = 256;

MARKENGINE_C_API uint8_t STDC_WCTYPE_MAP[STDC_WCMAP_SIZE];
MARKENGINE_C_API uint8_t STDC_WLOWER_MAP[STDC_WCMAP_SIZE];
MARKENGINE_C_API uint8_t STDC_WUPPER_MAP[STDC_WCMAP_SIZE];

#define STDC_WCTYPE_CONTROL_1    0x01
#define STDC_WCTYPE_MOTION       0x02
#define STDC_WCTYPE_SPACE_1      0x04
#define STDC_WCTYPE_PUNCT        0x08
#define STDC_WCTYPE_DIGIT        0x10
#define STDC_WCTYPE_XDIGIT       0x20
#define STDC_WCTYPE_LOWER        0x40
#define STDC_WCTYPE_UPPER        0x80
#define STDC_WCTYPE_ALPHA        (STDC_WCTYPE_LOWER     | STDC_WCTYPE_UPPER)
#define STDC_WCTYPE_ALNUM        (STDC_WCTYPE_ALPHA     | STDC_WCTYPE_DIGIT)
#define STDC_WCTYPE_GRAPH        (STDC_WCTYPE_ALNUM     | STDC_WCTYPE_PUNCT)
#define STDC_WCTYPE_PRINT        (STDC_WCTYPE_GRAPH     | STDC_WCTYPE_SPACE_1)
#define STDC_WCTYPE_SPACE        (STDC_WCTYPE_SPACE_1   | STDC_WCTYPE_MOTION)
#define STDC_WCTYPE_CONTROL      (STDC_WCTYPE_CONTROL_1 | STDC_WCTYPE_MOTION)

/**
	* @brief 문자나 숫자인지 검사한다. 문자나 숫자일 경우 0이 아닌 수를 반환한다.
	*/
inline int _isalnum(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_ALNUM;
}

inline int _isalnum(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_ALNUM) : 0);
}

inline int _isalnum(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_ALNUM) : 0);
}

inline int _isalnum(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_ALNUM) : 0);
}

/**
* @brief 알파벳 문자일경우 0이 아닌 수를 발환한다.
*/
inline int _isalpha(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_ALPHA;
}

inline int _isalpha(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_ALPHA) : 0);
}

inline int _isalpha(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_ALPHA) : 0);
}

inline int _isalpha(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_ALPHA) : 0);
}

/**
* @brief 숫자일경우 0이 아닌 수를 발환한다.
*/
inline int _isdist(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_DIGIT;
}

inline int _isdist(wchar_t c)
{
	return (((uint32_t)c - '0') < 10) ? 1 : 0;
}

inline int _isdist(char16_t c)
{
	return (((unsigned)c - '0') < 10) ? 1 : 0;
}

inline int _isdist(char32_t c)
{
	return (((uint32_t)c - '0') < 10) ? 1 : 0;
}

/**
* @brief 16진수 숫자일경우 0이 아닌 수를 발환한다.
*/
inline int _isxdist(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_XDIGIT;
}

inline int _isxdist(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_XDIGIT) : 0);
}

inline int _isxdist(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_XDIGIT) : 0);
}

inline int _isxdist(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_XDIGIT) : 0);
}


/**
* @brief c가 출력할수 있는 ASCII문자인지 판단한다. 0이 아닌수를 반환
*/
inline int _isgraph(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_GRAPH;
}

inline int _isgraph(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_GRAPH) : 0);
}

inline int _isgraph(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_GRAPH) : 0);
}

inline int _isgraph(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_GRAPH) : 0);
}


/**
* @brief 소문자인지 판별하여 0이 아닌 수를 반환한다.
*/
inline int _islower(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_LOWER;
}

inline int _islower(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_LOWER) : 0);
}

inline int _islower(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_LOWER) : 0);
}

inline int _islower(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_LOWER) : 0);
}

/**
* @brief c를 소문자로 변경하여 반환한다. StandardC의 _tolower와 구분하기 위해 _를 하나 더 추가하였다.
*/
inline char __tolower(char c)
{
	return (char)STDC_WLOWER_MAP[(uint8_t)c];
}

inline wchar_t __tolower(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (wchar_t)STDC_WLOWER_MAP[(char16_t)c] : c);
}

inline char16_t __tolower(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (char16_t)STDC_WLOWER_MAP[(char16_t)c] : c);
}

inline char32_t __tolower(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (char32_t)STDC_WLOWER_MAP[(char16_t)c] : c);
}

/**
* @brief c가 대문자인지 판별하여 0이 아닌 수를 반환한다.
*/
inline int _isupper(char c)
{
	return (char)STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_UPPER;
}

inline int _isupper(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_UPPER) : 0);
}

inline int _isupper(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_UPPER) : 0);
}

inline int _isupper(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_UPPER) : 0);
}

/**
* @brief c를 대문자로 변경하여 반환한다. StandardC의 _tolower와 구분하기 위해 _를 하나 더 추가하였다.
*/
inline char __toupper(char c)
{
	return (char)STDC_WUPPER_MAP[(uint8_t)c];
}

inline wchar_t __toupper(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (wchar_t)STDC_WUPPER_MAP[(char16_t)c] : c);
}

inline char16_t __toupper(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (char16_t)STDC_WUPPER_MAP[(char16_t)c] : c);
}

inline char32_t __toupper(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (char32_t)STDC_WUPPER_MAP[(char16_t)c] : c);
}

inline int _isprint(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_PRINT;
}

inline int _isprint(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_PRINT) : 0);
}

inline int _isprint(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_PRINT) : 0);
} 

inline int _isprint(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_PRINT) : 0);
}

/**
* @brief 입력된 문자가 구두문자인지 아닌지 판단합니다.
* 구두문자란 아스키 코드값이 33 - 47, 58 - 64, 91 - 96, 123 - 126인 문자로써
* 예를 들면 입력된 문자가 !@#$ % ^&*() 등과 같은 문자를 말합니다.
* 숫자와 알파벳을 제외한 문자를 말하는 것입니다.
* 구두문자이면 0이 아닌 값을 반환합니다.
*/
inline int _ispunct(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_PUNCT;
}

inline int _ispunct(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_PUNCT) : 0);
}

inline int _ispunct(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_PUNCT) : 0);
}

inline int _ispunct(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_PUNCT) : 0);
}


/**
* @brief 입력된 문자가 공백인지 확인합니다. 공백일 경우 0이 아닌 값을 반환합니다.
*/
inline int _isspace(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_SPACE;
}

inline int _isspace(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_SPACE) : 0);
}

inline int _isspace(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_SPACE) : 0);
}

inline int _isspace(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_SPACE) : 0);
}


/**
* @brief 입력된 문자가 제어문자(\\n,\\t,\\a,\\b)문자인지 확인합니다. 공백일 경우 0이 아닌 값을 반환합니다.
*/
inline int _iscntrl(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_CONTROL;
}

inline int _iscntrl(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_CONTROL) : 0);
}

inline int _iscntrl(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_CONTROL) : 0);
}

inline int _iscntrl(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_CONTROL) : 0);
}


/**
* @brief 입력된 문자가 ASCII문자인지 확인합니다. ASCII일 경우 0이 아닌 값을 반환합니다.
*/
inline int _isascii(char c)
{
	return (uint8_t)c < 0x80;
}

inline int _isascii(wchar_t c)
{
	return (uint8_t)c < 0x80;
}

inline int _isascii(char16_t c)
{
	return (uint8_t)c < 0x80;
}

inline int _isascii(char32_t c)
{
	return (uint8_t)c < 0x80;
}

#endif // __CTYPE_H__