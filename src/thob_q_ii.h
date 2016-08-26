#include <pebble.h>
#define NUM_CLOCK_TICKS 12

static const struct GPathInfo ANALOG_BG_POINTS[] = {

 { // [0] path rect3069
    .num_points = 4,
    .points = (GPoint []) {
      {69, -7},
      {76, -7},
      {76, 14},
      {69, 14}
    }
  },
  { // [1] path path3312
    .num_points = 4,
    .points = (GPoint []) {
      {109, 2},
      {114, 6},
      {104, 23},
      {98, 20}
    }
  },
  { // [2] path path3338
    .num_points = 4,
    .points = (GPoint []) {
      {138, 30},
      {142, 35},
      {124, 46},
      {120, 40}
    }
  },
  { // [3] path path3340
    .num_points = 4,
    .points = (GPoint []) {
      {150, 68},
      {150, 75},
      {130, 75},
      {130, 68}
    }
  },
  { // [4] path path3342
    .num_points = 4,
    .points = (GPoint []) {
      {142, 108},
      {138, 113},
      {120, 103},
      {124, 97}
    }
  },
  { // [5] path path3344
    .num_points = 4,
    .points = (GPoint []) {
      {114, 137},
      {109, 141},
      {98, 123},
      {104, 119}
    }
  },
  { // [6] path path3346
    .num_points = 4,
    .points = (GPoint []) {
      {76, 148},
      {69, 148},
      {69, 129},
      {76, 129}
    }
  },
  { // [7] path path3348
    .num_points = 4,
    .points = (GPoint []) {
      {36, 141},
      {31, 137},
      {41, 119},
      {47, 123}
    }
  },
  { // [8] path path3350
    .num_points = 4,
    .points = (GPoint []) {
      {7, 113},
      {3, 108},
      {21, 97},
      {24, 103}
    }
  },
  { // [9] path path3352
    .num_points = 4,
    .points = (GPoint []) {
      {-6, 75},
      {-6, 68},
      {15, 68},
      {15, 75}
    }
  },
  { // [10] path path3354
    .num_points = 4,
    .points = (GPoint []) {
      {3, 35},
      {7, 30},
      {24, 40},
      {21, 46}
    }
  },
  { // [11] path path3356
    .num_points = 4,
    .points = (GPoint []) {
      {31, 6},
      {36, 2},
      {47, 20},
      {41, 23}
    }
  }
};

//72
static const GPathInfo SECOND_HAND_POINTS = {
  4,
  (GPoint []) {
    { -2, 29 },
    { 2, 29 },
    { 2, -69 },
    { -2, -69 }
  }
};

static const GPathInfo SECOND_HAND_POINTS_B = {
  4,
  (GPoint []) {
    { -3, 29 },
    { 2, 29 },
    { 2, -69 },
    { -3, -69 }
  }
};

static const GPathInfo SECOND_HAND_POINTS_90 = {
  4,
  (GPoint []) {
    { -29, 2 },
    { -29, -2 },
    { 69, -2 },
    { 69, 2 }
  }
};

static const GPathInfo MINUTE_HAND_POINTS = {
  4,
  (GPoint []) {
    { -4, 20 },
    { 4, 20 },
    { 4, -72 },
    { -4, -72 }
  }
};

//75
static const GPathInfo MINUTE_HAND_POINTS_B = {
  4,
  (GPoint []) {
    { -5, 20 },
    { 4, 20 },
    { 4, -72 },
    { -5, -72 }
  }
};

static const GPathInfo MINUTE_HAND_POINTS_90 = {
  4,
  (GPoint []) {
    { -20, 4 },
    { -20, -4 },
    { 72, -4 },
    { 72, 4 }
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
  4,
  (GPoint []) {
    { -6, 20 },
    { 5, 20 },
    { 5, -51 },
    { -6, -51 }
  }
};

static const GPathInfo HOUR_HAND_POINTS_B = {
  4,
  (GPoint []) {
    { -6, 20 },
    { 6, 20 },
    { 6, -51 },
    { -6, -51 }
  }
};