#pragma once

#include <type_traits>
#include <utility>
#include <cstddef>

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

	// type_list

	template<typename... Ts>
	struct type_list
	{
		static constexpr size_t size = sizeof...(Ts);
	};

	template<size_t I, typename T>
	struct type_list_element;

	template<typename T, typename... Ts>
	struct type_list_element<0, type_list<T, Ts...>>
	{
		using type = T;
		static constexpr T get(); // Undefined
	};

	template<size_t I, typename T, typename... Ts>
	struct type_list_element<I, type_list<T, Ts...>>
	{
		using type = typename type_list_element<I - 1, type_list<Ts...>>::type;
		static constexpr type get(); // Undefined
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

	// types_of_fields_detector

	template<typename Type, typename TypeList>
	struct types_of_fields_detector
	{
		static constexpr size_t fields_count = fields_count_detector<Type>::detect();

		static constexpr auto detect() noexcept
		{
			return constexpr_array<size_t, fields_count, types_indexes_filler>();
		}
	private:
		struct types_indexes_sorter
		{
			template<typename T>
			static constexpr void fill(T* values, size_t size) noexcept
			{
				fill_impl(values, size, std::make_index_sequence<TypeList::size>{});
			}

			template<typename T, size_t... Is>
			static constexpr void fill_impl(T* values, size_t size, std::index_sequence<Is...>) noexcept
			{
				std::pair<size_t, size_t> type_sizes[]{ { Is, sizeof(type_list_element_t<Is, TypeList>) }... };
				for (int i = 1; i < static_cast<int>(size); ++i)
				{
					const auto x = type_sizes[i];
					int j = i - 1;
					while (j >= 0 && x.second < type_sizes[j].second)
					{
						type_sizes[j + 1].first = type_sizes[j].first;
						type_sizes[j + 1].second = type_sizes[j].second;
						--j;
					}
					type_sizes[j + 1].first = x.first;
					type_sizes[j + 1].second = x.second;
				}
				static_cast<void>(std::initializer_list<int>{((values[Is] = type_sizes[Is].first), 0)...});
			}
		};

		static constexpr constexpr_array<size_t, TypeList::size, types_indexes_sorter> indexes_of_types_sorted_by_size{};

		struct types_indexes_filler
		{
			template<typename T>
			static constexpr void fill(T* values, size_t size) noexcept
			{
				static_cast<void>(size);
				fill_impl(values, std::make_index_sequence<fields_count>{});
			}

			template<typename T, size_t... Is>
			static constexpr void fill_impl(T* values, std::index_sequence<Is...>) noexcept
			{
				static_cast<void>(std::initializer_list<int>{((values[Is] = detect_index_of_type<Is>()), 0)...});
			}

			template<size_t FieldIndex>
			static constexpr size_t detect_index_of_type()
			{
				return detect_index_of_type_impl<FieldIndex>(
					std::make_index_sequence<indexes_of_types_sorted_by_size.size()>{},
					std::make_index_sequence<FieldIndex>{},
					make_index_sequence_range<FieldIndex + 1, fields_count>()
					);
			}

			template<size_t FieldIndex, size_t... Is0, size_t... Is1, size_t... Is2>
			static constexpr size_t detect_index_of_type_impl(std::index_sequence<Is0...>, std::index_sequence<Is1...>, std::index_sequence<Is2...>)
			{
				return detect_index_of_type_impl<FieldIndex>(
					std::make_index_sequence<sizeof...(Is0)-1>{},
					std::make_index_sequence<FieldIndex>{},
					make_index_sequence_range<FieldIndex + 1, fields_count>()
					);
			}

			template<size_t FieldIndex, size_t I0, size_t... Is0, size_t... Is1, size_t... Is2>
			static constexpr std::enable_if_t<
				std::is_same<
					Type,
					decltype(Type{
						as_any_type<Is1>{}...,
						type_list_element<indexes_of_types_sorted_by_size[sizeof...(Is0)], TypeList>::get(),
						as_any_type<Is2>{}...
					}) > ::value,
				size_t>
				detect_index_of_type_impl(std::index_sequence<I0, Is0...>, std::index_sequence<Is1...>, std::index_sequence<Is2...>)
			{
				return indexes_of_types_sorted_by_size[sizeof...(Is0)];
			}
		};
	};

	// fields_scanner

	template<typename Type, typename TypeList>
	struct fields_scanner
	{
		static constexpr auto fields_count = fields_count_detector<Type>::detect();
		static constexpr auto types_of_fields_indexes = types_of_fields_detector<Type, TypeList>::detect();

#ifdef _MSC_VER // workaround for MS VC++
		static constexpr auto detect_types_of_fields()
		{
			return detect_types_of_fields<fields_count>(std::integral_constant<bool, fields_count == 0>{});
		}

		template<size_t N, size_t... Is>
		static constexpr auto detect_types_of_fields(std::false_type)
		{
			return detect_types_of_fields<N - 1, types_of_fields_indexes[N - 1], Is...>(std::integral_constant<bool, N == 1>{});
		}

		template<size_t N, size_t... Is>
		static constexpr auto detect_types_of_fields(std::true_type)
		{
			return type_list<type_list_element_t<Is, TypeList>...>{};
		}
#else
		static constexpr auto detect_types_of_fields()
		{
			return detect_types_of_fields(std::make_index_sequence<fields_count>{});
		}

		template<size_t... Is>
		static constexpr auto detect_types_of_fields(std::index_sequence<Is...>)
		{
			return type_list<type_list_element_t<types_of_fields_indexes[Is], TypeList>...>{};
		}
#endif
	};
}