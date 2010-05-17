
#ifndef GAME_H
#define GAME_H

#include "geometry.h"
#include "image.h"

#define MAX_BULLETS 400

enum GameState {
	MENU_STATE,
	GAME_STATE,
};
enum GameState gameState;

Image buttons, font, explosion, tick;

typedef struct {
	Vector2d pos;
	Vector2d dir;
	int dead;
} Bullet;

struct {
	Vector2d pos;
	Vector2d dp; // change in position, used for bullet velocity

	Vector2d posOffset;

	int bulletCount; // active array slots
	Bullet bullets[MAX_BULLETS];

	int deadCount;
	int dead[MAX_BULLETS];
	int score;
	int displayedScore;
	int health;
	float lastKill;
	float lastHit;
	float deathTime;
	int scoreSaved;
} player;

void moveStart(int x, int y);
void moveDrag(int x, int y);
void moveEnd(int x, int y);

void aimStart(int x, int y);
void aimDrag(int x, int y);
void aimEnd(int x, int y);


void renderAll(float dt);

void loadResources(); // start a new game
void initGame(); // initialize game data
void updateState(float dt);
void renderGame(float dt);


void updateBullets(float dt);
void renderBullets(); // render player bullets
void clearBullets();

void pushBullet(); // shoot a bullet
void killBullet(int i);

void renderBulletHitbox();
Box	bulletHitbox(Bullet b);
Box playerHitbox();

void onKillEnemy(int type);
void onPlayerHit();


#endif
