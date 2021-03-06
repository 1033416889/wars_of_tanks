#pragma once
#include<cocos2d.h>
#include <SimpleAudioEngine.h>
#include "OurTank.h"
#include"Enemy.h"
#include "EnemyAI.h"
#include "Bullet.h"
#include "MyUtility.h"
#include "PickupBase.h"

#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

using namespace cocos2d;

class Game :public Layer
{

	OurTank *_player;
	TMXLayer *_collidable;
	Enemy *_enemy_1;
	Enemy *_enemy_2;
	EnemyAI * enemyAIs[5];

public:
	TMXTiledMap * _tileMap;

	static Scene * createScene();
	void setPlayerPosition(Vec2 position);
	Vec2 tileCoordFromPosition(Vec2 position);
	virtual bool init();

	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event * event);
	virtual void onKeyReleased(EventKeyboard::KeyCode, Event *);
	void keepMoving(float dt);

	//static void RestartCallback(Ref * pSender);
	virtual void update(float dt);

private:
	int tileX, tileY, mapX, mapY;
	void setViewpointCenter(Point position);
	bool isMoveable(Vec2 position);
	CREATE_FUNC(Game);

};
#endif