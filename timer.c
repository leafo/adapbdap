
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
	if (tid < 0) return 0;
	timers[tid].dead = 1;
	deadTimers[deadTimerCount++] = tid;
	return 1;
}

void clearTimers() {
	int i;
	for (i = 0; i < timerCount; i++) {
		if (timers[i].dead) continue;
		removeTimer(i);
	}
}


int createTimer(float interval, int (*action)(void*)) {
	Timer t = { interval, 0, 0, action };
	int i = timerCount;
	if (deadTimerCount) {
		// printf("timer from trash, i: %f at %d\n",
		// interval, deadTimers[deadTimerCount-1]);
		deadTimerCount--;
		i = deadTimers[deadTimerCount];
	} else {
		// printf("created a timer, i: %f at %d\n", interval, i);
		timerCount++;
	}

	timers[i] = t;
	return i; // the timer id
}

