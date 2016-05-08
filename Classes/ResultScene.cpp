#include "ResultScene.h"
#include "TitleScene.h"

USING_NS_CC;
// じゃんけん勝利時の得点
const int JANKEN_WIN_POINT = 3;
// じゃんけん敗北時の得点
const int JANKEN_LOSE_POINT = -5;
// じゃんけんあいこ時の得点
const int JANKEN_DRAW_POINT = 1;

Scene* Result::createWithResult(int winCount, int loseCount, int drawCount){
	auto scene = Scene::create();
	auto layer = new Result();
	if (layer && layer->initWithResult(winCount, loseCount, drawCount)){
		layer->autorelease();
	}
	else{
		CC_SAFE_DELETE(layer);
	}
	scene->addChild(layer);
	return scene;
}

Result::Result(){

}

Result::~Result(){

}

bool Result::initWithResult(int winCount, int loseCount, int drawCount)
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

	int score = winCount * JANKEN_WIN_POINT + loseCount * JANKEN_LOSE_POINT + drawCount * JANKEN_DRAW_POINT;
	auto winLabel = Label::createWithSystemFont(
		StringUtils::format("win:%d", winCount), "Marker Felt", 48);
	winLabel->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
	winLabel->enableOutline(Color4B::RED, 3.0);
	winLabel->setVisible(false);
	winLabel->setPosition(Vec2(winSize.width / 2.0, winSize.height / 2.0 + 96));
	this->addChild(winLabel);
	auto loseLabel = Label::createWithSystemFont(
		StringUtils::format("lose:%d", loseCount), "Marker Felt", 48);
	loseLabel->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
	loseLabel->enableOutline(Color4B::RED, 3.0);
	loseLabel->setVisible(false);
	loseLabel->setPosition(Vec2(winSize.width / 2.0, winSize.height / 2.0 + 48));
	this->addChild(loseLabel);
	auto drawLabel = Label::createWithSystemFont(
		StringUtils::format("draw:%d", drawCount), "Marker Felt", 48);
	drawLabel->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
	drawLabel->enableOutline(Color4B::RED, 3.0);
	drawLabel->setVisible(false);
	drawLabel->setPosition(Vec2(winSize.width / 2.0, winSize.height / 2.0));
	this->addChild(drawLabel);
	auto resultLabel = Label::createWithSystemFont(
		StringUtils::format("result:%d", score), "Marker Felt", 48);
	resultLabel->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
	resultLabel->enableOutline(Color4B::RED, 3.0);
	resultLabel->setVisible(false);
	resultLabel->setPosition(Vec2(winSize.width / 2.0, winSize.height / 2.0 - 48));
	this->addChild(resultLabel);

	auto labelBtnLabel = LabelTTF::create("Go To Title", "Marker Felt", 48);
	auto labelItem1 = MenuItemLabel::create(labelBtnLabel, [](Ref* ref){
		auto scene = Title::createScene();
		Director::getInstance()->replaceScene(scene);
	});
	auto menu = Menu::create(labelItem1, NULL);
	menu->setColor(Color3B::RED);
	menu->setVisible(false);
	menu->setPosition(Vec2(winSize.width / 2.0, winSize.height / 2.0 - 96));
	this->addChild(menu);

	this->runAction(Sequence::create(
		DelayTime::create(0.5),
		CallFunc::create([winLabel]{ winLabel->setVisible(true); }), 
		DelayTime::create(0.5),
		CallFunc::create([loseLabel]{ loseLabel->setVisible(true); }),
		DelayTime::create(0.5),
		CallFunc::create([drawLabel]{ drawLabel->setVisible(true); }),
		DelayTime::create(0.5),
		CallFunc::create([resultLabel]{ resultLabel->setVisible(true); }),
		DelayTime::create(0.5),
		CallFunc::create([menu]{ menu->setVisible(true); }),
		NULL
		));

	return true;

}