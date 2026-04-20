// mark 네임스페이스 외부에서 std::formatter 특수화 정의
namespace std
{
	//-----------------------------------------------------------------------------
	// FLOAT2 Structure Formatter Specialization
	template <>
	struct std::formatter<mark::FLOAT2, char>
	{
		mutable std::formatter<float, char> float_fmt;

		constexpr auto parse(std::format_parse_context& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template <typename FormatContext>
		auto format(const mark::FLOAT2& f, FormatContext& ctx) const
		{
			return format_to(ctx.out(), "FLOAT2(x: {:.4f}, y: {:.4f})", f.x, f.y);
		}
	};

	template<>
	struct formatter<mark::FLOAT2, wchar_t>
	{
		mutable std::formatter<float, wchar_t> float_fmt;

		constexpr auto parse(std::basic_format_parse_context<wchar_t>& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template<typename FormatContext>
		auto format(const mark::FLOAT2& f, FormatContext& ctx) const
		{
			// 소수점 4번째까지 출력
			return format_to(ctx.out(), L"FLOAT2(x: {:.4f}, y: {:.4f})", f.x, f.y);
		}
	};

	//-----------------------------------------------------------------------------
	// FLOAT3 Structure Formatter Specialization
	template<>
	struct formatter<mark::FLOAT3, char>
	{
		mutable std::formatter<float, char> float_fmt;

		constexpr auto parse(std::format_parse_context& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template<typename FormatContext>
		auto format(const mark::FLOAT3& f, FormatContext& ctx) const
		{
			// 소수점 4번째까지 출력
			return format_to(ctx.out(), "FLOAT3(x: {:.4f}, y: {:.4f}, z: {:.4f})", f.x, f.y, f.z);
		}
	};

	template<>
	struct formatter<mark::FLOAT3, wchar_t>
	{
		mutable std::formatter<float, wchar_t> float_fmt;

		constexpr auto parse(std::basic_format_parse_context<wchar_t>& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template<typename FormatContext>
		auto format(const mark::FLOAT3& f, FormatContext& ctx) const
		{
			// 소수점 4번째까지 출력
			return format_to(ctx.out(), L"FLOAT3(x: {:.4f}, y: {:.4f}, z: {:.4f})", f.x, f.y, f.z);
		}
	};


	//-----------------------------------------------------------------------------
	// FLOAT4 Structure Formatter Specialization
	template<>
	struct formatter<mark::FLOAT4, char>
	{
		mutable std::formatter<float, char> float_fmt;

		constexpr auto parse(std::format_parse_context& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template<typename FormatContext>
		auto format(const mark::FLOAT4& f, FormatContext& ctx) const
		{
			// 소수점 4번째까지 출력
			return format_to(ctx.out(), "FLOAT4(x: {:.4f}, y: {:.4f}, z: {:.4f}, w: {:.4f})", f.x, f.y, f.z, f.w);
		}
	};

	template<>
	struct formatter<mark::FLOAT4, wchar_t>
	{
		mutable std::formatter<float, wchar_t> float_fmt;

		constexpr auto parse(std::basic_format_parse_context<wchar_t>& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template<typename FormatContext>
		auto format(const mark::FLOAT4& f, FormatContext& ctx) const
		{
			// 소수점 4번째까지 출력
			return format_to(ctx.out(), L"FLOAT4(x: {:.4f}, y: {:.4f}, z: {:.4f}, w: {:.4f})", f.x, f.y, f.z, f.w);
		}
	};

	//-----------------------------------------------------------------------------
	// XFLOAT2 Structure Formatter Specialization
	template<>
	struct formatter<mark::XFLOAT2, char>
	{
		mutable std::formatter<float, char> float_fmt;

		constexpr auto parse(std::format_parse_context& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template<typename FormatContext>
		auto format(const mark::XFLOAT2& v, FormatContext& ctx) const
		{
			return format_to(ctx.out(), "XFLOAT2(x: {:.4f}, y: {:.4f})", v.x, v.y);
		}
	};

	template<>
	struct formatter<mark::XFLOAT2, wchar_t>
	{
		mutable std::formatter<float, wchar_t> float_fmt;

		constexpr auto parse(std::basic_format_parse_context<wchar_t>& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template<typename FormatContext>
		auto format(const mark::XFLOAT2& v, FormatContext& ctx) const
		{
			return format_to(ctx.out(), L"XFLOAT2(x: {:.4f}, y: {:.4f})", v.x, v.y);
		}
	};

	//-----------------------------------------------------------------------------
	// XFLOAT3 Structure Formatter Specialization
	template<>
	struct formatter<mark::XFLOAT3, char>
	{
		mutable std::formatter<float, char> float_fmt;

		constexpr auto parse(std::format_parse_context& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template<typename FormatContext>
		auto format(const mark::XFLOAT3& v, FormatContext& ctx) const
		{
			return format_to(ctx.out(), "XFLOAT3(x: {:.4f}, y: {:.4f}, z: {:.4f}", v.x, v.y, v.z);
		}
	};

	template<>
	struct formatter<mark::XFLOAT3, wchar_t>
	{
		mutable std::formatter<float, wchar_t> float_fmt;

		constexpr auto parse(std::basic_format_parse_context<wchar_t>& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template<typename FormatContext>
		auto format(const mark::XFLOAT3& v, FormatContext& ctx) const
		{
			return format_to(ctx.out(), L"XFLOAT3(x: {:.4f}, y: {:.4f}, z: {:.4f})", v.x, v.y, v.z);
		}
	};

	//-----------------------------------------------------------------------------
	// XFLOAT4 Structure Formatter Specialization
	template<>
	struct formatter<mark::XFLOAT4, char>
	{
		mutable std::formatter<float, char> float_fmt;

		constexpr auto parse(std::format_parse_context& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template<typename FormatContext>
		auto format(const mark::XFLOAT4& v, FormatContext& ctx) const
		{
			return format_to(ctx.out(), "XFLOAT4(x: {:.4f}, y: {:.4f}, z: {:.4f}, w: {:.4f})", v.x, v.y, v.z, v.w);
		}
	};

	template<>
	struct formatter<mark::XFLOAT4, wchar_t>
	{
		mutable std::formatter<float, wchar_t> float_fmt;

		constexpr auto parse(std::basic_format_parse_context<wchar_t>& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template<typename FormatContext>
		auto format(const mark::XFLOAT4& v, FormatContext& ctx) const
		{
			return format_to(ctx.out(), L"XFLOAT4(x: {:.4f}, y: {:.4f}, z: {:.4f}, w: {:.4f})", v.x, v.y, v.z, v.w);
		}
	};

	//-----------------------------------------------------------------------------
	// MATRIX4 Structure Formatter Specialization
	template<>
	struct formatter<mark::MATRIX4, char>
	{
		mutable std::formatter<float, char> float_fmt;

		constexpr auto parse(std::format_parse_context& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template<typename FormatContext>
		auto format(const mark::MATRIX4& mat, FormatContext& ctx) const
		{
			return format_to(ctx.out(),
				"MATRIX4(\n"
				"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}]\n"
				"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}]\n"
				"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}]\n"
				"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}])",
				mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3],
				mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3],
				mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3],
				mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3]);
		}
	};

	template<>
	struct formatter<mark::MATRIX4, wchar_t>
	{
		mutable std::formatter<float, wchar_t> float_fmt;

		constexpr auto parse(std::basic_format_parse_context<wchar_t>& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template<typename FormatContext>
		auto format(const mark::MATRIX4& mat, FormatContext& ctx) const
		{
			return format_to(ctx.out(),
				L"MATRIX4(\n"
				L"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}]\n"
				L"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}]\n"
				L"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}]\n"
				L"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}])",
				mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3],
				mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3],
				mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3],
				mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3]);
		}
	};

	//-----------------------------------------------------------------------------
	// SIMD_MATRIX Structure Formatter Specialization
	template<>
	struct formatter<mark::XMATRIX4, char>
	{
		mutable std::formatter<float, char> float_fmt;

		constexpr auto parse(std::format_parse_context& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template<typename FormatContext>
		auto format(const mark::XMATRIX4& mat, FormatContext& ctx) const
		{
			return format_to(ctx.out(),
				"XMATRIX4(\n"
				"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}]\n"
				"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}]\n"
				"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}]\n"
				"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}])",
				mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3],
				mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3],
				mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3],
				mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3]);
		}
	};

	template<>
	struct formatter<mark::XMATRIX4, wchar_t>
	{
		mutable std::formatter<float, wchar_t> float_fmt;

		constexpr auto parse(std::basic_format_parse_context<wchar_t>& ctx)
		{
			return float_fmt.parse(ctx);
		}

		template<typename FormatContext>
		auto format(const mark::XMATRIX4& mat, FormatContext& ctx) const
		{
			return format_to(ctx.out(),
				L"XMATRIX4(\n"
				L"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}]\n"
				L"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}]\n"
				L"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}]\n"
				L"  [{:.4f}, {:.4f}, {:.4f}, {:.4f}])",
				mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3],
				mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3],
				mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3],
				mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3]);
		}
	};
}
