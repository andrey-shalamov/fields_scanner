#pragma once

#include "fields_scanner.h"

#define PP_EMPTY
#define PP_COMMA ,

#define DEFINE_NESTED_AGGREGATE_CONSTRUCTIBLE_CLASS_COUNT(N, template_args, type, braces)\
template<> struct count<N> : count_impl<N> { template<template_args typename = decltype(type braces)> constexpr operator type() const noexcept; };

#define DECLARE_NESTED_AGGREGATE_CONSTRUCTIBLE_CLASS(name, template_args, type)\
struct name##_nested_aggregate_constructible {\
	template<size_t N> struct count_impl { static constexpr size_t value = N; };\
	template<size_t> struct count;\
	DEFINE_NESTED_AGGREGATE_CONSTRUCTIBLE_CLASS_COUNT(0, template_args, type, {{}})\
	DEFINE_NESTED_AGGREGATE_CONSTRUCTIBLE_CLASS_COUNT(1, template_args, type, {{{}}})\
	DEFINE_NESTED_AGGREGATE_CONSTRUCTIBLE_CLASS_COUNT(2, template_args, type, {{{{}}}})\
	DEFINE_NESTED_AGGREGATE_CONSTRUCTIBLE_CLASS_COUNT(3, template_args, type, {{{{{}}}}})\
	DEFINE_NESTED_AGGREGATE_CONSTRUCTIBLE_CLASS_COUNT(4, template_args, type, {{{{{{}}}}}})\
	DEFINE_NESTED_AGGREGATE_CONSTRUCTIBLE_CLASS_COUNT(5, template_args, type, {{{{{{{}}}}}}})\
	DEFINE_NESTED_AGGREGATE_CONSTRUCTIBLE_CLASS_COUNT(6, template_args, type, {{{{{{{{}}}}}}}})\
};
namespace simple_reflection
{
namespace v2
{
DECLARE_NESTED_AGGREGATE_CONSTRUCTIBLE_CLASS(class, typename T PP_COMMA, T)

template<typename... Ts>
DECLARE_NESTED_AGGREGATE_CONSTRUCTIBLE_CLASS(template_class, template<typename...> class T PP_COMMA typename... Args PP_COMMA, T<Ts... PP_COMMA Args...>)

template<typename T>
DECLARE_NESTED_AGGREGATE_CONSTRUCTIBLE_CLASS(specific_class, PP_EMPTY, T)

template<template<typename...> class T, typename... Ts>
DECLARE_NESTED_AGGREGATE_CONSTRUCTIBLE_CLASS(specific_template_class, typename... Args PP_COMMA, T<Ts... PP_COMMA Args...>)

static constexpr size_t not_found_index = static_cast<size_t>(-1);

template<template<typename...> class T, size_t ArgsToDetect>
struct template_type
{
	template<typename... Args>
	using type = T<Args...>;

	template<typename... Args>
	static constexpr T<Args...> get() noexcept; // Undefined

	static constexpr size_t args_to_detect = ArgsToDetect;
};

template<typename...>
struct template_type_list;

template<>
struct template_type_list<>
{
	static constexpr size_t size = 0;
};

template<template<typename...> class... Ts, size_t... Is>
struct template_type_list<template_type<Ts, Is>...>
{
	static constexpr size_t size = sizeof...(Ts);
};

template<size_t I, typename T>
struct template_type_list_element;

template<typename T, typename... Ts>
struct template_type_list_element<0, template_type_list<T, Ts...>>
{
	using type = T;
};

template<size_t I, typename T, typename... Ts>
struct template_type_list_element<I, template_type_list<T, Ts...>>
{
	using type = typename template_type_list_element<I - 1, template_type_list<Ts...>>::type;
};

template<size_t I, typename TemplateTypeList>
using template_type_list_element_t = typename template_type_list_element<I, TemplateTypeList>::type;

//

template<typename Type, size_t FieldsCount, typename TypeList>
struct types_indexes_filler
{
	template<typename T>
	static constexpr void fill(T* values, size_t size) noexcept
	{
		static_cast<void>(size);
		fill_impl(values, std::make_index_sequence<FieldsCount>{});
	}

	template<typename T, size_t... Is>
	static constexpr void fill_impl(T* values, std::index_sequence<Is...>) noexcept
	{
		static_cast<void>(std::initializer_list<int>{((values[Is] = detect_index_of_type<Is>()), 0)...});
	}

	template<size_t FieldIndex>
	static constexpr size_t detect_index_of_type() noexcept
	{
		constexpr size_t result = detect_index_of_type_impl(
			std::make_index_sequence<TypeList::size>{},
			std::make_index_sequence<FieldIndex>{},
			make_index_sequence_range<FieldIndex + 1, FieldsCount>()
			);
		return result;
	}

	template<size_t... Is0, size_t... Is1, size_t... Is2>
	static constexpr size_t detect_index_of_type_impl(std::index_sequence<Is0...>, std::index_sequence<Is1...> is1, std::index_sequence<Is2...> is2) noexcept
	{
		return detect_index_of_type_impl(std::make_index_sequence<sizeof...(Is0)-1>{}, is1, is2);
	}

	template<size_t I0, size_t... Is0, size_t... Is1, size_t... Is2>
	static constexpr std::enable_if_t<
		std::is_same<
			Type,
			decltype(Type{
			as_any_type<Is1>{}...,
			type_list_element<sizeof...(Is0), TypeList>::get(),
			as_any_type<Is2>{}...
		}) > ::value,
		size_t>
		detect_index_of_type_impl(std::index_sequence<I0, Is0...>, std::index_sequence<Is1...>, std::index_sequence<Is2...>) noexcept
	{
		return sizeof...(Is0);
	}

	template<size_t... Is1, size_t... Is2>
	static constexpr size_t detect_index_of_type_impl(std::index_sequence<>, std::index_sequence<Is1...>, std::index_sequence<Is2...>) noexcept
	{
		return not_found_index;
	}
};

//

constexpr size_t max_nested_aggregate_constructible = 7;

template<typename NestedAggregateConstructible, size_t N>
constexpr auto create_nested_aggregate_constructible_type_list() noexcept
{
	return create_nested_aggregate_constructible_type_list<NestedAggregateConstructible>(std::make_index_sequence<N>{});
}

template<typename NestedAggregateConstructible, size_t... Is>
constexpr auto create_nested_aggregate_constructible_type_list(std::index_sequence<Is...>) noexcept
{
	return type_list<NestedAggregateConstructible::template count<Is>...>{};
}

//

template<typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, typename UserTemplateTypeList, typename TypeToClarify>
struct clarify_type;

template<typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, typename UserTemplateTypeList>
struct clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, any_class>
{
	static constexpr auto tmp_tl = create_nested_aggregate_constructible_type_list<class_nested_aggregate_constructible, max_nested_aggregate_constructible>();
	using tmp_tl_t = std::decay_t<decltype(tmp_tl)>;
	static constexpr size_t tmp_index = types_indexes_filler<Type, FieldsCount, tmp_tl_t>::detect_index_of_type<FieldIndex>();
	using next_stage_t = clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, type_list_element_t<tmp_index, tmp_tl_t>>;
	using type_list_t = typename next_stage_t::type_list_t;
	static constexpr size_t index() noexcept { return next_stage_t::index(); }
};

template<typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, typename UserTemplateTypeList, size_t N>
struct clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, class_nested_aggregate_constructible::count<N>>
{
	static constexpr auto create_type_list() noexcept
	{
		return create_type_list(std::make_index_sequence<UserTypeList::size>{});
	}

	template<size_t... Is>
	static constexpr auto create_type_list(std::index_sequence<Is...>) noexcept
	{
		return type_list<typename specific_class_nested_aggregate_constructible<type_list_element_t<Is, UserTypeList>>::template count<N>...>{};
	}

	static constexpr bool is_template = types_indexes_filler<Type, FieldsCount, type_list<template_class_nested_aggregate_constructible<>::count<N>>>::detect_index_of_type<FieldIndex>() == 0;
	using template_type_clarify_t = clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, template_class_nested_aggregate_constructible<>::count<N>>;

	static constexpr size_t index_user_type = types_indexes_filler<Type, FieldsCount, std::decay_t<decltype(create_type_list())>>::detect_index_of_type<FieldIndex>();
	using type_list_t = std::conditional_t<is_template, typename template_type_clarify_t::type_list_t, UserTypeList>;
	static constexpr size_t index() noexcept
	{
		if (index_user_type != not_found_index)
			return index_user_type;
		else if (is_template)
			return template_type_clarify_t::index();
		else
			static_assert(index_user_type != not_found_index || is_template, "type cannot be clarifyed");
	}
};

template<typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, typename UserTemplateTypeList, size_t N>
struct clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, template_class_nested_aggregate_constructible<>::count<N>>
{
	static constexpr auto create_type_list() noexcept
	{
		return create_type_list(std::make_index_sequence<UserTemplateTypeList::size>{});
	}

	template<size_t... Is>
	static constexpr auto create_type_list(std::index_sequence<Is...>) noexcept
	{
		return type_list<typename specific_template_class_nested_aggregate_constructible<template_type_list_element_t<Is, UserTemplateTypeList>::template type >::template count<N>...>{};
	}

	using type_list_t = UserTemplateTypeList;
	static constexpr size_t index() noexcept
	{
		return types_indexes_filler<Type, FieldsCount, std::decay_t<decltype(create_type_list())>>::detect_index_of_type<FieldIndex>();
	}
};

}
}