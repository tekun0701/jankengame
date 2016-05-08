#ifndef __JANKEN_SCENE_H__
#define __JANKEN_SCENE_H__

#include "cocos2d.h"

class Janken : public cocos2d::Layer
{
protected:
	Janken();
	virtual ~Janken();
	bool init() override;

public:
	static cocos2d::Scene* createScene();
	void update(float dt);
	CREATE_FUNC(Janken);
	// �e�ł��Ă����Ԃ���ۑ�
	CC_SYNTHESIZE(bool, _isShoot, IsShoot);

	// �e�̐i�s�󋵂�ۑ� ������Ԃ�0�Ńt���[�����Ƃ�+1���Ă���
	// 20�܂Ői�s������0�ɖ߂�
	// int��0�̂Ƃ��ɒe�𔭎˂���@
	CC_SYNTHESIZE(int, _shootFrame, ShootFrame);

	// ���������Ǘ�
	CC_SYNTHESIZE(int, _winCount, WinCount);
	// �������������Ǘ�
	CC_SYNTHESIZE(int, _drawCount, DrawCount);
	// ���������Ǘ�
	CC_SYNTHESIZE(int, _loseCount, LoseCount);
	// �ڐG��Ԃ����Ǘ�
	CC_SYNTHESIZE(bool, _isHit, IsHit);
	// ���Ԃ��Ǘ�
	CC_SYNTHESIZE(float, _second, Second);

	// �v���C���[��Ԃ��擾
	CC_SYNTHESIZE_RETAIN(cocos2d::Sprite *, _player, Player);
	// �e�̊Ǘ�
	CC_SYNTHESIZE(cocos2d::Vector<cocos2d::Sprite *>, _bullets, Bullets);
	
	// �G�̊Ǘ�
	CC_SYNTHESIZE(cocos2d::Vector<cocos2d::Sprite *>, _enemys, Enemys);

	// ���_�̃��x��
	CC_SYNTHESIZE(cocos2d::Label *, _scoreLabel, ScoreLabel);

	// ���Ԃ̃��x��
	CC_SYNTHESIZE(cocos2d::Label *, _secondLabel, SecondLabel);
	// �e�o��
	cocos2d::Sprite* addBullet();
	cocos2d::Sprite* addBulletBase(float width, float height, float sec);
	// �{���o��
	void addBulletBom(cocos2d::Sprite* enemy);

	// �e�폜
	bool removeBullet(cocos2d::Sprite* bullet);

	// �G�o��
	cocos2d::Sprite* addEnemy();
	// �G�폜
	bool removeEnemy(cocos2d::Sprite* enemy);
	// ����񂯂���v���C
	void playJanken(cocos2d::Sprite* bullet, cocos2d::Sprite* enemy);
	// �v���C���[���폜
	void deletePlayer(cocos2d::Sprite* enemy);
	// �����_���𐶐�
	float generateRandom(int min, int max);
	// ����񂯂��̕ύX
	void changeJanken();

private:
	// ����񂯂�̎�
	enum class JankenType
	{
		GU,
		CHOKI,
		PA,
		COUNT
	};
};

#endif // __JANKEN_SCENE_H__
