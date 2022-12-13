#ifndef _TIME_SYSTEM_H
#define _TIME_SYSTEM_H

class Time {
  public:
    static constexpr float fixed_delta_time = 0.02;

    static float delta_time;
    static float elapsed_time;
    static unsigned fixed_update_times_this_frame;

    static void Update();

  private:
    static float elapsed_time_last_frame;
    static float elapsed_time_fixed_update_remaining;
};

#endif