#include "fields_scanner_v2.h"

namespace
{

struct A0 { int _1; };
struct A1 { A0 _1; };
struct A2 { A1 _1; };

struct B0 { std::vector<int> _1; };
struct B1 { B0 _1; };
struct B2 { B1 _1; };


template<typename T>
constexpr T gettt() noexcept;

using namespace simple_reflection::v2;

void test01()
{
	static_assert(std::is_class<decltype(A0{ simple_reflection::any_arithmetic{} }) > ::value, "!");
	static_assert(std::is_class<decltype(A1{ gettt<class_nested_aggregate_constructible::count<0>>() }) > ::value, "!");
	static_assert(std::is_class<decltype(A2{ class_nested_aggregate_constructible::count<1>{} }) > ::value, "!");
	
	static_assert(std::is_class<decltype(B0{ class_nested_aggregate_constructible::count<1>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B0{ template_class_nested_aggregate_constructible<int>::count<1>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B0{ specific_template_class_nested_aggregate_constructible<std::vector, int>::count<1>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B0{ specific_class_nested_aggregate_constructible<std::vector<int>>::count<1>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B1{ class_nested_aggregate_constructible::count<2>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B2{ class_nested_aggregate_constructible::count<3>{} }) > ::value, "!");

	static_assert(std::is_class<decltype(B2{ specific_class_nested_aggregate_constructible<B0>::count<2>{} }) > ::value, "!");
	static_assert(std::is_class<decltype(B2{ specific_class_nested_aggregate_constructible<B1>::count<3>{} }) > ::value, "!");
}

void test02()
{
	static_assert(clarify_type<A1, 1, 0, simple_reflection::type_list<A0, int>, template_type_list<>, simple_reflection::any_class>::index() == 0, "!");
	static_assert(clarify_type<B0, 1, 0, simple_reflection::type_list<int>, template_type_list<template_type<std::vector, 1>, template_type<std::map, 2>>, simple_reflection::any_class>::index() == 0, "!");
}

void test03()
{
	using tl = template_type_list<template_type<std::vector, 1>, template_type<std::map, 2>>;
	static_assert(std::is_same<std::vector<int>, template_type_list_element_t<0, tl>::template type<int>>::value, "!");
}

}