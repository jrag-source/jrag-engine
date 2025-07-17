#pragma once

#include "vector2.hpp"

#include <optional>
#include <type_traits>
#include <utility>

namespace jrag::math
{
    template<typename NumType, typename = typename std::enable_if_t<std::is_signed_v<NumType>>>
    struct rect
    {
        vector2<NumType> position; // top left corner
        vector2<NumType> size;

        [[nodiscard]] constexpr auto left() const { return position.x; }
        [[nodiscard]] constexpr auto top() const { return position.y; };
        [[nodiscard]] constexpr auto right() const { return position.x + size.x; }
        [[nodiscard]] constexpr auto bottom() const { return position.y + size.y; }
        [[nodiscard]] constexpr auto center() const { return position + (size / static_cast<NumType>(2)); }

        constexpr auto set_center(vector2<NumType> const center) -> void
        {
            position = center - (size / static_cast<NumType>(2));
        }

        [[nodiscard]] constexpr auto get_area() const -> NumType
        {
            return size.x * size.y;
        };

        [[nodiscard]] constexpr auto get_intersection(rect<NumType> const other) const -> std::optional<rect<NumType>>
        {
            auto const rightmost_left {left() > other.left() ? left() : other.left()};
            auto const leftmost_right {right() < other.right() ? right() : other.right()};
            auto const horizontal_overlap {leftmost_right - rightmost_left};
            if (horizontal_overlap <= static_cast<NumType>(0))
            {
                return std::nullopt;
            }

            auto const bottommost_top {top() > other.top() ? top() : other.top()};
            auto const upmost_bottom {bottom() < other.bottom() ? bottom() : other.bottom()};
            auto const vertical_overlap {upmost_bottom - bottommost_top};
            if (vertical_overlap <= static_cast<NumType>(0))
            {
                return std::nullopt;
            }

            return std::optional<rect<NumType>>{std::in_place, vector2<NumType>{rightmost_left, bottommost_top}, vector2<NumType>{horizontal_overlap, vertical_overlap}};
        }

        [[nodiscard]] constexpr auto is_intersecting(rect<NumType> const other) const -> bool
        {
            return get_intersection(other).has_value();
        } 
    };
}