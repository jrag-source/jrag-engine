#pragma once

#include <cstddef>
#include <ranges>

namespace jrag::util
{
   template<class EnumType>
   constexpr auto as_iterable()
   {
      return std::ranges::iota_view{
         0U, static_cast<std::size_t>(EnumType::Count)};
   }

   template<class EnumType>
   constexpr auto as_index(EnumType const val) -> std::size_t
   {
      return static_cast<std::size_t>(val);
   }

   template<class EnumType>
   constexpr auto as_enum(std::size_t const index) -> EnumType
   {
      return static_cast<EnumType>(index);
   }
}