#include "timer.h"
struct timeval timerStart;

/* Code taken from HW2 boilerplate */

void StartTimer(){
  gettimeofday(&timerStart, NULL);
}

double GetTimer(){
  struct timeval timerStop, timerElapsed;
  gettimeofday(&timerStop, NULL);

  int microseconds = (timerStop.tv_sec - timerStart.tv_sec) * 1000000 + ((int)timerStop.tv_usec - (int)timerStart.tv_usec);
  timerElapsed.tv_sec = microseconds/1000000;
  timerElapsed.tv_usec = microseconds%1000000;
  return timerElapsed.tv_sec*1000.0+timerElapsed.tv_usec/1000.0;
    
}