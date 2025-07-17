#include <collision/map.hpp>

#include <collision/collider.hpp>

#include <algorithm>
#include <cassert>
#include <optional>
#include <ranges>

namespace
{
   auto find_index(std::string_view const collider_id,
      std::span<std::string const> const collider_ids) -> std::optional<long>
   {
      auto const id_it {std::ranges::find(collider_ids, collider_id)};
      if (id_it == collider_ids.cend())
      {
         return std::nullopt;
      }

      return std::distance(collider_ids.cbegin(), id_it);
   }

   template<typename CollisionType>
   auto ascending_area_predicate(CollisionType const & collision_a, CollisionType collision_b) -> bool
   {
      return collision_a.area > collision_b.area;
   }

   auto handle_stationary_collisions(std::span<jrag::collision::collider> const moving_colliders,
      std::span<jrag::collision::collider const > const stationary_colliders) -> void
   {
      struct stationary_collision
      {
         std::reference_wrapper<jrag::collision::collider> moving_collider;
         std::reference_wrapper<jrag::collision::collider const> stationary_collider;
         float area;
      };

      std::vector<stationary_collision> stationary_collisions {};

      for (auto & moving_collider : moving_colliders)
      {
         for (auto const & stationary_collider : stationary_colliders)
         {
            auto const intersection {moving_collider.get_intersection(stationary_collider)};
            if (intersection.has_value())
            {
               stationary_collisions.emplace_back(moving_collider, stationary_collider, intersection->get_area());
            }
         }
      }

      std::ranges::sort(stationary_collisions, ascending_area_predicate<stationary_collision>);

      for (auto const & collision : stationary_collisions)
      {
         auto & moving_collider {collision.moving_collider.get()};
         auto const & stationary_collider {collision.stationary_collider.get()};

         auto const potential_collision {moving_collider.get_intersection(stationary_collider)};

         if (!potential_collision.has_value()) { continue; }
         auto const collision_rect {*potential_collision};

         auto const is_colliding_less_horizontally {collision_rect.size.x < collision_rect.size.y};
         if (is_colliding_less_horizontally)
         {
            auto const is_right_of_stationary {moving_collider.get_center().x > stationary_collider.get_center().x};
            auto const x_resolve {is_right_of_stationary ? collision_rect.size.x : -collision_rect.size.x};
            moving_collider.move(jrag::math::vector2{x_resolve, 0.0F});
         }
         else
         {
            auto const is_under_stationary {moving_collider.get_center().y > stationary_collider.get_center().y};
            auto const y_resolve { is_under_stationary ? collision_rect.size.y : -collision_rect.size.y};
            moving_collider.move(jrag::math::vector2{0.0F, y_resolve});
         }
      }
   }

   auto handle_moving_collisions(std::span<jrag::collision::collider> const moving_colliders) -> void
   {
      struct moving_collision
      {
         std::reference_wrapper<jrag::collision::collider> collider_a;
         std::reference_wrapper<jrag::collision::collider> collider_b;
         float area;
      };

      std::vector<moving_collision> moving_collisions {};

      auto entity_count {0Z};

      for (auto & collider : moving_colliders)
      {
         ++entity_count;

         for (auto & other_collider : moving_colliders | std::views::drop(entity_count))
         {
            auto const intersection {collider.get_intersection(other_collider)};
            if (intersection.has_value())
            {
               moving_collisions.emplace_back(collider, other_collider, intersection->get_area());
            }
         }
      }

      std::ranges::sort(moving_collisions, ascending_area_predicate<moving_collision>);

      for (auto const & collision : moving_collisions)
      {
         auto & collider_a {collision.collider_a.get()};
         auto & collider_b {collision.collider_b.get()};

         auto const potential_collision {collider_a.get_intersection(collider_b)};

         if (!potential_collision.has_value()) { continue; }
         auto const collision_rect {*potential_collision};

         auto const is_colliding_less_horizontally {collision_rect.size.x < collision_rect.size.y};
         if (is_colliding_less_horizontally)
         {
            auto const is_collider_a_left {collider_a.get_center().x < collider_b.get_center().x};
            auto & left_collider {is_collider_a_left ? collider_a : collider_b};
            auto & right_collider {is_collider_a_left ? collider_b : collider_a};

            jrag::math::vector2 const x_resolve {collision_rect.size.x / 2, 0.0F};
            left_collider.move(-x_resolve);
            right_collider.move(x_resolve);
         }
         else
         {
            auto const is_collider_a_top {collider_a.get_center().y < collider_b.get_center().y};
            auto & top_collider {is_collider_a_top ? collider_a : collider_b};
            auto & bottom_collider {is_collider_a_top ? collider_b : collider_a};

            jrag::math::vector2 const y_resolve {0.0F, collision_rect.size.y / 2};
            top_collider.move(-y_resolve);
            bottom_collider.move(y_resolve);
         }
      }
   }
}

using map = jrag::collision::map;

auto map::create_stationary_collider(std::string uuid, vector2<float> const position, vector2<float> const size) -> void
{  
   m_stationary_colliders.emplace_back(position, size);
   m_stationary_collider_ids.emplace_back(std::move(uuid));
}

auto map::create_moving_collider(std::string uuid, vector2<float> const position, vector2<float> const size) -> void
{  
   m_moving_colliders.emplace_back(position, size);
   m_moving_collider_ids.emplace_back(std::move(uuid));
}

auto map::get_collider(std::string_view const collider_id) -> collider &
{
   auto collider_index {find_index(collider_id, m_stationary_collider_ids)};
   if (collider_index.has_value())
   {
      return m_stationary_colliders[*collider_index];
   }

   collider_index = find_index(collider_id, m_moving_collider_ids);
   assert(collider_index.has_value() && "Requested collider does not exist");

   return m_moving_colliders[*collider_index];
}

auto map::handle_collisions() -> void
{
   handle_stationary_collisions(m_moving_colliders, m_stationary_colliders);
   handle_moving_collisions(m_moving_colliders);
}
