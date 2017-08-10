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

	float _19[2]; // this field will be detected as 2 float. =(
};

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
	static_assert(scanner_t::fields_count == 19 + 1, "!");
	constexpr auto types_of_fields = scanner_t::detect_types_of_fields();
	static_assert(std::is_same<decltype(types_of_fields),
		const simple_reflection::type_list<
			int64_t,
			uint32_t,
			int16_t,
			uint8_t,
			double,
			float,
			A,
			B, 
			char*,
			const int32_t*,
			volatile const int16_t*,
			std::array<char, 3>,
			std::string,
			std::vector<int16_t>,
			std::set<int32_t>,
			std::map<std::string, uint32_t>,
			void*,
			long double,
			float, float // float[2] detects as 2 float
		>
		>::value, "!");

	return 0;
}