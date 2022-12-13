#include <time_system.h>
#include <defines.h>

float Time::delta_time = 0;
float Time::elapsed_time = 0;
float Time::last_debounced_time = 0;
unsigned Time::fixed_update_times_this_frame = 0;

float Time::elapsed_time_last_frame = 0;
float Time::elapsed_time_fixed_update_remaining = 0;

/*static*/ void Time::Update() {
  elapsed_time = glfwGetTime();
  delta_time = elapsed_time - elapsed_time_last_frame;
  elapsed_time_last_frame = elapsed_time;

  float elapsed_time_fixedUpdate = delta_time + elapsed_time_fixed_update_remaining;
  fixed_update_times_this_frame = unsigned(std::floor(elapsed_time_fixedUpdate / fixed_delta_time));

  elapsed_time_fixed_update_remaining = elapsed_time_fixedUpdate - float(fixed_update_times_this_frame) * fixed_delta_time;
}
