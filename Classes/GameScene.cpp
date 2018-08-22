#include "SimpleAudioEngine.h"
#include "GameScene.h"
#include "Gold.h"
#include "Bullet.h"
USING_NS_CC;

Scene *Game::createScene()
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

	auto layer = Game::create();
	scene->addChild(layer);
	return scene;
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

	_tileMap = TMXTiledMap::create("map/37.tmx");
	Bullet::walklay = _tileMap->getLayer("layer1");
	tileX = _tileMap->getTileSize().width;
	tileY = _tileMap->getTileSize().height;
	mapX = _tileMap->getMapSize().width;
	mapY = _tileMap->getMapSize().height;
	//log("%d,%d", tileX, tileY);

	//_tileMap->setScale(Director::getInstance()->getVisibleSize().width / (_tileMap->getMapSize().width * _tileMap->getTileSize().width));
	this->addChild(_tileMap);

	

	TMXObjectGroup *group = _tileMap->getObjectGroup("objects");
	ValueMap spawnPoint_0 = group->getObject("tankpoint");

	int  x0 = spawnPoint_0["x"].asInt();
	int  y0 = spawnPoint_0["y"].asInt();
	ValueMap spawnPoint_1 = group->getObject("re1");

	int x1 = spawnPoint_1["x"].asInt();
	int y1 = spawnPoint_1["y"].asInt();
	ValueMap spawnPoint_2 = group->getObject("re2");

	int x2 = spawnPoint_2["x"].asInt();
	int y2 = spawnPoint_2["y"].asInt();

	auto _enemy_1 = Enemy::createWithEnemyTypes(EnemyTypeEnemy1);
	auto _enemy_2 = Enemy::createWithEnemyTypes(EnemyTypeEnemy2);
	_enemy_1->setPosition(Vec2(x1,y1));
	_enemy_2->setPosition(Vec2(x2,y2));
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
	Vec2 playerPos = _player->getPosition();
	//log("%d,%d", playerPos.x, playerPos.y);
	_player->setDirection(146);

	/*setTouchEnabled(true);
	setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	setKeypadEnabled(true);*/

	setKeyboardEnabled(true);

	return true;
}

void Game::setPlayerPosition(Vec2 position)
{
	Size screenSize = Director::getInstance()->getVisibleSize();
	if (position.y > screenSize.height - tileY / 2 || position.y < 0 || position.x > screenSize.width - tileX / 2 || position.x < 0)
	{
		return;
	}
	Vec2 tileCoord = this->tileCoordFromPosition(position);
	log("%f, %f", tileCoord.x, tileCoord.y);
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
	_player->runAction(MoveTo::create(0.1, position));
}
Vec2 Game::tileCoordFromPosition(Vec2 pos) {
	int x = (int)pos.x / tileX;
	//log("%d,%d", tileX, tileY);
	//log("%f,%fasdas", pos.x, pos.y);
//	log("%d, %d", _tileMap->getTileSize().width, _tileMap->getMapSize().width);
	int y = (int)(mapY*tileY - pos.y) / tileY;
	//log("%d,%d", x, y);
	return Vec2(x, y);
}


void Game::onKeyPressed(EventKeyboard::KeyCode keyCode, Event * event)
{
	log("%d has been pressed", keyCode);
	Vec2 playerPos = _player->getPosition();
	log("%f,%f", _player->getPosition().x, _player->getPosition().y);
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
	//log("%f,%f", playerPos.x, playerPos.y);
	this->setPlayerPosition(playerPos);

	this->schedule(schedule_selector(Game::keepMoving), 0.1f);
}

void Game::onKeyReleased(EventKeyboard::KeyCode keyCode, Event * event)
{
	this->unscheduleAllSelectors();
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
