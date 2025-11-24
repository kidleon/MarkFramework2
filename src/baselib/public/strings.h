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
#ifndef __STRINGS_H__
#define __STRINGS_H__


#define SIZE_TYPE_UNSET (size_t)~0


/**
* fstrlen
* 문자열의 길이를 반환합니다. 종료 문자 0자를 포함하지 않고 src의 길이를 반환합니다.
* @brief 문자열의 길이를 얻어옵니다.
* @param str 길이를 알고자 하는 문자열
* @return size_t NULL문자를 제외한 문자열의 길이
*/
MARKENGINE_API size_t fstrlen(
	const char* str
);

/**
* fstrlenw
* 문자열의 길이를 반환합니다. 종료 문자 0자를 포함하지 않고 src의 길이를 반환합니다.
* @brief 문자열의 길이를 얻어옵니다.
* @param str 길이를 알고자 하는 문자열
* @return size_t NULL문자를 제외한 문자열의 길이
*/
MARKENGINE_API size_t fstrlen(
	const wchar_t* str
);

/**
* fstrlen16
* 문자열의 길이를 반환합니다. 종료 문자 0자를 포함하지 않고 src의 길이를 반환합니다.
* @brief 문자열의 길이를 얻어옵니다.
* @param str 길이를 알고자 하는 문자열
* @return size_t NULL문자를 제외한 문자열의 길이
*/
MARKENGINE_API size_t fstrlen(
	const char16_t* str
);

/**
* fstrlen32
* 문자열의 길이를 반환합니다. 종료 문자 0자를 포함하지 않고 src의 길이를 반환합니다.
* @brief 문자열의 길이를 얻어옵니다.
* @param str 길이를 알고자 하는 문자열
* @return size_t NULL문자를 제외한 문자열의 길이
*/
MARKENGINE_API size_t fstrlen(
	const char32_t* str
);

/**
* fstrcpy
* @brief 문자열을 다른 버퍼에 복사 합니다.
* @param src 복사하고 싶은 소스 문자열 입니다.
* @param dest 복사하고자 하는 목적지 문자열 버퍼 입니다. 해당 버퍼는 소스문자열보다 커야 합니다.
* @return dest 문자열 버퍼를 반환합니다.
*/
MARKENGINE_API char* fstrcpy(
	char* dest, 
	const char* src
);

/**
* fstrcpyw
* @brief 문자열을 다른 버퍼에 복사 합니다.
* @param src 복사하고 싶은 소스 문자열 입니다.
* @param dest 복사하고자 하는 목적지 문자열 버퍼 입니다. 해당 버퍼는 소스문자열보다 커야 합니다.
* @return dest 문자열 버퍼를 반환합니다.
*/
MARKENGINE_API wchar_t* fstrcpy(
	wchar_t* dest, 
	const wchar_t* str
);

/**
* fstrcpy16
* @brief 문자열을 다른 버퍼에 복사 합니다.
* @param src 복사하고 싶은 소스 문자열 입니다.
* @param dest 복사하고자 하는 목적지 문자열 버퍼 입니다. 해당 버퍼는 소스문자열보다 커야 합니다.
* @return dest 문자열 버퍼를 반환합니다.
*/
MARKENGINE_API char16_t* fstrcpy(
	char16_t* dest,
	const char16_t* src
);

/**
* fstrcpy32
* @brief 문자열을 다른 버퍼에 복사 합니다.
* @param src 복사하고 싶은 소스 문자열 입니다.
* @param dest 복사하고자 하는 목적지 문자열 버퍼 입니다. 해당 버퍼는 소스문자열보다 커야 합니다.
* @return dest 문자열 버퍼를 반환합니다.
*/
MARKENGINE_API char32_t* fstrcpy(
	char32_t* dest,
	const char32_t* src
);

/**
* fstrncpy
* src의 처음 n개 문자를 dest에 복사합니다. 만약 종료문자가 복사되기 전에 src 문자열이 발견되면
* dest 문자의 합이 쓰여질 때까지 후행 0 문자로 채워집니다. dest은 src의 길이가 n보다 작은 경우에만 0으로 종료됩니다.
* 이 특성은 소프트웨어 버그의 일반적인 원인이며 코드 작업시 반드시 종료 문자에 대한 작업이 필요 합니다.
* @brief n개의 문자수만큼 dest에 복사합니다.
* @param src 복사하고자 하는 문자열 소스
* @param dest 복사가 될 목적지 버퍼
* @param n 복사할 문자열 수 (주의: dest버퍼 크기를 고려하지 않습니다.)
* @result 복사된 dest 버퍼를 반환
* @code
*	char buffer[32];
*	fstrncpy(buffer, pSomeString, 32);
*	buffer[31] = 0;
* @endcode
*/
MARKENGINE_API char* fstrncpy(
	char* dest, 
	const char* src,
	size_t n
);

/**
* fstrncpyw
* src의 처음 n개 문자를 dest에 복사합니다. 만약 종료문자가 복사되기 전에 src 문자열이 발견되면
* dest 문자의 합이 쓰여질 때까지 후행 0 문자로 채워집니다. dest은 src의 길이가 n보다 작은 경우에만 0으로 종료됩니다.
* 이 특성은 소프트웨어 버그의 일반적인 원인이며 코드 작업시 반드시 종료 문자에 대한 작업이 필요 합니다.
* @brief n개의 문자수만큼 dest에 복사합니다.
* @param src 복사하고자 하는 문자열 소스
* @param dest 복사가 될 목적지 버퍼
* @param n 복사할 문자열 수 (주의: dest버퍼 크기를 고려하지 않습니다.)
* @result 복사된 dest 버퍼를 반환
* @code
*	char buffer[32];
*	fstrncpy(buffer, pSomeString, 32);
*	buffer[31] = 0;
* @endcode
*/
MARKENGINE_API wchar_t* fstrncpy(
	wchar_t* dest, 
	const wchar_t* src, 
	size_t n
);

/**
* fstrncpy16
* src의 처음 n개 문자를 dest에 복사합니다. 만약 종료문자가 복사되기 전에 src 문자열이 발견되면
* dest 문자의 합이 쓰여질 때까지 후행 0 문자로 채워집니다. dest은 src의 길이가 n보다 작은 경우에만 0으로 종료됩니다.
* 이 특성은 소프트웨어 버그의 일반적인 원인이며 코드 작업시 반드시 종료 문자에 대한 작업이 필요 합니다.
* @brief n개의 문자수만큼 dest에 복사합니다.
* @param src 복사하고자 하는 문자열 소스
* @param dest 복사가 될 목적지 버퍼
* @param n 복사할 문자열 수 (주의: dest버퍼 크기를 고려하지 않습니다.)
* @result 복사된 dest 버퍼를 반환
* @code
*	char buffer[32];
*	fstrncpy(buffer, pSomeString, 32);
*	buffer[31] = 0;
* @endcode
*/
MARKENGINE_API char16_t* fstrncpy(
	char16_t* dest, 
	const char16_t* src, 
	size_t n
);

/**
* fstrncpy32
* src의 처음 n개 문자를 dest에 복사합니다. 만약 종료문자가 복사되기 전에 src 문자열이 발견되면
* dest 문자의 합이 쓰여질 때까지 후행 0 문자로 채워집니다. dest은 src의 길이가 n보다 작은 경우에만 0으로 종료됩니다.
* 이 특성은 소프트웨어 버그의 일반적인 원인이며 코드 작업시 반드시 종료 문자에 대한 작업이 필요 합니다.
* @brief n개의 문자수만큼 dest에 복사합니다.
* @param src 복사하고자 하는 문자열 소스
* @param dest 복사가 될 목적지 버퍼
* @param n 복사할 문자열 수 (주의: dest버퍼 크기를 고려하지 않습니다.)
* @result 복사된 dest 버퍼를 반환
* @code
*	char buffer[32];
*	fstrncpy(buffer, pSomeString, 32);
*	buffer[31] = 0;
* @endcode
*/
MARKENGINE_API char32_t* fstrncpy(
	char32_t* dest, 
	const char32_t* src, 
	size_t n
);

/**
* fstrlcpy
* strlcpy는 strcpy, strncpy에 비해 안전한 문자열 복사 방법입니다.
* strlcpy는 dest buffer의 크기를 확인하여 최대 dest buffer의 크기보다 -1만큼 복사하게 됩니다.
* dest buffer의 마지막 배열은 0으로 세팅됩니다. dest buffer크기보다 소스 문자열의 크기가 더 클경우
* 소스 문자열은 일부가 소실되게 됩니다.
* @brief n개의 문자수만큼 dest에 안전하게 복사합니다.
* @param src 복사하고자 하는 문자열 소스
* @param dest 복사가 될 목적지 버퍼
* @param dest_size 복사가 될 버퍼의 사이즈
* @result 복사된 문자열의 사이즈 (src가 온전히 복사될 경우의 사이즈를 반환합니다. 해당 사이즈가 src의 길이보다 작다면 dest buffer크기의 사이즈가 작은것입니다)
* @code
*	char *s, *p, buffer[BUFFERSIZE];
*	fstrlcpy(s, buffer, BUFFERSIZE);
* @endcode
*/
MARKENGINE_API size_t fstrlcpy(
	char* dest, 
	const char* src,
	size_t dest_size
);

/**
* fstrlcpyw
* strlcpy는 strcpy, strncpy에 비해 안전한 문자열 복사 방법입니다.
* strlcpy는 dest buffer의 크기를 확인하여 최대 dest buffer의 크기보다 -1만큼 복사하게 됩니다.
* dest buffer의 마지막 배열은 0으로 세팅됩니다. dest buffer크기보다 소스 문자열의 크기가 더 클경우
* 소스 문자열은 일부가 소실되게 됩니다.
* @brief n개의 문자수만큼 dest에 안전하게 복사합니다.
* @param src 복사하고자 하는 문자열 소스
* @param dest 복사가 될 목적지 버퍼
* @param dest_size 복사가 될 버퍼의 사이즈
* @result 복사된 문자열의 사이즈 (src가 온전히 복사될 경우의 사이즈를 반환합니다. 해당 사이즈가 src의 길이보다 작다면 dest buffer크기의 사이즈가 작은것입니다)
* @code
*	wchar_t *s, *p, buffer[BUFFERSIZE];
*	fstrlcpy(s, buffer, BUFFERSIZE);
* @endcode
*/
MARKENGINE_API size_t fstrlcpy(
	wchar_t* dest, 
	const wchar_t* src, 
	size_t dest_size
);

/**
* fstrlcpy16
* strlcpy는 strcpy, strncpy에 비해 안전한 문자열 복사 방법입니다.
* strlcpy는 dest buffer의 크기를 확인하여 최대 dest buffer의 크기보다 -1만큼 복사하게 됩니다.
* dest buffer의 마지막 배열은 0으로 세팅됩니다. dest buffer크기보다 소스 문자열의 크기가 더 클경우
* 소스 문자열은 일부가 소실되게 됩니다.
* @brief n개의 문자수만큼 dest에 안전하게 복사합니다.
* @param src 복사하고자 하는 문자열 소스
* @param dest 복사가 될 목적지 버퍼
* @param dest_size 복사가 될 버퍼의 사이즈
* @result 복사된 문자열의 사이즈 (src가 온전히 복사될 경우의 사이즈를 반환합니다. 해당 사이즈가 src의 길이보다 작다면 dest buffer크기의 사이즈가 작은것입니다)
* @code
*	char16_t *s, *p, buffer[BUFFERSIZE];
*	fstrlcpy(s, buffer, BUFFERSIZE);
* @endcode
*/
MARKENGINE_API size_t fstrlcpy(
	char16_t* dest, 
	const char16_t* src,
	size_t dest_size
);

/**
* fstrlcpy32
* strlcpy는 strcpy, strncpy에 비해 안전한 문자열 복사 방법입니다.
* strlcpy는 dest buffer의 크기를 확인하여 최대 dest buffer의 크기보다 -1만큼 복사하게 됩니다.
* dest buffer의 마지막 배열은 0으로 세팅됩니다. dest buffer크기보다 소스 문자열의 크기가 더 클경우
* 소스 문자열은 일부가 소실되게 됩니다.
* @brief n개의 문자수만큼 dest에 안전하게 복사합니다.
* @param src 복사하고자 하는 문자열 소스
* @param dest 복사가 될 목적지 버퍼
* @param dest_size 복사가 될 버퍼의 사이즈
* @result 복사된 문자열의 사이즈 (src가 온전히 복사될 경우의 사이즈를 반환합니다. 해당 사이즈가 src의 길이보다 작다면 dest buffer크기의 사이즈가 작은것입니다)
* @code
*	char32_t *s, *p, buffer[BUFFERSIZE];
*	fstrlcpy(s, buffer, BUFFERSIZE);
* @endcode
*/
MARKENGINE_API size_t fstrlcpy(
	char32_t* dest, 
	const char32_t* src,
	size_t dest_size
);

/**
* fstrcat
* dest의 문자열 끝에 src를 추가합니다.
* dest buffer에 필요한 용량은 (fstrlen(src) + strlen(dest) + 1)의 용량이 필요 합니다.
* 그렇치 않고 dest buffer용량이 부족 할 경우 memory overflow가 발생하며 이는 코드의 심각한 버그로 이어집니다.
* 안전한 문자열을 추가 하기 위해서는 fstrlcat을 사용하기를 추천합니다.
* @brief dest의 문자열 끝에 src를 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API char* fstrcat(
	char* dest,
	const char* src
);

/**
* fstrcatw
* dest의 문자열 끝에 src를 추가합니다.
* dest buffer에 필요한 용량은 (fstrlen(src) + strlen(dest) + 1)의 용량이 필요 합니다.
* 그렇치 않고 dest buffer용량이 부족 할 경우 memory overflow가 발생하며 이는 코드의 심각한 버그로 이어집니다.
* 안전한 문자열을 추가 하기 위해서는 fstrlcat을 사용하기를 추천합니다.
* @brief dest의 문자열 끝에 src를 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API wchar_t* fstrcat(
	wchar_t* dest,
	const wchar_t* src
);

/**
* fstrcat16
* dest의 문자열 끝에 src를 추가합니다.
* dest buffer에 필요한 용량은 (fstrlen(src) + strlen(dest) + 1)의 용량이 필요 합니다.
* 그렇치 않고 dest buffer용량이 부족 할 경우 memory overflow가 발생하며 이는 코드의 심각한 버그로 이어집니다.
* 안전한 문자열을 추가 하기 위해서는 fstrlcat을 사용하기를 추천합니다.
* @brief dest의 문자열 끝에 src를 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API char16_t* fstrcat(
	char16_t* dest,
	const char16_t* src
);

/**
* fstrcat32
* dest의 문자열 끝에 src를 추가합니다.
* dest buffer에 필요한 용량은 (fstrlen(src) + strlen(dest) + 1)의 용량이 필요 합니다.
* 그렇치 않고 dest buffer용량이 부족 할 경우 memory overflow가 발생하며 이는 코드의 심각한 버그로 이어집니다.
* 안전한 문자열을 추가 하기 위해서는 fstrlcat을 사용하기를 추천합니다.
* @brief dest의 문자열 끝에 src를 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API char32_t* fstrcat(
	char32_t* dest, 
	const char32_t* src
);

/**
* fstrncat
* src의 처음 n개 문자를 dest에 추가하고 dest을 0으로 종료합니다.
* fstrncat에 대한 더 안전한 대안으로 fstrlcat을 사용하는 것을 고려합니다.
* @brief dest의 문자열 끝에 src를 n만큼 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @param n 추가하고자 하는 문자열 수
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API char* fstrncat(
	char* dest, 
	const char* src,
	size_t n
);

/**
* fstrncatw
* src의 처음 n개 문자를 dest에 추가하고 dest을 0으로 종료합니다.
* fstrncat에 대한 더 안전한 대안으로 fstrlcat을 사용하는 것을 고려합니다.
* @brief dest의 문자열 끝에 src를 n만큼 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @param n 추가하고자 하는 문자열 수
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API wchar_t* fstrncat(
	wchar_t* dest, 
	const wchar_t* src, 
	size_t n
);

/**
* fstrncat16
* src의 처음 n개 문자를 dest에 추가하고 dest을 0으로 종료합니다.
* fstrncat에 대한 더 안전한 대안으로 fstrlcat을 사용하는 것을 고려합니다.
* @brief dest의 문자열 끝에 src를 n만큼 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @param n 추가하고자 하는 문자열 수
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API char16_t* fstrncat(
	char16_t* dest, 
	const char16_t* src, 
	size_t n
);

/**
* fstrncat32
* src의 처음 n개 문자를 dest에 추가하고 dest을 0으로 종료합니다.
* fstrncat에 대한 더 안전한 대안으로 fstrlcat을 사용하는 것을 고려합니다.
* @brief dest의 문자열 끝에 src를 n만큼 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @param n 추가하고자 하는 문자열 수
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API char32_t* fstrncat(
	char32_t* dest,
	const char32_t* src,
	size_t n
);

/**
* fstrlcat
* fstrlcat는 strcat, strncat에 비해 안전하게 문자열을 병합 합니다.
* fstrlcat는 dest buffer의 크기를 확인하여 최대 dest buffer의 크기보다 -1만큼 병합하게 됩니다.
* @brief dest_size의 버퍼수만큼 dest에 안전하게 복사합니다.
* @param src 병합하고자 하는 문자열 소스
* @param dest 병합가 될 목적지 버퍼
* @param dest_size 병합 될 버퍼의 사이즈
* @result 병합된 문자열의 사이즈 (결과값은 src와 dest가 온전히 합쳐졌을때의 크기를 반환한다. 실제로는 dest buffer의 사이즈가 부족하여 적게 복사될수 있다)
* @code
*	char *s, *p, buffer[BUFFERSIZE];
*	fstrlcat(s, buffer, BUFFERSIZE);
* @endcode
*/
MARKENGINE_API size_t fstrlcat(
	char* dest,
	const char* src, 
	size_t dest_size
);

/**
* fstrlcatw
* fstrlcat는 strcat, strncat에 비해 안전하게 문자열을 병합 합니다.
* fstrlcat는 dest buffer의 크기를 확인하여 최대 dest buffer의 크기보다 -1만큼 병합하게 됩니다.
* @brief dest_size의 버퍼수만큼 dest에 안전하게 복사합니다.
* @param src 병합하고자 하는 문자열 소스
* @param dest 병합가 될 목적지 버퍼
* @param dest_size 병합 될 버퍼의 사이즈
* @result 병합된 문자열의 사이즈 (결과값은 src와 dest가 온전히 합쳐졌을때의 크기를 반환한다. 실제로는 dest buffer의 사이즈가 부족하여 적게 복사될수 있다)
* @code
*	char *s, *p, buffer[BUFFERSIZE];
*	fstrlcat(s, buffer, BUFFERSIZE);
* @endcode
*/
MARKENGINE_API size_t fstrlcat(
	wchar_t* dest, 
	const wchar_t* src,
	size_t dest_size
);

/**
* fstrlcat16
* fstrlcat는 strcat, strncat에 비해 안전하게 문자열을 병합 합니다.
* fstrlcat는 dest buffer의 크기를 확인하여 최대 dest buffer의 크기보다 -1만큼 병합하게 됩니다.
* @brief dest_size의 버퍼수만큼 dest에 안전하게 복사합니다.
* @param src 병합하고자 하는 문자열 소스
* @param dest 병합가 될 목적지 버퍼
* @param dest_size 병합 될 버퍼의 사이즈
* @result 병합된 문자열의 사이즈 (결과값은 src와 dest가 온전히 합쳐졌을때의 크기를 반환한다. 실제로는 dest buffer의 사이즈가 부족하여 적게 복사될수 있다)
* @code
*	char *s, *p, buffer[BUFFERSIZE];
*	fstrlcat(s, buffer, BUFFERSIZE);
* @endcode
*/
MARKENGINE_API size_t fstrlcat(
	char16_t* dest, 
	const char16_t* src,
	size_t dest_size
);

/**
* fstrlcat32
* fstrlcat는 strcat, strncat에 비해 안전하게 문자열을 병합 합니다.
* fstrlcat는 dest buffer의 크기를 확인하여 최대 dest buffer의 크기보다 -1만큼 병합하게 됩니다.
* @brief dest_size의 버퍼수만큼 dest에 안전하게 복사합니다.
* @param src 병합하고자 하는 문자열 소스
* @param dest 병합가 될 목적지 버퍼
* @param dest_size 병합 될 버퍼의 사이즈
* @result 병합된 문자열의 사이즈 (결과값은 src와 dest가 온전히 합쳐졌을때의 크기를 반환한다. 실제로는 dest buffer의 사이즈가 부족하여 적게 복사될수 있다)
* @code
*	char *s, *p, buffer[BUFFERSIZE];
*	fstrlcat(s, buffer, BUFFERSIZE);
* @endcode
*/
MARKENGINE_API size_t fstrlcat(
	char32_t* dest,
	const char32_t* src, 
	size_t dest_size
);

/**
* fstrupr
* ASCII 문자열을 대문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrupr C 함수와 유사합니다.
* @brief ASCII 문자열을 대문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API char* fstrupr(
	char* src
);

/**
* fstruprw
* ASCII 문자열을 대문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrupr C 함수와 유사합니다.
* @brief ASCII 문자열을 대문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API wchar_t* fstrupr(
	wchar_t* src
);

/**
* fstrupr16
* ASCII 문자열을 대문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrupr C 함수와 유사합니다.
* @brief ASCII 문자열을 대문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API char16_t* fstrupr(
	char16_t* src
);

/**
* fstrupr32
* ASCII 문자열을 대문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrupr C 함수와 유사합니다.
* @brief ASCII 문자열을 대문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API char32_t* fstrupr(
	char32_t* src
);

/**
* fstrlwr
* ASCII 문자열을 소문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrlwr C 함수와 유사합니다.
* @brief ASCII 문자열을 소문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API char* fstrlwr(
	char* src
);

/**
* fstrlwrw
* ASCII 문자열을 소문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrlwr C 함수와 유사합니다.
* @brief ASCII 문자열을 소문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API wchar_t* fstrlwr(
	wchar_t* src
);

/**
* fstrlwr16
* ASCII 문자열을 소문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrlwr C 함수와 유사합니다.
* @brief ASCII 문자열을 소문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API char16_t* fstrlwr(
	char16_t* src
);

/**
* fstrlwr32
* ASCII 문자열을 소문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrlwr C 함수와 유사합니다.
* @brief ASCII 문자열을 소문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API char32_t* fstrlwr(
	char32_t* src
);


/**
* fstrchr
* src에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 발견되지 않으면 NULL을 반환합니다. 이것은 strchr C 함수와 유사합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstrchr(
	const char* str, 
	char c
);

/**
* fstrchr
* src에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 발견되지 않으면 NULL을 반환합니다. 이것은 strchr C 함수와 유사합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstrchr(
	const wchar_t* str, 
	wchar_t c
);

/**
* fstrchr16
* src에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 발견되지 않으면 NULL을 반환합니다. 이것은 strchr C 함수와 유사합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstrchr(
	const char16_t* str, 
	char16_t c
);

/**
* fstrchr32
* src에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 발견되지 않으면 NULL을 반환합니다. 이것은 strchr C 함수와 유사합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstrchr(
	const char32_t* str, 
	char32_t c
);

/**
* fstrnchr
* str에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 n문자 발견되지 않으면 NULL을 반환합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자.
* @param n 문자열 중에서 찾고자 하는 범위 (문자수)
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstrnchr(
	const char* str, 
	char c, 
	size_t n
);

/**
* fstrnchrw
* str에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 n문자 발견되지 않으면 NULL을 반환합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자.
* @param n 문자열 중에서 찾고자 하는 범위 (문자수)
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstrnchr(
	const wchar_t* str,
	wchar_t c, 
	size_t n
);

/**
* fstrnchr16
* str에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 n문자 발견되지 않으면 NULL을 반환합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자.
* @param n 문자열 중에서 찾고자 하는 범위 (문자수)
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstrnchr(
	const char16_t* str,
	char16_t c,
	size_t n
);

/**
* fstrnchr32
* str에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 n문자 발견되지 않으면 NULL을 반환합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자.
* @param n 문자열 중에서 찾고자 하는 범위 (문자수)
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstrnchr(
	const char32_t* str,
	char32_t c,
	size_t n
);

/**
* fstrcspn
* str1에서 문자 단위로 스캔하여 읽은 문자 수를 반환합니다. 이것은 strcspn C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 읽은 문자 수를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 str2 갯수를 반환합니다.
*/
MARKENGINE_API size_t fstrcspn(
	const char* str1, 
	const char* str2
);

/**
* fstrcspnw
* str1에서 문자 단위로 스캔하여 읽은 문자 수를 반환합니다. 이것은 strcspn C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 읽은 문자 수를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 str2 갯수를 반환합니다.
*/
MARKENGINE_API size_t fstrcspn(
	const wchar_t* str1, 
	const wchar_t* str2
);

/**
* fstrcspn16
* str1에서 문자 단위로 스캔하여 읽은 문자 수를 반환합니다. 이것은 strcspn C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 읽은 문자 수를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 str2 갯수를 반환합니다.
*/
MARKENGINE_API size_t fstrcspn(
	const char16_t* str1,
	const char16_t* str2
);

/**
* fstrcspn32
* str1에서 문자 단위로 스캔하여 읽은 문자 수를 반환합니다. 이것은 strcspn C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 읽은 문자 수를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 str2 갯수를 반환합니다.
*/
MARKENGINE_API size_t fstrcspn(
	const char32_t* str1, 
	const char32_t* str2
);

/**
* fstrpbrk
* str1을 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다. 이것은 strpbrk C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstrpbrk(
	const char* str1,
	const char* str2
);

/**
* fstrpbrkw
* str1을 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다. 이것은 strpbrk C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstrpbrk(
	const wchar_t* str1,
	const wchar_t* str2
);

/**
* fstrpbrk16
* str1을 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다. 이것은 strpbrk C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstrpbrk(
	const char16_t* str1, 
	const char16_t* str2
);

/**
* fstrpbrk32
* str1을 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다. 이것은 strpbrk C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstrpbrk(
	const char32_t* str1,
	const char32_t* str2
);

/**
* fstrrchr
* src에서 c의 마지막 항목을 반환하거나 c가 발견되지 않으면 NULL을 반환합니다. 이것은 strrchr C 함수와 유사합니다.
* null 종료 문자는 문자열의 일부로 포함되며 검색할 수도 있습니다.
* @brief src에서 c의 마지막 항목을 반환
* @param str1 문자열 소스 전체
* @param c 찾고자 하는 문자
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstrrchr(
	const char* str, 
	char c
);

/**
* fstrrchrw
* src에서 c의 마지막 항목을 반환하거나 c가 발견되지 않으면 NULL을 반환합니다. 이것은 strrchr C 함수와 유사합니다.
* null 종료 문자는 문자열의 일부로 포함되며 검색할 수도 있습니다.
* @brief src에서 c의 마지막 항목을 반환
* @param str1 문자열 소스 전체
* @param c 찾고자 하는 문자
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstrrchr(
	const wchar_t* str, 
	wchar_t c
);

/**
* fstrrchr16
* src에서 c의 마지막 항목을 반환하거나 c가 발견되지 않으면 NULL을 반환합니다. 이것은 strrchr C 함수와 유사합니다.
* null 종료 문자는 문자열의 일부로 포함되며 검색할 수도 있습니다.
* @brief src에서 c의 마지막 항목을 반환
* @param str1 문자열 소스 전체
* @param c 찾고자 하는 문자
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstrrchr(
	const char16_t* str,
	char16_t c
);

/**
* fstrrchr32
* src에서 c의 마지막 항목을 반환하거나 c가 발견되지 않으면 NULL을 반환합니다. 이것은 strrchr C 함수와 유사합니다.
* null 종료 문자는 문자열의 일부로 포함되며 검색할 수도 있습니다.
* @brief src에서 c의 마지막 항목을 반환
* @param str1 문자열 소스 전체
* @param c 찾고자 하는 문자
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstrrchr(
	const char32_t* str,
	char32_t c
);

/**
* fstrspn
* str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @brief str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src 오프셋을 반환합니다.
*/
MARKENGINE_API size_t fstrspn(
	const char* str1, 
	const char* str2
);

/**
* fstrspn
* str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @brief str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src 오프셋을 반환합니다.
*/
MARKENGINE_API size_t fstrspn(
	const wchar_t* str1,
	const wchar_t* str2
);

/**
* fstrspn
* str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @brief str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src 오프셋을 반환합니다.
*/
MARKENGINE_API size_t fstrspn(
	const char16_t* str1, 
	const char16_t* str2
);

/**
* fstrspn
* str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @brief str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src 오프셋을 반환합니다.
*/
MARKENGINE_API size_t fstrspn(
	const char32_t* str1,
	const char32_t* str2
);

/**
* fstrstr
* str 내에서 sub의 첫 번째 발생을 찾습니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstrstr(
	const char* str,
	const char* sub
);

/**
* fstrstrw
* str 내에서 sub의 첫 번째 발생을 찾습니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstrstr(
	const wchar_t* str,
	const wchar_t* sub
);

/**
* fstrstr16
* str 내에서 sub의 첫 번째 발생을 찾습니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstrstr(
	const char16_t* str,
	const char16_t* sub
);

/**
* fstrstr32
* str 내에서 sub의 첫 번째 발생을 찾습니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstrstr(
	const char32_t* str,
	const char32_t* sub
);

/**
* fstristr
* fstrstr의 대소문자를 구분하지 않는 버전입니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstristr(
	const char* str, 
	const char* sub
);

/**
* fstristrw
* fstrstr의 대소문자를 구분하지 않는 버전입니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstristr(
	const wchar_t* str,
	const wchar_t* sub
);

/**
* fstristr16
* fstrstr의 대소문자를 구분하지 않는 버전입니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstristr(
	const char16_t* str,
	const char16_t* sub
);

/**
* fstristr32
* fstrstr의 대소문자를 구분하지 않는 버전입니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstristr(
	const char32_t* str, 
	const char32_t* sub
);

/**
* fstrrstr
* str 내에서 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstrrstr(
	const char* str, 
	const char* sub
);

/**
* fstrrstrw
* str 내에서 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstrrstr(
	const wchar_t* str,
	const wchar_t* sub
);

/**
* fstrrstr16
* str 내에서 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstrrstr(
	const char16_t* str, 
	const char16_t* sub
);

/**
* fstrrstr32
* str 내에서 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstrrstr(
	const char32_t* str,
	const char32_t* sub
);

/**
* fstrrstr
* str 내에서 대소문자 구분없이 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstrirstr(
	const char* str,
	const char* sub
);

/**
* fstrrstrw
* str 내에서 대소문자 구분없이 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstrirstr(
	const wchar_t* str,
	const wchar_t* sub
);

/**
* fstrrstr16
* str 내에서 대소문자 구분없이 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstrirstr(
	const char16_t* str,
	const char16_t* sub
);

/**
* fstrrstr32
* str 내에서 대소문자 구분없이 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstrirstr(
	const char32_t* str,
	const char32_t* sub
);

/**
* fstrstart
* str이 prefix로 시작하면 true를 반환합니다.
* @brief str이 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstrstart(
	const char* str,
	const char* prefix
);

/**
* fstrstartw
* str이 prefix로 시작하면 true를 반환합니다.
* @brief str이 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstrstart(
	const wchar_t* str,
	const wchar_t* prefix
);

/**
* fstrstart16
* str이 prefix로 시작하면 true를 반환합니다.
* @brief str이 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstrstart(
	const char16_t* str, 
	const char16_t* prefix
);

/**
* fstrstart32
* str이 prefix로 시작하면 true를 반환합니다.
* @brief str이 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstrstart(
	const char32_t* str,
	const char32_t* prefix
);

/**
* fstristart
* str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다.
* @brief str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstristart(
	const char* str,
	const char* prefix
);

/**
* fstristartw
* str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다.
* @brief str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstristart(
	const wchar_t* str, 
	const wchar_t* prefix
);

/**
* fstristart16
* str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다.
* @brief str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstristart(
	const char16_t* str,
	const char16_t* prefix
);

/**
* fstristart32
* str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다.
* @brief str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstristart(
	const char32_t* str,
	const char32_t* prefix
);

/**
* fstrend
* str 문장의 마지막 포인터를 반환합니다.
* @brief str 문장의 마지막 포인터를 반환합니다.
* @param str 문자열 소스 전체
* @result str문자열의 null이 아닌 마지막 포인터
*/
MARKENGINE_API char* fstrend(
	const char* str
);

/**
* fstrendw
* str 문장의 마지막 포인터를 반환합니다.
* @brief str 문장의 마지막 포인터를 반환합니다.
* @param str 문자열 소스 전체
* @result str문자열의 null이 아닌 마지막 포인터
*/
MARKENGINE_API wchar_t* fstrend(
	const wchar_t* str
);

/**
* fstrend16
* str 문장의 마지막 포인터를 반환합니다.
* @brief str 문장의 마지막 포인터를 반환합니다.
* @param str 문자열 소스 전체
* @result str문자열의 null이 아닌 마지막 포인터
*/
MARKENGINE_API char16_t* fstrend(
	const char16_t* str
);

/**
* fstrend32
* str 문장의 마지막 포인터를 반환합니다.
* @brief str 문장의 마지막 포인터를 반환합니다.
* @param str 문자열 소스 전체
* @result str문자열의 null이 아닌 마지막 포인터
*/
MARKENGINE_API char32_t* fstrend(
	const char32_t* str
);

/**
* fstrend
* str의 내용이 suffix의 내용으로 끝나면 true를 반환합니다.
* @brief str의 내용이 suffix의 내용으로 끝나면 true를 반환합니다. 빈 suffix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstrend(
	const char* str,
	const char* suffix, 
	size_t strlen = SIZE_TYPE_UNSET,
	size_t suffixlen = SIZE_TYPE_UNSET
);

/**
* fstrendw
* str의 내용이 suffix의 내용으로 끝나면 true를 반환합니다.
* @brief str의 내용이 suffix의 내용으로 끝나면 true를 반환합니다. 빈 suffix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstrend(
	const wchar_t* str, 
	const wchar_t* suffix,
	size_t strlen = SIZE_TYPE_UNSET,
	size_t suffixlen = SIZE_TYPE_UNSET
);

/**
* fstrend16
* str의 내용이 suffix의 내용으로 끝나면 true를 반환합니다.
* @brief str의 내용이 suffix의 내용으로 끝나면 true를 반환합니다. 빈 suffix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstrend(
	const char16_t* str, 
	const char16_t* suffix, 
	size_t strlen = SIZE_TYPE_UNSET, 
	size_t suffixlen = SIZE_TYPE_UNSET
);

/**
* fstrend32
* str의 내용이 suffix의 내용으로 끝나면 true를 반환합니다.
* @brief str의 내용이 suffix의 내용으로 끝나면 true를 반환합니다. 빈 suffix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstrend(
	const char32_t* str,
	const char32_t* suffix, 
	size_t strlen = SIZE_TYPE_UNSET, 
	size_t suffixlen = SIZE_TYPE_UNSET
);

/**
* fstriend
* str의 내용이 대소문자 구분않고 suffix의 내용으로 끝나면 true를 반환합니다.
* @brief str의 내용이 대소문자 구분않고 suffix의 내용으로 끝나면 true를 반환합니다. 빈 suffix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstriend(
	const char* str, 
	const char* suffix, 
	size_t strlen = SIZE_TYPE_UNSET,
	size_t suffixlen = SIZE_TYPE_UNSET
);

/**
* fstriendw
* str의 내용이 대소문자 구분않고 suffix의 내용으로 끝나면 true를 반환합니다.
* @brief str의 내용이 대소문자 구분않고 suffix의 내용으로 끝나면 true를 반환합니다. 빈 suffix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstriend(
	const wchar_t* str,
	const wchar_t* suffix,
	size_t strlen = SIZE_TYPE_UNSET,
	size_t suffixlen = SIZE_TYPE_UNSET
);

/**
* fstriend16
* str의 내용이 대소문자 구분않고 suffix의 내용으로 끝나면 true를 반환합니다.
* @brief str의 내용이 대소문자 구분않고 suffix의 내용으로 끝나면 true를 반환합니다. 빈 suffix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstriend(
	const char16_t* str, 
	const char16_t* suffix, 
	size_t strlen = SIZE_TYPE_UNSET, 
	size_t suffixlen = SIZE_TYPE_UNSET
);

/**
* fstriend32
* str의 내용이 대소문자 구분않고 suffix의 내용으로 끝나면 true를 반환합니다.
* @brief str의 내용이 대소문자 구분않고 suffix의 내용으로 끝나면 true를 반환합니다. 빈 suffix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstriend(
	const char32_t* str, 
	const char32_t* suffix,
	size_t strlen = SIZE_TYPE_UNSET,
	size_t suffixlen = SIZE_TYPE_UNSET
);

/**
* fstrtok
* str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @brief str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @param str 찾을 토큰 또는 토큰이 포함된 문자열입니다.
* @param delim 사용할 구분 기호 문자 집합입니다.
* @param context 함수 호출 간에 위치 정보를 저장하는 데 사용됩니다.
* @result str에서 발견된 다음 토큰에 대한 포인터를 반환합니다. 토큰을 더 이상 찾을 수 없으면 를 반환 NULL 합니다. 각 호출은 str 반환된 토큰 이후에 발생하는 첫 번째 구분 기호에 대해 null 문자를 대체하여 수정합니다.
* @code
*	char16_t str[] = u"A,B,C,D,E,F,G";
	char16_t* context = nullptr;
	char16_t* r = stdc::fstrtok(str, u",", &context);
	while (r)
	r = stdc::fstrtok(nullptr, u",", &context);
* @endcode
*/
MARKENGINE_API char* fstrtok(
	char* str, 
	const char* delim,
	char** context
);

/**
* fstrtokw
* str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @brief str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @param str 찾을 토큰 또는 토큰이 포함된 문자열입니다.
* @param delim 사용할 구분 기호 문자 집합입니다.
* @param context 함수 호출 간에 위치 정보를 저장하는 데 사용됩니다.
* @result str에서 발견된 다음 토큰에 대한 포인터를 반환합니다. 토큰을 더 이상 찾을 수 없으면 를 반환 NULL 합니다. 각 호출은 str 반환된 토큰 이후에 발생하는 첫 번째 구분 기호에 대해 null 문자를 대체하여 수정합니다.
* @code
*	char16_t str[] = u"A,B,C,D,E,F,G";
	char16_t* context = nullptr;
	char16_t* r = stdc::fstrtok(str, u",", &context);
	while (r)
	r = stdc::fstrtok(nullptr, u",", &context);
* @endcode
*/
MARKENGINE_API wchar_t* fstrtok(
	wchar_t* str, 
	const wchar_t* delim, 
	wchar_t** context
);

/**
* fstrtok16
* str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @brief str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @param str 찾을 토큰 또는 토큰이 포함된 문자열입니다.
* @param delim 사용할 구분 기호 문자 집합입니다.
* @param context 함수 호출 간에 위치 정보를 저장하는 데 사용됩니다.
* @result str에서 발견된 다음 토큰에 대한 포인터를 반환합니다. 토큰을 더 이상 찾을 수 없으면 를 반환 NULL 합니다. 각 호출은 str 반환된 토큰 이후에 발생하는 첫 번째 구분 기호에 대해 null 문자를 대체하여 수정합니다.
* @code
*	char16_t str[] = u"A,B,C,D,E,F,G";
	char16_t* context = nullptr;
	char16_t* r = stdc::fstrtok(str, u",", &context);
	while (r)
	r = stdc::fstrtok(nullptr, u",", &context);
* @endcode
*/
MARKENGINE_API char16_t* fstrtok(
	char16_t* str,
	const char16_t* delim, 
	char16_t** context
);

/**
* fstrtok32
* str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @brief str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @param str 찾을 토큰 또는 토큰이 포함된 문자열입니다.
* @param delim 사용할 구분 기호 문자 집합입니다.
* @param context 함수 호출 간에 위치 정보를 저장하는 데 사용됩니다.
* @result str에서 발견된 다음 토큰에 대한 포인터를 반환합니다. 토큰을 더 이상 찾을 수 없으면 를 반환 NULL 합니다. 각 호출은 str 반환된 토큰 이후에 발생하는 첫 번째 구분 기호에 대해 null 문자를 대체하여 수정합니다.
* @code
*	char16_t str[] = u"A,B,C,D,E,F,G";
	char16_t* context = nullptr;
	char16_t* r = stdc::fstrtok(str, u",", &context);
	while (r)
	r = stdc::fstrtok(nullptr, u",", &context);
* @endcode
*/
MARKENGINE_API char32_t* fstrtok(
	char32_t* str, 
	const char32_t* delim,
	char32_t** context
);

/**
* fstrrev2
* str을 뒤집어서 반환합니다.
* @brief str을 뒤집어서 반환합니다.
* @param str 문자열 소스 전체
* @result 분리된 str 포인터를 반환합니다.
*/
MARKENGINE_API char* fstrrev2(
	char* src
);

/**
* fstrrev2w
* str을 뒤집어서 반환합니다.
* @brief str을 뒤집어서 반환합니다.
* @param str 문자열 소스 전체
* @result 분리된 str 포인터를 반환합니다.
*/
MARKENGINE_API wchar_t* fstrrev2(
	wchar_t* src
);

/**
* fstrrev216
* str을 뒤집어서 반환합니다.
* @brief str을 뒤집어서 반환합니다.
* @param str 문자열 소스 전체
* @result 분리된 str 포인터를 반환합니다.
*/
MARKENGINE_API char16_t* fstrrev2(
	char16_t* src
);

/**
* fstrrev232
* str을 뒤집어서 반환합니다.
* @brief str을 뒤집어서 반환합니다.
* @param str 문자열 소스 전체
* @result 분리된 str 포인터를 반환합니다.
*/
MARKENGINE_API char32_t* fstrrev2(
	char32_t* src
);

/**
* fstrstrip
* _isspace 함수에 의해 정의된 선행 및 후행 공백을 제거합니다.
* @brief 선행 및 후행 공백을 제거합니다. 선행 공백이 제거되지 않고 오히려  str 에서 공백이 아닌 첫 번째 문자가 반환됩니다
* @brief 따라서 주의가 필요 합니다. 이 함수는 포인터 >= str을 반환할 수 있으며 이를 해제할 수 없습니다. 동적 메모리일 경우 반환된 값으로 해제(free/delete)하지 마십시오.
* @param str 문자열 소스 전체
* @result 공백이 제거된 str포인터가 반환됩니다.
*/
MARKENGINE_API char* fstrstrip(
	char* str
);

/**
* fstrstripw
* _isspace 함수에 의해 정의된 선행 및 후행 공백을 제거합니다.
* @brief 선행 및 후행 공백을 제거합니다. 선행 공백이 제거되지 않고 오히려  str 에서 공백이 아닌 첫 번째 문자가 반환됩니다
* @brief 따라서 주의가 필요 합니다. 이 함수는 포인터 >= str을 반환할 수 있으며 이를 해제할 수 없습니다. 동적 메모리일 경우 반환된 값으로 해제(free/delete)하지 마십시오.
* @param str 문자열 소스 전체
* @result 공백이 제거된 str포인터가 반환됩니다.
*/
MARKENGINE_API wchar_t* fstrstrip(
	wchar_t* str
);

/**
* fstrstrip16
* _isspace 함수에 의해 정의된 선행 및 후행 공백을 제거합니다.
* @brief 선행 및 후행 공백을 제거합니다. 선행 공백이 제거되지 않고 오히려  str 에서 공백이 아닌 첫 번째 문자가 반환됩니다
* @brief 따라서 주의가 필요 합니다. 이 함수는 포인터 >= str을 반환할 수 있으며 이를 해제할 수 없습니다. 동적 메모리일 경우 반환된 값으로 해제(free/delete)하지 마십시오.
* @param str 문자열 소스 전체
* @result 공백이 제거된 str포인터가 반환됩니다.
*/
MARKENGINE_API char16_t* fstrstrip(
	char16_t* str
);

/**
* fstrstrip32
* _isspace 함수에 의해 정의된 선행 및 후행 공백을 제거합니다.
* @brief 선행 및 후행 공백을 제거합니다. 선행 공백이 제거되지 않고 오히려  str 에서 공백이 아닌 첫 번째 문자가 반환됩니다
* @brief 따라서 주의가 필요 합니다. 이 함수는 포인터 >= str을 반환할 수 있으며 이를 해제할 수 없습니다. 동적 메모리일 경우 반환된 값으로 해제(free/delete)하지 마십시오.
* @param str 문자열 소스 전체
* @result 공백이 제거된 str포인터가 반환됩니다.
*/
MARKENGINE_API char32_t* fstrstrip(
	char32_t* str
);

/**
* fstrcmp
* 문자열을 비교 합니다.
* @brief 문자열을 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrcmp(
	const char* str1,
	const char* str2
);

/**
* fstrcmpw
* 문자열을 비교 합니다.
* @brief 문자열을 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrcmp(
	const wchar_t* str1,
	const wchar_t* str2
);

/**
* fstrcmp16
* 문자열을 비교 합니다.
* @brief 문자열을 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrcmp(
	const char16_t* str1, 
	const char16_t* str2
);

/**
* fstrcmp32
* 문자열을 비교 합니다.
* @brief 문자열을 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrcmp(
	const char32_t* str1, 
	const char32_t* str2
);

/**
* fstrcmp
* 문자열을 n개수 만큼 비교 합니다.
* @brief 문자열을 n개수 만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrncmp(
	const char* str1, 
	const char* str2, 
	size_t n
);

/**
* fstrcmpw
* 문자열을 n개수 만큼 비교 합니다.
* @brief 문자열을 n개수 만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrncmp(
	const wchar_t* str1,
	const wchar_t* str2, 
	size_t n
);

/**
* fstrcmp16
* 문자열을 n개수 만큼 비교 합니다.
* @brief 문자열을 n개수 만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrncmp(
	const char16_t* str1,
	const char16_t* str2, 
	size_t n
);

/**
* fstrcmp32
* 문자열을 n개수 만큼 비교 합니다.
* @brief 문자열을 n개수 만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrncmp(
	const char32_t* str1,
	const char32_t* str2, 
	size_t n
);

/**
* fstricmp
* 문자열을 대소문자 구별없이 비교 합니다.
* @brief 문자열을 대소문자 구별없이 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstricmp(
	const char* str1, 
	const char* str2
);

/**
* fstricmpw
* 문자열을 대소문자 구별없이 비교 합니다.
* @brief 문자열을 대소문자 구별없이 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstricmp(
	const wchar_t* str1,
	const wchar_t* str2
);

/**
* fstricmp16
* 문자열을 대소문자 구별없이 비교 합니다.
* @brief 문자열을 대소문자 구별없이 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstricmp(
	const char16_t* str1,
	const char16_t* str2
);

/**
* fstricmp32
* 문자열을 대소문자 구별없이 비교 합니다.
* @brief 문자열을 대소문자 구별없이 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstricmp(
	const char32_t* str1, 
	const char32_t* str2
);

/**
* fstrincmp
* 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @brief 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrincmp(
	const char* str1,
	const char* str2, 
	size_t n
);

/**
* fstrincmpw
* 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @brief 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrincmp(
	const wchar_t* str1, 
	const wchar_t* str2,
	size_t n
);

/**
* fstrincmp16
* 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @brief 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrincmp(
	const char16_t* str1, 
	const char16_t* str2, 
	size_t n
);

/**
* fstrincmp32
* 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @brief 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrincmp(
	const char32_t* str1, 
	const char32_t* str2, 
	size_t n
);

/**
* fstrcat
* dest의 문자열 끝에 src를 추가합니다.
* dest buffer에 필요한 용량은 (fstrlen(src) + strlen(dest) + 1)의 용량이 필요 합니다.
* 그렇치 않고 dest buffer용량이 부족 할 경우 memory overflow가 발생하며 이는 코드의 심각한 버그로 이어집니다.
* 안전한 문자열을 추가 하기 위해서는 fstrlcat을 사용하기를 추천합니다.
* @brief dest의 문자열 끝에 src를 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API char* fstrcat(
	char* dest, 
	const char* src
);

/**
* fstrcatw
* dest의 문자열 끝에 src를 추가합니다.
* dest buffer에 필요한 용량은 (fstrlen(src) + strlen(dest) + 1)의 용량이 필요 합니다.
* 그렇치 않고 dest buffer용량이 부족 할 경우 memory overflow가 발생하며 이는 코드의 심각한 버그로 이어집니다.
* 안전한 문자열을 추가 하기 위해서는 fstrlcat을 사용하기를 추천합니다.
* @brief dest의 문자열 끝에 src를 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API wchar_t* fstrcat(
	wchar_t* dest,
	const wchar_t* src
);

/**
* fstrcat16
* dest의 문자열 끝에 src를 추가합니다.
* dest buffer에 필요한 용량은 (fstrlen(src) + strlen(dest) + 1)의 용량이 필요 합니다.
* 그렇치 않고 dest buffer용량이 부족 할 경우 memory overflow가 발생하며 이는 코드의 심각한 버그로 이어집니다.
* 안전한 문자열을 추가 하기 위해서는 fstrlcat을 사용하기를 추천합니다.
* @brief dest의 문자열 끝에 src를 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API char16_t* fstrcat(
	char16_t* dest, 
	const char16_t* src
);

/**
* fstrcat32
* dest의 문자열 끝에 src를 추가합니다.
* dest buffer에 필요한 용량은 (fstrlen(src) + strlen(dest) + 1)의 용량이 필요 합니다.
* 그렇치 않고 dest buffer용량이 부족 할 경우 memory overflow가 발생하며 이는 코드의 심각한 버그로 이어집니다.
* 안전한 문자열을 추가 하기 위해서는 fstrlcat을 사용하기를 추천합니다.
* @brief dest의 문자열 끝에 src를 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API char32_t* fstrcat(
	char32_t* dest,
	const char32_t* src
);

/**
* fstrncat
*  src의 처음 n개 문자를 dest에 추가하고 dest을 0으로 종료합니다.
* fstrncat에 대한 더 안전한 대안으로 fstrlcat을 사용하는 것을 고려합니다.
* @brief dest의 문자열 끝에 src를 n만큼 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @param n 추가하고자 하는 문자열 수
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API char* fstrncat(
	char* dest, 
	const char* src, 
	size_t n
);

/**
* fstrncatw
*  src의 처음 n개 문자를 dest에 추가하고 dest을 0으로 종료합니다.
* fstrncat에 대한 더 안전한 대안으로 fstrlcat을 사용하는 것을 고려합니다.
* @brief dest의 문자열 끝에 src를 n만큼 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @param n 추가하고자 하는 문자열 수
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API wchar_t* fstrncat(
	wchar_t* dest,
	const wchar_t* src, 
	size_t n
);

/**
* fstrncat16
*  src의 처음 n개 문자를 dest에 추가하고 dest을 0으로 종료합니다.
* fstrncat에 대한 더 안전한 대안으로 fstrlcat을 사용하는 것을 고려합니다.
* @brief dest의 문자열 끝에 src를 n만큼 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @param n 추가하고자 하는 문자열 수
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API char16_t* fstrncat(
	char16_t* dest,
	const char16_t* src,
	size_t n
);

/**
* fstrncat32
*  src의 처음 n개 문자를 dest에 추가하고 dest을 0으로 종료합니다.
* fstrncat에 대한 더 안전한 대안으로 fstrlcat을 사용하는 것을 고려합니다.
* @brief dest의 문자열 끝에 src를 n만큼 추가합니다.
* @param src 추가하고자 하는 문자열
* @param dest src가 추가될 문자열 버퍼
* @param n 추가하고자 하는 문자열 수
* @result dest 포인터를 반환합니다.
*/
MARKENGINE_API char32_t* fstrncat(
	char32_t* dest,
	const char32_t* src,
	size_t n
);

/**
* fstrlcat
* fstrlcat는 strcat, strncat에 비해 안전하게 문자열을 병합 합니다.
* fstrlcat는 dest buffer의 크기를 확인하여 최대 dest buffer의 크기보다 -1만큼 병합하게 됩니다.
* @brief dest_size의 버퍼수만큼 dest에 안전하게 복사합니다.
* @param src 병합하고자 하는 문자열 소스
* @param dest 병합가 될 목적지 버퍼
* @param dest_size 병합 될 버퍼의 사이즈
* @result 병합된 문자열의 사이즈 (결과값은 src와 dest가 온전히 합쳐졌을때의 크기를 반환한다. 실제로는 dest buffer의 사이즈가 부족하여 적게 복사될수 있다)
* @code
*	char *s, *p, buffer[BUFFERSIZE];
*	fstrlcat(s, buffer, BUFFERSIZE);
* @endcode
*/
MARKENGINE_API size_t fstrlcat(
	char* dest, 
	const char* src,
	size_t dest_size
);

/**
* fstrlcatw
* fstrlcat는 strcat, strncat에 비해 안전하게 문자열을 병합 합니다.
* fstrlcat는 dest buffer의 크기를 확인하여 최대 dest buffer의 크기보다 -1만큼 병합하게 됩니다.
* @brief dest_size의 버퍼수만큼 dest에 안전하게 복사합니다.
* @param src 병합하고자 하는 문자열 소스
* @param dest 병합가 될 목적지 버퍼
* @param dest_size 병합 될 버퍼의 사이즈
* @result 병합된 문자열의 사이즈 (결과값은 src와 dest가 온전히 합쳐졌을때의 크기를 반환한다. 실제로는 dest buffer의 사이즈가 부족하여 적게 복사될수 있다)
* @code
*	char *s, *p, buffer[BUFFERSIZE];
*	fstrlcat(s, buffer, BUFFERSIZE);
* @endcode
*/
MARKENGINE_API size_t fstrlcat(
	wchar_t* dest,
	const wchar_t* src,
	size_t dest_size
);

/**
* fstrlcat16
* fstrlcat는 strcat, strncat에 비해 안전하게 문자열을 병합 합니다.
* fstrlcat는 dest buffer의 크기를 확인하여 최대 dest buffer의 크기보다 -1만큼 병합하게 됩니다.
* @brief dest_size의 버퍼수만큼 dest에 안전하게 복사합니다.
* @param src 병합하고자 하는 문자열 소스
* @param dest 병합가 될 목적지 버퍼
* @param dest_size 병합 될 버퍼의 사이즈
* @result 병합된 문자열의 사이즈 (결과값은 src와 dest가 온전히 합쳐졌을때의 크기를 반환한다. 실제로는 dest buffer의 사이즈가 부족하여 적게 복사될수 있다)
* @code
*	char *s, *p, buffer[BUFFERSIZE];
*	fstrlcat(s, buffer, BUFFERSIZE);
* @endcode
*/
MARKENGINE_API size_t fstrlcat(
	char16_t* dest, 
	const char16_t* src, 
	size_t dest_size
);

/**
* fstrlcat32
* fstrlcat는 strcat, strncat에 비해 안전하게 문자열을 병합 합니다.
* fstrlcat는 dest buffer의 크기를 확인하여 최대 dest buffer의 크기보다 -1만큼 병합하게 됩니다.
* @brief dest_size의 버퍼수만큼 dest에 안전하게 복사합니다.
* @param src 병합하고자 하는 문자열 소스
* @param dest 병합가 될 목적지 버퍼
* @param dest_size 병합 될 버퍼의 사이즈
* @result 병합된 문자열의 사이즈 (결과값은 src와 dest가 온전히 합쳐졌을때의 크기를 반환한다. 실제로는 dest buffer의 사이즈가 부족하여 적게 복사될수 있다)
* @code
*	char *s, *p, buffer[BUFFERSIZE];
*	fstrlcat(s, buffer, BUFFERSIZE);
* @endcode
*/
MARKENGINE_API size_t fstrlcat(
	char32_t* dest,
	const char32_t* src,
	size_t dest_size
);

/**
* fstrupr
* ASCII 문자열을 대문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrupr C 함수와 유사합니다.
* @brief ASCII 문자열을 대문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API char* fstrupr(
	char* src
);

/**
* fstruprw
* ASCII 문자열을 대문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrupr C 함수와 유사합니다.
* @brief ASCII 문자열을 대문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API wchar_t* fstrupr(
	wchar_t* src
);

/**
* fstrupr16
* ASCII 문자열을 대문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrupr C 함수와 유사합니다.
* @brief ASCII 문자열을 대문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API char16_t* fstrupr(
	char16_t* src
);

/**
* fstrupr32
* ASCII 문자열을 대문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrupr C 함수와 유사합니다.
* @brief ASCII 문자열을 대문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API char32_t* fstrupr(
	char32_t* src
);

/**
* fstrlwr
* ASCII 문자열을 소문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrlwr C 함수와 유사합니다.
* @brief ASCII 문자열을 소문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API char* fstrlwr(
	char* src
);

/**
* fstrlwrw
* ASCII 문자열을 소문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrlwr C 함수와 유사합니다.
* @brief ASCII 문자열을 소문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API wchar_t* fstrlwr(
	wchar_t* src
);

/**
* fstrlwr16
* ASCII 문자열을 소문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrlwr C 함수와 유사합니다.
* @brief ASCII 문자열을 소문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API char16_t* fstrlwr(
	char16_t* src
);

/**
* fstrlwr32
* ASCII 문자열을 소문자로 변환합니다. 이것은 가끔 볼 수 있는 fstrlwr C 함수와 유사합니다.
* @brief ASCII 문자열을 소문자로 변환합니다.
* @param src 변환하고자 하는 문자열
* @result 변환된 src문자열 포인트를 바로 변환합니다. (내부 할당 없음)
*/
MARKENGINE_API char32_t* fstrlwr(
	char32_t* src
);


/**
* fstrchr
* src에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 발견되지 않으면 NULL을 반환합니다. 이것은 strchr C 함수와 유사합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstrchr(
	const char* str, 
	char c
);

/**
* fstrchrw
* src에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 발견되지 않으면 NULL을 반환합니다. 이것은 strchr C 함수와 유사합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstrchr(
	const wchar_t* str,
	wchar_t c
);

/**
* fstrchr16
* src에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 발견되지 않으면 NULL을 반환합니다. 이것은 strchr C 함수와 유사합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstrchr(
	const char16_t* str, 
	char16_t c
);

/**
* fstrchr32
* src에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 발견되지 않으면 NULL을 반환합니다. 이것은 strchr C 함수와 유사합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstrchr(
	const char32_t* str, 
	char32_t c
);

/**
* fstrnchr
* str에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 n문자 발견되지 않으면 NULL을 반환합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자.
* @param n 문자열 중에서 찾고자 하는 범위 (문자수)
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstrnchr(
	const char* str, 
	char c,
	size_t n
);

/**
* fstrnchrw
* str에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 n문자 발견되지 않으면 NULL을 반환합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자.
* @param n 문자열 중에서 찾고자 하는 범위 (문자수)
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstrnchr(
	const wchar_t* str, 
	wchar_t c,
	size_t n
);

/**
* fstrnchr16
* str에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 n문자 발견되지 않으면 NULL을 반환합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자.
* @param n 문자열 중에서 찾고자 하는 범위 (문자수)
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstrnchr(
	const char16_t* str, 
	char16_t c, 
	size_t n
);

/**
* fstrnchr32
* str에서 c의 첫 번째 발생에 대한 포인터를 반환하거나, c가 n문자 발견되지 않으면 NULL을 반환합니다.
* @brief c의 첫 번째 발생에 대한 포인터를 반환
* @param src 변환하고자 하는 문자열
* @param c 찾고자 하는 문자.
* @param n 문자열 중에서 찾고자 하는 범위 (문자수)
* @result 찾게된 src문자열 포인트를 바로 변환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstrnchr(
	const char32_t* str,
	char32_t c, 
	size_t n
);

/**
* fstrcspn
* str1에서 문자 단위로 스캔하여 읽은 문자 수를 반환합니다. 이것은 strcspn C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 읽은 문자 수를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 str2 갯수를 반환합니다.
*/
MARKENGINE_API size_t fstrcspn(
	const char* str1, 
	const char* str2
);

/**
* fstrcspnw
* str1에서 문자 단위로 스캔하여 읽은 문자 수를 반환합니다. 이것은 strcspn C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 읽은 문자 수를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 str2 갯수를 반환합니다.
*/
MARKENGINE_API size_t fstrcspn(
	const wchar_t* str1,
	const wchar_t* str2
);

/**
* fstrcspn16
* str1에서 문자 단위로 스캔하여 읽은 문자 수를 반환합니다. 이것은 strcspn C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 읽은 문자 수를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 str2 갯수를 반환합니다.
*/
MARKENGINE_API size_t fstrcspn(
	const char16_t* str1, 
	const char16_t* str2
);

/**
* fstrcspn32
* str1에서 문자 단위로 스캔하여 읽은 문자 수를 반환합니다. 이것은 strcspn C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 읽은 문자 수를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 str2 갯수를 반환합니다.
*/
MARKENGINE_API size_t fstrcspn(
	const char32_t* str1, 
	const char32_t* str2
);

/**
* fstrpbrk
* str1을 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다. 이것은 strpbrk C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstrpbrk(
	const char* str1, 
	const char* str2
);

/**
* fstrpbrkw
* str1을 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다. 이것은 strpbrk C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstrpbrk(
	const wchar_t* str1,
	const wchar_t* str2
);

/**
* fstrpbrk16
* str1을 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다. 이것은 strpbrk C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstrpbrk(
	const char16_t* str1,
	const char16_t* str2
);

/**
* fstrpbrk32
* str1을 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다. 이것은 strpbrk C 함수와 유사합니다.
* @brief 문자 단위로 스캔하여 첫 번째 포인터를 반환합니다.
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstrpbrk(
	const char32_t* str1, 
	const char32_t* str2
);

/**
* fstrrchr
* src에서 c의 마지막 항목을 반환하거나 c가 발견되지 않으면 NULL을 반환합니다. 이것은 strrchr C 함수와 유사합니다.
* null 종료 문자는 문자열의 일부로 포함되며 검색할 수도 있습니다.
* @brief src에서 c의 마지막 항목을 반환
* @param str1 문자열 소스 전체
* @param c 찾고자 하는 문자
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstrrchr(
	const char* str, 
	char c
);

/**
* fstrrchrw
* src에서 c의 마지막 항목을 반환하거나 c가 발견되지 않으면 NULL을 반환합니다. 이것은 strrchr C 함수와 유사합니다.
* null 종료 문자는 문자열의 일부로 포함되며 검색할 수도 있습니다.
* @brief src에서 c의 마지막 항목을 반환
* @param str1 문자열 소스 전체
* @param c 찾고자 하는 문자
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstrrchr(
	const wchar_t* str,
	wchar_t c
);

/**
* fstrrchr16
* src에서 c의 마지막 항목을 반환하거나 c가 발견되지 않으면 NULL을 반환합니다. 이것은 strrchr C 함수와 유사합니다.
* null 종료 문자는 문자열의 일부로 포함되며 검색할 수도 있습니다.
* @brief src에서 c의 마지막 항목을 반환
* @param str1 문자열 소스 전체
* @param c 찾고자 하는 문자
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstrrchr(
	const char16_t* str,
	char16_t c
);

/**
* fstrrchr32
* src에서 c의 마지막 항목을 반환하거나 c가 발견되지 않으면 NULL을 반환합니다. 이것은 strrchr C 함수와 유사합니다.
* null 종료 문자는 문자열의 일부로 포함되며 검색할 수도 있습니다.
* @brief src에서 c의 마지막 항목을 반환
* @param str1 문자열 소스 전체
* @param c 찾고자 하는 문자
* @result 찾게된 src포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstrrchr(
	const char32_t* str,
	char32_t c
);

/**
* fstrspn
* str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @brief str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src 오프셋을 반환합니다.
*/
MARKENGINE_API size_t fstrspn(
	const char* str1,
	const char* str2
);

/**
* fstrspnw
* str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @brief str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src 오프셋을 반환합니다.
*/
MARKENGINE_API size_t fstrspn(
	const wchar_t* str1, 
	const wchar_t* str2
);

/**
* fstrspn16
* str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @brief str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src 오프셋을 반환합니다.
*/
MARKENGINE_API size_t fstrspn(
	const char16_t* str1,
	const char16_t* str2
);

/**
* fstrspn32
* str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @brief str1에서 일치하지 않는 첫 번째 문자의 오프셋 찾기
* @param str1 문자열 소스 전체
* @param str2 찾고자 하는 단어
* @result 찾게된 src 오프셋을 반환합니다.
*/
MARKENGINE_API size_t fstrspn(
	const char32_t* str1, 
	const char32_t* str2
);

/**
* fstrstr
* str 내에서 sub의 첫 번째 발생을 찾습니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstrstr(
	const char* str, 
	const char* sub
);

/**
* fstrstrw
* str 내에서 sub의 첫 번째 발생을 찾습니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstrstr(
	const wchar_t* str, 
	const wchar_t* sub
);

/**
* fstrstr16
* str 내에서 sub의 첫 번째 발생을 찾습니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstrstr(
	const char16_t* str, 
	const char16_t* sub
);

/**
* fstrstr32
* str 내에서 sub의 첫 번째 발생을 찾습니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstrstr(
	const char32_t* str, 
	const char32_t* sub
);

/**
* fstristr
* fstrstr의 대소문자를 구분하지 않는 버전입니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstristr(
	const char* str, 
	const char* sub
);

/**
* fstristrw
* fstrstr의 대소문자를 구분하지 않는 버전입니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstristr(
	const wchar_t* str,
	const wchar_t* sub
);

/**
* fstristr16
* fstrstr의 대소문자를 구분하지 않는 버전입니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstristr(
	const char16_t* str, 
	const char16_t* sub
);

/**
* fstristr32
* fstrstr의 대소문자를 구분하지 않는 버전입니다.
* @brief str 내에서 sub의 첫 번째 발생을 찾습니다.
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstristr(
	const char32_t* str, 
	const char32_t* sub
);

/**
* fstrrstr
* str 내에서 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstrrstr(
	const char* str, 
	const char* sub
);

/**
* fstrrstrw
* str 내에서 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstrrstr(
	const wchar_t* str,
	const wchar_t* sub
);

/**
* fstrrstr16
* str 내에서 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstrrstr(
	const char16_t* str, 
	const char16_t* sub
);

/**
* fstrrstr32
* str 내에서 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstrrstr(
	const char32_t* str,
	const char32_t* sub
);

/**
* fstrrstr
* str 내에서 대소문자 구분없이 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char* fstrirstr(
	const char* str, 
	const char* sub
);

/**
* fstrrstrw
* str 내에서 대소문자 구분없이 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API wchar_t* fstrirstr(
	const wchar_t* str, 
	const wchar_t* sub
);

/**
* fstrrstr16
* str 내에서 대소문자 구분없이 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char16_t* fstrirstr(
	const char16_t* str, 
	const char16_t* sub
);

/**
* fstrrstr32
* str 내에서 대소문자 구분없이 sub의 마지막 발생을 찾습니다.
* @brief str 내에서 sub의 마지막 발생을 찾습니다
* @param str 문자열 소스 전체
* @param sub 찾고자 하는 단어
* @result 찾게된 str포인터를 반환합니다. 실패할 경우 nullptr반환
*/
MARKENGINE_API char32_t* fstrirstr(
	const char32_t* str, 
	const char32_t* sub
);

/**
* fstrstart
* str이 prefix로 시작하면 true를 반환합니다.
* @brief str이 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstrstart(
	const char* str, 
	const char* prefix
);

/**
* fstrstartw
* str이 prefix로 시작하면 true를 반환합니다.
* @brief str이 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstrstart(
	const wchar_t* str,
	const wchar_t* prefix
);

/**
* fstrstart16
* str이 prefix로 시작하면 true를 반환합니다.
* @brief str이 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstrstart(
	const char16_t* str,
	const char16_t* prefix
);

/**
* fstrstart32
* str이 prefix로 시작하면 true를 반환합니다.
* @brief str이 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstrstart(
	const char32_t* str, 
	const char32_t* prefix
);

/**
* fstristart
* str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다.
* @brief str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstristart(
	const char* str,
	const char* prefix
);

/**
* fstristart
* str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다.
* @brief str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstristart(
	const wchar_t* str, 
	const wchar_t* prefix
);

/**
* fstristart
* str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다.
* @brief str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstristart(
	const char16_t* str,
	const char16_t* prefix
);

/**
* fstristart
* str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다.
* @brief str이 대소문자를 구별하지 않고 prefix로 시작하면 true를 반환합니다. 빈 prefix는 str이 무엇인지에 관계없이 성공합니다.
* @param str 문자열 소스 전체
* @param prefix 시작할때 체크하고자 하는 단어
* @result 성공시 true, 실패시 false
*/
MARKENGINE_API bool fstristart(
	const char32_t* str,
	const char32_t* prefix
);

/**
* fstrend
* str 문장의 마지막 포인터를 반환합니다.
* @brief str 문장의 마지막 포인터를 반환합니다.
* @param str 문자열 소스 전체
* @result str문자열의 null이 아닌 마지막 포인터
*/
MARKENGINE_API char* fstrend(
	const char* str
);

/**
* fstrendw
* str 문장의 마지막 포인터를 반환합니다.
* @brief str 문장의 마지막 포인터를 반환합니다.
* @param str 문자열 소스 전체
* @result str문자열의 null이 아닌 마지막 포인터
*/
MARKENGINE_API wchar_t* fstrend(
	const wchar_t* str
);

/**
* fstrend16
* str 문장의 마지막 포인터를 반환합니다.
* @brief str 문장의 마지막 포인터를 반환합니다.
* @param str 문자열 소스 전체
* @result str문자열의 null이 아닌 마지막 포인터
*/
MARKENGINE_API char16_t* fstrend(
	const char16_t* str
);

/**
* fstrend32
* str 문장의 마지막 포인터를 반환합니다.
* @brief str 문장의 마지막 포인터를 반환합니다.
* @param str 문자열 소스 전체
* @result str문자열의 null이 아닌 마지막 포인터
*/
MARKENGINE_API char32_t* fstrend(
	const char32_t* str
);


/**
* fstrtok
* str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @brief str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @param str 찾을 토큰 또는 토큰이 포함된 문자열입니다.
* @param delim 사용할 구분 기호 문자 집합입니다.
* @param context 함수 호출 간에 위치 정보를 저장하는 데 사용됩니다.
* @result str에서 발견된 다음 토큰에 대한 포인터를 반환합니다. 토큰을 더 이상 찾을 수 없으면 를 반환 NULL 합니다. 각 호출은 str 반환된 토큰 이후에 발생하는 첫 번째 구분 기호에 대해 null 문자를 대체하여 수정합니다.
* @code
*	char16_t str[] = u"A,B,C,D,E,F,G";
	char16_t* context = nullptr;
	char16_t* r = stdc::fstrtok(str, u",", &context);
	while (r)
		r = stdc::fstrtok(nullptr, u",", &context);
* @endcode
*/
MARKENGINE_API char* fstrtok(
	char* str, 
	const char* delim, 
	char** context
);

/**
* fstrtokw
* str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @brief str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @param str 찾을 토큰 또는 토큰이 포함된 문자열입니다.
* @param delim 사용할 구분 기호 문자 집합입니다.
* @param context 함수 호출 간에 위치 정보를 저장하는 데 사용됩니다.
* @result str에서 발견된 다음 토큰에 대한 포인터를 반환합니다. 토큰을 더 이상 찾을 수 없으면 를 반환 NULL 합니다. 각 호출은 str 반환된 토큰 이후에 발생하는 첫 번째 구분 기호에 대해 null 문자를 대체하여 수정합니다.
* @code
*	char16_t str[] = L"A,B,C,D,E,F,G";
	char16_t* context = nullptr;
	char16_t* r = stdc::fstrtok(str, u",", &context);
	while (r)
		r = stdc::fstrtok(nullptr, u",", &context);
* @endcode
*/
MARKENGINE_API wchar_t* fstrtok(
	wchar_t* str, 
	const wchar_t* delim, 
	wchar_t** context
);

/**
* fstrtok16
* str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @brief str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @param str 찾을 토큰 또는 토큰이 포함된 문자열입니다.
* @param delim 사용할 구분 기호 문자 집합입니다.
* @param context 함수 호출 간에 위치 정보를 저장하는 데 사용됩니다.
* @result str에서 발견된 다음 토큰에 대한 포인터를 반환합니다. 토큰을 더 이상 찾을 수 없으면 를 반환 NULL 합니다. 각 호출은 str 반환된 토큰 이후에 발생하는 첫 번째 구분 기호에 대해 null 문자를 대체하여 수정합니다.
* @code
*	char16_t str[] = u"A,B,C,D,E,F,G";
	char16_t* context = nullptr;
	char16_t* r = stdc::fstrtok(str, u",", &context);
	while (r)
		r = stdc::fstrtok(nullptr, u",", &context);
* @endcode
*/
MARKENGINE_API char16_t* fstrtok(
	char16_t* str, 
	const char16_t* delim, 
	char16_t** context
);

/**
* fstrtok32
* str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @brief str을 delim의 문자로 구분된 토큰으로 구문 분석합니다.
* @param str 찾을 토큰 또는 토큰이 포함된 문자열입니다.
* @param delim 사용할 구분 기호 문자 집합입니다.
* @param context 함수 호출 간에 위치 정보를 저장하는 데 사용됩니다.
* @result str에서 발견된 다음 토큰에 대한 포인터를 반환합니다. 토큰을 더 이상 찾을 수 없으면 를 반환 NULL 합니다. 각 호출은 str 반환된 토큰 이후에 발생하는 첫 번째 구분 기호에 대해 null 문자를 대체하여 수정합니다.
* @code
*	char16_t str[] = U"A,B,C,D,E,F,G";
	char16_t* context = nullptr;
	char16_t* r = stdc::fstrtok(str, u",", &context);
	while (r)
		r = stdc::fstrtok(nullptr, u",", &context);
* @endcode
*/
MARKENGINE_API char32_t* fstrtok(
	char32_t* str,
	const char32_t* delim, 
	char32_t** context
);

/**
* fstrrev2
* str을 뒤집어서 반환합니다.
* @brief str을 뒤집어서 반환합니다.
* @param str 문자열 소스 전체
* @result 분리된 str 포인터를 반환합니다.
*/
MARKENGINE_API char* fstrrev2(
	char* src
);

/**
* fstrrev2
* str을 뒤집어서 반환합니다.
* @brief str을 뒤집어서 반환합니다.
* @param str 문자열 소스 전체
* @result 분리된 str 포인터를 반환합니다.
*/
MARKENGINE_API wchar_t* fstrrev2(
	wchar_t* src
);

/**
* fstrrev2
* str을 뒤집어서 반환합니다.
* @brief str을 뒤집어서 반환합니다.
* @param str 문자열 소스 전체
* @result 분리된 str 포인터를 반환합니다.
*/
MARKENGINE_API char16_t* fstrrev2(
	char16_t* src
);

/**
* fstrrev2
* str을 뒤집어서 반환합니다.
* @brief str을 뒤집어서 반환합니다.
* @param str 문자열 소스 전체
* @result 분리된 str 포인터를 반환합니다.
*/
MARKENGINE_API char32_t* fstrrev2(
	char32_t* src
);

/**
* fstrstrip
* _isspace 함수에 의해 정의된 선행 및 후행 공백을 제거합니다.
* @brief 선행 및 후행 공백을 제거합니다. 선행 공백이 제거되지 않고 오히려  str 에서 공백이 아닌 첫 번째 문자가 반환됩니다
* @brief 따라서 주의가 필요 합니다. 이 함수는 포인터 >= str을 반환할 수 있으며 이를 해제할 수 없습니다. 동적 메모리일 경우 반환된 값으로 해제(free/delete)하지 마십시오.
* @param str 문자열 소스 전체
* @result 공백이 제거된 str포인터가 반환됩니다.
*/
MARKENGINE_API char* fstrstrip(
	char* str
);

/**
* fstrstripw
* _isspace 함수에 의해 정의된 선행 및 후행 공백을 제거합니다.
* @brief 선행 및 후행 공백을 제거합니다. 선행 공백이 제거되지 않고 오히려  str 에서 공백이 아닌 첫 번째 문자가 반환됩니다
* @brief 따라서 주의가 필요 합니다. 이 함수는 포인터 >= str을 반환할 수 있으며 이를 해제할 수 없습니다. 동적 메모리일 경우 반환된 값으로 해제(free/delete)하지 마십시오.
* @param str 문자열 소스 전체
* @result 공백이 제거된 str포인터가 반환됩니다.
*/
MARKENGINE_API wchar_t* fstrstrip(
	wchar_t* str
);

/**
* fstrstrip16
* _isspace 함수에 의해 정의된 선행 및 후행 공백을 제거합니다.
* @brief 선행 및 후행 공백을 제거합니다. 선행 공백이 제거되지 않고 오히려  str 에서 공백이 아닌 첫 번째 문자가 반환됩니다
* @brief 따라서 주의가 필요 합니다. 이 함수는 포인터 >= str을 반환할 수 있으며 이를 해제할 수 없습니다. 동적 메모리일 경우 반환된 값으로 해제(free/delete)하지 마십시오.
* @param str 문자열 소스 전체
* @result 공백이 제거된 str포인터가 반환됩니다.
*/
MARKENGINE_API char16_t* fstrstrip(
	char16_t* str
);

/**
* fstrstrip32
* _isspace 함수에 의해 정의된 선행 및 후행 공백을 제거합니다.
* @brief 선행 및 후행 공백을 제거합니다. 선행 공백이 제거되지 않고 오히려  str 에서 공백이 아닌 첫 번째 문자가 반환됩니다
* @brief 따라서 주의가 필요 합니다. 이 함수는 포인터 >= str을 반환할 수 있으며 이를 해제할 수 없습니다. 동적 메모리일 경우 반환된 값으로 해제(free/delete)하지 마십시오.
* @param str 문자열 소스 전체
* @result 공백이 제거된 str포인터가 반환됩니다.
*/
MARKENGINE_API char32_t* fstrstrip(
	char32_t* str
);

/**
* fstrcmp
* 문자열을 비교 합니다.
* @brief 문자열을 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrcmp(
	const char* str1,
	const char* str2
);

/**
* fstrcmpw
* 문자열을 비교 합니다.
* @brief 문자열을 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrcmp(
	const wchar_t* str1, 
	const wchar_t* str2
);

/**
* fstrcmp16
* 문자열을 비교 합니다.
* @brief 문자열을 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrcmp(
	const char16_t* str1, 
	const char16_t* str2
);

/**
* fstrcmp32
* 문자열을 비교 합니다.
* @brief 문자열을 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrcmp(
	const char32_t* str1,
	const char32_t* str2
);

/**
* fstrcmp
* 문자열을 n개수 만큼 비교 합니다.
* @brief 문자열을 n개수 만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrncmp(
	const char* str1,
	const char* str2, 
	size_t n
);

/**
* fstrcmpw
* 문자열을 n개수 만큼 비교 합니다.
* @brief 문자열을 n개수 만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrncmp(
	const wchar_t* str1,
	const wchar_t* str2, 
	size_t n
);

/**
* fstrcmp16
* 문자열을 n개수 만큼 비교 합니다.
* @brief 문자열을 n개수 만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrncmp(
	const char16_t* str1,
	const char16_t* str2, 
	size_t n
);

/**
* fstrcmp32
* 문자열을 n개수 만큼 비교 합니다.
* @brief 문자열을 n개수 만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrncmp(
	const char32_t* str1,
	const char32_t* str2,
	size_t n
);

/**
* fstricmp
* 문자열을 대소문자 구별없이 비교 합니다.
* @brief 문자열을 대소문자 구별없이 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstricmp(
	const char* str1, 
	const char* str2
);

/**
* fstricmpw
* 문자열을 대소문자 구별없이 비교 합니다.
* @brief 문자열을 대소문자 구별없이 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstricmp(
	const wchar_t* str1, 
	const wchar_t* str2
);

/**
* fstricmp16
* 문자열을 대소문자 구별없이 비교 합니다.
* @brief 문자열을 대소문자 구별없이 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstricmp(
	const char16_t* str1, 
	const char16_t* str2
);

/**
* fstricmp32
* 문자열을 대소문자 구별없이 비교 합니다.
* @brief 문자열을 대소문자 구별없이 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstricmp(
	const char32_t* str1,
	const char32_t* str2
);

/**
* fstrincmp
* 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @brief 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrincmp(
	const char* str1, 
	const char* str2, 
	size_t n
);

/**
* fstrincmpw
* 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @brief 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrincmp(
	const wchar_t* str1, 
	const wchar_t* str2, 
	size_t n
);

/**
* fstrincmp16
* 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @brief 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrincmp(
	const char16_t* str1, 
	const char16_t* str2, 
	size_t n
);

/**
* fstrincmp32
* 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @brief 문자열을 대소문자 구별없이 n개의 길이만큼 비교 합니다.
* @param str1 비교하고자 하는 문자열1
* @param str2 비교하고자 하는 문자열2
* @param n 비교하고자 하는 문자열 수
* @result 같으면 0, 다를경우 0이 아는 수를 반환합니다.
*/
MARKENGINE_API int fstrincmp(
	const char32_t* str1,
	const char32_t* str2, 
	size_t n
);


#endif // __STRINGS_H__
