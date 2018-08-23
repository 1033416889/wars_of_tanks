#include "PickupBase.h"
#include <cstring>

PickupBase * PickupBase::createWithType(PickupTypes type)
{
	auto item = new PickupBase();
	std::string path;
	switch (type)
	{
	case Gold:
		path = GOLDPATH;
		break;

	}
	if (item && item->initWithFile(path))
	{
		item->autorelease();
	}
	auto body = PhysicsBody::createEdgeBox(item->getContentSize());
	body->setCategoryBitmask(0x01);
	body->setContactTestBitmask(0x01);
	item->setPhysicsBody(body);
	return item;
}

PickupBase * PickupBase::createWithImage(const char * path)
{
	auto item = new PickupBase();
	if (item && item->initWithFile(path))
	{
		item->autorelease();
	}
	auto body = PhysicsBody::createEdgeBox(item->getContentSize());
	body->setCategoryBitmask(0x01);
	body->setContactTestBitmask(0x01);
	item->setPhysicsBody(body);
	return item;
}

void PickupBase::isContact(OurTank * player)
{
	log("pick up");
	switch (this->getPickupType())
	{
	case Gold:
		player->setHP(player->getHP()+1);
		player->setWeaponType(WEAPON_3);
		log("HP: %d", player->getHP());
		this->removeFromParent();
		break;

	}

}