#include "GameScene.h"

USING_NS_CC;

TMXTiledMap * EnemyAI::tileMap = nullptr;
TMXLayer * EnemyAI::layer = nullptr;
int EnemyAI::mapSize = 20;
int EnemyAI::tileSize = 32;

Scene *Game::createScene()
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

	auto layer = Game::create();
	scene->addChild(layer);
	return scene;
}
void Game::setPlayerPosition(Vec2 position)
{
	Vec2 tileCoord = this->tileCoordFromPosition(position);
	int tileGid = _collidable->getTileGIDAt(tileCoord);
	if (tileGid > 0) {
		Value prop = _tileMap->getPropertiesForGID(tileGid);
		ValueMap propValueMap = prop.asValueMap();

		std::string collision = propValueMap["collidable"].asString();
		if (collision == "true") {
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("empty.mp3");
			return;
		}
	}
	Size screenSize = Director::getInstance()->getVisibleSize();
	if (position.y >= screenSize.height || position.y <= 0 || position.x >= screenSize.width || position.x <= 0)
	{
		return;
	}
	_player->runAction(MoveTo::create(0.1, position));
}
Vec2 Game::tileCoordFromPosition(Vec2 pos) {
	int x = pos.x / _tileMap->getTileSize().width;
	int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - pos.y) / _tileMap->getTileSize().height;
	return Vec2(x, y);
}

bool Game::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto listener = EventListenerPhysicsContact::create();
	listener->onContactBegin = [](PhysicsContact & contact)
	{
		log("onContactBegin");
		auto spriteA = (Sprite *)contact.getShapeA()->getBody()->getNode();
		auto spriteB = (Sprite *)contact.getShapeB()->getBody()->getNode();
		//log("%d %d", spriteA->getTag(), spriteB->getTag());
		if (spriteA && spriteA->getTag()==2 && spriteB && spriteB->getTag()==3)
		{
			spriteA->removeFromParent();
			spriteB->removeFromParent();
		}
		else if (spriteA && spriteB && spriteA->getTag()==1 && spriteB->getTag()==6)
		{
			log("%d %d", spriteA->getTag(), spriteB->getTag());
			((PickupBase *)spriteB)->isContact((OurTank *)spriteA);
		}

		//auto item = MenuItemImage::create("weixin.png", "weixin_h.png");

		return true;
	};

	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 1);

	_tileMap = TMXTiledMap::create("map/map2.tmx");
	Bullet::walklay = _tileMap->getLayer("layer1");

	EnemyAI::tileMap = _tileMap;
	
	_tileMap->setTag(1);
	//_tileMap->setScale(Director::getInstance()->getVisibleSize().width / (_tileMap->getMapSize().width * _tileMap->getTileSize().width));
	this->addChild(_tileMap);

	TMXObjectGroup *group = _tileMap->getObjectGroup("objects");
	ValueMap spawnPoint_0 = group->getObject("ninja");

	ValueMap spawnPoint_4 = group->getObject("enemyTest");
	float x4 = spawnPoint_4["x"].asFloat();
	float y4 = spawnPoint_4["y"].asFloat();
	auto enemy4 = Enemy::createWithEnemyTypes(EnemyTypeEnemy1);
	enemy4->setAnchorPoint(Vec2(0.5, 0.5));
	enemy4->setPosition(Vec2(x4, y4));
	this->addChild(enemy4);

	
	enemyAIs[0] = EnemyAI::createWithEnemy(enemy4);

	float x0 = spawnPoint_0["x"].asFloat();
	float y0 = spawnPoint_0["y"].asFloat();
	ValueMap spawnPoint_1 = group->getObject("enemy_1");

	float x1 = spawnPoint_1["x"].asFloat();
	float y1 = spawnPoint_1["y"].asFloat();
	ValueMap spawnPoint_2 = group->getObject("enemy_2");

	float x2 = spawnPoint_2["x"].asFloat();
	float y2 = spawnPoint_2["y"].asFloat();

	auto _enemy_1 = Enemy::createWithEnemyTypes(EnemyTypeEnemy1);
	auto _enemy_2 = Enemy::createWithEnemyTypes(EnemyTypeEnemy2);
	_enemy_1->setPosition(Vec2(x1,y1));
	_enemy_2->setPosition(Vec2(x2,y2));
	enemyAIs[1] = EnemyAI::createWithEnemy(_enemy_1);
	enemyAIs[2] = EnemyAI::createWithEnemy(_enemy_2);

	addChild(_enemy_1, 2, 3);
	addChild(_enemy_2, 2, 3);

	ValueMap spawnPoint_3 = group->getObject("gold");
	float x3 = spawnPoint_3["x"].asFloat();
	float y3 = spawnPoint_3["y"].asFloat();

	auto gold = PickupBase::createWithType(Gold);
	gold->setPosition(Vec2(x3, y3));
	this->addChild(gold);
	gold->setTag(6);

	_player = OurTank::createWithImage(5);
	_player->setAnchorPoint(Vec2(0.5, 0.5));
	_player->setPosition(Vec2(x0, y0));
	addChild(_player);
	_player->setTag(1);
	_collidable = _tileMap->getLayer("collidable");
	Bullet::coll = _collidable;

	EnemyAI::layer = _collidable;

	_player->setDirection(146);

	/*setTouchEnabled(true);
	setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	setKeypadEnabled(true);*/

	setKeyboardEnabled(true);

	auto path = FileUtils::getInstance()->getWritablePath();
	log("%s", MyUtility::gbk_2_utf8(path));
	auto str = FileUtils::getInstance()->writeStringToFile("123", path + "file_data.json");

	this->scheduleUpdate();

	return true;
}

void Game::onKeyPressed(EventKeyboard::KeyCode keyCode, Event * event)
{
	log("%d has been pressed", keyCode);
	Vec2 playerPos = _player->getPosition();
	if ((int)keyCode == 59)
	{
		_player->openFire();
		return;
	}
	if ((int)keyCode != _player->getDirection())
	{
		switch ((int)keyCode)
		{
		case 146:
			_player->runAction(RotateTo::create(0.2, 0));
			break;
		case 142:
			_player->runAction(RotateTo::create(0.2, 180));
			break;
		case 124:
			_player->runAction(RotateTo::create(0.2, 270));
			break;
		case 127:
			_player->runAction(RotateTo::create(0.2, 90));
			break;
		}
		_player->setDirection((int)keyCode);
		this->schedule(schedule_selector(Game::keepMoving), 0.1f);
		return;
	}
	switch ((int)keyCode)
	{
	case 146:
		playerPos.y += _tileMap->getTileSize().height;
		break;
	case 142:
		playerPos.y -= _tileMap->getTileSize().height;
		break;
	case 124:
		playerPos.x -= _tileMap->getTileSize().width;
		break;
	case 127:
		playerPos.x += _tileMap->getTileSize().width;
		break;
	}
	this->setPlayerPosition(playerPos);

	this->schedule(schedule_selector(Game::keepMoving), 0.1f);
}

void Game::onKeyReleased(EventKeyboard::KeyCode keyCode, Event * event)
{
	this->unschedule(schedule_selector(Game::keepMoving));
}

void Game::keepMoving(float dt)
{
	Vec2 playerPos = _player->getPosition();
	switch (_player->getDirection())
	{
	case 146:
		playerPos.y += _tileMap->getTileSize().height;
		break;
	case 142:
		playerPos.y -= _tileMap->getTileSize().height;
		break;
	case 124:
		playerPos.x -= _tileMap->getTileSize().width;
		break;
	case 127:
		playerPos.x += _tileMap->getTileSize().width;
		break;
	}
	this->setPlayerPosition(playerPos);

}

void Game::update(float dt)
{
	for (int i = 0; enemyAIs[i]; ++i)
	{
		enemyAIs[i]->update(dt);
	}
}