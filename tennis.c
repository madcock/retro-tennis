/*
   Copyright [yyyy] [name of copyright owner]

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
#define BALL_SPEED 1.50 /* 1.50 */
#define BALL_ENGLISH 0.25 /* 2.30 */
#define BALL_WIDTH 1
#define BALL_HEIGHT 1
#define BALL_RIGHT (BASE_WIDTH - BALL_WIDTH)
#define BALL_BOTTOM (BASE_HEIGHT - BALL_HEIGHT)
#define PADDLE_WIDTH 1.5
#define PADDLE_HEIGHT 10
#define PADDLE_LEFT (PADDLE_WIDTH * 15)
#define PADDLE_RIGHT (BASE_WIDTH - PADDLE_LEFT)
#define PADDLE_STEP 0.86274509803 /* 44/51 */

static float ball_x;
static float ball_y;
static float ball_dx;
static float ball_dy;
static float ball_english;
static float ball_speed;
static float ball_width;
static float ball_height;
static float ball_left;
static float ball_top;
static float ball_right;
static float ball_bottom;
static float paddle_right_pos;
static float paddle_left_pos;
static float paddle_left;
static float paddle_right;
static float paddle_width;
static float paddle_height;
static float paddle_step;
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
   ball_width = BALL_WIDTH * sx;
   ball_height = BALL_HEIGHT * sy;
   ball_left = BALL_WIDTH * sx;
   ball_top = BALL_HEIGHT * sy;
   ball_right = BALL_RIGHT * sx;
   ball_bottom = BALL_BOTTOM * sy;
   paddle_width = PADDLE_WIDTH * sx;
   paddle_height = PADDLE_HEIGHT * sy;
   paddle_left = PADDLE_LEFT * sx;
   paddle_right = PADDLE_RIGHT * sx;
   paddle_step = PADDLE_STEP * sy;
   score_left = 0;
   score_right = 0;
}

void tennis_ball_step(void)
{
   ball_x += ball_dx;
   ball_y += ball_dy;
   /* Bounce off top/bottom walls and paddles */
   if (ball_x < paddle_left && ball_x > paddle_left - paddle_width && ball_dx < 0) {
      if (ball_y > paddle_left_pos - paddle_height - ball_height && ball_y < paddle_left_pos + paddle_height + ball_height) {
	 ball_dx = -ball_dx;
	 ball_dy = (ball_y - paddle_left_pos) * ball_english;
	 ball_x += 2 * (ball_x - paddle_left);
      }
   } else if (ball_x > paddle_right && ball_x < paddle_right + paddle_width && ball_dx > 0) {
      if (ball_y > paddle_right_pos - paddle_height && ball_y < paddle_right_pos + paddle_height) {
	 ball_dx = -ball_dx;
	 ball_dy = (ball_y - paddle_right_pos) * ball_english;
	 ball_x += 2 * (ball_x - paddle_right);
      }
   }
   if (ball_y - ball_height < ball_top && ball_dy < 0) {
      ball_dy = -ball_dy;
      ball_y += 2 * (ball_y - ball_top);
   } else if (ball_y + ball_height > ball_bottom && ball_dy > 0) {
      ball_dy = -ball_dy;
      ball_y += 2 * (ball_y - ball_bottom);
   }
   /* Fall through court left or right */
   if (ball_x - ball_width < ball_left && ball_dx < 0) {
      ball_dx = -ball_dx;
      ball_x = (ball_right - ball_left) / 2;
   } else if (ball_x + ball_width > ball_right && ball_dx > 0) {
      ball_dx = -ball_dx;
      ball_x = (ball_right - ball_left) / 2;
   }
}

void tennis_paddle_move(unsigned char right, unsigned char left)
{
   paddle_right_pos = paddle_height + right * paddle_step;
   paddle_left_pos = paddle_height + left * paddle_step;
}

void tennis_render_screen(uint32_t *buf, unsigned stride, unsigned pixels)
{
   for (unsigned x = 0; x < pixels; x++)
      buf[x] = 0x000000;

   for (unsigned y = ball_y - ball_height; y <= ball_y + ball_height; y++)
      for (unsigned x = ball_x - ball_width; x <= ball_x + ball_width; x++)
         buf[y * stride + x] = 0xffffff;

   for (unsigned y = paddle_right_pos - paddle_height; y <= paddle_right_pos + paddle_height; y++)
      for (unsigned x = paddle_right + paddle_width; x <= paddle_right + paddle_width * 3; x++)
         buf[y * stride + x] = 0xffffff;

   for (unsigned y = paddle_left_pos - paddle_height; y <= paddle_left_pos + paddle_height; y++)
      for (unsigned x = paddle_left - paddle_width * 3; x <= paddle_left - paddle_width; x++)
         buf[y * stride + x] = 0xffffff;
}
