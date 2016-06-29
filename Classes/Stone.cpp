#include "Stone.h"

Stone * Stone::create()
{

	Stone *pRet = new Stone();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool Stone::init()
{
	if (!Item::init())return false;
	initWithFile("pix/stageSozai/stone2.png");
	//setTextureRect(Rect(100, 100, 50, 50));
	setScale(0.6);

	_isWind = false;

	_speed = Vec2(0, 0);

	_maxSpeed = 30;

	_rotCnt = 0;
	_syougaibutu.clear();

	Vec2 StonePos = getPosition();

	this->scheduleUpdate();
	return true;
}

void Stone::update(float dt)
{
	//windSpeedDown();
	speedKeisan();
	targetHitCheak();
	move();

	if (_isWind)
	{
		windHitMoveing();
	}
}

void Stone::windHitMoveing()
{
	windHitRotation();
}

void Stone::windHitRotation()
{
	setRotation(getRotation() + _speed.x);
}

void Stone::move()
{
	//移動量を加算してイドウ
	setPosition(Vec2(getPositionX() + _speed.x, getPositionY() + _speed.y));

	if (getPositionY() < designResolutionSize.height * 0.16f)
	{
		setPositionY(designResolutionSize.height * 0.16f);
	}
}

//speedの計算
void Stone::speedKeisan()
{
	setNoOverMaxSpeed();

	//重力をスピードの減少にどの程度反映するか調整
	int p = 5;

	//y方向の運動量の計算-----------------------------------------------------
	if (!_isGoDown)	//上向き
	{
		if (_speed.y > _gSpeed)
		{
			_speed.y += _gSpeed / p;
		}
		else
		{
			_speed.y = _gSpeed;
			_isGoDown = true;
		}
	}
	else            //下向き
	{
		if (_speed.y < _gSpeed)
		{
			_speed.y -= _gSpeed / p;
		}

		else
		{
			_speed.y = _gSpeed;
		}
	}
	//------------------------------------------------------------------------

	//x方向の運動量の計算-----------------------------------------------------
	if (_speed.x != 0)
	{
		if (_isGoRight)
		{
			if (_speed.x > _gSpeed)
			{
				_speed.x += _gSpeed / p;
			}
			else
			{
				_speed.x = 0;
			}
		}
		else
		{
			if (_speed.x < _gSpeed)
			{
				_speed.x -= _gSpeed / p;
			}
			else
			{
				_speed.x = 0;
			}
		}
	}
	//------------------------------------------------------------------------
}

void Stone::targetHitCheak()
{  
	Rect myRect = this->getBoundingBox();

	//障害物と当たっているか
	for (auto syougaibutu : _syougaibutu)
	{
		//if (syougaibutu->getTag() == 5)continue;
		Rect syougaiRect = syougaibutu->getBoundingBox();
		Size syougaiSize = Size(syougaiRect.size.width, syougaiRect.size.height);
		syougaiRect = Rect{ syougaiRect.getMinX() + syougaiSize.width * 0.05f,
			syougaiRect.getMinY(),
			syougaiSize.width - syougaiSize.width * 0.1f,
			syougaiSize.height + 5 };

		if (myRect.intersectsRect(syougaiRect))
		{
			int angleNum = 0;
			float kyori = 100000;

			if (myRect.getMinY() <= syougaiRect.getMaxY() &&			//上
				kyori >= syougaiRect.getMaxY() - myRect.getMinY())
			{
				angleNum = 1;
				kyori = syougaiRect.getMaxY() - myRect.getMinY();
			}

			if (myRect.getMinX() <= syougaiRect.getMaxX() &&			//右
				kyori >= syougaiRect.getMaxX() - myRect.getMinX())
			{
				angleNum = 2;
				kyori = syougaiRect.getMaxX() - myRect.getMinX();
			}

			if (myRect.getMaxY() >= syougaiRect.getMinY() &&			//下
				kyori >= myRect.getMaxY() - syougaiRect.getMinY())
			{
				angleNum = 3;
				kyori = myRect.getMaxY() - syougaiRect.getMinY();
			}

			if (myRect.getMaxX() >= syougaiRect.getMinX() &&			//左
				kyori >= myRect.getMaxX() - syougaiRect.getMinX())
			{
				angleNum = 4;
				kyori = myRect.getMaxX() - syougaiRect.getMinX();
			}

			switch (angleNum)
			{
			case 1://上
				this->setPosition(Vec2(this->getPosition().x, this->getPosition().y + kyori));
				break;
			case 2://右
				this->setPosition(Vec2(this->getPosition().x + kyori, this->getPosition().y));
				break;
			case 3://下
				this->setPosition(Vec2(this->getPosition().x, this->getPosition().y - kyori));
				break;
			case 4://左
				this->setPosition(Vec2(this->getPosition().x - kyori, this->getPosition().y));
				break;

			default:
				break;
			}
			//isHit = true;
			//break;
		}
	}
}