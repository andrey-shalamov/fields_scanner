#include "fields_scanner_v2.h"
#include <vector>
#include <map>
#include <iostream>

struct E0
{
	bool* _1;
	bool* _2;
};

struct E1
{
	std::vector<E0> _1;
	std::map<int, E0> _2;
	std::vector<bool> _3;
	std::map<int, bool*> _4;
};

struct E2
{
	E1 _0; E1 _1; E1 _2; E1 _3; E1 _4; E1 _5; E1 _6; E1 _7; E1 _8; E1 _9;
	E1 _10; E1 _11; E1 _12; E1 _13; E1 _14; E1 _15; E1 _16; E1 _17; E1 _18; E1 _19;
	E1 _20; E1 _21; E1 _22; E1 _23; E1 _24; E1 _25; E1 _26; E1 _27; E1 _28; E1 _29;
	E1 _30; E1 _31; E1 _32; E1 _33; E1 _34; E1 _35; E1 _36; E1 _37; E1 _38; E1 _39;
	E1 _40; E1 _41; E1 _42; E1 _43; E1 _44; E1 _45; E1 _46; E1 _47; E1 _48; E1 _49;
	E1 _50; E1 _51; E1 _52; E1 _53; E1 _54; E1 _55; E1 _56; E1 _57; E1 _58; E1 _59;
	E1 _60; E1 _61; E1 _62; E1 _63; E1 _64; E1 _65; E1 _66; E1 _67; E1 _68; E1 _69;
	E1 _70; E1 _71; E1 _72; E1 _73; E1 _74; E1 _75; E1 _76; E1 _77; E1 _78; E1 _79;
	E1 _80; E1 _81; E1 _82; E1 _83; E1 _84; E1 _85; E1 _86; E1 _87; E1 _88; E1 _89;
	E1 _90; E1 _91; E1 _92; E1 _93; E1 _94; E1 _95; E1 _96; E1 _97; E1 _98; E1 _99;
};

void compiler_stress_test_01()
{
	using namespace simple_reflection::v2;
	using user_type_list = type_list<E0, E1>;
	using user_template_type_list = template_type_list<template_type<std::vector, 1>, template_type<std::map, 2>>;
	using serializer_t = aggregate_serializer<E2, user_type_list, user_template_type_list>;
	using types = serializer_t::fields_type_list_t;
	static_assert(std::is_same<E1, type_list_element_t<2, types>>::value, "_!_");
	E2 e2{};
	serializer_t::serialize(e2, std::cout);
	std::cout << std::endl;
}