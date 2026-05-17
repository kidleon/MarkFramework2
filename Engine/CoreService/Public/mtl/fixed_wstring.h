#pragma once

#include "fixed_string.h"


// ----------------------------------------------------------------------------
// mtl::fixed_wstring<N> — fixed_basic_string<wchar_t, N>의 별칭.
//
// 구현은 fixed_string.h의 fixed_basic_string 템플릿을 그대로 사용한다.
// std::string : std::wstring = mtl::fixed_string : mtl::fixed_wstring
//
// 사용 용도:
//   - Windows API와의 상호작용 (HANDLE, MAX_PATH 경로, 윈도우 타이틀 등).
//   - 유니코드 텍스트가 필요한 짧은 라벨 (메뉴 항목, UI 텍스트).
//   - DirectX/Win32 함수에 전달할 null-terminated wchar_t 버퍼.
//
// 안 쓰는 게 좋은 경우:
//   - UTF-8 처리 — std::string + u8 리터럴 또는 fixed_string<N> 사용.
//   - 크로스플랫폼 텍스트 처리 (wchar_t는 Windows에서만 UTF-16, 그 외 보장 없음).
//
// 사용 특성/정책은 fixed_string과 동일 ([fixed_string.h] 참고).
//
// 사용 예:
//   mtl::fixed_wstring<128> ws = L"hello";
//   ws += L", world";
//   ws.format_append(L"{}", 42);
//   if (ws.starts_with(L"hello")) { ... }
//
//   auto path = mtl::fixed_wstring<MAX_PATH>::from_format(L"{}\\{}.png", folder, name);
//   ::CreateFileW(path.c_str(), ...);
// ----------------------------------------------------------------------------

namespace mtl
{
	template <std::size_t N>
	using fixed_wstring = fixed_basic_string<wchar_t, N>;
}
