#include "fields_scanner_v2.h"

struct S
{
	int _1[128]; // bad =(
};

void compiler_stress_test_02()
{
	using namespace simple_reflection::v2;
	using user_type_list = type_list<>;
	using user_template_type_list = template_type_list<>;
	using serializer_t = aggregate_serializer<S, user_type_list, user_template_type_list>;
	using types = serializer_t::fields_type_list_t;
	static_assert(std::is_same<int, type_list_element_t<2, types>>::value, "_!_");
}