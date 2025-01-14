#include "catch/catch.hpp"

#include <string>

#include "calendar.h"

// SUN TESTS

// The 24-hour solar cycle is divided into four parts, as returned by four calendar.cpp functions:
//
// is_night : from the end of dusk, until the following sunrise (start of dawn)
// is_dawn  : begins at sunrise, lasts twilight_duration (1 hour)
// is_day   : from the end of dawn, until sunset (start of dusk)
// is_dusk  : begins at sunset, lasts twilight_duration (1 hour)
//
// These are inclusive at their endpoints; in other words, each overlaps with the next like so:
//
// 00:00 is_night
//   :   is_night
// 06:00 is_night && is_dawn ( sunrise )
//   :   is_dawn ( sunrise + twilight )
// 07:00 is_dawn && is_day
//   :   is_day
// 19:00 is_day && is_dusk ( sunset )
//   :   is_dusk ( sunset + twilight )
// 20:00 is_dusk && is_night
//   :   is_night
// 23:59 is_night
//
// The times of sunrise and sunset will naturally depend on the current time of year; this aspect is
// covered by the "sunrise and sunset" and solstice/equinox tests later in this file. Here we simply
// use the first day of spring as a baseline.
//
// This test covers is_night, is_dawn, is_day, is_dusk, and their behavior in relation to time of day.
TEST_CASE( "daily solar cycle", "[sun][night][dawn][day][dusk]" )
{
    // Use sunrise/sunset on the first day (spring equinox)
    static const time_point midnight = calendar::turn_zero;
    static const time_point noon = calendar::turn_zero + 12_hours;
    static const time_point today_sunrise = sunrise( midnight );
    static const time_point today_sunset = sunset( midnight );

    REQUIRE( "Year 1, Spring, day 1 6:00:00 AM" == to_string( today_sunrise ) );
    REQUIRE( "Year 1, Spring, day 1 7:00:00 PM" == to_string( today_sunset ) );

    // First, at the risk of stating the obvious
    CHECK( is_night( midnight ) );
    // And while we're at it
    CHECK_FALSE( is_day( midnight ) );
    CHECK_FALSE( is_dawn( midnight ) );
    CHECK_FALSE( is_dusk( midnight ) );

    // Yep, still dark
    CHECK( is_night( midnight + 1_seconds ) );
    CHECK( is_night( midnight + 2_hours ) );
    CHECK( is_night( midnight + 3_hours ) );
    CHECK( is_night( midnight + 4_hours ) );

    // The point of sunrise is both "night" and "dawn"
    CHECK( is_night( today_sunrise ) );
    CHECK( is_dawn( today_sunrise ) );

    // Dawn
    CHECK_FALSE( is_night( today_sunrise + 1_seconds ) );
    CHECK( is_dawn( today_sunrise + 1_seconds ) );
    CHECK( is_dawn( today_sunrise + 30_minutes ) );
    CHECK( is_dawn( today_sunrise + 1_hours - 1_seconds ) );
    CHECK_FALSE( is_day( today_sunrise + 1_hours - 1_seconds ) );

    // The endpoint of dawn is both "dawn" and "day"
    CHECK( is_dawn( today_sunrise + 1_hours ) );
    CHECK( is_day( today_sunrise + 1_hours ) );

    // Breakfast
    CHECK_FALSE( is_dawn( today_sunrise + 1_hours + 1_seconds ) );
    CHECK( is_day( today_sunrise + 1_hours + 1_seconds ) );
    CHECK( is_day( today_sunrise + 2_hours ) );
    // Second breakfast
    CHECK( is_day( today_sunrise + 3_hours ) );
    CHECK( is_day( today_sunrise + 4_hours ) );
    // Luncheon
    CHECK( is_day( noon - 3_hours ) );
    CHECK( is_day( noon - 2_hours ) );
    // Elevenses
    CHECK( is_day( noon - 1_hours ) );

    // Noon
    CHECK( is_day( noon ) );
    CHECK_FALSE( is_dawn( noon ) );
    CHECK_FALSE( is_dusk( noon ) );
    CHECK_FALSE( is_night( noon ) );

    // Afternoon tea
    CHECK( is_day( noon + 1_hours ) );
    CHECK( is_day( noon + 2_hours ) );
    // Dinner
    CHECK( is_day( noon + 3_hours ) );
    CHECK( is_day( today_sunset - 2_hours ) );
    // Supper
    CHECK( is_day( today_sunset - 1_hours ) );
    CHECK( is_day( today_sunset - 1_seconds ) );
    CHECK_FALSE( is_dusk( today_sunset - 1_seconds ) );

    // The beginning of sunset is both "day" and "dusk"
    CHECK( is_day( today_sunset ) );
    CHECK( is_dusk( today_sunset ) );

    // Dusk
    CHECK_FALSE( is_day( today_sunset + 1_seconds ) );
    CHECK( is_dusk( today_sunset + 1_seconds ) );
    CHECK( is_dusk( today_sunset + 30_minutes ) );
    CHECK( is_dusk( today_sunset + 1_hours - 1_seconds ) );
    CHECK_FALSE( is_night( today_sunset + 1_hours - 1_seconds ) );

    // The point when dusk ends is both "dusk" and "night"
    CHECK( is_dusk( today_sunset + 1_hours ) );
    CHECK( is_night( today_sunset + 1_hours ) );

    // Night again
    CHECK( is_night( today_sunset + 1_hours + 1_seconds ) );
    CHECK( is_night( today_sunset + 2_hours ) );
    CHECK( is_night( today_sunset + 3_hours ) );
    CHECK( is_night( today_sunset + 4_hours ) );
}

// The calendar `sunlight` function returns light level for both sun and moon.
TEST_CASE( "sunlight and moonlight", "[sun][sunlight][moonlight][.]" )
{
    // Use sunrise/sunset on the first day (spring equinox)
    static const time_point midnight = calendar::turn_zero;
    static const time_point today_sunrise = sunrise( midnight );
    static const time_point today_sunset = sunset( midnight );

    // Expected numbers below assume 100.0f maximum daylight level
    // (maximum daylight is different at other times of year - see [daylight] tests)
    REQUIRE( 100.0f == default_daylight_level() );

    SECTION( "sunlight" ) {
        // Before dawn
        CHECK( 1.0f == sunlight( midnight ) );
        CHECK( 1.0f == sunlight( today_sunrise ) );
        // Dawn
        CHECK( 1.0275f == sunlight( today_sunrise + 1_seconds ) );
        CHECK( 2.65f == sunlight( today_sunrise + 1_minutes ) );
        CHECK( 25.75f == sunlight( today_sunrise + 15_minutes ) );
        CHECK( 50.50f == sunlight( today_sunrise + 30_minutes ) );
        CHECK( 75.25f == sunlight( today_sunrise + 45_minutes ) );
        // 1 second before full daylight
        CHECK( 99.9725f == sunlight( today_sunrise + 1_hours - 1_seconds ) );
        CHECK( 100.0f == sunlight( today_sunrise + 1_hours ) );
        // End of dawn, full light all day
        CHECK( 100.0f == sunlight( today_sunrise + 2_hours ) );
        CHECK( 100.0f == sunlight( today_sunrise + 3_hours ) );
        // Noon
        CHECK( 100.0f == sunlight( midnight + 12_hours ) );
        CHECK( 100.0f == sunlight( midnight + 13_hours ) );
        CHECK( 100.0f == sunlight( midnight + 14_hours ) );
        // Dusk begins
        CHECK( 100.0f == sunlight( today_sunset ) );
        // 1 second after dusk begins
        CHECK( 99.9725f == sunlight( today_sunset + 1_seconds ) );
        CHECK( 75.25f == sunlight( today_sunset + 15_minutes ) );
        CHECK( 50.50f == sunlight( today_sunset + 30_minutes ) );
        CHECK( 25.75f == sunlight( today_sunset + 45_minutes ) );
        // 1 second before full night
        CHECK( 1.0275f == sunlight( today_sunset + 1_hours - 1_seconds ) );
        CHECK( 1.0f == sunlight( today_sunset + 1_hours ) );
        // After dusk
        CHECK( 1.0f == sunlight( today_sunset + 2_hours ) );
        CHECK( 1.0f == sunlight( today_sunset + 3_hours ) );
    }

    // This moonlight test is intentionally simple, only checking new moon (minimal light) and full
    // moon (maximum moonlight). More detailed tests of moon phase and light should be expressed in
    // `moon_test.cpp`. Including here simply to check that `sunlight` also calculates moonlight.
    SECTION( "moonlight" ) {
        static const time_duration phase_time = calendar::season_length() / 6;
        static const time_point new_moon = calendar::turn_zero;
        static const time_point full_moon = new_moon + phase_time;

        WHEN( "the moon is new" ) {
            REQUIRE( get_moon_phase( new_moon ) == MOON_NEW );
            THEN( "moonlight is 1.0" ) {
                CHECK( 1.0f == sunlight( new_moon ) );
            }
        }

        WHEN( "the moon is full" ) {
            REQUIRE( get_moon_phase( full_moon ) == MOON_FULL );
            THEN( "moonlight is 10.0" ) {
                CHECK( 10.0f == sunlight( full_moon ) );
            }
        }
    }
}

// current_daylight_level returns seasonally-adjusted maximum daylight level
TEST_CASE( "current daylight level", "[sun][daylight][equinox][solstice]" )
{
    static const time_duration one_season = calendar::season_length();
    static const time_point spring = calendar::turn_zero;
    static const time_point summer = spring + one_season;
    static const time_point autumn = summer + one_season;
    static const time_point winter = autumn + one_season;

    SECTION( "baseline 100 daylight on the spring and autumn equinoxes" ) {
        CHECK( 100.0f == current_daylight_level( spring ) );
        CHECK( 100.0f == current_daylight_level( autumn ) );
    }

    SECTION( "25 percent more daylight on the summer solstice" ) {
        CHECK( 125.0f == current_daylight_level( summer ) );
    }

    SECTION( "25 percent less daylight on the winter solstice" ) {
        CHECK( 75.0f == current_daylight_level( winter ) );
    }

    // Many other times of day have peak daylight level, but noon is for sure
    SECTION( "noon is peak daylight level" ) {
        CHECK( 100.0f == sunlight( spring + 12_hours ) );
        CHECK( 125.0f == sunlight( summer + 12_hours ) );
        CHECK( 100.0f == sunlight( autumn + 12_hours ) );
        CHECK( 75.0f == sunlight( winter + 12_hours ) );
    }
}
