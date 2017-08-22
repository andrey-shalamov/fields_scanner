#include "fields_scanner_v2.h"

#include <string>
#include <array>
#include <vector>
#include <map>

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
	static_assert(std::is_class<decltype(A1{ gettt<class_depth_of_aggregate_initialization_helper::depth<0>>() }) > ::value, "!");
	static_assert(std::is_class<decltype(A2{ class_depth_of_aggregate_initialization_helper::depth<1>{} }) > ::value, "!");
	
	static_assert(std::is_class<decltype(B0{ class_depth_of_aggregate_initialization_helper::depth<1>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B0{ template_class_depth_of_aggregate_initialization_helper<int>::depth<1>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B0{ specific_template_class_depth_of_aggregate_initialization_helper<std::vector, int>::depth<1>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B0{ specific_class_depth_of_aggregate_initialization_helper<std::vector<int>>::depth<1>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B1{ class_depth_of_aggregate_initialization_helper::depth<2>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B2{ class_depth_of_aggregate_initialization_helper::depth<3>{} }) > ::value, "!");

	static_assert(std::is_class<decltype(B2{ specific_class_depth_of_aggregate_initialization_helper<B0>::depth<2>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B2{ specific_class_depth_of_aggregate_initialization_helper<B1>::depth<3>{} }) > ::value, "!");
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

}

int main_()
{
	return 0;
}