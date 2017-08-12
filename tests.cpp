#include "fields_scanner.h"

#include <iostream>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <set>
#include <map>

struct A
{
	int i;
};

struct B
{
	char c;
};

struct S
{
	long long _1;
	unsigned int _2;
	short _3;
	unsigned char _4;

	double _5;
	float _6;

	A _7;
	B _8;

	char* _9;
	const int* _10;
	volatile const short* _11;

	std::array<char, 3> _12;

	std::string _13;
	std::vector<short> _14;
	std::set<int> _15;
	std::map<std::string, unsigned int> _16;

	void* _17;
	long double _18;
	bool _19;

	float _20[2]; // this field will be detected as 2 float. =(
};

template<size_t I, typename T>
using get_t = simple_reflection::type_list_element_t<I, T>;

int main()
{
	using type_list_t = simple_reflection::type_list
		<
		A, B,
		std::array<int16_t, 10>, std::array<int8_t, 3>, std::array<char, 3>,
		std::string, std::wstring,
		std::vector<int8_t>, std::vector<int16_t>,
		std::set<int8_t>, std::set<int32_t>,
		std::map<uint32_t, std::string>, std::map<std::string, uint32_t>
		//float[2]
		>;

	using scanner_t = simple_reflection::fields_scanner<S, type_list_t>;
	static_assert(scanner_t::fields_count == 20 + 1, "!");
	constexpr auto types_of_fields = scanner_t::detect_types_of_fields();
	using fields_t = std::decay_t<decltype(types_of_fields)>;
	static_assert(std::is_same<int64_t, get_t<0, fields_t>>::value, "0");
	static_assert(std::is_same<uint32_t, get_t<1, fields_t>>::value, "1");
	static_assert(std::is_same<int16_t, get_t<2, fields_t>>::value, "2");
	static_assert(std::is_same<uint8_t, get_t<3, fields_t>>::value, "3");
	static_assert(std::is_same<double, get_t<4, fields_t>>::value, "4");
	static_assert(std::is_same<float, get_t<5, fields_t>>::value, "5");
	static_assert(std::is_same<A, get_t<6, fields_t>>::value, "6");
	static_assert(std::is_same<B, get_t<7, fields_t>>::value, "7");
	static_assert(std::is_same<char*, get_t<8, fields_t>>::value, "8");
	static_assert(std::is_same<const int32_t*, get_t<9, fields_t>>::value, "9");
	static_assert(std::is_same<volatile const int16_t*, get_t<10, fields_t>>::value, "10");
	static_assert(std::is_same<std::array<char, 3>, get_t<11, fields_t>>::value, "11");
	static_assert(std::is_same<std::string, get_t<12, fields_t>>::value, "12");
	static_assert(std::is_same<std::vector<int16_t>, get_t<13, fields_t>>::value, "13");
	static_assert(std::is_same<std::set<int32_t>, get_t<14, fields_t>>::value, "14");
	static_assert(std::is_same<std::map<std::string, uint32_t>, get_t<15, fields_t>>::value, "15");
	static_assert(std::is_same<void*, get_t<16, fields_t>>::value, "16");
	static_assert(std::is_same<long double, get_t<17, fields_t>>::value, "17");
#if (defined(__GNUC__) || defined(__GNUG__)) && !defined(__clang__)
#pragma message "on gcc bool fields can NOT be detected until next bug is not fixed https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65043"
#else
	static_assert(std::is_same<bool, get_t<18, fields_t>>::value, "18");
#endif
	static_assert(std::is_same<float, get_t<19, fields_t>>::value, "19"); // float[2] detects as 2 float
	static_assert(std::is_same<float, get_t<20, fields_t>>::value, "20"); // float[2] detects as 2 float

	return 0;
}