#pragma once

#include <type_traits>
#include <utility>
#include <cstddef>
#include <cstdint>
#include <cassert>

#include <vector>
#include <map>
#include <string>

#define PP_EMPTY
#define PP_ARG(...) __VA_ARGS__

#define PP_DECLARE_DEPTH_OF_AGGREGATE_INITIALIZATION_HELPER(name, N, helper_template_params, helper_template_args, template_args, type, braces)\
template<typename main_struct helper_template_params>\
struct name##_depth_of_aggregate_initialization_helper<main_struct, N helper_template_args> {\
	template<template_args\
	  typename = std::enable_if_t<std::is_class<type>::value>\
	, typename = std::enable_if_t<!std::is_same<type, main_struct>::value>\
	, typename = decltype(type braces)\
	>\
	constexpr operator type() const noexcept;\
};

#define PP_DECLARE_DEPTH_OF_AGGREGATE_INITIALIZATION_HELPERS(name, helper_template_params, helper_template_args, template_args, type)\
struct name##_depth_of_aggregate_initialization_tag{};\
template<typename,size_t helper_template_params> struct name##_depth_of_aggregate_initialization_helper;\
PP_DECLARE_DEPTH_OF_AGGREGATE_INITIALIZATION_HELPER(name, 0, PP_ARG(helper_template_params), PP_ARG(helper_template_args), PP_ARG(template_args), PP_ARG(type), {{}})\
PP_DECLARE_DEPTH_OF_AGGREGATE_INITIALIZATION_HELPER(name, 1, PP_ARG(helper_template_params), PP_ARG(helper_template_args), PP_ARG(template_args), PP_ARG(type), {{{}}})\
PP_DECLARE_DEPTH_OF_AGGREGATE_INITIALIZATION_HELPER(name, 2, PP_ARG(helper_template_params), PP_ARG(helper_template_args), PP_ARG(template_args), PP_ARG(type), {{{{}}}})\
PP_DECLARE_DEPTH_OF_AGGREGATE_INITIALIZATION_HELPER(name, 3, PP_ARG(helper_template_params), PP_ARG(helper_template_args), PP_ARG(template_args), PP_ARG(type), {{{{{}}}}})\
PP_DECLARE_DEPTH_OF_AGGREGATE_INITIALIZATION_HELPER(name, 4, PP_ARG(helper_template_params), PP_ARG(helper_template_args), PP_ARG(template_args), PP_ARG(type), {{{{{{}}}}}})\
PP_DECLARE_DEPTH_OF_AGGREGATE_INITIALIZATION_HELPER(name, 5, PP_ARG(helper_template_params), PP_ARG(helper_template_args), PP_ARG(template_args), PP_ARG(type), {{{{{{{}}}}}}})\
PP_DECLARE_DEPTH_OF_AGGREGATE_INITIALIZATION_HELPER(name, 6, PP_ARG(helper_template_params), PP_ARG(helper_template_args), PP_ARG(template_args), PP_ARG(type), {{{{{{{{}}}}}}}})

namespace simple_reflection
{
namespace v2
{

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic error "-Wmany-braces-around-scalar-init"
#pragma clang diagnostic ignored "-Wundefined-inline"
#endif

PP_DECLARE_DEPTH_OF_AGGREGATE_INITIALIZATION_HELPERS(class, PP_EMPTY, PP_EMPTY, PP_ARG(typename T, ), T)

PP_DECLARE_DEPTH_OF_AGGREGATE_INITIALIZATION_HELPERS(template_class, PP_ARG(, typename... Ts), PP_ARG(, Ts...), PP_ARG(template<typename...> class T, typename... Args, ), PP_ARG(T<Ts..., Args...>))

PP_DECLARE_DEPTH_OF_AGGREGATE_INITIALIZATION_HELPERS(specific_class, PP_ARG(, typename T), PP_ARG(, T), PP_EMPTY, T)

PP_DECLARE_DEPTH_OF_AGGREGATE_INITIALIZATION_HELPERS(specific_template_class, PP_ARG(, template<typename...> class T, typename... Ts), PP_ARG(, T, Ts...), PP_ARG(typename... Args, ), PP_ARG(T<Ts..., Args...>))

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

static constexpr size_t not_found_index = static_cast<size_t>(-1);
struct not_found_tag {};

// index_sequence_range

template<size_t MinIndex, size_t... Is>
constexpr auto make_index_sequence_range(std::index_sequence<Is...>) noexcept
{
	return std::index_sequence<(MinIndex + Is)...>{};
}

template<size_t MinIndex, size_t MaxIndex>
constexpr auto make_index_sequence_range() noexcept
{
	return make_index_sequence_range<MinIndex>(std::make_index_sequence<MaxIndex - MinIndex>{});
}

// any

struct any_type
{
	template<typename T>
	constexpr operator T() const noexcept;
};

template<size_t>
struct indexed_any_type
{
	template<typename T>
	constexpr operator T() const noexcept;
};

struct any_arithmetic
{
	template<typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
	constexpr operator T() const noexcept;
};

struct any_class
{
	template<typename T, typename = std::enable_if_t<std::is_class<T>::value>>
	constexpr operator T() const noexcept;
};

struct any_pointer
{
	template<typename T, typename = std::enable_if_t<std::is_pointer<T>::value>>
	constexpr operator T() const noexcept;
};

// type_list

template<typename... Ts>
struct type_list
{
	static constexpr size_t size = sizeof...(Ts);
};

template<typename TL1, typename TL2>
struct type_lists_joiner;

template<typename... Ts1, typename... Ts2>
struct type_lists_joiner<type_list<Ts1...>, type_list<Ts2...>>
{
	using type = type_list<Ts1..., Ts2...>;
};

template<typename TL1, typename TL2>
using type_lists_joiner_t = typename type_lists_joiner<TL1, TL2>::type;

template<size_t I, typename T>
struct type_list_element;

template<typename T, typename... Ts>
struct type_list_element<0, type_list<T, Ts...>>
{
	using type = T;
	static constexpr T get() noexcept; // Undefined
};

template<size_t I, typename T, typename... Ts>
struct type_list_element<I, type_list<T, Ts...>>
{
	using type = typename type_list_element<I - 1, type_list<Ts...>>::type;
	static constexpr type get() noexcept; // Undefined
};

template<size_t I, typename TypeList>
using type_list_element_t = typename type_list_element<I, TypeList>::type;

// template_type

template<template<typename...> class T, size_t ArgsToDetect, typename... Ts>
struct template_type
{
	using template_type_tag = void;

	template<typename... Args>
	using type = T<Ts..., Args...>;

	template<typename... Args>
	static constexpr T<Ts..., Args...> get() noexcept; // Undefined

	static constexpr size_t args_to_detect = ArgsToDetect;
};

template<typename T, typename = void>
struct has_template_type_tag : std::false_type {};

template<typename T>
struct has_template_type_tag<T, std::void_t<typename T::template_type_tag>> : std::true_type {};

// template_type_list

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

using arithmetic_type_list_t = type_list
<
	bool,
	int8_t, uint8_t,
	int16_t,
	uint16_t, // char16_t and wchar_t will be detected as uint16_t
	int32_t,
	uint32_t, // char32_t will be detected as uint32_t
	int64_t, uint64_t,
	float, double, long double
>;

template<typename TypeList, size_t... Is>
constexpr auto create_pointers_type_list(std::index_sequence<Is...>) noexcept
{
	return type_list
		< std::add_pointer_t<std::remove_cv_t<type_list_element_t<Is, TypeList>>>...
		, std::add_pointer_t<std::add_const_t<std::remove_cv_t<type_list_element_t<Is, TypeList>>>>...
		, std::add_pointer_t<std::add_volatile_t<std::remove_cv_t<type_list_element_t<Is, TypeList>>>>...
		, std::add_pointer_t<std::add_cv_t<std::remove_cv_t<type_list_element_t<Is, TypeList>>>>...
		>{};
}

template<typename TypeList>
constexpr auto create_pointers_type_list() noexcept
{
	return create_pointers_type_list<TypeList>(std::make_index_sequence<TypeList::size>{});
}

using void_pointers_type_list_t = type_list<void*, const void*, volatile void*, volatile const void*>;

using arithmetic_pointers_type_list_t = type_lists_joiner_t
<
	std::decay_t<decltype(create_pointers_type_list<arithmetic_type_list_t>())>,
	type_list<char*, const char*, volatile char*, volatile const char*>
>;

//

template<typename Type, size_t FieldsCount, typename TypeList>
struct field_type_detector
{
	template<size_t FieldIndex>
	static constexpr size_t index() noexcept
	{
		return index_impl(
			std::make_index_sequence<TypeList::size>{},
			std::make_index_sequence<FieldIndex>{},
			make_index_sequence_range<FieldIndex + 1, FieldsCount>()
		);;
	}

	template<size_t... Is0, size_t... Is1, size_t... Is2>
	static constexpr size_t index_impl(std::index_sequence<Is0...>, std::index_sequence<Is1...> is1, std::index_sequence<Is2...> is2) noexcept
	{
		return index_impl(std::make_index_sequence<sizeof...(Is0)-1>{}, is1, is2);
	}

	template<size_t I0, size_t... Is0, size_t... Is1, size_t... Is2>
	static constexpr std::enable_if_t<
		std::is_same<
		Type,
		decltype(Type{
		indexed_any_type<Is1>{}...,
		type_list_element<sizeof...(Is0), TypeList>::get(),
		indexed_any_type<Is2>{}...
	}) > ::value,
		size_t>
		index_impl(std::index_sequence<I0, Is0...>, std::index_sequence<Is1...>, std::index_sequence<Is2...>) noexcept
	{
		return sizeof...(Is0);
	}

	template<size_t... Is1, size_t... Is2>
	static constexpr size_t index_impl(std::index_sequence<>, std::index_sequence<Is1...>, std::index_sequence<Is2...>) noexcept
	{
		return not_found_index;
	}
};

//

template<typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, typename UserTemplateTypeList, size_t DepthOfAggInit, typename TypeToClarify>
struct clarify_type;

template<typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, typename UserTemplateTypeList, size_t DepthOfAggInit>
struct clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit, not_found_tag>
{
	using type_list_t = type_list<>;
	static constexpr size_t index() noexcept { return not_found_index; }
};

template<typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, typename UserTemplateTypeList, size_t DepthOfAggInit>
struct clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit, any_type>
{
	using rough_estimate_type_list_t = type_list<any_arithmetic, any_pointer, any_class>;
	static constexpr size_t rough_estimate_type_index = field_type_detector<Type, FieldsCount, rough_estimate_type_list_t>::template index<FieldIndex>();
	using next_stage_t = clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit, type_list_element_t<rough_estimate_type_index, rough_estimate_type_list_t>>;
	using type_list_t = typename next_stage_t::type_list_t;
	static constexpr size_t index() noexcept { return next_stage_t::index(); }
};

template<typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, size_t DepthOfAggInit, typename UserTemplateTypeList>
struct clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit, any_arithmetic>
{
	using type_list_t = arithmetic_type_list_t;
	static constexpr size_t index() noexcept { return field_type_detector<Type, FieldsCount, arithmetic_type_list_t>::template index<FieldIndex>(); }
};

template<typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, size_t DepthOfAggInit, typename UserTemplateTypeList>
struct clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit, any_pointer>
{
	using type_list_t = type_lists_joiner_t<arithmetic_pointers_type_list_t,
		type_lists_joiner_t
		<
		std::decay_t<decltype(create_pointers_type_list<UserTypeList>())>,
		void_pointers_type_list_t
		>
	>;
	static constexpr size_t index() noexcept { return field_type_detector<Type, FieldsCount, type_list_t>::template index<FieldIndex>(); }
};

template<typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, size_t DepthOfAggInit, typename UserTemplateTypeList>
struct clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit, any_class>
{
	static constexpr size_t max_depth_of_aggregate_initialization = 7;

	static constexpr auto create_type_list() noexcept
	{
		return create_type_list(std::make_index_sequence<max_depth_of_aggregate_initialization>{});
	}

	template<size_t... Is>
	static constexpr auto create_type_list(std::index_sequence<Is...>) noexcept
	{
		return type_list<class_depth_of_aggregate_initialization_helper<Type, Is>...>{};
	}

	using depth_of_aggregate_initialization_helper_type_list_t = std::decay_t<decltype(create_type_list())>;
	static constexpr size_t aggregate_constructible_depth = field_type_detector<Type, FieldsCount, depth_of_aggregate_initialization_helper_type_list_t>::template index<FieldIndex>();
	using next_stage_t = clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, aggregate_constructible_depth, class_depth_of_aggregate_initialization_tag>;
	using type_list_t = typename next_stage_t::type_list_t;
	static constexpr size_t index() noexcept { return next_stage_t::index(); }
};

template<typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, typename UserTemplateTypeList, size_t DepthOfAggInit>
struct clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit, class_depth_of_aggregate_initialization_tag>
{
	static constexpr auto create_type_list() noexcept
	{
		return create_type_list(std::make_index_sequence<UserTypeList::size>{});
	}

	template<size_t... Is>
	static constexpr auto create_type_list(std::index_sequence<Is...>) noexcept
	{
		return type_list<specific_class_depth_of_aggregate_initialization_helper<Type, DepthOfAggInit, type_list_element_t<Is, UserTypeList>>...>{};
	}

	static constexpr bool is_template = field_type_detector<Type, FieldsCount, type_list<template_class_depth_of_aggregate_initialization_helper<Type, DepthOfAggInit>>>::template index<FieldIndex>() == 0;
	using template_type_clarify_t = clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit, template_class_depth_of_aggregate_initialization_tag>;

	static constexpr size_t index_user_type = field_type_detector<Type, FieldsCount, std::decay_t<decltype(create_type_list())>>::template index<FieldIndex>();
	using type_list_t = std::conditional_t<index_user_type != not_found_index, UserTypeList, typename template_type_clarify_t::type_list_t>;
	static constexpr size_t index() noexcept
	{
		if (index_user_type != not_found_index)
			return index_user_type;
		else if (is_template)
			return template_type_clarify_t::index();
		else
			static_assert(index_user_type != not_found_index || is_template, "type cannot be clarified");
	}
};

template<size_t I, typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, typename UserTemplateTypeList, size_t DepthOfAggInit>
struct next_clarify_type_by_index
{
	using type = clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit, template_type_list_element_t<I, UserTemplateTypeList>>;
};

template<typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, typename UserTemplateTypeList, size_t DepthOfAggInit>
struct next_clarify_type_by_index<not_found_index, Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit>
{
	using type = clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit, not_found_tag>;
};

template<typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, typename UserTemplateTypeList, size_t DepthOfAggInit>
struct clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit, template_class_depth_of_aggregate_initialization_tag>
{
	static constexpr auto create_type_list() noexcept
	{
		return create_type_list(std::make_index_sequence<UserTemplateTypeList::size>{});
	}

	template<size_t... Is>
	static constexpr auto create_type_list(std::index_sequence<Is...>) noexcept
	{
		return type_list<specific_template_class_depth_of_aggregate_initialization_helper<Type, DepthOfAggInit, template_type_list_element_t<Is, UserTemplateTypeList>::template type >...>{};
	}

	using user_template_type_list_t = std::decay_t<decltype(create_type_list())>;
	static constexpr size_t index_user_template_type = field_type_detector<Type, FieldsCount, user_template_type_list_t>::template index<FieldIndex>();
	using next_clarify_t = typename next_clarify_type_by_index<index_user_template_type, Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit>::type;
	using type_list_t = typename next_clarify_t::type_list_t;
	static constexpr size_t index() noexcept { return next_clarify_t::index(); }
};

template<typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, typename UserTemplateTypeList, size_t DepthOfAggInit, template<typename...> class TemplateType, size_t ArgsToDetect, typename... Args>
struct clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit, template_type<TemplateType, ArgsToDetect, Args...>>
{
	using type_list_of_template_parameters = type_lists_joiner_t<
		arithmetic_pointers_type_list_t,
		type_lists_joiner_t
		<
		std::decay_t<decltype(create_pointers_type_list<UserTypeList>())>,
		type_lists_joiner_t<
		void_pointers_type_list_t,
		type_lists_joiner_t<arithmetic_type_list_t, UserTypeList>
		>>>;

	static constexpr auto create_type_list() noexcept
	{
		return create_type_list(std::make_index_sequence<type_list_of_template_parameters::size>{});
	}

	template<size_t... Is>
	static constexpr auto create_type_list(std::index_sequence<Is...>) noexcept
	{
		return type_list<specific_template_class_depth_of_aggregate_initialization_helper<Type, DepthOfAggInit, TemplateType, Args..., type_list_element_t<Is, type_list_of_template_parameters>>...>{};
	}

	using specific_template_with_args_type_list_t = std::decay_t<decltype(create_type_list())>;
	static constexpr size_t index_specific_template_with_args = field_type_detector<Type, FieldsCount, specific_template_with_args_type_list_t>::template index<FieldIndex>();

	using next_clarify_t = clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit,
		std::conditional_t<index_specific_template_with_args == not_found_index, not_found_tag,
		template_type<TemplateType, ArgsToDetect - 1, Args..., type_list_element_t<index_specific_template_with_args, type_list_of_template_parameters>>
		>>;

	using type_list_t = typename next_clarify_t::type_list_t;
	static constexpr size_t index() noexcept { return next_clarify_t::index(); }
};

template<typename Type, size_t FieldsCount, size_t FieldIndex, typename UserTypeList, typename UserTemplateTypeList, size_t DepthOfAggInit, template<typename...> class TemplateType, typename... Args>
struct clarify_type<Type, FieldsCount, FieldIndex, UserTypeList, UserTemplateTypeList, DepthOfAggInit, template_type<TemplateType, 0, Args...>>
{
	using type_list_t = type_list<template_type<TemplateType, sizeof...(Args), Args...>>;
	static constexpr size_t index() noexcept { return 0; }
};

// fields_count_detector

template<typename T>
struct fields_count_detector
{
	static constexpr size_t detect() noexcept
	{
		// TODO: bit fields
		return detect_impl(std::make_index_sequence<sizeof(T)>{});
	}
private:
	template<size_t... Is>
	static constexpr size_t detect_impl(std::index_sequence<Is...>) noexcept
	{
		return detect_impl(std::make_index_sequence<sizeof...(Is)-1>{});
	}

	static constexpr size_t detect_impl(std::index_sequence<>) noexcept
	{
		return 0;
	}

	template<size_t I0, size_t... Is>
	static constexpr std::enable_if_t < std::is_same < T, decltype(T{ indexed_any_type<I0>{}, indexed_any_type<Is>{}... }) > ::value, size_t >
		detect_impl(std::index_sequence<I0, Is...>) noexcept
	{
		return 1 + sizeof...(Is);
	}
};

// template_type_unwrapper

template<size_t I, typename FieldsTypes, bool>
struct type_unwrapper;

template<size_t I, typename FieldsTypes>
struct type_unwrapper<I, FieldsTypes,false>
{
	using type = type_list_element_t<I, FieldsTypes>;
};

template<size_t I, typename FieldsTypes>
struct type_unwrapper<I, FieldsTypes, true>
{
	using type = typename type_list_element_t<I, FieldsTypes>::template type<>;
};

template<size_t I, typename FieldsTypes>
using type_unwrapper_t = typename type_unwrapper<I, FieldsTypes, has_template_type_tag<type_list_element_t<I, FieldsTypes>>::value>::type;

// field_offset

template<typename Type, typename FieldsTypes>
struct field_offset
{
	template<size_t I>
	static constexpr size_t of() noexcept
	{
		static_assert(I < FieldsTypes::size, "index out of range");
		return of_impl(std::integral_constant<size_t, I>{});
	}

	static constexpr size_t of_impl(std::integral_constant<size_t, 0>) noexcept
	{
		return 0;
	}

	template<size_t I>
	static constexpr size_t of_impl(std::integral_constant<size_t, I>) noexcept
	{
		using previous_type = type_unwrapper_t<I-1, FieldsTypes>;
		using current_type = type_unwrapper_t<I, FieldsTypes>;
		constexpr size_t previous_offset = of_impl(std::integral_constant<size_t, I - 1>{});
		constexpr size_t previous_align = alignof(previous_type);
		constexpr size_t current_align = alignof(current_type);
		if (previous_align >= current_align || ((previous_offset + sizeof(previous_type)) % current_align == 0))
			return previous_offset + sizeof(previous_type);
		else
			return previous_offset + sizeof(previous_type) + current_align - previous_align;
	}
};

// fields_scanner

template<typename Type, typename UserTypeList = type_list<>, typename UserTemplateTypeList = template_type_list<>>
struct fields_scanner
{
	static constexpr auto fields_count = fields_count_detector<Type>::detect();

	template<size_t N, typename... Ts>
	struct scanner_impl
	{
		using clarify_type_t = clarify_type<Type, fields_count, N - 1, UserTypeList, UserTemplateTypeList, 0, any_type>;
		using type_list_t = typename scanner_impl<N - 1, type_list_element_t<clarify_type_t::index(), typename clarify_type_t::type_list_t>, Ts...>::type_list_t;
	};

	template<typename... Ts>
	struct scanner_impl<0, Ts...>
	{
		using type_list_t = type_list<Ts...>;
	};

	using type_list_t = typename scanner_impl<fields_count>::type_list_t;

	template<size_t I>
	static std::add_lvalue_reference_t<type_unwrapper_t<I, type_list_t>>
		get(Type& obj) noexcept
	{
		return *reinterpret_cast<std::add_pointer_t<type_unwrapper_t<I, type_list_t>>>(reinterpret_cast<uint8_t*>(&obj) + field_offset<Type, type_list_t>::of<I>());
	}
};

// has_ostream_operator

template<typename, typename, typename = void>
struct has_ostream_operator : std::false_type {};

template<typename T, typename OStream>
struct has_ostream_operator<T, OStream, std::void_t<decltype(std::declval<OStream&>() << std::declval<const T&>())>> : std::true_type {};

// has_istream_operator

template<typename, typename, typename = void>
struct has_istream_operator : std::false_type {};

template<typename T, typename IStream>
struct has_istream_operator<T, IStream, std::void_t<decltype(std::declval<IStream&>() >> std::declval<T&>())>> : std::true_type {};

// special_serializer

template<typename ParentSerializer, typename T>
struct special_serializer
{
	using is_special = std::integral_constant<bool, false>;
};

template<typename ParentSerializer, typename T, typename Alloc>
struct special_serializer<ParentSerializer, std::vector<T, Alloc>>
{
	using is_special = std::integral_constant<bool, true>;

	template<typename OStream>
	static void serialize(const std::vector<T, Alloc>& obj, OStream& out)
	{
		const auto size = obj.size();
		out << '{' << size << '|';
		for (size_t i = 0; i < size; ++i)
		{
			using serializer_t = typename ParentSerializer::template rebind<T>;
			serializer_t::serialize(obj[i], out);
			if (i + 1 < size)
				out << ',';
		}
		out << '}';
	}

	template<typename IStream>
	static void deserialize(std::vector<T, Alloc>& obj, IStream& in)
	{
		typename IStream::char_type ignore = 0;
		in >> ignore;
		assert(ignore == '{');
		size_t size = 0;
		in >> size >> ignore;
		assert(ignore == '|');
		obj.resize(size);
		for (size_t i = 0; i < size; ++i)
		{
			using serializer_t = typename ParentSerializer::template rebind<T>;
			serializer_t::deserialize(obj[i], in);
			if (i + 1 < size)
			{
				in >> ignore;
				assert(ignore == ',');
			}
		}
		in >> ignore;
		assert(ignore == '}');
	}
};

template<typename ParentSerializer, typename K, typename V, typename Pred, typename Alloc>
struct special_serializer<ParentSerializer, std::map<K, V, Pred, Alloc>>
{
	using is_special = std::integral_constant<bool, true>;

	template<typename OStream>
	static void serialize(const std::map<K, V, Pred, Alloc>& obj, OStream& out)
	{
		out << '{' << obj.size() << '|';
		for (auto i = obj.begin(); i != obj.end(); ++i)
		{
			using key_serializer_t = typename ParentSerializer::template rebind<K>;
			using value_serializer_t = typename ParentSerializer::template rebind<V>;
			out << '{';
			key_serializer_t::serialize(i->first, out);
			value_serializer_t::serialize(i->second, out);
			out << '}';
			if (std::next(i) != obj.end())
				out << ',';
		}
		out << '}';	}

	template<typename IStream>
	static void deserialize(std::map<K, V, Pred, Alloc>& obj, IStream& in)
	{
		typename IStream::char_type ignore = 0;
		in >> ignore;
		assert(ignore == '{');
		size_t size = 0;
		in >> size >> ignore;
		assert(ignore == '|');
		for (size_t i = 0; i < size; ++i)
		{
			using key_serializer_t = typename ParentSerializer::template rebind<K>;
			using value_serializer_t = typename ParentSerializer::template rebind<V>;
			in >> ignore;
			assert(ignore == '{');
			K key{};
			V value{};
			key_serializer_t::deserialize(key, in);
			value_serializer_t::deserialize(value, in);
			obj[key] = value;
			in >> ignore;
			assert(ignore == '}');
			if (i + 1 < size)
			{
				in >> ignore;
				assert(ignore == ',');
			}
		}
		in >> ignore;
		assert(ignore == '}');
	}
};

template<typename ParentSerializer, typename T, typename Traits, typename Alloc>
struct special_serializer<ParentSerializer, std::basic_string<T, Traits, Alloc>>
{
	using is_special = std::integral_constant<bool, true>;

	template<typename OStream>
	static void serialize(const std::basic_string<T, Traits, Alloc>& obj, OStream& out)
	{
		static_assert(std::is_same<T, typename OStream::char_type>::value, "char types must be the same");
		out << '{' << obj.size() << '|' << obj << '}';
	}

	template<typename IStream>
	static void deserialize(std::basic_string<T, Traits, Alloc>& obj, IStream& in)
	{
		static_assert(std::is_same<T, typename IStream::char_type>::value, "char types must be the same");
		typename IStream::char_type ignore = 0;
		in >> ignore;
		assert(ignore == '{');
		size_t size = 0;
		in >> size >> ignore;
		assert(ignore == '|');
		obj.resize(size);
		in.read(&obj[0], size);
		in >> ignore;
		assert(ignore == '}');
	}
};

// aggregate_serializer

template<typename Type, typename UserTypeList = type_list<>, typename UserTemplateTypeList = template_type_list<>>
struct aggregate_serializer
{
	using fields_scanner_t = fields_scanner<Type, UserTypeList, UserTemplateTypeList>;
	using fields_type_list_t = typename fields_scanner_t::type_list_t;

	template<typename U>
	using rebind = aggregate_serializer<U, UserTypeList, UserTemplateTypeList>;

	using this_type = aggregate_serializer<Type, UserTypeList, UserTemplateTypeList>;

	template<size_t I>
	static std::add_lvalue_reference_t<type_unwrapper_t<I, fields_type_list_t>>
		set(Type& obj) noexcept
	{
		return *reinterpret_cast<std::add_pointer_t<type_unwrapper_t<I, fields_type_list_t>>>(reinterpret_cast<uint8_t*>(&obj) + field_offset<Type, fields_type_list_t>::of<I>());
	}

	template<size_t I>
	static std::add_lvalue_reference_t<std::add_const_t<type_unwrapper_t<I, fields_type_list_t>>>
		get(const Type& obj) noexcept
	{
		return *reinterpret_cast<std::add_pointer_t<std::add_const_t<type_unwrapper_t<I, fields_type_list_t>>>>(reinterpret_cast<const uint8_t*>(&obj) + field_offset<Type, fields_type_list_t>::of<I>());
	}

	// serialize
	template<typename OStream>
	static void serialize(const Type& obj, OStream& out)
	{
		serialize(obj, out, has_ostream_operator<Type, OStream>{}, std::make_index_sequence<fields_scanner_t::fields_count>{}, typename special_serializer<this_type, Type>::is_special{});
	}

private:
	template<typename AggregateType, typename OStream, size_t... Is>
	static void serialize(const AggregateType& obj, OStream& out, std::true_type, std::index_sequence<Is...>, std::false_type)
	{
		out << obj;
	}

	template<typename AggregateType, typename OStream, typename HasOStreamOperator, size_t... Is>
	static void serialize(const AggregateType& obj, OStream& out, HasOStreamOperator, std::index_sequence<Is...>, std::true_type)
	{
		special_serializer<this_type, AggregateType>::serialize(obj, out);
	}

	template<typename AggregateType, typename OStream, size_t... Is>
	static void serialize(const AggregateType& obj, OStream& out, std::false_type, std::index_sequence<Is...>, std::false_type)
	{
		out << '{';
		std::initializer_list<int>{
			(serialize_impl(
				get<Is>(obj),
				out,
				std::integral_constant<bool, has_ostream_operator<std::decay_t<decltype(get<Is>(obj))>, OStream>::value >{},
				std::is_class<std::decay_t<decltype(get<Is>(obj))>>{},
				std::integral_constant<bool, Is + 1 == fields_scanner_t::fields_count>{},
				typename special_serializer<this_type, std::decay_t<decltype(get<Is>(obj))>>::is_special{}
			), 0)...};
		out << '}';
	}

	template<typename T, typename OStream>
	static void serialize_impl(const T& obj, OStream& out, std::false_type, std::true_type, std::false_type, std::false_type)
	{
		aggregate_serializer<T, UserTypeList, UserTemplateTypeList>::serialize(obj, out);
		out << ',';
	}

	template<typename T, typename OStream>
	static void serialize_impl(const T& obj, OStream& out, std::false_type, std::true_type, std::true_type, std::false_type)
	{
		aggregate_serializer<T, UserTypeList, UserTemplateTypeList>::serialize(obj, out);
	}

	template<typename T, typename OStream, typename HasOStreamOperator, typename IsClass>
	static void serialize_impl(const T& obj, OStream& out, HasOStreamOperator, IsClass, std::false_type, std::true_type)
	{
		special_serializer<this_type, T>::serialize(obj, out);
		out << ',';
	}

	template<typename T, typename OStream, typename HasOStreamOperator, typename IsClass>
	static void serialize_impl(const T& obj, OStream& out, HasOStreamOperator, IsClass, std::true_type, std::true_type)
	{
		special_serializer<this_type, T>::serialize(obj, out);
	}

	template<typename T, typename OStream, typename IsClass>
	static void serialize_impl(const T& value, OStream& out, std::true_type, IsClass, std::false_type, std::false_type)
	{
		out << value << ',';
	}

	template<typename T, typename OStream, typename IsClass>
	static void serialize_impl(const T& value, OStream& out, std::true_type, IsClass, std::true_type, std::false_type)
	{
		out << value;
	}

public:
	// deserialize
	template<typename IStream>
	static void deserialize(Type& obj, IStream& in)
	{
		deserialize(obj, in, has_istream_operator<Type, IStream>{}, std::make_index_sequence<fields_scanner_t::fields_count>{}, typename special_serializer<this_type, Type>::is_special{});
	}

private:
	template<typename AggregateType, typename IStream, size_t... Is>
	static void deserialize(AggregateType& obj, IStream& in, std::true_type, std::index_sequence<Is...>, std::false_type)
	{
		in >> obj;
	}

	template<typename AggregateType, typename IStream, typename HasIStreamOperator, size_t... Is>
	static void deserialize(AggregateType& obj, IStream& in, HasIStreamOperator, std::index_sequence<Is...>, std::true_type)
	{
		special_serializer<this_type, AggregateType>::deserialize(obj, in);
	}

	template<typename AggregateType, typename IStream, size_t... Is>
	static void deserialize(AggregateType& obj, IStream& in, std::false_type, std::index_sequence<Is...>, std::false_type)
	{
		typename IStream::char_type ignore = 0;
		in >> ignore;
		assert(ignore == '{');
		std::initializer_list<int>{
			(deserialize_impl(
				set<Is>(obj),
				in,
				std::integral_constant<bool, has_istream_operator<std::decay_t<decltype(set<Is>(obj))>, IStream>::value >{},
				std::is_class<std::decay_t<decltype(set<Is>(obj))>>{},
				std::integral_constant<bool, Is + 1 == fields_scanner_t::fields_count>{},
				typename special_serializer<this_type, std::decay_t<decltype(set<Is>(obj))>>::is_special{}
			), 0)...};
		in >> ignore;
		assert(ignore == '}');
	}

	template<typename T, typename IStream>
	static void deserialize_impl(T& obj, IStream& in, std::false_type, std::true_type, std::false_type, std::false_type)
	{
		aggregate_serializer<T, UserTypeList, UserTemplateTypeList>::deserialize(obj, in);
		typename IStream::char_type ignore = 0;
		in >> ignore;
		assert(ignore == ',');
	}

	template<typename T, typename IStream>
	static void deserialize_impl(T& obj, IStream& in, std::false_type, std::true_type, std::true_type, std::false_type)
	{
		aggregate_serializer<T, UserTypeList, UserTemplateTypeList>::deserialize(obj, in);
	}

	template<typename T, typename IStream, typename IsClass>
	static void deserialize_impl(T& value, IStream& in, std::true_type, IsClass, std::false_type, std::false_type)
	{
		in >> value;
		typename IStream::char_type ignore = 0;
		in >> ignore;
		assert(ignore == ',');
	}

	template<typename T, typename IStream, typename IsClass>
	static void deserialize_impl(T& value, IStream& in, std::true_type, IsClass, std::true_type, std::false_type)
	{
		in >> value;
	}

	template<typename T, typename IStream, typename HasIStreamOperator, typename IsClass>
	static void deserialize_impl(T& value, IStream& in, HasIStreamOperator, IsClass, std::false_type, std::true_type)
	{
		special_serializer<this_type, T>::deserialize(value, in);
		typename IStream::char_type ignore = 0;
		in >> ignore;
		assert(ignore == ',');
	}

	template<typename T, typename IStream, typename HasIStreamOperator, typename IsClass>
	static void deserialize_impl(T& value, IStream& in, HasIStreamOperator, IsClass, std::true_type, std::true_type)
	{
		special_serializer<this_type, T>::deserialize(value, in);
	}
};

}
}