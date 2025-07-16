#include <cmath>
#include <collision/map.hpp>

#include <collision/collider.hpp>

#include <print>
#include <ranges>
#include <cassert>

namespace
{
   auto get_collider(std::string_view const collider_id,
      std::span<std::string const> const collider_ids,
      std::span<jrag::collision::collider> const colliders) -> jrag::collision::collider &
   {
      auto const id_it {std::ranges::find(collider_ids, collider_id)};
      assert(id_it != collider_ids.cend());
      auto const index {std::distance(collider_ids.cbegin(), id_it)};
      return colliders[index];
   }
}

using map = jrag::collision::map;

auto map::create_collider(std::string uuid, vector2<float> position, vector2<float> size) -> void
{  
   m_colliders.emplace_back(position, size);
   m_collider_ids.emplace_back(std::move(uuid));
}

auto map::get_collider(std::string_view const collider_id) -> collider &
{
   return ::get_collider(collider_id, m_collider_ids, m_colliders);
}

auto map::handle_collisions() -> void
{
   auto entity_count{0Z};

   for (auto & collider : m_colliders)
   {
      entity_count += 1Z;

      for (auto & other_collider : m_colliders | std::views::drop(entity_count))
      {
         if (collider.is_intersecting(other_collider))
         {
            // std::println("collision!");

            /*
            Collision Logic:
            1. Get direction of dynamic collider's movement collider.get_center() - collider.get_old_center()
            2. Determine general direction, right, up_right, up, up_left, left, down_left, down, down_right
            3. If right, up, left, or down, then the collision_side is just that.
            4. If diagonal, then pick corresponding corner, and get vector to static collider's matching opposite corner.
            e.g: If down_left, then compare collider's bottom_left corner to other's top_right corner.
            5. Compare movement vector to corner vector. Depending on what side of the corner vector the movement vector lies,
            we can determine the collision_side.
            */

            auto normalize_vector = [] (jrag::math::vector2<float> const vec) -> jrag::math::vector2<float>
            {
               if (vec.x == 0.0F && vec.y == 0.0F)
               {
                  return {0.0F, 0.0F};
               }
               
               auto const x_sqrd {vec.x * vec.x};
               auto const y_sqrd {vec.y * vec.y};
               
               return vec / std::sqrt(x_sqrd + y_sqrd);
            };

            auto const move_direction {normalize_vector(collider.get_center() - collider.get_old_center())};

            enum class general_direction : std::uint8_t
            {
               right,
               up_right,
               up,
               up_left,
               left,
               down_left,
               down,
               down_right,
            };

            auto general_dir {general_direction::right};

            // if(move_direction.x == 0.0F && move_direction.y == 0.0F)
            // {
            //    std::println("no movement");
            // }

            if (move_direction.x == 0.0F)
            {
               // std::println("only vertical");
               general_dir = move_direction.y < 0 ? general_direction::up : general_direction::down;
            }
            else if (move_direction.y == 0.0F)
            {
               // std::println("only horizontal");
               general_dir = move_direction.x < 0 ? general_direction::left : general_direction::right;
            }
            else if (move_direction.x < 0)
            {
               general_dir = move_direction.y < 0 ? general_direction::up_left : general_direction::down_left;
            }
            else // moving right
            {
               general_dir = move_direction.y < 0 ? general_direction::up_right : general_direction::down_right;
            }

            enum class collision_side : std::uint8_t
            {
               right,
               top,
               left,
               bottom,
            };

            auto process_diagonal_direction = [&] (general_direction direction) -> collision_side
            {
               // std::println("diag calc");
               jrag::math::vector2 dynamic_corner {0.0F, 0.0F};
               jrag::math::vector2 static_corner {0.0F, 0.0F};

               auto dynamic_rect {collider.get_bounding_rect()};
               dynamic_rect.set_center(collider.get_old_center());

               auto const static_rect {other_collider.get_bounding_rect()};
               
               switch (direction)
               {
                  using enum general_direction;
                  case up_right:
                     dynamic_corner = {dynamic_rect.right(), dynamic_rect.top()};
                     static_corner = {static_rect.left(), static_rect.bottom()};
                     break;
                  
                  case up_left:
                     dynamic_corner = {dynamic_rect.left(), dynamic_rect.top()};
                     static_corner = {static_rect.right(), static_rect.bottom()};
                     break;
                  
                  case down_left:
                     dynamic_corner = {dynamic_rect.left(), dynamic_rect.bottom()};
                     static_corner = {static_rect.right(), static_rect.top()};
                     break;
                  
                  case down_right:
                     dynamic_corner = {dynamic_rect.right(), dynamic_rect.bottom()};
                     static_corner = {static_rect.left(), static_rect.top()};
                     break;
                  
                  default:
                     break;
               }

               auto const corner_direction {normalize_vector(static_corner - dynamic_corner)};

               auto side = collision_side::right;

               if (direction == general_direction::up_right)
               {
                  if (dynamic_corner.x > static_corner.x)
                  {
                     side = collision_side::top;
                  }
                  else if (dynamic_corner.y < static_corner.y)
                  {
                     side = collision_side::right;
                  }
                  else
                  {
                     side = move_direction.x > corner_direction.x ? collision_side::top : collision_side::right;
                  }
               }
               else if (direction == general_direction::up_left)
               {
                  if (dynamic_corner.x < static_corner.x)
                  {
                     side = collision_side::top;
                  }
                  else if (dynamic_corner.y < static_corner.y)
                  {
                     side = collision_side::left;
                  }
                  else
                  {
                     side = move_direction.x < corner_direction.x ? collision_side::top : collision_side::left;
                  }
               }
               else if (direction == general_direction::down_left)
               {
                  if (dynamic_corner.x < static_corner.x)
                  {
                     side = collision_side::bottom;
                  }
                  else if (dynamic_corner.y > static_corner.y)
                  {
                     side = collision_side::left;
                  }
                  else
                  {
                     side = move_direction.x < corner_direction.x ? collision_side::bottom : collision_side::left;
                  }
                  
               }
               else //down-right
               {
                  if (dynamic_corner.x > static_corner.x)
                  {
                     side = collision_side::bottom;
                  }
                  else if (dynamic_corner.y > static_corner.y)
                  {
                     side = collision_side::right;
                  }
                  else
                  {
                     side = move_direction.x > corner_direction.x ? collision_side::bottom : collision_side::right;
                  }
                  
               }
               
               return side;
            };

            auto side {collision_side::right};

            switch (general_dir)
            {
               case general_direction::right:
                  side = collision_side::right;
                  break;

               case general_direction::up:
                  side = collision_side::top;
                  break;
               
               case general_direction::left:
                  side = collision_side::left;
                  break;
               
               case general_direction::down:
                  side = collision_side::bottom;
                  break;
               
               default:
                  side = process_diagonal_direction(general_dir);
                  break;
            };

            auto const other_rect {other_collider.get_bounding_rect()};

            switch (side)
            {
               using enum collision_side;
               case right:
                  collider.set_center({other_rect.left() - (collider.get_size().x / 2), collider.get_center().y});
                  break;

               case top:
                  std::println("push down");
                  collider.set_center({collider.get_center().x , other_rect.bottom() + (collider.get_size().y / 2)});
                  break;
               
               case left:
                  collider.set_center({other_rect.right() + (collider.get_size().x / 2), collider.get_center().y});
                  break;

               case bottom:
                  std::println("push up");
                  collider.set_center({collider.get_center().x , other_rect.top() - (collider.get_size().y / 2)});
                  break;
            }
            
            // auto const collision_vector {collider.get_center() - collider.get_old_center()};
            // TODO: We can figure out what side of the other_collider was hit first based on the collision vector.
            // e.g: if collision_vector is up, then we hit the other_collider from the bottom.
            // In the case of diagonal vectors, then we can figure out if the vertical or horizontal side was
            // hit first based on the collision_vector direction compared to the direction between the colliders' corners


            // auto const center {collider.get_bounding_rect().center()};
            // auto const other_center {other_collider.get_bounding_rect().center()};

            // auto const center_vec {center - other_center};
            // if ()

            // TODO: keep track of old position to know where to correct position if a collision happens
            // TODO: create an enum for type of collider such as static (fixed in place) or dynamic (movable)
            // for now only the player will be dynamic, walls will be static, and everything else is just an area.
            // auto const right_edge {other_collider.get_bounding_rect().right()};
            // collider.set_center({right_edge + (collider.get_size().x / 2), collider.get_center().y});
         }
      }
   }
}
