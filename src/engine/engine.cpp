#include <engine/engine.hpp>

#include <app_state/state_manager.hpp>
#include <input/dispatcher.hpp>
#include <input/loader.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <chrono>
#include <thread>

namespace
{
   using namespace std::chrono_literals;
   using clock = std::chrono::steady_clock;

   using process_status = jrag::app_state::state_manager::process_status;
   [[nodiscard]] auto update_input_dispatcher(
      jrag::input::dispatcher & input_dispatcher,
      sf::RenderWindow & window) -> process_status
   {
      sf::Event event{};

      while (window.pollEvent(event))
      {
         switch (event.type)
         {
            using enum sf::Event::EventType;

            case Closed: return process_status::inactive;

            default: break;
         }

         input_dispatcher.record_event(event);
      }

      input_dispatcher.evaluate_bindings();

      return process_status::active;
   }

   [[nodiscard]] auto process_logic(clock::time_point & next_tick_time,
      std::chrono::nanoseconds const tick_interval,
      jrag::input::dispatcher & input_dispatcher, sf::RenderWindow & window,
      jrag::app_state::state_manager & state_manager) -> process_status
   {
      while (next_tick_time <= clock::now())
      {
         next_tick_time += tick_interval;

         if (update_input_dispatcher(input_dispatcher, window) ==
               process_status::inactive ||
            state_manager.logic_process(window, input_dispatcher,
               tick_interval) == process_status::inactive)
         {
            return process_status::inactive;
         }
      }

      return process_status::active;
   };

   void process_graphics(clock::time_point const next_tick_time,
      std::chrono::nanoseconds const tick_interval, sf::RenderWindow & window,
      jrag::app_state::state_manager & state_manager)
   {
      using clock_time = clock::time_point;
      auto calculate_interpolation =
         [] [[nodiscard]] (clock_time const prev_time,
            clock_time const next_time, clock_time const current_time) -> float
      {
         auto const interval_duration{next_time - prev_time};
         auto const elapsed_duration{current_time - prev_time};
         auto const elapsed_ratio{static_cast<float>(elapsed_duration.count()) /
            static_cast<float>(interval_duration.count())};
         return std::min(elapsed_ratio, 1.0F);
      };

      window.clear();
      state_manager.graphics_process(window,
         calculate_interpolation(
            next_tick_time - tick_interval, next_tick_time, clock::now()));
      window.display();
   };

   void wait_until(std::chrono::time_point<clock> const resume_time)
   {
      if (clock::now() >= resume_time)
      {
         return; // Need to go back to processing immediately, no need to wait.
      }

      auto asymptotic_average = [](auto const value, auto const target)
      {
         constexpr auto percent_to_target{5U};
         auto const target_delta{(target - value) * percent_to_target / 100U};
         return value + target_delta;
      };

      static auto wake_delay_estimate{0ns};

      // Perform bulk of waiting by putting thread to sleep.
      // Wake thread early to mitigate potential delays while resuming
      // execution.
      if (auto const wake_time{resume_time - wake_delay_estimate};
          clock::now() < wake_time)
      {
         std::this_thread::sleep_until(wake_time);
         auto const actual_wake_time{clock::now()};
         auto const latest_wake_delay{actual_wake_time - wake_time};
         wake_delay_estimate =
            asymptotic_average(wake_delay_estimate, latest_wake_delay);
      }
      else
      {
         // Wake delay estimate caused thread sleep to be skipped. Progressively
         // dial delay back.
         wake_delay_estimate = asymptotic_average(wake_delay_estimate, 0ns);
      }

      // Busy wait until resume time
      while (clock::now() < resume_time) {}
   }
}

void jrag::engine::run(jrag::engine::parameters const params)
{
   constexpr auto window_width{640U};
   constexpr auto window_height{480U};

   sf::RenderWindow window{
      sf::VideoMode(window_width, window_height), "SFML Game"};

   jrag::input::dispatcher input_dispatcher{};
   input_dispatcher.set_bindings(jrag::input::loader::load());
   jrag::app_state::state_manager state_manager{*(params.root_state)};

   auto const tick_interval{
      std::chrono::nanoseconds{1s} / params.ticks_per_second};
   auto const frame_interval{
      std::chrono::nanoseconds{1s} / params.frames_per_second};

   auto next_tick_time{clock::now()};
   auto next_frame_time{next_tick_time};

   while (true)
   {
      if (process_logic(next_tick_time, tick_interval, input_dispatcher, window,
             state_manager) == process_status::inactive)
      {
         break;
      }

      if (auto const current_time{clock::now()};
          next_frame_time <= current_time)
      {
         next_frame_time = current_time + frame_interval;
         process_graphics(next_tick_time, tick_interval, window, state_manager);
      }

      wait_until(std::min(next_tick_time, next_frame_time));
   }

   // TODO: Return std::expected<void, error_code>
}
