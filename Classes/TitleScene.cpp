#include "TitleScene.h"
#include "JankenScene.h"

USING_NS_CC;

Scene* Title::createScene()
{
	auto scene = Scene::create();
	auto layer = Title::create();
	scene->addChild(layer);
	return scene;
}

Title::Title()
{

}

Title::~Title()
{

}

bool Title::init()
{
	if (!Layer::init()){
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

	auto titleLabel = Label::createWithSystemFont(
		"Janken Game", "Marker Felt", 48);
	titleLabel->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
	titleLabel->enableOutline(Color4B::BLACK, 3.0);
	titleLabel->setPosition(Vec2(winSize.width / 2.0, winSize.height / 4.0 * 3.0));
	this->addChild(titleLabel);

	auto startLabel = Label::createWithSystemFont(
		"Touch to Start", "Marker Felt", 48);
	startLabel->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
	startLabel->enableOutline(Color4B::BLACK, 3.0);
	startLabel->setPosition(Vec2(winSize.width / 2.0, winSize.height / 4.0));
	auto blink = Sequence::create(FadeTo::create(0.5, 127), FadeTo::create(0.5, 255), NULL);
	startLabel->runAction(RepeatForever::create(blink));
	this->addChild(startLabel);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](Touch* touch, Event* event){
		this->getEventDispatcher()->removeAllEventListeners();
		auto scene = Janken::createScene();
		auto transition = TransitionFade::create(0.5, scene);
		Director::getInstance()->replaceScene(transition);
		return true;
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}