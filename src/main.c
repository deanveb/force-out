#include "raylib.h"
#include "time.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>

#define TIME_DURATION 10   // seconds
#define TIMER_MAX_LENGTH 5 // 00:00

bool isLogOutCondition(time_t deadline);
void numberToTime(char *buffer, time_t time);
void drawTimer(time_t deadline);
void updateTimerStatus(time_t *deadline, bool *status);

int main() {
  InitWindow(800, 600, "Force Out");
  SetTargetFPS(60);
  time_t deadline = (time_t)-1;
  bool isRunning = false;
  while (!WindowShouldClose()) {
    updateTimerStatus(&deadline, &isRunning);
    if (isLogOutCondition(deadline)) {
      system("loginctl terminate-user $USER");
    }

    BeginDrawing();
    // char time[TIMER_MAX_LENGTH];
    // numberToTime(time, )
    // TraceLog(LOG_INFO, "%s", time);
    // TraceLog(LOG_INFO, "%d", deadline);
    drawTimer(deadline);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
void numberToTime(char *buffer, time_t time) {
  uint8_t minutes = time / 60;
  uint8_t seconds = time % 60;

  snprintf(buffer, TIMER_MAX_LENGTH + 1, "%02d:%02d", minutes, seconds);
}

void drawTimer(time_t deadline) {
  char timer[TIMER_MAX_LENGTH + 1];
  time_t now = time(NULL);
  if (deadline == (time_t)(-1)) {
    numberToTime(timer, TIME_DURATION);
  } else {
    numberToTime(timer, deadline - now);
  }
  TraceLog(LOG_INFO, "%s", timer);

  // FIXME: Somehow the text is wrong
  DrawText(timer, GetScreenWidth() / 2, GetScreenHeight() / 2, 20, BLUE);
}

void updateTimerStatus(time_t *deadline, bool *status) {
  if (IsKeyPressed(KEY_SPACE) && *status == false) {
    *deadline = time(NULL) + (time_t)TIME_DURATION;
    *status = true;

    if (*deadline == (time_t)(-1)) {
      TraceLog(LOG_ERROR, "can't initialize time");
      exit(1);
    }
  }
}

bool isLogOutCondition(time_t deadline) {
  if (deadline == (time_t)(-1)) {
    return false;
  }
  time_t now = time(NULL);

  if (deadline - now <= 0) {
    return true;
  }
  return false;
}
