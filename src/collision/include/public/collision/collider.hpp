#pragma once

#include <math/vector2.hpp>

#include <utility>

namespace jrag::collision
{
   struct collider
   {
      template<typename NumType>
      using vector2 = jrag::math::vector2<NumType>;

      vector2<float> position{0.0F, 0.0F};
      vector2<float> size{0.0F, 0.0F};

      [[nodiscard]] auto get_bounding_rect() const -> std::pair<vector2<float>, vector2<float>>;
   };
}