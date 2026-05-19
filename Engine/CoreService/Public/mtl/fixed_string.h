#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <format>
#include <initializer_list>
#include <iterator>
#include <string>        // std::char_traits
#include <string_view>
#include <type_traits>
#include <utility>


// ----------------------------------------------------------------------------
// mtl::fixed_basic_string<CharT, N> — 고정 용량 문자열. 동적 할당 없음.
//
// 사용 용도:
//   - 짧고 상한이 예측 가능한 문자열 (파일 경로, 객체/리소스 이름, 로그 라벨).
//   - struct 멤버로 두는 문자열 — POD-like 다루기 (직렬화/복사 용이).
//   - 매 프레임 만들었다 버리는 임시 문자열 (디버그 라벨, HUD 표시 등).
//   - 동적 할당 회피가 중요한 핫 패스 (포맷팅 후 즉시 소비).
//   - Windows API 등 null-terminated가 필수인 곳 (fixed_wstring + c_str()).
//
// 안 쓰는 게 좋은 경우:
//   - 길이를 전혀 예측할 수 없는 사용자 입력 — Release에서 잘림 위험.
//   - 매우 가변적인 길이 또는 N이 1KB 이상으로 큰 경우 → std::string.
//   - 빈번한 append/insert로 크기가 동적으로 자라는 빌더 패턴 → string_buffer.
//   - 짧은 SSO 문자열로 충분한 경우 → std::string의 SSO도 동적할당 안 함.
//
// 특성:
//   - 내부 버퍼 CharT[N + 1] (항상 null-terminated 보장 — c_str() 안전).
//   - std::basic_string_view<CharT>로 암묵 변환 가능.
//   - std::format에 인자로 그대로 전달 가능 (formatter 특수화 제공).
//   - format_append / from_format 헬퍼로 std::format 통합.
//
// Overflow 정책:
//   - Debug (NDEBUG 미정의): assert 실패 → 버그를 즉시 감지.
//   - Release (NDEBUG 정의): 자동 truncation (안전하게 잘림, UB 없음).
//   - 이 정책은 데이터 overflow에만 적용 (assign/append/insert/push_back/resize).
//   - 인덱스 범위 위반(at, [], front, back)은 assert만 — 명확한 호출자 버그.
//
// 별칭:
//   fixed_string<N>   = fixed_basic_string<char,    N>
//   fixed_wstring<N>  = fixed_basic_string<wchar_t, N>   (fixed_wstring.h)
//
// 사용 예:
//   mtl::fixed_string<64> s = "hello";
//   s += ", world";
//   s.format_append("{}", 42);               // "hello, world42"
//   if (s.starts_with("hello")) { ... }
//
//   mtl::fixed_string<4> tiny;
//   tiny.assign("overflowing string"); // debug: assert; release: "over"
// ----------------------------------------------------------------------------

namespace mtl
{
	template <typename CharT, std::size_t N>
	class fixed_basic_string
	{
		static_assert(N > 0, "fixed_basic_string: N must be greater than 0.");

	public:
		using value_type = CharT;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = CharT&;
		using const_reference = const CharT&;
		using pointer = CharT*;
		using const_pointer = const CharT*;
		using iterator = CharT*;
		using const_iterator = const CharT*;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using traits_type = std::char_traits<CharT>;
		using view_type = std::basic_string_view<CharT>;

		static constexpr size_type npos = static_cast<size_type>(-1);
		static constexpr size_type kCapacity = N;

		// --------------------------------------------------------------------
		// 생성
		// --------------------------------------------------------------------
		fixed_basic_string() noexcept : m_size(0)
		{
			m_buffer[0] = CharT();
		}

		fixed_basic_string(const CharT* str)
		{
			assign(str);
		}

		fixed_basic_string(const CharT* str, size_type count)
		{
			assign(str, count);
		}

		fixed_basic_string(size_type count, CharT ch)
		{
			assign(count, ch);
		}

		fixed_basic_string(view_type sv)
		{
			assign(sv);
		}

		fixed_basic_string(std::initializer_list<CharT> il)
		{
			assign(il.begin(), il.size());
		}

		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		fixed_basic_string(InputIt first, InputIt last)
			: m_size(0)
		{
			m_buffer[0] = CharT();  // 빈 범위에서도 null-terminated 보장
			for (; first != last; ++first) push_back(*first);
		}

		fixed_basic_string(const fixed_basic_string& other) noexcept
			: m_size(other.m_size)
		{
			traits_type::copy(m_buffer, other.m_buffer, m_size + 1);
		}

		// 다른 용량의 fixed_basic_string으로부터 복사
		template <std::size_t N2, std::enable_if_t<N2 != N, int> = 0>
		fixed_basic_string(const fixed_basic_string<CharT, N2>& other)
		{
			assign(other.data(), other.size());
		}

		// --------------------------------------------------------------------
		// 대입
		// --------------------------------------------------------------------
		fixed_basic_string& operator=(const fixed_basic_string& other) noexcept
		{
			if (this != &other)
			{
				m_size = other.m_size;
				traits_type::copy(m_buffer, other.m_buffer, m_size + 1);
			}
			return *this;
		}

		template <std::size_t N2, std::enable_if_t<N2 != N, int> = 0>
		fixed_basic_string& operator=(const fixed_basic_string<CharT, N2>& other)
		{
			assign(other.data(), other.size());
			return *this;
		}

		fixed_basic_string& operator=(const CharT* str) { assign(str); return *this; }
		fixed_basic_string& operator=(view_type sv) { assign(sv); return *this; }
		fixed_basic_string& operator=(std::initializer_list<CharT> il) { assign(il.begin(), il.size()); return *this; }
		fixed_basic_string& operator=(CharT ch)
		{
			m_buffer[0] = ch;
			m_buffer[1] = CharT();
			m_size = 1;
			return *this;
		}

		// --------------------------------------------------------------------
		// assign
		// --------------------------------------------------------------------
		void assign(const CharT* str)
		{
			assert(str != nullptr);
			assign(str, traits_type::length(str));
		}

		void assign(const CharT* str, size_type count)
		{
			assert(count <= N && "fixed_basic_string::assign: 용량 초과");
			if (count > N) count = N;  // release-safe truncation
			traits_type::copy(m_buffer, str, count);
			m_buffer[count] = CharT();
			m_size = count;
		}

		void assign(size_type count, CharT ch)
		{
			assert(count <= N && "fixed_basic_string::assign: 용량 초과");
			if (count > N) count = N;  // release-safe truncation
			traits_type::assign(m_buffer, count, ch);
			m_buffer[count] = CharT();
			m_size = count;
		}

		void assign(view_type sv) { assign(sv.data(), sv.size()); }

		// --------------------------------------------------------------------
		// 요소 접근
		// --------------------------------------------------------------------
		reference       operator[](size_type i)       noexcept { assert(i <= m_size); return m_buffer[i]; }
		const_reference operator[](size_type i) const noexcept { assert(i <= m_size); return m_buffer[i]; }

		reference       at(size_type i)       { assert(i < m_size && "fixed_basic_string::at: index out of range"); return m_buffer[i]; }
		const_reference at(size_type i) const { assert(i < m_size && "fixed_basic_string::at: index out of range"); return m_buffer[i]; }

		reference       front()       noexcept { assert(!empty()); return m_buffer[0]; }
		const_reference front() const noexcept { assert(!empty()); return m_buffer[0]; }
		reference       back()        noexcept { assert(!empty()); return m_buffer[m_size - 1]; }
		const_reference back()  const noexcept { assert(!empty()); return m_buffer[m_size - 1]; }

		const CharT*    c_str() const noexcept { return m_buffer; }
		const CharT*    data()  const noexcept { return m_buffer; }
		CharT*          data()        noexcept { return m_buffer; }

		// --------------------------------------------------------------------
		// 이터레이터
		// --------------------------------------------------------------------
		iterator        begin()        noexcept { return m_buffer; }
		const_iterator  begin()  const noexcept { return m_buffer; }
		const_iterator  cbegin() const noexcept { return m_buffer; }
		iterator        end()          noexcept { return m_buffer + m_size; }
		const_iterator  end()    const noexcept { return m_buffer + m_size; }
		const_iterator  cend()   const noexcept { return m_buffer + m_size; }

		reverse_iterator        rbegin()        noexcept { return reverse_iterator(end()); }
		const_reverse_iterator  rbegin()  const noexcept { return const_reverse_iterator(end()); }
		const_reverse_iterator  crbegin() const noexcept { return rbegin(); }
		reverse_iterator        rend()          noexcept { return reverse_iterator(begin()); }
		const_reverse_iterator  rend()    const noexcept { return const_reverse_iterator(begin()); }
		const_reverse_iterator  crend()   const noexcept { return rend(); }

		// --------------------------------------------------------------------
		// 용량
		// --------------------------------------------------------------------
		size_type size()    const noexcept { return m_size; }
		size_type length()  const noexcept { return m_size; }
		bool      empty()   const noexcept { return m_size == 0; }
		bool      full()    const noexcept { return m_size == N; }

		static constexpr size_type capacity() noexcept { return N; }
		static constexpr size_type max_size() noexcept { return N; }

		// 빈 용량 (앞으로 더 담을 수 있는 문자 수)
		size_type available() const noexcept { return N - m_size; }

		// --------------------------------------------------------------------
		// 변환
		// --------------------------------------------------------------------
		view_type to_string_view() const noexcept { return view_type(m_buffer, m_size); }
		operator view_type()       const noexcept { return view_type(m_buffer, m_size); }

		// --------------------------------------------------------------------
		// 수정
		// --------------------------------------------------------------------
		void clear() noexcept
		{
			m_size = 0;
			m_buffer[0] = CharT();
		}

		void push_back(CharT ch)
		{
			assert(m_size < N && "fixed_basic_string::push_back: 용량 초과");
			if (m_size >= N) return;  // release-safe: 가득 차면 무시
			m_buffer[m_size++] = ch;
			m_buffer[m_size] = CharT();
		}

		void pop_back() noexcept
		{
			assert(!empty());
			if (empty()) return;  // release-safe
			--m_size;
			m_buffer[m_size] = CharT();
		}

		fixed_basic_string& append(const CharT* str)
		{
			assert(str != nullptr);
			return append(str, traits_type::length(str));
		}

		fixed_basic_string& append(const CharT* str, size_type count)
		{
			assert(m_size + count <= N && "fixed_basic_string::append: 용량 초과");
			if (count > N - m_size) count = N - m_size;  // release-safe truncation
			traits_type::copy(m_buffer + m_size, str, count);
			m_size += count;
			m_buffer[m_size] = CharT();
			return *this;
		}

		fixed_basic_string& append(size_type count, CharT ch)
		{
			assert(m_size + count <= N && "fixed_basic_string::append: 용량 초과");
			if (count > N - m_size) count = N - m_size;  // release-safe truncation
			traits_type::assign(m_buffer + m_size, count, ch);
			m_size += count;
			m_buffer[m_size] = CharT();
			return *this;
		}

		fixed_basic_string& append(view_type sv) { return append(sv.data(), sv.size()); }
		fixed_basic_string& append(CharT ch) { push_back(ch); return *this; }

		template <std::size_t N2>
		fixed_basic_string& append(const fixed_basic_string<CharT, N2>& other) { return append(other.data(), other.size()); }

		fixed_basic_string& operator+=(const CharT* str)  { return append(str); }
		fixed_basic_string& operator+=(CharT ch)          { push_back(ch); return *this; }
		fixed_basic_string& operator+=(view_type sv)      { return append(sv); }
		template <std::size_t N2>
		fixed_basic_string& operator+=(const fixed_basic_string<CharT, N2>& other) { return append(other); }

		// --------------------------------------------------------------------
		// std::format 통합
		// --------------------------------------------------------------------
		template <typename... Args>
		fixed_basic_string& format_append(std::basic_format_string<CharT, std::type_identity_t<Args>...> fmt, Args&&... args)
		{
			auto out = std::format_to_n(m_buffer + m_size, static_cast<difference_type>(N - m_size),
				fmt, std::forward<Args>(args)...);
			// Debug: 출력 truncate 감지. Release: out.out 까지만 기록되어 자동 잘림.
			assert(static_cast<size_type>(out.size) <= N - m_size && "fixed_basic_string::format_append: 출력이 잘림 (truncated)");
			m_size = static_cast<size_type>(out.out - m_buffer);
			m_buffer[m_size] = CharT();
			return *this;
		}

		template <typename... Args>
		[[nodiscard]] static fixed_basic_string from_format(std::basic_format_string<CharT, std::type_identity_t<Args>...> fmt, Args&&... args)
		{
			fixed_basic_string result;
			result.format_append(fmt, std::forward<Args>(args)...);
			return result;
		}

		// --------------------------------------------------------------------
		// 삽입/삭제
		// --------------------------------------------------------------------
		fixed_basic_string& insert(size_type pos, const CharT* str, size_type count)
		{
			assert(pos <= m_size && "fixed_basic_string::insert: pos out of range");
			assert(m_size + count <= N && "fixed_basic_string::insert: 용량 초과");
			if (pos > m_size) pos = m_size;                // release-safe: 끝에 붙임
			if (count > N - m_size) count = N - m_size;    // release-safe truncation
			traits_type::move(m_buffer + pos + count, m_buffer + pos, m_size - pos);
			traits_type::copy(m_buffer + pos, str, count);
			m_size += count;
			m_buffer[m_size] = CharT();
			return *this;
		}

		fixed_basic_string& insert(size_type pos, const CharT* str)
		{
			assert(str != nullptr);
			return insert(pos, str, traits_type::length(str));
		}

		fixed_basic_string& insert(size_type pos, view_type sv) { return insert(pos, sv.data(), sv.size()); }

		fixed_basic_string& insert(size_type pos, size_type count, CharT ch)
		{
			assert(pos <= m_size && "fixed_basic_string::insert: pos out of range");
			assert(m_size + count <= N && "fixed_basic_string::insert: 용량 초과");
			if (pos > m_size) pos = m_size;                // release-safe
			if (count > N - m_size) count = N - m_size;    // release-safe truncation
			traits_type::move(m_buffer + pos + count, m_buffer + pos, m_size - pos);
			traits_type::assign(m_buffer + pos, count, ch);
			m_size += count;
			m_buffer[m_size] = CharT();
			return *this;
		}

		fixed_basic_string& erase(size_type pos = 0, size_type count = npos)
		{
			assert(pos <= m_size && "fixed_basic_string::erase: pos out of range");
			if (pos > m_size) pos = m_size;  // release-safe (no-op)
			const size_type erase_count = (std::min)(count, m_size - pos);
			traits_type::move(m_buffer + pos, m_buffer + pos + erase_count, m_size - pos - erase_count);
			m_size -= erase_count;
			m_buffer[m_size] = CharT();
			return *this;
		}

		iterator erase(const_iterator pos)
		{
			assert(pos >= begin() && pos < end());
			const size_type idx = static_cast<size_type>(pos - begin());
			erase(idx, 1);
			return begin() + idx;
		}

		iterator erase(const_iterator first, const_iterator last)
		{
			assert(first >= begin() && last <= end() && first <= last);
			const size_type idx = static_cast<size_type>(first - begin());
			const size_type count = static_cast<size_type>(last - first);
			erase(idx, count);
			return begin() + idx;
		}

		void resize(size_type new_size) { resize(new_size, CharT()); }

		void resize(size_type new_size, CharT ch)
		{
			assert(new_size <= N && "fixed_basic_string::resize: 용량 초과");
			if (new_size > N) new_size = N;  // release-safe truncation
			if (new_size > m_size)
				traits_type::assign(m_buffer + m_size, new_size - m_size, ch);
			m_size = new_size;
			m_buffer[m_size] = CharT();
		}

		void swap(fixed_basic_string& other) noexcept
		{
			// 버퍼가 인플레이스라서 std::basic_string 대비 O(N).
			const size_type max_size_val = (m_size > other.m_size) ? m_size : other.m_size;
			for (size_type i = 0; i <= max_size_val; ++i)
			{
				CharT tmp = m_buffer[i];
				m_buffer[i] = other.m_buffer[i];
				other.m_buffer[i] = tmp;
			}
			std::swap(m_size, other.m_size);
		}

		// --------------------------------------------------------------------
		// 검색
		// --------------------------------------------------------------------
		size_type find(CharT ch, size_type pos = 0) const noexcept
		{
			if (pos >= m_size) return npos;
			const CharT* p = traits_type::find(m_buffer + pos, m_size - pos, ch);
			return p ? static_cast<size_type>(p - m_buffer) : npos;
		}

		size_type find(view_type sv, size_type pos = 0) const noexcept
		{
			return to_string_view().find(sv, pos);
		}

		size_type find(const CharT* str, size_type pos = 0) const noexcept
		{
			assert(str != nullptr);
			return find(view_type(str), pos);
		}

		size_type rfind(CharT ch, size_type pos = npos) const noexcept
		{
			return to_string_view().rfind(ch, pos);
		}

		size_type rfind(view_type sv, size_type pos = npos) const noexcept
		{
			return to_string_view().rfind(sv, pos);
		}

		bool contains(CharT ch)         const noexcept { return find(ch) != npos; }
		bool contains(view_type sv)     const noexcept { return find(sv) != npos; }
		bool contains(const CharT* str) const noexcept { assert(str != nullptr); return find(str) != npos; }

		bool starts_with(view_type sv) const noexcept
		{
			return m_size >= sv.size()
				&& traits_type::compare(m_buffer, sv.data(), sv.size()) == 0;
		}
		bool starts_with(CharT ch) const noexcept { return !empty() && traits_type::eq(m_buffer[0], ch); }

		bool ends_with(view_type sv) const noexcept
		{
			return m_size >= sv.size()
				&& traits_type::compare(m_buffer + m_size - sv.size(), sv.data(), sv.size()) == 0;
		}
		bool ends_with(CharT ch) const noexcept { return !empty() && traits_type::eq(m_buffer[m_size - 1], ch); }

		// 부분 문자열 → view (소유 없음, 효율적)
		view_type substr(size_type pos = 0, size_type count = npos) const noexcept
		{
			assert(pos <= m_size);
			return view_type(m_buffer + pos, (std::min)(count, m_size - pos));
		}

		// 사전식 비교
		int compare(view_type sv) const noexcept { return to_string_view().compare(sv); }

	private:
		CharT     m_buffer[N + 1];
		size_type m_size;
	};

	// ----------------------------------------------------------------------------
	// 별칭
	// ----------------------------------------------------------------------------
	template <std::size_t N>
	using fixed_string = fixed_basic_string<char, N>;

	// ----------------------------------------------------------------------------
	// 비교 연산자 — view 변환을 활용
	// ----------------------------------------------------------------------------
	template <typename CharT, std::size_t N1, std::size_t N2>
	bool operator==(const fixed_basic_string<CharT, N1>& a, const fixed_basic_string<CharT, N2>& b) noexcept
	{
		return a.to_string_view() == b.to_string_view();
	}

	template <typename CharT, std::size_t N>
	bool operator==(const fixed_basic_string<CharT, N>& a, std::basic_string_view<CharT> b) noexcept { return a.to_string_view() == b; }

	template <typename CharT, std::size_t N>
	bool operator==(std::basic_string_view<CharT> a, const fixed_basic_string<CharT, N>& b) noexcept { return a == b.to_string_view(); }

	template <typename CharT, std::size_t N>
	bool operator==(const fixed_basic_string<CharT, N>& a, const CharT* b) noexcept { return a.to_string_view() == std::basic_string_view<CharT>(b); }

	template <typename CharT, std::size_t N>
	bool operator==(const CharT* a, const fixed_basic_string<CharT, N>& b) noexcept { return std::basic_string_view<CharT>(a) == b.to_string_view(); }

	template <typename CharT, std::size_t N1, std::size_t N2>
	bool operator!=(const fixed_basic_string<CharT, N1>& a, const fixed_basic_string<CharT, N2>& b) noexcept { return !(a == b); }
	template <typename CharT, std::size_t N>
	bool operator!=(const fixed_basic_string<CharT, N>& a, std::basic_string_view<CharT> b) noexcept { return !(a == b); }
	template <typename CharT, std::size_t N>
	bool operator!=(std::basic_string_view<CharT> a, const fixed_basic_string<CharT, N>& b) noexcept { return !(a == b); }
	template <typename CharT, std::size_t N>
	bool operator!=(const fixed_basic_string<CharT, N>& a, const CharT* b) noexcept { return !(a == b); }
	template <typename CharT, std::size_t N>
	bool operator!=(const CharT* a, const fixed_basic_string<CharT, N>& b) noexcept { return !(a == b); }

	template <typename CharT, std::size_t N1, std::size_t N2>
	bool operator<(const fixed_basic_string<CharT, N1>& a, const fixed_basic_string<CharT, N2>& b) noexcept
	{
		return a.to_string_view() < b.to_string_view();
	}
	template <typename CharT, std::size_t N>
	bool operator<(const fixed_basic_string<CharT, N>& a, std::basic_string_view<CharT> b) noexcept { return a.to_string_view() < b; }
	template <typename CharT, std::size_t N>
	bool operator<(std::basic_string_view<CharT> a, const fixed_basic_string<CharT, N>& b) noexcept { return a < b.to_string_view(); }

	template <typename CharT, std::size_t N1, std::size_t N2>
	bool operator>(const fixed_basic_string<CharT, N1>& a, const fixed_basic_string<CharT, N2>& b) noexcept { return b < a; }
	template <typename CharT, std::size_t N>
	bool operator>(const fixed_basic_string<CharT, N>& a, std::basic_string_view<CharT> b) noexcept { return b < a; }
	template <typename CharT, std::size_t N>
	bool operator>(std::basic_string_view<CharT> a, const fixed_basic_string<CharT, N>& b) noexcept { return b < a; }

	template <typename CharT, std::size_t N1, std::size_t N2>
	bool operator<=(const fixed_basic_string<CharT, N1>& a, const fixed_basic_string<CharT, N2>& b) noexcept { return !(b < a); }
	template <typename CharT, std::size_t N>
	bool operator<=(const fixed_basic_string<CharT, N>& a, std::basic_string_view<CharT> b) noexcept { return !(b < a); }
	template <typename CharT, std::size_t N>
	bool operator<=(std::basic_string_view<CharT> a, const fixed_basic_string<CharT, N>& b) noexcept { return !(b < a); }

	template <typename CharT, std::size_t N1, std::size_t N2>
	bool operator>=(const fixed_basic_string<CharT, N1>& a, const fixed_basic_string<CharT, N2>& b) noexcept { return !(a < b); }
	template <typename CharT, std::size_t N>
	bool operator>=(const fixed_basic_string<CharT, N>& a, std::basic_string_view<CharT> b) noexcept { return !(a < b); }
	template <typename CharT, std::size_t N>
	bool operator>=(std::basic_string_view<CharT> a, const fixed_basic_string<CharT, N>& b) noexcept { return !(a < b); }

	template <typename CharT, std::size_t N>
	void swap(fixed_basic_string<CharT, N>& a, fixed_basic_string<CharT, N>& b) noexcept { a.swap(b); }

} // namespace mtl


// ----------------------------------------------------------------------------
// std::format 지원 — fixed_basic_string을 그대로 std::format에 넘길 수 있게.
//
// 사용 예:
//   mtl::fixed_string<16>  s  = "world";
//   auto out  = std::format("hello, {}",  s);   // "hello, world"
//   mtl::fixed_wstring<16> ws = L"world";
//   auto wout = std::format(L"hello, {}", ws);  // L"hello, world"
// ----------------------------------------------------------------------------
template <typename CharT, std::size_t N>
struct std::formatter<mtl::fixed_basic_string<CharT, N>, CharT>
	: std::formatter<std::basic_string_view<CharT>, CharT>
{
	template <typename FormatContext>
	auto format(const mtl::fixed_basic_string<CharT, N>& s, FormatContext& ctx) const
	{
		return std::formatter<std::basic_string_view<CharT>, CharT>::format(s.to_string_view(), ctx);
	}
};
