/*
   Copyright 2023 Eric Duhamel

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/	   
#include <stdint.h>

#include "tennis.h"

#define BASE_WIDTH 320
#define BASE_HEIGHT 240
#define BALL_X (BASE_WIDTH / 2)
#define BALL_Y (BASE_HEIGHT / 2)
#define BALL_WIDTH 2
#define BALL_HEIGHT 2
#define BALL_SPEED 1.75 /* 1.50 */
#define BALL_ENGLISH 0.25 /* 2.30 */
#define BALL_RIGHT (BASE_WIDTH - BALL_WIDTH)
#define BALL_BOTTOM (BASE_HEIGHT - BALL_HEIGHT)
#define PADDLE_WIDTH 2
#define PADDLE_HEIGHT 10
#define PADDLE_LEFT (PADDLE_WIDTH * 15)
#define PADDLE_RIGHT (BASE_WIDTH - PADDLE_LEFT)

static float ball_x;
static float ball_y;
static float ball_dx;
static float ball_dy;
static float ball_english;
static float ball_speed;
static float ball_left;
static float ball_top;
static float ball_right;
static float ball_bottom;
static float paddle_right_pos;
static float paddle_left_pos;
static int score_left = 0;
static int score_right = 0;

void tennis_initialize(int x, int y)
{
   int sx = x / BASE_WIDTH;
   int sy = y / BASE_HEIGHT;

   ball_x = BALL_X * sx;;
   ball_y = BALL_Y * sy;
   ball_dx = BALL_SPEED * sx;
   ball_dy = BALL_ENGLISH * sy;
   ball_speed = BALL_SPEED * sx;
   ball_english = BALL_ENGLISH * sy;
   ball_left = BALL_WIDTH * sx;
   ball_top = BALL_HEIGHT * sy;
   ball_right = BALL_RIGHT * sx;
   ball_bottom = BALL_BOTTOM * sy;
   score_left = 0;
   score_right = 0;
}

void tennis_ball_step(void)
{
   ball_x += ball_dx;
   ball_y += ball_dy;
   /* Bounce off top/bottom walls and paddles */
   if (ball_x < PADDLE_LEFT && ball_x > PADDLE_LEFT - PADDLE_WIDTH && ball_dx < 0) {
      if (ball_y > paddle_left_pos - PADDLE_HEIGHT - BALL_HEIGHT && ball_y < paddle_left_pos + PADDLE_HEIGHT + BALL_HEIGHT) {
	 ball_dx = -ball_dx;
	 ball_dy = (ball_y - paddle_left_pos) * ball_english;
	 ball_x += 2 * (ball_x - PADDLE_LEFT);
      }
   } else if (ball_x > PADDLE_RIGHT && ball_x < PADDLE_RIGHT + PADDLE_WIDTH && ball_dx > 0) {
      if (ball_y > paddle_right_pos - PADDLE_HEIGHT && ball_y < paddle_right_pos + PADDLE_HEIGHT) {
	 ball_dx = -ball_dx;
	 ball_dy = (ball_y - paddle_right_pos) * ball_english;
	 ball_x += 2 * (ball_x - PADDLE_RIGHT);
      }
   }
   if (ball_y - BALL_HEIGHT < ball_top && ball_dy < 0) {
      ball_dy = -ball_dy;
      ball_y += 2 * (ball_y - ball_top);
   } else if (ball_y + BALL_HEIGHT > ball_bottom && ball_dy > 0) {
      ball_dy = -ball_dy;
      ball_y += 2 * (ball_y - ball_bottom);
   }
   /* Fall through court left or right */
   if (ball_x - BALL_WIDTH < ball_left && ball_dx < 0) {
      ball_dx = -ball_dx;
      ball_x = (ball_right - ball_left) / 2;
   } else if (ball_x + BALL_WIDTH > ball_right && ball_dx > 0) {
      ball_dx = -ball_dx;
      ball_x = (ball_right - ball_left) / 2;
   }
}

void tennis_paddle_move(unsigned char right, unsigned char left)
{
   paddle_right_pos = right;
   paddle_left_pos = left;
}

void tennis_render_screen(uint32_t *buf, unsigned stride, unsigned pixels)
{
   uint32_t *line   = buf;

   for (unsigned y = 0; y < pixels / stride; y++, line += stride)
   {
      unsigned index_y = (y >> 4) & 1;
      for (unsigned x = 0; x < stride; x++)
      {
         unsigned index_x = (x >> 4) & 1;
         line[x] = (index_y ^ index_x) ? 0x000000 : 0x555555;
      }
   }

   for (unsigned y = ball_y - BALL_HEIGHT; y <= ball_y + BALL_HEIGHT; y++)
      for (unsigned x = ball_x - BALL_WIDTH; x <= ball_x + BALL_WIDTH; x++)
         if (y * stride + x <= pixels && y * stride + x >= 0)
            buf[y * stride + x] = 0xffffff;

   for (unsigned y = paddle_right_pos - PADDLE_HEIGHT; y <= paddle_right_pos + PADDLE_HEIGHT; y++)
      for (unsigned x = PADDLE_RIGHT + PADDLE_WIDTH; x <= PADDLE_RIGHT + PADDLE_WIDTH * 3; x++)
         if (y * stride + x <= pixels && y * stride + x >= 0)
            buf[y * stride + x] = 0xffffff;

   for (unsigned y = paddle_left_pos - PADDLE_HEIGHT; y <= paddle_left_pos + PADDLE_HEIGHT; y++)
      for (unsigned x = PADDLE_LEFT - PADDLE_WIDTH * 3; x <= PADDLE_LEFT - PADDLE_WIDTH; x++)
         if (y * stride + x <= pixels && y * stride + x >= 0)
            buf[y * stride + x] = 0xffffff;
}
