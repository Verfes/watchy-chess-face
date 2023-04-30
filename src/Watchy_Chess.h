#ifndef WATCHY_7_SEG_H
#define WATCHY_7_SEG_H

#include <Watchy.h>
#include "Seven_Segment10pt7b.h"
#include "DSEG7_Classic_Regular_15.h"
#include "DSEG7_Classic_Bold_25.h"
#include "DSEG7_Classic_Regular_39.h"
#include "icons.h"

class WatchyChess : public Watchy{
    using Watchy::Watchy;
    public:
        void drawWatchFace();
        void drawTime();
        void drawDate();
        void drawSteps();
        void drawWeather();
        void drawBattery();
        void draw_board();
        void render_board();
        void render_square(uint8_t file, uint8_t row);
        uint16_t make_move(uint16_t move);
        void draw_border(uint8_t file, uint8_t rank);
        void draw_pawn(uint8_t x, uint8_t y);
        void draw_black_pawn(uint8_t x, uint8_t y);
};

#endif