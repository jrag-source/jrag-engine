#include <collision/collider.hpp>

using collider = jrag::collision::collider;

auto collider::get_bounding_rect() const -> std::pair<vector2<float>, vector2<float>>
{
   auto const left{position.x - (size.x / 2)};
   auto const top{position.y - (size.y / 2)};

   auto const right{position.x + (size.x / 2)};
   auto const bottom{position.y + (size.y / 2)};

   return {{left, top}, {right, bottom}};
}
