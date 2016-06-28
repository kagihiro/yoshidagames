#include "Yoshidasan.h"
#include "YoshidasanNoManager.h"

bool Yoshidasan::init(const char *fileName, int maxSpeed, float gravity, bool isYanki, int myNo)
{
	if (!Sprite::init())return false;
	initWithFile(fileName);
	setScale(0.15f);
	//重力の値
	_gSpeed = gravity;
	//プレイヤーの速度
	_pSpeed = Vec2(0, _gSpeed);
	//右向きに向かっているか
	_isGoRight = true;
	//下向きに向かっているか
	_isGoDown = true;
	//風に吹かれているか	
	_isWind = true;
	//ゴールしたかどうか
	_isGool = false;
	//やんきーかどうか
	_isYanki = isYanki;
	//自分の番号(ナニ田さんなのかの番号)
	_myNo = myNo;
	//最大移動速度
	_maxSpeed = maxSpeed;
	//ほかの吉田と当たっているか
	_isHit = false;

	//ステージ内での最大移動量（X方向）
	_moveMaxX = (designResolutionSize.width * 2) - 100;

	// updateを毎フレーム実行するように登録する
	this->scheduleUpdate();

	return true;
}

void Yoshidasan::update(float dt)
{
	if (!_isGool)speedKeisan();
	move();
}

//風からの移動量の計算
void Yoshidasan::vecKeisan(Vec2 touchPos, float windRange, float actionTime)
{
	//実際移動量
	float vecPulus = 0.0f;
	//適用範囲
	float hani = windRange;

	//移動値の計算
	if ((sqrt(
		pow(touchPos.x - getPositionX(), 2) +
		pow(touchPos.y - getPositionY(), 2)))
		<= hani)
	{
		if (!_isWind)kazekaiten(actionTime);
		vecPulus = _maxSpeed *
			((hani -
				sqrt(
					pow(touchPos.x - getPositionX(), 2) +
					pow(touchPos.y - getPositionY(), 2)))
			/ hani) ;


		//角度の計算
		float angle = atan2(getPositionY() - touchPos.y, getPositionX() - touchPos.x);
		//移動量の適用
		_pSpeed += Vec2(vecPulus * cos(angle), vecPulus * sin(angle));
	}

	//移動方向の変更X
	if (touchPos.x - getPositionX() <= 0)_isGoRight = true;
	else _isGoRight = false;

	//移動方向の変更Y
	if (touchPos.y - getPositionY() >= 0)_isGoDown = true;
	else _isGoDown = false;
	setNoOverMaxSpeed();
}

//speedの計算
void Yoshidasan::speedKeisan()
{
	setNoOverMaxSpeed();

	//重力をスピードの減少にどの程度反映するか調整
	int p = 6;

	//y方向の運動量の計算-----------------------------------------------------
	if (!_isGoDown)	//上向き
	{
		if (_pSpeed.y > _gSpeed)
		{
			_pSpeed.y += _gSpeed / p;
		}
		else
		{
			_pSpeed.y = _gSpeed;
			_isGoDown = true;
		}
	}
	else            //下向き
	{
		if (_pSpeed.y < _gSpeed)
		{
			_pSpeed.y -= _gSpeed / p;
		}

		else
		{
			_pSpeed.y = _gSpeed;
		}
	}
	//------------------------------------------------------------------------

	//x方向の運動量の計算-----------------------------------------------------
	if (_pSpeed.x != 0)
	{
		if (_isGoRight)
		{
			if (_pSpeed.x > _gSpeed)
			{
				_pSpeed.x += _gSpeed / p;
			}
			else
			{
				_pSpeed.x = 0;
				actionYuraYura();

			}
		}
		else
		{
			if (_pSpeed.x < _gSpeed)
			{
				_pSpeed.x -= _gSpeed / p;
			}
			else
			{
				_pSpeed.x = 0;
				actionYuraYura();
			}
		}
	}
	//------------------------------------------------------------------------
}

//rotateの計算
void Yoshidasan::rotateKeisan()
{
	//自分の角度をspeedのベクトルから計算して適用　度 = ラジアン × 180 ÷ 円周率
	setRotation((atan2(_pSpeed.y, _pSpeed.x) * 180.0f / M_PI) + 90);
	bool xBool = false;
	if (_isYanki) xBool = _isGoRight == _isGoDown;
	else xBool = _isGoRight != _isGoDown;
	setFlipX(xBool);
	setFlipY(!_isGoDown);
}

//speedを調べる
Vec2 Yoshidasan::getSpeed()
{
	return _pSpeed;
}

void Yoshidasan::setNoOverMaxSpeed()
{

	//y方向-------------------------------------------------------------------

	if (!_isGoDown &&_pSpeed.y >= _maxSpeed )//上向き
	{
		_pSpeed.y = _maxSpeed;
	}

	else if (_isGoDown &&_pSpeed.y <= -_maxSpeed)//下向き
	{
		_pSpeed.y = -_maxSpeed;
	}
	//------------------------------------------------------------------------

	//x方向-------------------------------------------------------------------

	if (_isGoRight && _pSpeed.x >= _maxSpeed)
	{
		_pSpeed.x = _maxSpeed;
	}
	else if (!_isGoRight && _pSpeed.x <= -_maxSpeed)
	{
		_pSpeed.x = -_maxSpeed;
	}
	//------------------------------------------------------------------------
}

void Yoshidasan::move()
{
	//移動量を加算してイドウ
	setPosition(Vec2(getPositionX() + _pSpeed.x, getPositionY() + _pSpeed.y));

	if (getPositionY() < designResolutionSize.height * 0.16f)
	{
		setPositionY(designResolutionSize.height * 0.16f);
		_isWind = false;
		allChangeReset();
	}
}

void Yoshidasan::hitAction(Vec2 movePos)
{
	if (!_isHit)
	{
		stopAllActions();
		_isHit = true;
		auto move = MoveTo::create(0.2f, movePos);
		auto func = CCCallFunc::create([=]()
		{
			_isHit = false;
			actionYuraYura();
		});
		auto seq = Sequence::create(move, func, nullptr);
		runAction(seq);
	}

}

//いつもの揺れ
void Yoshidasan::actionYuraYura()
{
	stopAllActions();
	setFlipY(false);
	//setFlipX(false);
	auto modosu = RotateTo::create(0.5f, 0);
	auto func = CallFunc::create([=]()
	{
		auto yuraR = RotateBy::create(0.8f, 30);
		auto easeYuraR = EaseInOut::create(yuraR, 2);
		auto yuraL = RotateBy::create(0.8f, -30);
		auto easeYuraL = EaseInOut::create(yuraL, 2);
		auto yura = Sequence::create(easeYuraR, easeYuraL, nullptr);
		auto yurayura = RepeatForever::create(yura);
		runAction(yurayura);
	});
	auto sequence = Sequence::create(modosu, func, nullptr);
	runAction(sequence);

}

//始めの飛び出し
void Yoshidasan::startGo(Vec2 moveState, float dilayTime)
{
	stopAllActions();
	auto  delay = DelayTime::create(dilayTime);
	auto move = MoveBy::create(0.3f, moveState);
	auto func = CallFunc::create([=]()
	{
		actionYuraYura();
		_isWind = false;
	});
	auto sequence = Sequence::create(delay, move, func, nullptr);
	runAction(sequence);

}

//変化を戻す
void Yoshidasan::allChangeReset()
{
	stopAllActions();
	setFlipY(false);
	//setFlipX(false);
	setRotation(0);
}

//ゴールした時の処理
void Yoshidasan::actionGoolMove(Vec2 centerPos, float dilayTime)
{
	if (!_isGool)
	{
		allChangeReset();
		_isGool = true;
		auto junp = JumpTo::create(dilayTime, centerPos, 100, 1);
		auto kaiten = RotateTo::create(dilayTime, 180.0f);
		auto spw = Spawn::create(junp, kaiten, nullptr);
		auto desu = CallFunc::create([=]()
		{
			YoshidasanNoManager *manager = (YoshidasanNoManager*)getParent();
			manager->yosidaLiveingCheck();
			removeFromParentAndCleanup(true);
		});
		auto seq = Sequence::create(spw, desu, nullptr);
		runAction(seq);
	}
}

void Yoshidasan::kazekaiten(float actontime)
{
	_isWind = true;
	stopAllActions();
	//rotateKeisan();
	log("%f", 1500 * actontime);
	auto kaiten = RotateBy::create(actontime, 500 * actontime);
	auto eas = EaseOut::create(kaiten, 3);
	auto modmod = RotateTo::create(0.1f, 0);
	auto func = CCCallFunc::create([=]()
	{
		_isWind = false;
		
	});
	auto seq = Sequence::create(eas, modmod, func, nullptr);
	runAction(seq);
}

//speedを反転
void Yoshidasan::speedFlip()
{
	_pSpeed = -_pSpeed;
	_isGoDown != _isGoDown;
	_isGoRight != _isGoRight;
}

