#include "JankenScene.h"
#include "ResultScene.h"

USING_NS_CC;

// �e�̔��˃t���[��
const int BULLET_SHOOT_FRAME = 20;
// �G�̏o���y�[�X
const int ENEMY_SPAWN_RATE = 20;
// ���̏o���y�[�X
const int ENEMY_STAR_RATE = 20;
// ����񂯂�̕ω��y�[�X
const int JANKEN_SPAWN_RATE = 200;
// ����񂯂񏟗����̓��_
const int JANKEN_WIN_POINT = 3;
// ����񂯂�s�k���̓��_
const int JANKEN_LOSE_POINT = -5;
// ����񂯂񂠂������̓��_
const int JANKEN_DRAW_POINT = 1;
// �c�莞��
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


	// director�����o��
	auto director = Director::getInstance();
	// ��ʃT�C�Y�����o��
	auto winSize = director->getWinSize();

	// �w�i���Z�b�g
	// http://bg-patterns.com/?p=321

	Texture2D *bgTexture = Director::getInstance()->getTextureCache()->addImage("sky.png");
	const Texture2D::TexParams tp = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
	Sprite *background = Sprite::createWithTexture(bgTexture, Rect(0, 0, winSize.width, winSize.height));
	background->getTexture()->setTexParameters(tp);
	background->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	this->addChild(background, -10);

	// ����񂯂�̎�
	// http://lmsnn.fc2web.com/material/janken.html

	// ����񂯂�̎�ނ�I������
	int jankenType = rand() % static_cast<int>(JankenType::COUNT);

	// �v���C���[��u��
	this->setPlayer(Sprite::create(StringUtils::format("janken%d.png", jankenType)));
	_player->setTag(jankenType);
	_player->setPosition(Vec2(winSize.width / 2.0, winSize.height / 5.0));
	_player->setScale(0.3f);
	_player->setColor(Color3B::ORANGE);
	this->addChild(_player);

	// ���Ԃ̃��x����u��
	auto secondLabel = Label::createWithSystemFont(
		StringUtils::format("second:%4d", static_cast<int>(TIME_LIMIT_SECOND)), "Marker Felt", 16);
	
	this->setSecondLabel(secondLabel);
	secondLabel->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
	secondLabel->enableOutline(Color4B::RED, 3.0);
	secondLabel->setPosition(Vec2(winSize.width / 4.0, winSize.height - 20));
	this->addChild(secondLabel);

	// ���_�̃��x����u��
	auto scoreLabel = Label::createWithSystemFont("score: 0", "Marker Felt", 16);
	this->setScoreLabel(scoreLabel);
	scoreLabel->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
	scoreLabel->enableOutline(Color4B::RED, 3.0);
	scoreLabel->setPosition(Vec2(winSize.width / 4.0 * 3.0 , winSize.height - 20));
	this->addChild(scoreLabel);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](Touch* touch, Event* event){
		// �q�b�g���Ă���Ƃ��͔��ˏo���Ȃ�
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
	// �q�b�g���͒e�łĂȂ�
	if (_isHit){
		_isShoot = false;
	}
	else{
		// �q�b�g���łȂ��Ƃ��̂ݓ����蔻��
		// �����蔻��
		// TODO: ������͂₳�����Ȃ��̂ł������Y��ɂ�����(�e�ƓG�̋������������͔̂��肵�Ȃ��Ƃ�)
		for (auto& enemy : _enemys){
			for (auto& bullet : _bullets){
				if (_enemys.contains(enemy) && _bullets.contains(bullet)){
					if (enemy->getBoundingBox().containsPoint(bullet->getPosition())){
						this->playJanken(bullet, enemy);
					}
				}
			}

			// �G�ƃv���C���[�̓����蔻��
			if (_enemys.contains(enemy) && enemy->getBoundingBox().containsPoint(_player->getPosition())){
				// ���̂Ƃ�
				if (enemy->getTag() == 3){
					this->addBulletBom(enemy);
				}
				else{
					_isHit = true;
					this->deletePlayer(enemy);
				}
			}
		}

		// �c��b�������炷
		_second -= dt;
		_secondLabel->setString(StringUtils::format("second:%4d", static_cast<int>(_second)));

		// 0�ɂȂ����猋�ʉ�ʂ�
		if (_second < 0){
			auto scene = Result::createWithResult(_winCount, _loseCount, _drawCount);
			auto transition = TransitionPageTurn::create(0.5, scene, true);
			Director::getInstance()->replaceScene(transition);
		}
	}

	// �e�̏o��
	if (_isShoot == true){
		if (_shootFrame == 0){
			this->addBullet();
		}
		else if (_shootFrame == BULLET_SHOOT_FRAME - 1){
			_shootFrame = -1;
		}
		_shootFrame += 1;
	}

	// �G�̏o��
	if (rand() % ENEMY_SPAWN_RATE == 0){
		this->addEnemy();
	}

	// ����񂯂��̕ύX 
	if (rand() % JANKEN_SPAWN_RATE == 0){
		this->changeJanken();
	}
}

cocos2d::Sprite* Janken::addBullet(){
	// director�����o��
	auto director = Director::getInstance();
	// ��ʃT�C�Y�����o��
	auto winSize = director->getWinSize();
	return this->addBulletBase(0, winSize.height, 2.0);
}

cocos2d::Sprite* Janken::addBulletBase(float width, float height, float sec){
	Vec2 position = _player->getPosition();
	auto bullet = Sprite::create(StringUtils::format("janken%d.png", _player->getTag()));
	// �e�̍��W�̓v���C���[�ʒu����
	bullet->setPosition(Vec2(position.x, position.y));
	// �e�̃^�O��ݒ�
	bullet->setTag(_player->getTag());
	this->addChild(bullet, -1);
	bullet->setScale(0.3f);
	bullet->setColor(Color3B::ORANGE);
	// _buttles �z��ɒe��ǉ�
	_bullets.pushBack(bullet);
	// �e�̃A�j���쐬
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
	// director�����o��
	auto director = Director::getInstance();
	// ��ʃT�C�Y�����o��
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
	// director�����o��
	auto director = Director::getInstance();
	// ��ʃT�C�Y�����o��
	auto winSize = director->getWinSize();

	// ���o���̏ꍇ
	if (rand() % static_cast<int>(ENEMY_STAR_RATE) == 0){
		auto enemy = Sprite::create("star.png");
		enemy->setTag(3);
		// �ʒu�������_����
		int enemyXPos = rand() % static_cast<int>(winSize.width);
		auto enemySize = enemy->getContentSize();
		enemy->setPosition(Vec2(enemyXPos, winSize.height));
		enemy->setScale(0.3f);
		this->addChild(enemy);
		_enemys.pushBack(enemy);
		// �G�̃A�j���쐬(���͑���)
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
	// ��ނ������_���ō쐬
	auto enemy = Sprite::create(StringUtils::format(
		"janken%d.png", jankenType));
	enemy->setTag(jankenType);
	// �ʒu�������_����
	int enemyXPos = rand() % static_cast<int>(winSize.width);
	auto enemySize = enemy->getContentSize();
	enemy->setPosition(Vec2(enemyXPos, winSize.height));
	enemy->setScale(0.3f);
	this->addChild(enemy);
	_enemys.pushBack(enemy);
	// �G�̃A�j���쐬
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
	// ���̂Ƃ��͉��������Ȃ�
	if (enemy->getTag() == 3){
		return;
	}
	// ����񂯂񔻒�
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

	// �G���폜
	this->removeEnemy(enemy);
	// �e���폜
	this->removeBullet(bullet);

}

void Janken::deletePlayer(cocos2d::Sprite* enemy){
	// ����񂯂񔻒�
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

	// �G���폜
	this->removeEnemy(enemy);
	// �v���C���[���폜(�Ƃ����������ǂ����͔�\����)
	// �A�j���[�V�����𐶐�
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
	// ����񂯂�̎�ނ�I������
	int jankenType = rand() % static_cast<int>(JankenType::COUNT);

	// �v���C���[��u��
	_player->setTexture(StringUtils::format("janken%d.png", jankenType));
	_player->setTag(jankenType);
	_player->setScale(0.3f);
	this->addChild(_player);
}