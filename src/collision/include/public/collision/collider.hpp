#pragma once

#include <math/rect.hpp>
#include <math/vector2.hpp>

#include <optional>

namespace jrag::collision
{
   class collider
   {
      template<typename NumType>
      using vector2 = jrag::math::vector2<NumType>;

      public:
         collider(vector2<float> center, vector2<float> size);

         auto set_center(vector2<float> new_center) -> void;
         auto set_size(vector2<float> new_size) -> void;
         auto move(vector2<float> move_vector) -> void;

         [[nodiscard]] auto get_center() const -> vector2<float>;
         [[nodiscard]] auto get_size() const -> vector2<float>;

         [[nodiscard]] auto get_bounding_rect() const -> jrag::math::rect<float>;
         [[nodiscard]] auto get_intersection(collider const & other) const -> std::optional<jrag::math::rect<float>>;
         [[nodiscard]] auto is_intersecting(collider const & other) const -> bool;

      private:
         vector2<float> m_center{0.0F, 0.0F};
         vector2<float> m_size{0.0F, 0.0F};
   };
}