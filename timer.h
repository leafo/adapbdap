
#ifndef TIMER_H
#define TIMER_H

#define MAX_TIMERS 10

typedef struct {
	float interval;
	float passed; // time passed
	int dead;
	int (*action)(float);
} Timer;

void updateTimers(float dt);
int createTimer(float i, int (*action)(float));
int removeTimer(int tid);

#endif
