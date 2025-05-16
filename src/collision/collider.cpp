#include <collision/collider.hpp>

using collider = jrag::collision::collider;

auto collider::get_bounding_rect() const -> jrag::math::rect<float>
{
   auto const left{position.x - (size.x / 2)};
   auto const top{position.y - (size.y / 2)};

   return {{left, top}, size};
}

auto collider::is_intersecting(collider const & other) const -> bool
{
   return get_bounding_rect().is_intersecting(other.get_bounding_rect());
}
