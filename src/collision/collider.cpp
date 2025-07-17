#include <collision/collider.hpp>

using collider = jrag::collision::collider;

collider::collider(vector2<float> center, vector2<float> size)
   : m_center {center}
   , m_size {size}
{}

auto collider::set_center(vector2<float> new_center) -> void
{
   m_center = new_center;
}

auto collider::set_size(vector2<float> new_size) -> void
{
   m_size = new_size;
}

auto collider::move(vector2<float> const move_vector) -> void
{
   m_center += move_vector;
}

auto collider::get_center() const -> vector2<float>
{
   return m_center;
}

auto collider::get_size() const -> vector2<float>
{
   return m_size;
}

auto collider::get_bounding_rect() const -> jrag::math::rect<float>
{
   auto const left {m_center.x - (m_size.x / 2)};
   auto const top {m_center.y - (m_size.y / 2)};

   return {{left, top}, m_size};
}

auto collider::get_intersection(collider const & other) const -> std::optional<jrag::math::rect<float>>
{
   return get_bounding_rect().get_intersection(other.get_bounding_rect());
}

auto collider::is_intersecting(collider const & other) const -> bool
{
   return get_bounding_rect().is_intersecting(other.get_bounding_rect());
}
