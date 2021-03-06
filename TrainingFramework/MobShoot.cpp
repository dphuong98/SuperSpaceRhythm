#include "stdafx.h"
#include "MobShoot.h"
MobShoot::MobShoot()
{
	SetState(&MobShoot::Spawn);
}

MobShoot::MobShoot(Blueprint * blueprint, Vector3 pos, Vector3 scale, Vector3 rotation) : MobShoot()
{
	name = _strdup(blueprint->name);


	//Clone components
	for (vector<Component *>::iterator it = blueprint->componentList.begin(); it != blueprint->componentList.end(); ++it) {
		AddComponent((*it)->Clone());
	}

	//Update transform
	UpdatePosition(pos.x, pos.y, pos.z);
	UpdateRotation(rotation.x, rotation.y, rotation.z);
	UpdateScale(scale.x, scale.y, scale.z);

	Init();
}


MobShoot::~MobShoot()
{
	b2Body* tempBody = GetComponent<Collision2D>()->body;
	tempBody->GetWorld()->DestroyBody(tempBody);
}


void MobShoot::Init()
{
	b2Filter filter = GetComponent<Collision2D>()->body->GetFixtureList()->GetFilterData();
	//type of body
	filter.categoryBits = MOB;
	//collide with what
	filter.maskBits = PLAYER | BULLET_RED | BULLET_BLUE| WALL | EXPLOSION | BOSS | CRATE | MOB | MOB_RED | MOB_BLUE | SHIELD;
	GetComponent<Collision2D>()->body->GetFixtureList()->SetFilterData(filter);
}

void MobShoot::CalculateVelocity(float mX, float mY)
{
	x = transform->position.x;
	y = transform->position.y;
	distance = sqrt(pow((mX - x), 2) + pow((mY - y), 2));
	float speed;
	speed = 0.5f * MOVE_SPEED;
	velX = ((mX - x) * speed / distance);
	velY = ((mY - y) * speed / distance);
}


void MobShoot::AddComponent(Component * comp)
{
	GameObject::AddComponent(comp);
	if (dynamic_cast<Animation *>(comp) != NULL) {
		comp->isActive = false;
		animeList.push_back((Animation*)comp);
	}
}

void MobShoot::PlayAnimation(int key)
{
	//Deactivate active animation and active this one if its not active
	if (!animeList.at(key)->isActive) {
		animeList.at(key)->isActive = true;
		if (activeAnimation != -1)
			animeList.at(activeAnimation)->isActive = false;
		activeAnimation = key;
	}
}

void MobShoot::Spawn()
{
	GameManager::GetInstance()->Spawn("spawn",
		SceneManager::GetInstance()->GetBlueprintByName("spawn"),
		Vector3(transform->position.x, transform->position.y, EFFECT_LAYER),
		Vector3(1, 1, 1),
		Vector3());
	SetState(&MobShoot::Idle);
}

void MobShoot::Idle()
{
	if (GameManager::GetInstance()->player->transform->position.x <= transform->position.x)
		PlayAnimation(0);
	else
		PlayAnimation(1);
}

void MobShoot::Death()
{
	GameManager::GetInstance()->Spawn("smoke",
		SceneManager::GetInstance()->GetBlueprintByName("smoke"),
		Vector3(transform->position.x, transform->position.y, EFFECT_LAYER),
		Vector3(1.0f, 1.0f, 1.0f),
		Vector3());
	GameManager::GetInstance()->addToRemovalList(this);
	GameManager::GetInstance()->mobCount++;
}

void MobShoot::Update(float deltaTime)
{
	//Transition
	if (GetComponent<HP>()->dead) {
		if (((Player*)(GameManager::GetInstance()->player))->SpeedBoost)
		{
			((Player*)(GameManager::GetInstance()->player))->speedIncrease = TRUE;
		}
		SetState(&MobShoot::Death);
	}

	if (activeState != NULL)
		(this->*activeState)();

	if (SoundManager::GetInstance()->enemySignal == 2)
	{
		BulletManager::GetInstance()->Trace(transform->position, "eBullet_mob");
	}
	

	b2Vec2 bodyPos = GetComponent<Collision2D>()->body->GetPosition();
	transform->setPosition(bodyPos.x * PIXEL_RATIO, bodyPos.y * PIXEL_RATIO, transform->position.z);
	CalculateVelocity(GameManager::GetInstance()->player->transform->position.x,
		GameManager::GetInstance()->player->transform->position.y);
	if (distance <= 200.0f) {
		AddToPosition(0.0f, 0.0f);
	}
	else
		AddToPosition(velX, velY);
	GameObject::Update(deltaTime);
}

void MobShoot::checkCollision(GameObject * tempObj)
{
	if (strcmp(tempObj->name, "pBullet_red") == 0
		|| strcmp(tempObj->name, "pBullet_blue_crit") == 0
		|| strcmp(tempObj->name, "pBullet_red_crit") == 0) {
		GetComponent<HP>()->Damage(((Bullet*)tempObj)->damage);
		((Bullet*)tempObj)->SetState(&Bullet::Despawn);
	}
	if (strcmp(tempObj->name, "pBullet_blue") == 0) {
		GetComponent<HP>()->Damage(((Bullet*)tempObj)->damage);
		((Bullet*)tempObj)->SetState(&Bullet::Despawn);
	}
	if (strcmp(tempObj->name, "explosion") == 0) {
		GetComponent<HP>()->Damage(((Explosion*)tempObj)->damage);
	}
}

