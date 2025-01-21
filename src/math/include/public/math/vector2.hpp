#pragma once

#include <type_traits>

namespace jrag::math
{
   template<typename NumType, typename = typename std::enable_if_t<std::is_arithmetic_v<NumType>>>
   struct vector2
   {
      constexpr vector2(NumType const x_val, NumType const y_val) noexcept
         : x{x_val}
         , y{y_val}
      {}

      NumType x;
      NumType y;
   };

   using vector2i = vector2<int>;
   using vector2u = vector2<unsigned int>;
   using vector2f = vector2<float>;
   using vector2d = vector2<double>;

   template<typename NumType>
   [[nodiscard]] constexpr auto operator+(vector2<NumType> const lhs, vector2<NumType> const rhs) noexcept
   {
      return vector2{lhs.x + rhs.x, rhs.y + lhs.y};
   }

   template<typename NumType>
   [[nodiscard]] constexpr auto operator-(vector2<NumType> const lhs, vector2<NumType> const rhs) noexcept
   {
      return vector2{lhs.x - rhs.x, lhs.y - rhs.y};
   }

   template<typename NumType>
   [[nodiscard]] constexpr auto operator*(vector2<NumType> const vec, NumType const scalar) noexcept
   {
      return vector2{vec.x * scalar, vec.y * scalar};
   }

   template<typename NumType>
   [[nodiscard]] constexpr auto operator*(NumType const scalar, vector2<NumType> const vec) noexcept
   {
      return vec * scalar;
   }

   template<typename NumType>
   [[nodiscard]] constexpr auto operator/(vector2<NumType> const vec, NumType const scalar) noexcept
   {
      return vector2{vec.x / scalar, vec.y / scalar};
   }

   template<typename NumType>
   [[nodiscard]] constexpr auto operator/(NumType const scalar, vector2<NumType> const vec) noexcept
   {
      return vec / scalar;
   }
}