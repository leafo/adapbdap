
#include "timer.h"

#include "common.h"

int timerCount = 0;
Timer timers[MAX_TIMERS];

int deadTimerCount = 0;
int deadTimers[MAX_TIMERS];

// update all the living timers
void updateTimers(float dt) {
	int i;
	for (i = 0; i < timerCount; i++) {
		if (timers[i].dead) continue;
		timers[i].passed += dt;
		if (timers[i].passed > timers[i].interval) {
			timers[i].passed -= timers[i].interval;
			int finished = timers[i].action(&timers[i]);
			if (finished) removeTimer(i);
		}
	}
}

int removeTimer(int tid) {
	timers[tid].dead = 1;
	deadTimers[deadTimerCount++] = tid;
}

int createTimer(float interval, int (*action)(void*)) {
	Timer t = { interval, 0, 0, action };
	int i = timerCount;
	if (deadTimerCount) {
		i = deadTimers[deadTimerCount--];
	} else {
		timerCount++;
	}

	printf("created a timer with i = %f\n", interval);

	timers[i] = t;
	return i; // the timer id
}

