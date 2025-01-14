/*
  This code is copyright 2019 Jonathan Thomson, jethomson.wordpress.com

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/


#ifndef NEW_RGB_WIFILAMP_H
#define NEW_RGB_WIFILAMP_H

#include <FastLED.h>
#include <stdint.h>
#include "Arduino.h"

//#define UFO_DEBUG

#ifdef UFO_DEBUG
 #define DEBUG_PRINT(x)     Serial.print (x)
 #define DEBUG_PRINTDEC(x)     Serial.print (x, DEC)
 #define DEBUG_PRINTHEX(x)     Serial.print (x, HEX)
 #define DEBUG_PRINTLN(x)  Serial.println (x)
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTDEC(x)
 #define DEBUG_PRINTHEX(x)
 #define DEBUG_PRINTLN(x)
#endif

#define NUM_LEDS 132
#define LEDS_DATA_PIN 5
#define LED_STRIP_MILLIAMPS 100
#define LED_STRIP_VOLTAGE 5


#define SOUND_VALUE_GAIN_INITIAL 2
#define HUE_ALIEN_GREEN 112

enum Pattern {   ORBIT = 0, THEATER_CHASE = 1,
                 RUNNING_LIGHTS = 2, SHOOTING_STAR = 3,
                 CYLON = 4, SOLID = 5, JUGGLE = 6, MITOSIS = 7,
                 BUBBLES = 8, SPARKLE = 9, MATRIX = 10, WEAVE = 11,
                 STARSHIP_RACE = 12, PAC_MAN = 13, BALLS = 14,
                 HALLOWEEN_FADE = 15, HALLOWEEN_ORBIT = 16,
                 SOUND_RIBBONS = 17, SOUND_RIPPLE = 18, SOUND_BLOCKS = 19, SOUND_ORBIT = 20,
                 DYNAMIC_RAINBOW = 21};
enum Overlay {NO_OVERLAY = 0, GLITTER = 1, BREATHING = 2, CONFETTI = 3, FLICKER = 4, FROZEN_DECAY = 5};





// Conventions
// -----------
// Forward means a directional pattern moves away from pixel 0 and toward the last pixel in the strip.
// Backward means a directional pattern moves toward pixel 0 and away from the last pixel in the strip.
// If LEFT_TO_RIGHT_IS_FORWARD is true then pixel 0 is the leftmost pixel as seen by the viewer (i.e. the viewer's left),
// pushing a right arrow button moves a pattern forward from left to right, and pushing a left arrow button moves a
// pattern backward from right to left.
// If LEFT_TO_RIGHT_IS_FORWARD is false then pixel 0 is the rightmost pixel as seen by the viewer (i.e. the viewer's right),
// pushing a right arrow button moves a pattern backward from left to right, and pushing a left arrow button moves a
// pattern forward from right to left.
#define LEFT_TO_RIGHT_IS_FORWARD true

#define MIC_PIN    A1

#define HOMOGENIZE_BRIGHTNESS true

class WifiLamp {

    CRGB *leds;
    uint8_t *selected_hue;
    uint16_t selected_led_strip_milliamps;

    uint8_t homogenized_brightness;

    Pattern pattern;
    Overlay transient_overlay;
    Overlay persistent_overlay;

    uint16_t(WifiLamp::*direction_fp)(uint16_t);
    uint16_t(WifiLamp::*antidirection_fp)(uint16_t);

    bool reverse;

    Pattern last_pattern_ran;

    bool autocycle_enabled;
    uint32_t autocycle_previous_millis;
    uint32_t autocycle_interval;

    bool flipflop_enabled;
    uint32_t flipflop_previous_millis;
    uint32_t flipflop_interval;

    class Freezer {
        WifiLamp &parent;
        bool m_frozen;
        const uint16_t m_after_all_black_pause = 500;
        const uint16_t m_failsafe_timeout = 3000;
        uint32_t m_frozen_previous_millis;

      public:
        Freezer(WifiLamp &r);
        void timer(uint16_t freeze_interval);
        bool is_frozen();
    };

    Freezer freezer;

    struct Starship {
        uint16_t distance;
        uint8_t  color;
    };

    uint16_t previous_sample;
    bool sample_peak;
    uint16_t sample_average;
    uint8_t sample_threshold;
    uint16_t sound_value;
    uint8_t sound_value_gain;

  public:
    WifiLamp(CRGB leds[NUM_LEDS], uint8_t *hue_type, uint16_t led_strip_milliamps);

    void set_selected_hue(uint8_t *hue_type);
    void set_selected_led_strip_milliamps(uint16_t led_strip_milliamps);

    void homogenize_brightness();

    Pattern get_pattern();
    int8_t set_pattern(Pattern pattern);
    int8_t set_pattern(Pattern pattern, bool reverse);
    int8_t set_pattern(Pattern pattern, bool reverse, bool disable_autocycle_flipflop);
    int8_t increment_pattern();
    int8_t increment_pattern(bool disable_autocycle_flipflop);

    Overlay get_overlay(bool is_persistent);
    int8_t set_overlay(Overlay overlay, bool is_persistent);
    void increment_overlay(bool is_persistent);

    void set_sound_value_gain(uint8_t gain);

    uint32_t get_autocycle_interval();
    void set_autocycle_interval(uint32_t inteval);
    bool get_autocycle_enabled();
    void set_autocycle_enabled(bool enabled);

    uint32_t get_flipflop_interval();
    void set_flipflop_interval(uint32_t inteval);
    bool get_flipflop_enabled();
    void set_flipflop_enabled(bool enabled);

    void reanimate();

  private:
    int8_t run_pattern(Pattern pattern);
    void apply_overlay(Overlay overlay);

// ++++++++++++++++++++++++++++++
// ++++++++++ PATTERNS ++++++++++
// ++++++++++++++++++++++++++++++
    void orbit(uint16_t draw_interval, int8_t delta);
    void theater_chase(uint16_t draw_interval, uint16_t(WifiLamp::*dfp)(uint16_t));
    void accelerate_decelerate_theater_chase(uint16_t(WifiLamp::*dfp)(uint16_t));
    void running_lights(uint16_t draw_interval, uint16_t(WifiLamp::*dfp)(uint16_t));
    void accelerate_decelerate_running_lights(uint16_t(WifiLamp::*dfp)(uint16_t));
    void shooting_star(uint16_t draw_interval, uint8_t star_size, uint8_t star_trail_decay, uint8_t spm, uint16_t(WifiLamp::*dfp)(uint16_t));
    void cylon(uint16_t draw_interval, uint16_t(WifiLamp::*dfp)(uint16_t));

    void solid(uint16_t draw_interval);
    void juggle();
    void mitosis(uint16_t draw_interval, uint8_t cell_size);
    void bubbles(uint16_t draw_interval, uint16_t(WifiLamp::*dfp)(uint16_t));
    void sparkle(uint16_t draw_interval, bool random_color, uint8_t fade);
    void matrix(uint16_t draw_interval);
    void weave(uint16_t draw_interval);
    void starship_race(uint16_t draw_interval, uint16_t(WifiLamp::*dfp)(uint16_t));
    void pac_man(uint16_t draw_interval, uint16_t(WifiLamp::*dfp)(uint16_t));
    void bouncing_balls(uint16_t draw_interval, uint16_t(WifiLamp::*dfp)(uint16_t));

    void halloween_colors_fade(uint16_t draw_interval);
    void halloween_colors_orbit(uint16_t draw_interval, int8_t delta);

    void sound_ribbons(uint16_t draw_interval);
    void sound_ripple(uint16_t draw_interval, bool trigger);
    void sound_orbit(uint16_t draw_interval, uint16_t(WifiLamp::*dfp)(uint16_t));
    void sound_blocks(uint16_t draw_interval, bool trigger);

    void dynamic_rainbow(uint16_t draw_interval, uint16_t(WifiLamp::*dfp)(uint16_t));

// ++++++++++++++++++++++++++++++
// ++++++++++ OVERLAYS ++++++++++
// ++++++++++++++++++++++++++++++
    void breathing(uint16_t interval);
    void flicker(uint16_t interval);
    void glitter(uint16_t chance_of_glitter);
    void fade_randomly(uint8_t chance_of_fade, uint8_t decay);

// ++++++++++++++++++++++++++++++
// ++++++++++ HELPERS +++++++++++
// ++++++++++++++++++++++++++++++
    uint16_t forwards(uint16_t index);
    uint16_t backwards(uint16_t index);

    void autocycle();
    void flipflop();

    bool is_wait_over(uint16_t interval);
    bool finished_waiting(uint16_t interval);

    void accelerate_decelerate_pattern(uint16_t draw_interval_initial, uint16_t delta_initial, uint16_t update_period, void(WifiLamp::*pfp)(uint16_t, uint16_t(WifiLamp::*dfp)(uint16_t)), uint16_t(WifiLamp::*dfp)(uint16_t));
    void process_sound();
    void motion_blur(int8_t blur_num, uint16_t pos, uint16_t(WifiLamp::*dfp)(uint16_t));
    void fission();

    static int compare(const void * a, const void * b);

    //void print_dt();

};


#endif
