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
	// 弾打っている状態かを保存
	CC_SYNTHESIZE(bool, _isShoot, IsShoot);

	// 弾の進行状況を保存 初期状態は0でフレームごとに+1していく
	// 20まで進行したら0に戻す
	// intが0のときに弾を発射する　
	CC_SYNTHESIZE(int, _shootFrame, ShootFrame);

	// 勝利数を管理
	CC_SYNTHESIZE(int, _winCount, WinCount);
	// 引き分け数を管理
	CC_SYNTHESIZE(int, _drawCount, DrawCount);
	// 負け数を管理
	CC_SYNTHESIZE(int, _loseCount, LoseCount);
	// 接触状態かを管理
	CC_SYNTHESIZE(bool, _isHit, IsHit);
	// 時間を管理
	CC_SYNTHESIZE(float, _second, Second);

	// プレイヤー状態を取得
	CC_SYNTHESIZE_RETAIN(cocos2d::Sprite *, _player, Player);
	// 弾の管理
	CC_SYNTHESIZE(cocos2d::Vector<cocos2d::Sprite *>, _bullets, Bullets);
	
	// 敵の管理
	CC_SYNTHESIZE(cocos2d::Vector<cocos2d::Sprite *>, _enemys, Enemys);

	// 得点のラベル
	CC_SYNTHESIZE(cocos2d::Label *, _scoreLabel, ScoreLabel);

	// 時間のラベル
	CC_SYNTHESIZE(cocos2d::Label *, _secondLabel, SecondLabel);
	// 弾出現
	cocos2d::Sprite* addBullet();
	cocos2d::Sprite* addBulletBase(float width, float height, float sec);
	// ボム出現
	void addBulletBom(cocos2d::Sprite* enemy);

	// 弾削除
	bool removeBullet(cocos2d::Sprite* bullet);

	// 敵出現
	cocos2d::Sprite* addEnemy();
	// 敵削除
	bool removeEnemy(cocos2d::Sprite* enemy);
	// じゃんけんをプレイ
	void playJanken(cocos2d::Sprite* bullet, cocos2d::Sprite* enemy);
	// プレイヤーを削除
	void deletePlayer(cocos2d::Sprite* enemy);
	// ランダムを生成
	float generateRandom(int min, int max);
	// じゃんけん手の変更
	void changeJanken();

private:
	// じゃんけんの手
	enum class JankenType
	{
		GU,
		CHOKI,
		PA,
		COUNT
	};
};

#endif // __JANKEN_SCENE_H__
