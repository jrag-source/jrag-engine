#pragma once

#include "vector2.hpp"

#include <type_traits>
#include <utility>

#include <print>

namespace jrag::math
{
    template<typename NumType, typename = typename std::enable_if_t<std::is_arithmetic_v<NumType>>>
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

        [[nodiscard]] constexpr auto is_intersecting(rect<NumType> const other) const -> bool
        {
            static constexpr auto is_in_bounds = [] (auto const number, auto const bounds) -> bool
            {
                return number > bounds.first && number < bounds.second;
            };

            auto is_vertically_aligned = [this, other] () -> bool
            {
                auto have_same_x_bounds = [this, other] () -> bool
                {
                    return size.x == other.size.x && left() == other.left();
                };

                auto const [thinner, wider] {size.x < other.size.x ? std::pair{*this, other} : std::pair{other, *this}};
                std::pair const horizontal_bounds {wider.left(), wider.right()};

                return have_same_x_bounds() || is_in_bounds(thinner.left(), horizontal_bounds) || is_in_bounds(thinner.right(), horizontal_bounds);
            };

            auto is_horizontally_aligned = [this, other] () -> bool
            {
                auto have_same_y_bounds = [this, other] () -> bool
                {
                    return size.y == other.size.y && top() == other.top();
                };
                
                auto const [shorter, taller] {size.y < other.size.y ? std::pair{*this, other} : std::pair{other, *this}};
                std::pair const vertical_bounds {taller.top(), taller.bottom()};
                return have_same_y_bounds() || is_in_bounds(shorter.bottom(), vertical_bounds) || is_in_bounds(shorter.top(), vertical_bounds);;
            };

            return is_vertically_aligned() && is_horizontally_aligned();
        } 
    };
}