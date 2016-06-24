#include "Enemy.h"

bool Enemy::init(const char *fileName, int maxSpeed, float gravity, bool isYanki, int myNo)
{
	if (!Sprite::init())return false;
	initWithFile(fileName);
	setScale(0.2f);
	//重力の値
	_gSpeed = 0;
	//プレイヤーの速度
	_pSpeed = Vec2(0, _gSpeed);
	//右向きに向かっているか
	_isGoRight = true;
	//下向きに向かっているか
	_isGoDown = true;
	//タッチしているか	
	_isWind = false;
	//ゴールしたかどうか
	_isGool = false;
	//やんきーかどうか
	_isYanki = isYanki;
	//自分の番号(ナニ田さんなのかの番号)
	_myNo = myNo;
	//最大移動速度
	_maxSpeed = maxSpeed;

	_speed = 1.0;

	_isMoved = false;

	//ステージ内での最大移動量（X方向）
	_moveMaxX = (designResolutionSize.width * 2) - 100;

	//一番近い吉田さんの場所
	_yoshidaPos = Vec2(0,0);

	_yoshidatonoAngle = 0.0f;

	// updateを毎フレーム実行するように登録する
	this->scheduleUpdate();

	return true;
}

void Enemy::update(float dt)
{
	if (!_isGool)speedKeisan();
	if (_isWind)rotateKeisan();
	//if(_isMoved)enemyMove();
}

void Enemy::enemyMove() 
{
	Vec2 myPos = getPosition();
	//Vec2 a = _yoshidaPos - myPos;
	////float kyori = sqrt(pow(a.x, 2) + pow(a.y, 2));
	//_yoshidatonoAngle = atan2(a.y, a.x);
	Vec2 idou = Vec2(cos(_yoshidatonoAngle) * _speed, sin(_yoshidatonoAngle) * _speed);
	//Vec2 idou = Vec2((a.x/kyori) *_speed, (a.y/kyori) * _speed);
	setPosition(myPos + idou);
}
