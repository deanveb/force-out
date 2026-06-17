#include "raylib.h"
#include "resource_dir.h"
#include "time.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>

#define TIME_DURATION 20   // seconds
#define TIMER_MAX_LENGTH 5 // 00:00
#define MIN_SOUND "min-sound.mp3"
#define SEC_SOUND "sec-sound.mp3"

struct TimeState {
  bool isRunning;
  time_t deadline;
  bool isMinAnnouncementPlayed;
  bool isSecAnnouncementPlayed;
};

bool isLogOutCondition(struct TimeState state);
void playTimerSound(struct TimeState *state, Sound minSound, Sound secSound);
void numberToTime(char *buffer, time_t time);
void drawTimer(struct TimeState state);
void updateTimerStatus(struct TimeState *state);

int main() {
  InitWindow(800, 600, "Force Out");
  InitAudioDevice();
  SetTargetFPS(60);
  if (!SearchAndSetResourceDir("resources"))
    TraceLog(LOG_ERROR, "Resources dir Not Found!");

  Sound minAnnouncment = LoadSound(MIN_SOUND);
  Sound secAnnouncment = LoadSound(SEC_SOUND);

  struct TimeState timerState = {false, (time_t)-1, false, false};

  while (!WindowShouldClose()) {
    updateTimerStatus(&timerState);
    if (isLogOutCondition(timerState)) {
      system("loginctl terminate-user $USER");
    }
    playTimerSound(&timerState, minAnnouncment, secAnnouncment);

    BeginDrawing();
    // char time[TIMER_MAX_LENGTH];
    // numberToTime(time, )
    // TraceLog(LOG_INFO, "%s", time);
    // TraceLog(LOG_INFO, "%d", deadline);
    drawTimer(timerState);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

void playTimerSound(struct TimeState *state, Sound minSound, Sound secSound) {
  if (state->deadline == (time_t)-1) {
    return;
  }
  time_t now = time(NULL);

  if (state->deadline - now <= 60 && state->isMinAnnouncementPlayed == false) {
    state->isMinAnnouncementPlayed = true;
    PlaySound(minSound);
  } else if (state->deadline - now <= 10 &&
             state->isSecAnnouncementPlayed == false) {
    state->isSecAnnouncementPlayed = true;
    PlaySound(secSound);
  }
}

void numberToTime(char *buffer, time_t time) {
  uint8_t minutes = time / 60;
  uint8_t seconds = time % 60;

  snprintf(buffer, TIMER_MAX_LENGTH + 1, "%02d:%02d", minutes, seconds);
}

void drawTimer(struct TimeState state) {
  char timer[TIMER_MAX_LENGTH + 1];
  time_t now = time(NULL);
  if (state.deadline == (time_t)(-1)) {
    numberToTime(timer, TIME_DURATION);
  } else {
    numberToTime(timer, state.deadline - now);
  }
  TraceLog(LOG_INFO, "%s", timer);

  // FIXME: Somehow the text is wrong
  DrawText(&timer, GetScreenWidth() / 2, GetScreenHeight() / 2, 20, BLUE);
}

void updateTimerStatus(struct TimeState *state) {
  if (IsKeyPressed(KEY_SPACE) && state->isRunning == false) {
    state->deadline = time(NULL) + (time_t)TIME_DURATION;
    state->isRunning = true;

    if (state->deadline == (time_t)(-1)) {
      TraceLog(LOG_ERROR, "can't initialize time");
      exit(1);
    }
  }
}

bool isLogOutCondition(struct TimeState state) {
  if (state.deadline == (time_t)(-1)) {
    return false;
  }
  time_t now = time(NULL);

  if (state.deadline - now <= 0) {
    return true;
  }
  return false;
}
