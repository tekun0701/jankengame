#include "JankenScene.h"
#include "ResultScene.h"

USING_NS_CC;

// 弾の発射フレーム
const int BULLET_SHOOT_FRAME = 20;
// 敵の出現ペース
const int ENEMY_SPAWN_RATE = 20;
// 星の出現ペース
const int ENEMY_STAR_RATE = 20;
// じゃんけんの変化ペース
const int JANKEN_SPAWN_RATE = 200;
// じゃんけん勝利時の得点
const int JANKEN_WIN_POINT = 3;
// じゃんけん敗北時の得点
const int JANKEN_LOSE_POINT = -5;
// じゃんけんあいこ時の得点
const int JANKEN_DRAW_POINT = 1;
// 残り時間
const float TIME_LIMIT_SECOND = 60.0f;

Scene* Janken::createScene()
{
	auto scene = Scene::create();

	auto layer = Janken::create();

	scene->addChild(layer);

	return scene;
}

Janken::Janken():
_isShoot(false),
_isHit(false),
_shootFrame(0),
_winCount(0),
_drawCount(0),
_loseCount(0),
_second(TIME_LIMIT_SECOND),
_player(NULL),
_bullets(NULL),
_scoreLabel(NULL),
_secondLabel(NULL)
{
	srand((unsigned int)time(NULL));
};

Janken::~Janken(){
	CC_SAFE_RELEASE_NULL(_player);
}

bool Janken::init()
{
	if (!Layer::init())
	{
		return false;
	}


	// directorを取り出す
	auto director = Director::getInstance();
	// 画面サイズを取り出す
	auto winSize = director->getWinSize();

	// 背景をセット
	// http://bg-patterns.com/?p=321

	Texture2D *bgTexture = Director::getInstance()->getTextureCache()->addImage("sky.png");
	const Texture2D::TexParams tp = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
	Sprite *background = Sprite::createWithTexture(bgTexture, Rect(0, 0, winSize.width, winSize.height));
	background->getTexture()->setTexParameters(tp);
	background->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	this->addChild(background, -10);

	// じゃんけんの手
	// http://lmsnn.fc2web.com/material/janken.html

	// じゃんけんの種類を選択する
	int jankenType = rand() % static_cast<int>(JankenType::COUNT);

	// プレイヤーを置く
	this->setPlayer(Sprite::create(StringUtils::format("janken%d.png", jankenType)));
	_player->setTag(jankenType);
	_player->setPosition(Vec2(winSize.width / 2.0, winSize.height / 5.0));
	_player->setScale(0.3f);
	_player->setColor(Color3B::ORANGE);
	this->addChild(_player);

	// 時間のラベルを置く
	auto secondLabel = Label::createWithSystemFont(
		StringUtils::format("second:%4d", static_cast<int>(TIME_LIMIT_SECOND)), "Marker Felt", 16);
	
	this->setSecondLabel(secondLabel);
	secondLabel->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
	secondLabel->enableOutline(Color4B::RED, 3.0);
	secondLabel->setPosition(Vec2(winSize.width / 4.0, winSize.height - 20));
	this->addChild(secondLabel);

	// 得点のラベルを置く
	auto scoreLabel = Label::createWithSystemFont("score: 0", "Marker Felt", 16);
	this->setScoreLabel(scoreLabel);
	scoreLabel->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
	scoreLabel->enableOutline(Color4B::RED, 3.0);
	scoreLabel->setPosition(Vec2(winSize.width / 4.0 * 3.0 , winSize.height - 20));
	this->addChild(scoreLabel);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](Touch* touch, Event* event){
		// ヒットしているときは発射出来ない
		if (!_isHit){
			_isShoot = true;
		}
		return true;
	};
	listener->onTouchEnded = [this](Touch* touch, Event* event){
		_isShoot = false;
		_shootFrame = 0;
		return true;
	};
	listener->onTouchMoved = [this, winSize](Touch* touch, Event* event){
		Vec2 delta = touch->getDelta();
		Vec2 position = _player->getPosition();
		Vec2 newPosition = position + delta;
		if (newPosition.x < 0){
			newPosition.x = 0;
		}
		else if (newPosition.x > winSize.width){
			newPosition.x = winSize.width;
		}
		if (newPosition.y < 0){
			newPosition.y = 0;
		}
		else if (newPosition.y > winSize.height){
			newPosition.y = winSize.height;
		}
		_player->setPosition(newPosition);
	};
	director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	this->scheduleUpdate();
	return true;
}

void Janken::update(float dt)
{
	// ヒット時は弾打てない
	if (_isHit){
		_isShoot = false;
	}
	else{
		// ヒット時でないときのみ当たり判定
		// 当たり判定
		// TODO: 総当りはやさしくないのでもっと綺麗にしたい(弾と敵の距離が遠いものは判定しないとか)
		for (auto& enemy : _enemys){
			for (auto& bullet : _bullets){
				if (_enemys.contains(enemy) && _bullets.contains(bullet)){
					if (enemy->getBoundingBox().containsPoint(bullet->getPosition())){
						this->playJanken(bullet, enemy);
					}
				}
			}

			// 敵とプレイヤーの当たり判定
			if (_enemys.contains(enemy) && enemy->getBoundingBox().containsPoint(_player->getPosition())){
				// 星のとき
				if (enemy->getTag() == 3){
					this->addBulletBom(enemy);
				}
				else{
					_isHit = true;
					this->deletePlayer(enemy);
				}
			}
		}

		// 残り秒数を減らす
		_second -= dt;
		_secondLabel->setString(StringUtils::format("second:%4d", static_cast<int>(_second)));

		// 0になったら結果画面へ
		if (_second < 0){
			auto scene = Result::createWithResult(_winCount, _loseCount, _drawCount);
			auto transition = TransitionPageTurn::create(0.5, scene, true);
			Director::getInstance()->replaceScene(transition);
		}
	}

	// 弾の出現
	if (_isShoot == true){
		if (_shootFrame == 0){
			this->addBullet();
		}
		else if (_shootFrame == BULLET_SHOOT_FRAME - 1){
			_shootFrame = -1;
		}
		_shootFrame += 1;
	}

	// 敵の出現
	if (rand() % ENEMY_SPAWN_RATE == 0){
		this->addEnemy();
	}

	// じゃんけん手の変更 
	if (rand() % JANKEN_SPAWN_RATE == 0){
		this->changeJanken();
	}
}

cocos2d::Sprite* Janken::addBullet(){
	// directorを取り出す
	auto director = Director::getInstance();
	// 画面サイズを取り出す
	auto winSize = director->getWinSize();
	return this->addBulletBase(0, winSize.height, 2.0);
}

cocos2d::Sprite* Janken::addBulletBase(float width, float height, float sec){
	Vec2 position = _player->getPosition();
	auto bullet = Sprite::create(StringUtils::format("janken%d.png", _player->getTag()));
	// 弾の座標はプレイヤー位置から
	bullet->setPosition(Vec2(position.x, position.y));
	// 弾のタグを設定
	bullet->setTag(_player->getTag());
	this->addChild(bullet, -1);
	bullet->setScale(0.3f);
	bullet->setColor(Color3B::ORANGE);
	// _buttles 配列に弾を追加
	_bullets.pushBack(bullet);
	// 弾のアニメ作成
	auto sequence = Sequence::create(
		MoveTo::create(sec, Vec2(position.x + width, position.y + height)),
		CallFuncN::create([this](Node *node){
		auto sprite = dynamic_cast<Sprite *>(node);
		this->removeBullet(sprite);
	}),
		NULL
		);
	bullet->runAction(sequence);
	return bullet;
}

void Janken::addBulletBom(cocos2d::Sprite* enemy){
	// directorを取り出す
	auto director = Director::getInstance();
	// 画面サイズを取り出す
	auto winSize = director->getWinSize();
	this->addBulletBase(-winSize.width, -winSize.height, 1.0);
	this->addBulletBase(-winSize.width, 0, 1.0);
	this->addBulletBase(-winSize.width, winSize.height, 1.0);
	this->addBulletBase(0, -winSize.height, 1.0);
	this->addBulletBase(0, winSize.height, 1.0);
	this->addBulletBase(winSize.width, -winSize.height, 1.0);
	this->addBulletBase(winSize.width, 0, 1.0);
	this->addBulletBase(winSize.width, winSize.height, 1.0);
	this->removeEnemy(enemy);
}


bool Janken::removeBullet(cocos2d::Sprite* bullet){
	if (_bullets.contains(bullet)){
		bullet->removeFromParent();
		_bullets.eraseObject(bullet);
		return true;
	}
	return false;
}

cocos2d::Sprite* Janken::addEnemy(){
	// directorを取り出す
	auto director = Director::getInstance();
	// 画面サイズを取り出す
	auto winSize = director->getWinSize();

	// 星出現の場合
	if (rand() % static_cast<int>(ENEMY_STAR_RATE) == 0){
		auto enemy = Sprite::create("star.png");
		enemy->setTag(3);
		// 位置をランダムで
		int enemyXPos = rand() % static_cast<int>(winSize.width);
		auto enemySize = enemy->getContentSize();
		enemy->setPosition(Vec2(enemyXPos, winSize.height));
		enemy->setScale(0.3f);
		this->addChild(enemy);
		_enemys.pushBack(enemy);
		// 敵のアニメ作成(星は速い)
		auto sequence = Sequence::create(
			MoveTo::create(1.0, Vec2(enemyXPos, 0)),
			CallFuncN::create([this](Node *node){
			auto sprite = dynamic_cast<Sprite *>(node);
			this->removeEnemy(sprite);
		}),
			NULL
			);
		enemy->runAction(sequence);
		return enemy;
	}

	int jankenType = rand() % static_cast<int>(JankenType::COUNT);
	// 種類をランダムで作成
	auto enemy = Sprite::create(StringUtils::format(
		"janken%d.png", jankenType));
	enemy->setTag(jankenType);
	// 位置をランダムで
	int enemyXPos = rand() % static_cast<int>(winSize.width);
	auto enemySize = enemy->getContentSize();
	enemy->setPosition(Vec2(enemyXPos, winSize.height));
	enemy->setScale(0.3f);
	this->addChild(enemy);
	_enemys.pushBack(enemy);
	// 敵のアニメ作成
	auto sequence = Sequence::create(
		MoveTo::create(2, Vec2(enemyXPos, 0)),
		CallFuncN::create([this](Node *node){
		auto sprite = dynamic_cast<Sprite *>(node);
		this->removeEnemy(sprite);
	}),
		NULL
		);
	enemy->runAction(sequence);
	return enemy;
}

bool Janken::removeEnemy(cocos2d::Sprite* enemy){
	if (_enemys.contains(enemy)){
		enemy->removeFromParent();
		_enemys.eraseObject(enemy);
		return true;
	}
	return false;
}

void Janken::playJanken(cocos2d::Sprite* bullet, cocos2d::Sprite* enemy){
	// 星のときは何もおきない
	if (enemy->getTag() == 3){
		return;
	}
	// じゃんけん判定
	int jankenResult = (bullet->getTag() - enemy->getTag() + 3) % 3;
	if (jankenResult == 0){
		_drawCount += 1;
	}
	else if (jankenResult == 1){
		_loseCount += 1;
	}
	else if (jankenResult == 2){
		_winCount += 1;
	}
	int score = _winCount * JANKEN_WIN_POINT + _loseCount * JANKEN_LOSE_POINT + _drawCount * JANKEN_DRAW_POINT;
	_scoreLabel->setString(StringUtils::format("score:%4d", score));

	// 敵を削除
	this->removeEnemy(enemy);
	// 弾を削除
	this->removeBullet(bullet);

}

void Janken::deletePlayer(cocos2d::Sprite* enemy){
	// じゃんけん判定
	int jankenResult = (_player->getTag() - enemy->getTag() + 3) % 3;
	if (jankenResult == 0){
		_drawCount += 1;
	}
	else if (jankenResult == 1){
		_loseCount += 1;
	}
	else if (jankenResult == 2){
		_winCount += 1;
	}
	int score = _winCount * JANKEN_WIN_POINT + _loseCount * JANKEN_LOSE_POINT + _drawCount * JANKEN_DRAW_POINT;
	_scoreLabel->setString(StringUtils::format("score:%4d", score));

	// 敵を削除
	this->removeEnemy(enemy);
	// プレイヤーを削除(といいたいけどここは非表示で)
	// アニメーションを生成
	_player->setVisible(false);
	
	this->runAction(Sequence::create(
		CallFunc::create([this]{
			auto particle = ParticleExplosion::create();
			particle->setPosition(_player->getPosition());
			this->addChild(particle);
	}), DelayTime::create(2.0),
		CallFunc::create([this]{
		auto scene = Result::createWithResult(_winCount, _loseCount, _drawCount);
		auto transition = TransitionPageTurn::create(0.5, scene, true);
		Director::getInstance()->replaceScene(transition);
	}), NULL
		));
}

void Janken::changeJanken(){
	// じゃんけんの種類を選択する
	int jankenType = rand() % static_cast<int>(JankenType::COUNT);

	// プレイヤーを置く
	_player->setTexture(StringUtils::format("janken%d.png", jankenType));
	_player->setTag(jankenType);
	_player->setScale(0.3f);
	this->addChild(_player);
}