
#ifndef RAND_H
#define RAND_H

#define mt_rand() (mt_random() % ((unsigned)RAND_MAX + 1))
#define rand_float() ((mt_rand() % 1001) / 1000.0)

void mt_init();
unsigned long mt_random();

#endif

