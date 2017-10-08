#include "fields_scanner_v2.h"

#include <string>
#include <array>
#include <vector>
#include <map>

#include <cassert>
#include <iostream>
#include <iterator>

namespace
{

struct A0 { int _1; };
struct A1 { A0 _1; };
struct A2 { A1 _1; };

struct B0 { std::vector<int> _1; std::map<int, float> _2; A2 _3; std::map<A0, A2*> _4; };
struct B1 { B0 _1; };
struct B2 { B1 _1; };


template<typename T>
constexpr T gettt() noexcept;

using namespace simple_reflection::v2;

void test01()
{
	static_assert(std::is_class<decltype(A0{ any_arithmetic{} }) > ::value, "!");
	static_assert(std::is_class<decltype(A1{ gettt<class_depth_of_aggregate_initialization_helper<A1, 0>>() }) > ::value, "!");
	static_assert(std::is_class<decltype(A2{ class_depth_of_aggregate_initialization_helper<A2, 1>{} }) > ::value, "!");
	
	static_assert(std::is_class<decltype(B0{ class_depth_of_aggregate_initialization_helper<B0, 1>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B0{ template_class_depth_of_aggregate_initialization_helper<B0, 1, int>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B0{ specific_template_class_depth_of_aggregate_initialization_helper<B0, 1, std::vector, int>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B0{ specific_class_depth_of_aggregate_initialization_helper<B0, 1, std::vector<int>>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B1{ class_depth_of_aggregate_initialization_helper<B1, 2>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B2{ class_depth_of_aggregate_initialization_helper<B2, 3>{} }) > ::value, "!");

	static_assert(std::is_class<decltype(B2{ specific_class_depth_of_aggregate_initialization_helper<B2, 2, B0>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B2{ specific_class_depth_of_aggregate_initialization_helper<B2, 3, B1>{} }) > ::value, "!");
}

void test02()
{
	constexpr size_t b0_fields_count = fields_count_detector<B0>::detect();

	static_assert(clarify_type<A1, 1, 0, type_list<A0, int>, template_type_list<>, 0, any_class>::index() == 0, "!");
	using clarify_vector_t = clarify_type<B0, b0_fields_count, 0, type_list<int>, template_type_list<template_type<std::vector, 1>, template_type<std::map, 2>>, 0, any_class>;
	static_assert(clarify_vector_t::index() == 0, "!");
	static_assert(std::is_same<template_type<std::vector, 1, int>, type_list_element_t<clarify_vector_t::index(), typename clarify_vector_t::type_list_t>>::value, "!");

	using clarify_map_t = clarify_type<B0, b0_fields_count, 1, type_list<int>, template_type_list<template_type<std::vector, 1>, template_type<std::map, 2>>, 0, any_class>;
	static_assert(clarify_map_t::index() == 0, "!");
	static_assert(std::is_same<template_type<std::map, 2, int, float>, type_list_element_t<clarify_map_t::index(), typename clarify_map_t::type_list_t>>::value, "!");

	using clarify_A2_t = clarify_type<B0, b0_fields_count, 2, type_list<A2, A1, A0, B2, B1, B0>, template_type_list<template_type<std::vector, 1>, template_type<std::map, 2>>, 0, any_class>;
	static_assert(clarify_A2_t::index() == 0, "!");
	static_assert(std::is_same<A2, type_list_element_t<clarify_A2_t::index(), typename clarify_A2_t::type_list_t>>::value, "!");

	using clarify_map_of_class_t = clarify_type<B0, b0_fields_count, 3, type_list<A2, A1, A0, B2, B1, B0>, template_type_list<template_type<std::vector, 1>, template_type<std::map, 2>>, 0, any_type>;
	static_assert(clarify_map_of_class_t::index() == 0, "!");
	static_assert(std::is_same<template_type<std::map, 2, A0, A2*>, type_list_element_t<clarify_map_of_class_t::index(), typename clarify_map_of_class_t::type_list_t>>::value, "!");
}

void test03()
{
	using tl = template_type_list<template_type<std::vector, 1>, template_type<std::map, 2>>;
	static_assert(std::is_same<std::vector<int>, template_type_list_element_t<0, tl>::template type<int>>::value, "!");
}

template<typename, typename>
struct X {};

void test04()
{
	static_assert(std::is_same<int, type_list_element_t<0, fields_scanner<A0>::type_list_t>>::value, "A0");

	using B0_type_list_t = fields_scanner<
		B0, 
		type_list<A2, A1, A0>,
		template_type_list<template_type<std::map, 2>, template_type<std::vector, 1>, template_type<std::vector, 1>, template_type<X, 2>>
	>::type_list_t;
	static_assert(std::is_same<template_type<std::vector, 1, int>, type_list_element_t<0, B0_type_list_t>>::value, "B0 - 1");
	static_assert(std::is_same<template_type<std::map, 2, int, float>, type_list_element_t<1, B0_type_list_t>>::value, "B0 - 2");
	static_assert(std::is_same<A2, type_list_element_t<2, B0_type_list_t>>::value, "B0 - 3");
	static_assert(std::is_same<template_type<std::map, 2, A0, A2*>, type_list_element_t<3, B0_type_list_t>>::value, "B0 - 4");
}

struct C0
{
	char* _1;
	const int* _2;
	volatile short* _3;
	const volatile void* _4;
	void* _5;
};

struct C1
{
	int _2;
};

void test05()
{
	static_assert(std::is_same<char*, type_list_element_t<0, fields_scanner<C0>::type_list_t>>::value, "C0 - 1");
	static_assert(std::is_same<const int*, type_list_element_t<1, fields_scanner<C0>::type_list_t>>::value, "C0 - 2");
	static_assert(std::is_same<volatile short*, type_list_element_t<2, fields_scanner<C0>::type_list_t>>::value, "C0 - 3");
	static_assert(std::is_same<const volatile void*, type_list_element_t<3, fields_scanner<C0>::type_list_t>>::value, "C0 - 4");
	static_assert(std::is_same<void*, type_list_element_t<4, fields_scanner<C0>::type_list_t>>::value, "C0 - 5");
}

struct D0
{
	int _1;
	std::string _2;
};

struct D1
{
	double _1;
	D0 _2;
	short _3;
};

struct D2
{
	int _1;
	std::vector<int> _2;
	double _3;
};

void test06()
{
	D0 d0{ 77, "string d0" };
	using d0_scanner = fields_scanner<D0, type_list<std::string>>;
	assert(d0_scanner::get<0>(d0) == 77);
	assert(d0_scanner::get<1>(d0) == "string d0");

	D1 d1{ 0.99, { 33, "d1" }, 55 };
	using d1_scanner = fields_scanner<D1, type_list<D0>>;
	assert(d1_scanner::get<0>(d1) == 0.99);
	assert(d1_scanner::get<1>(d1)._1 == 33);
	assert(d1_scanner::get<1>(d1)._2 == "d1");
	assert(d1_scanner::get<2>(d1) == 55);

	d1_scanner::get<1>(d1) = { 777, "new string" };
	assert(d1_scanner::get<1>(d1)._1 == 777);
	assert(d1_scanner::get<1>(d1)._2 == "new string");
}

template<typename T>
inline std::ostream& operator << (std::ostream& out, const std::vector<T>& vec)
{
	out << "{";
	if (!vec.empty())
	{
		out << vec.front();
		for (auto i = std::next(vec.begin()); i != vec.end(); ++i)
			out << "," << *i;
	}
	out << "}";
	return out;
}

void test07()
{
	static_assert(has_ostream_operator<std::string>::value, "!");
	static_assert(!has_ostream_operator<D0>::value, "!");

	D0 d0{ 77, "string d0" };
	using d0_serializer = aggregate_serializer<D0, type_list<std::string>>;
	d0_serializer::serialize(d0, std::cout);
	std::cout << std::endl;

	D2 d2{ 11, {55,77,88}, 22.22 };

	using d2_serializer = aggregate_serializer<D2, type_list<>, template_type_list<template_type<std::vector, 1>>>;

	/*std::cout << offsetof(D2, _1) << "=" << field_offset<D2, d2_serializer::fields_type_list_t>::of<0>() << std::endl;
	std::cout << offsetof(D2, _2) << "=" << field_offset<D2, d2_serializer::fields_type_list_t>::of<1>() << std::endl;
	std::cout << offsetof(D2, _3) << "=" << field_offset<D2, d2_serializer::fields_type_list_t>::of<2>() << std::endl;
	*/
	assert(d2_serializer::get<0>(d2) == 11);
	const auto v = d2_serializer::get<1>(d2);
	assert(d2_serializer::get<1>(d2).size() == 3);
	assert(d2_serializer::get<1>(d2)[0] == 55);
	assert(d2_serializer::get<1>(d2)[1] == 77);
	assert(d2_serializer::get<1>(d2)[2] == 88);
	assert(d2_serializer::get<2>(d2) == 22.22);
	d2_serializer::serialize(d2, std::cout);
	std::cout << std::endl;

	D1 d1{ 1, { 123, "hello!" }, 44 };
	using d1_serializer = aggregate_serializer<D1, type_list<D0, std::string>>;
	d1_serializer::serialize(d1, std::cout);
	std::cout << std::endl;
}

struct D3
{
	int _1;
	char _2;
	float _3;
};

void test08()
{
	D3 d3{1010,'a',3.4f};
	using d3_serializer = aggregate_serializer<D3>;
	d3_serializer::serialize(d3, std::cout);
	std::cout << std::endl;
}

struct D4
{
	char _1;
	D3 _2;
	short _3;
	std::vector<float> _4;
	D2 _5;
};

void test09()
{
	D4 d4{ 'A', { 23, 'd', 5.5f }, 100, { 1.f, 2.f, 100500.f }, { 2, { 4, 3, 0 }, 99.7 } };
	using d4_serializer = aggregate_serializer<D4, type_list<D3, D2>, template_type_list<template_type<std::vector, 1>>>;
	/*std::cout << offsetof(D4, _2) << " = " << field_offset<D4, d4_serializer::fields_type_list_t>::of<1>() << std::endl;
	std::cout << offsetof(D4, _3) << " = " << field_offset<D4, d4_serializer::fields_type_list_t>::of<2>() << std::endl;
	std::cout << offsetof(D4, _4) << " = " << field_offset<D4, d4_serializer::fields_type_list_t>::of<3>() << std::endl;
	std::cout << offsetof(D4, _5) << " = " << field_offset<D4, d4_serializer::fields_type_list_t>::of<4>() << std::endl;*/
	d4_serializer::serialize(d4, std::cout);
	std::cout << std::endl;
}

struct D5
{
	char _1;
	short _2;
	int _3;
	D1 _4;
	double _5;
	short _6;
	char _7;
};

void test10()
{
	D5 d5;
	using d5_serializer = aggregate_serializer<D5, type_list<D1, D0, std::string>>;
	using offset_t = field_offset<D5, d5_serializer::fields_type_list_t>;
	assert(offsetof(D5, _1) == offset_t::of<0>());
	assert(offsetof(D5, _2) == offset_t::of<1>());
	assert(offsetof(D5, _3) == offset_t::of<2>());
	assert(offsetof(D5, _4) == offset_t::of<3>());
	assert(offsetof(D5, _5) == offset_t::of<4>());
	assert(offsetof(D5, _6) == offset_t::of<5>());
	assert(offsetof(D5, _7) == offset_t::of<6>());

	d5_serializer::set<0>(d5) = 'W';
	d5_serializer::set<1>(d5) = 150;
	d5_serializer::set<2>(d5) = 7070;
	d5_serializer::set<3>(d5) = { 6.7, { 3300, "std::string" }, 69 };
	d5_serializer::set<4>(d5) = 55.9999;
	d5_serializer::set<5>(d5) = 88;
	d5_serializer::set<6>(d5) = 'E';

	assert(d5._1 == 'W');
	assert(d5._2 == 150);
	assert(d5._3 == 7070);
	assert(d5._4._1 == 6.7);
	assert(d5._4._2._1 == 3300);
	assert(d5._4._2._2 == "std::string");
	assert(d5._4._3 == 69);
	assert(d5._5 == 55.9999);
	assert(d5._6 == 88);
	assert(d5._7 == 'E');

	d5_serializer::serialize(d5, std::cout);
	std::cout << std::endl;
}

}

int main()
{
	test06();
	test07();
	test08();
	test09();
	test10();

	std::vector<int> v{3,5,7,9,193};
	std::cout << v << std::endl;
	return 0;
}