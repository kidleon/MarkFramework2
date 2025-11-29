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


#define STDC_WCMAP_SIZE 256

MARK_BASELIB_C_API uint8_t STDC_WCTYPE_MAP[STDC_WCMAP_SIZE];
MARK_BASELIB_C_API uint8_t STDC_WLOWER_MAP[STDC_WCMAP_SIZE];
MARK_BASELIB_C_API uint8_t STDC_WUPPER_MAP[STDC_WCMAP_SIZE];

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
static inline int _isalnum(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_ALNUM;
}

static inline int _isalnumw(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_ALNUM) : 0);
}

static inline int _isalnum16(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_ALNUM) : 0);
}

static inline int _isalnum32(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_ALNUM) : 0);
}

/**
* @brief 알파벳 문자일경우 0이 아닌 수를 발환한다.
*/
static inline int _isalpha(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_ALPHA;
}

static inline int _isalphaw(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_ALPHA) : 0);
}

static inline int _isalpha16(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_ALPHA) : 0);
}

static inline int _isalpha32(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_ALPHA) : 0);
}

/**
* @brief 숫자일경우 0이 아닌 수를 발환한다.
*/
static inline int _isdist(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_DIGIT;
}

static inline int _isdistw(wchar_t c)
{
	return (((uint32_t)c - '0') < 10) ? 1 : 0;
}

static inline int _isdist16(char16_t c)
{
	return (((unsigned)c - '0') < 10) ? 1 : 0;
}

static inline int _isdist32(char32_t c)
{
	return (((uint32_t)c - '0') < 10) ? 1 : 0;
}

/**
* @brief 16진수 숫자일경우 0이 아닌 수를 발환한다.
*/
static inline int _isxdist(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_XDIGIT;
}

static inline int _isxdistw(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_XDIGIT) : 0);
}

static inline int _isxdist16(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_XDIGIT) : 0);
}

static inline int _isxdist32(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_XDIGIT) : 0);
}


/**
* @brief c가 출력할수 있는 ASCII문자인지 판단한다. 0이 아닌수를 반환
*/
static inline int _isgraph(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_GRAPH;
}

static inline int _isgraphw(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_GRAPH) : 0);
}

static inline int _isgraph16(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_GRAPH) : 0);
}

static inline int _isgraph32(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_GRAPH) : 0);
}


/**
* @brief 소문자인지 판별하여 0이 아닌 수를 반환한다.
*/
static inline int _islower(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_LOWER;
}

static inline int _islowerw(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_LOWER) : 0);
}

static inline int _islower16(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_LOWER) : 0);
}

static inline int _islower32(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_LOWER) : 0);
}

/**
* @brief c를 소문자로 변경하여 반환한다. StandardC의 _tolower와 구분하기 위해 _를 하나 더 추가하였다.
*/
static inline char __tolower(char c)
{
	return (char)STDC_WLOWER_MAP[(uint8_t)c];
}

static inline wchar_t __tolowerw(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (wchar_t)STDC_WLOWER_MAP[(char16_t)c] : c);
}

static inline char16_t __tolower16(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (char16_t)STDC_WLOWER_MAP[(char16_t)c] : c);
}

static inline char32_t __tolower32(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (char32_t)STDC_WLOWER_MAP[(char16_t)c] : c);
}

/**
* @brief c가 대문자인지 판별하여 0이 아닌 수를 반환한다.
*/
static inline int _isupper(char c)
{
	return (char)STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_UPPER;
}

static inline int _isupperw(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_UPPER) : 0);
}

static inline int _isupper16(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_UPPER) : 0);
}

static inline int _isupper32(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_UPPER) : 0);
}

/**
* @brief c를 대문자로 변경하여 반환한다. StandardC의 _tolower와 구분하기 위해 _를 하나 더 추가하였다.
*/
static inline char __toupper(char c)
{
	return (char)STDC_WUPPER_MAP[(uint8_t)c];
}

static inline wchar_t __toupperw(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (wchar_t)STDC_WUPPER_MAP[(char16_t)c] : c);
}

static inline char16_t __toupper16(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (char16_t)STDC_WUPPER_MAP[(char16_t)c] : c);
}

static inline char32_t __toupper32(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (char32_t)STDC_WUPPER_MAP[(char16_t)c] : c);
}

static inline int _isprint(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_PRINT;
}

static inline int _isprintw(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_PRINT) : 0);
}

static inline int _isprint16(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_PRINT) : 0);
} 

static inline int _isprint32(char32_t c)
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
static inline int _ispunct(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_PUNCT;
}

static inline int _ispunctw(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_PUNCT) : 0);
}

static inline int _ispunct16(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_PUNCT) : 0);
}

static inline int _ispunct32(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_PUNCT) : 0);
}


/**
* @brief 입력된 문자가 공백인지 확인합니다. 공백일 경우 0이 아닌 값을 반환합니다.
*/
static inline int _isspace(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_SPACE;
}

static inline int _isspacew(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_SPACE) : 0);
}

static inline int _isspace16(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_SPACE) : 0);
}

static inline int _isspace32(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_SPACE) : 0);
}


/**
* @brief 입력된 문자가 제어문자(\\n,\\t,\\a,\\b)문자인지 확인합니다. 공백일 경우 0이 아닌 값을 반환합니다.
*/
static inline int _iscntrl(char c)
{
	return STDC_WCTYPE_MAP[(uint8_t)c] & STDC_WCTYPE_CONTROL;
}

static inline int _iscntrlw(wchar_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_CONTROL) : 0);
}

static inline int _iscntrl16(char16_t c)
{
	return (((uint16_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_CONTROL) : 0);
}

static inline int _iscntrl32(char32_t c)
{
	return (((uint32_t)c < STDC_WCMAP_SIZE) ? (STDC_WCTYPE_MAP[c] & STDC_WCTYPE_CONTROL) : 0);
}


/**
* @brief 입력된 문자가 ASCII문자인지 확인합니다. ASCII일 경우 0이 아닌 값을 반환합니다.
*/
static inline int _isascii(char c)
{
	return (uint8_t)c < 0x80;
}

static inline int _isasciiw(wchar_t c)
{
	return (uint8_t)c < 0x80;
}

static inline int _isascii16(char16_t c)
{
	return (uint8_t)c < 0x80;
}

static inline int _isascii32(char32_t c)
{
	return (uint8_t)c < 0x80;
}

#endif // __CTYPE_H__