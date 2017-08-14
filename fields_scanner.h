#pragma once

#include <type_traits>
#include <utility>
#include <cstddef>
#include <cstdint>

#include <string>
#include <array>
#include <vector>
#include <map>

namespace simple_reflection
{
	// constexpr_array

	template<typename T, size_t N, typename ConstexprFiller>
	struct constexpr_array
	{
		template<typename... Args>
		constexpr constexpr_array(Args&&... args) noexcept : values()
		{
			ConstexprFiller::template fill<T>(values, N, std::forward<Args>(args)...);
		}
		constexpr T operator[](size_t i) const noexcept { return values[i]; }
		constexpr size_t size() const noexcept { return N; }
		T values[N];
	};

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

	// as_any_type

	template<size_t>
	struct as_any_type
	{
		template<typename T>
		constexpr operator T() const noexcept;
	};

	// any

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

	struct any_std_string
	{
		template<typename T, typename Traits, typename Alloc>
		constexpr operator std::basic_string<T, Traits, Alloc>() const noexcept;
	};

	struct any_std_array
	{
		template<typename T, size_t N>
		constexpr operator std::array<T, N>() const noexcept;
	};

	struct any_std_vector
	{
		template<typename T, typename Alloc>
		constexpr operator std::vector<T, Alloc>() const noexcept;
	};

	struct any_std_map
	{
		template<typename K, typename V, typename Pred, typename Alloc>
		constexpr operator std::map<K, V, Pred, Alloc>() const noexcept;
	};

	// type_list

	template<typename... Ts>
	struct type_list
	{
		static constexpr size_t size = sizeof...(Ts);
	};

	template<typename... Ts1, typename... Ts2>
	constexpr auto join_type_lists(type_list<Ts1...>, type_list<Ts2...>) noexcept
	{
		return type_list<Ts1..., Ts2...>{};
	}

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
		static constexpr std::enable_if_t < std::is_same < T, decltype(T{ as_any_type<I0>{}, as_any_type<Is>{}... }) > ::value, size_t >
			detect_impl(std::index_sequence<I0, Is...>) noexcept
		{
			return 1 + sizeof...(Is);
		}
	};

	// types_indexes_filler

	template<typename Type, size_t FieldsCount, typename TypeList>
	struct types_indexes_filler
	{
		static constexpr size_t not_found_index = static_cast<size_t>(-1);

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
			static_assert(result != not_found_index, "no one match");
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

	// types_of_fields_rough_estimate

	template<typename Type, typename UserTypeList>
	struct types_of_fields_rough_estimate
	{
		static constexpr size_t fields_count = fields_count_detector<Type>::detect();

		static constexpr auto create_rough_type_list() noexcept
		{
			using rough_type_list_t = type_list<any_pointer, any_arithmetic, any_class, any_std_string, any_std_array, any_std_vector, any_std_map>;
			return join_type_lists(rough_type_list_t{}, UserTypeList{});
		}
		using type_list_t = decltype(create_rough_type_list());

		static constexpr auto detect() noexcept
		{
			return constexpr_array<size_t, fields_count, types_indexes_filler<Type, fields_count, type_list_t>>();
		}
	};

	// types_of_fields_detector

	template<typename Type, typename UserTypeList>
	struct types_of_fields_detector
	{
		static constexpr size_t fields_count = fields_count_detector<Type>::detect();

		using pre_defined_type_list_t = type_list
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

		template<typename TypeList>
		static constexpr auto create_pointers_type_list() noexcept
		{
			return create_pointers_type_list<TypeList>(std::make_index_sequence<TypeList::size>{});
		}

		template<typename TypeList, size_t... Is>
		static constexpr auto create_pointers_type_list(std::index_sequence<Is...>) noexcept
		{
			return type_list
				<
				std::add_pointer_t<std::remove_cv_t<type_list_element_t<Is, TypeList>>>...,
				std::add_pointer_t<std::add_const_t<std::remove_cv_t<type_list_element_t<Is, TypeList>>>>...,
				std::add_pointer_t<std::add_cv_t<std::remove_cv_t<type_list_element_t<Is, TypeList>>>>
				...>{};
		}

		static constexpr auto create_full_type_list() noexcept
		{
			constexpr type_list<char*, const char*, volatile const char*, void*, const void*, volatile const void*> additional_pointers{};
			return join_type_lists(create_pointers_type_list<pre_defined_type_list_t>(),
				join_type_lists(additional_pointers,
					join_type_lists(pre_defined_type_list_t{}, UserTypeList{})));
		}

		using type_list_t = decltype(create_full_type_list());

		static constexpr auto detect() noexcept
		{
			return constexpr_array<size_t, fields_count, types_indexes_filler<Type, fields_count, type_list_t>>();
		}
	};

	// fields_scanner

	template<template<typename,typename> class TypesDetector, typename Type, typename UserTypeList>
	struct fields_scanner
	{
		static constexpr auto fields_count = fields_count_detector<Type>::detect();
		static constexpr auto types_of_fields_indexes = TypesDetector<Type, UserTypeList>::detect();
		using type_list_t = typename TypesDetector<Type, UserTypeList>::type_list_t;

#ifdef _MSC_VER // workaround for MS VC++
		static constexpr auto detect_types_of_fields() noexcept
		{
			return detect_types_of_fields<fields_count>(std::integral_constant<bool, fields_count == 0>{});
		}

		template<size_t N, size_t... Is>
		static constexpr auto detect_types_of_fields(std::false_type) noexcept
		{
			return detect_types_of_fields<N - 1, types_of_fields_indexes[N - 1], Is...>(std::integral_constant<bool, N == 1>{});
		}

		template<size_t N, size_t... Is>
		static constexpr auto detect_types_of_fields(std::true_type) noexcept
		{
			return type_list<type_list_element_t<Is, type_list_t>...>{};
		}
#else
		static constexpr auto detect_types_of_fields() noexcept
		{
			return detect_types_of_fields(std::make_index_sequence<fields_count>{});
		}

		template<size_t... Is>
		static constexpr auto detect_types_of_fields(std::index_sequence<Is...>) noexcept
		{
			return type_list<type_list_element_t<types_of_fields_indexes[Is], type_list_t>...>{};
		}
#endif
	};
}