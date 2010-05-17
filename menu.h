
#ifndef MENU_H
#define MENU_H

#define SCORE_COUNT 10

static int scores[SCORE_COUNT];
extern void getScores(int *sc, int count);
extern void saveScores(int *sc, int count);

int insertScore(int val);
void onClick(Vector2d point);
void renderMenu(float dt);

#endif
