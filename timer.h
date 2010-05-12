
#ifndef TIMER_H
#define TIMER_H

#define MAX_TIMERS 10

typedef struct {
	float interval;
	float passed; // time passed
	int dead;
	int (*action)(void*);
} Timer;

void updateTimers(float dt);
int createTimer(float i, int (*action)(void *));
int removeTimer(int tid);

#endif
