#ifndef __TITLE_SCENE_H__
#define __TITLE_SCENE_H__

#include "cocos2d.h"

class Title : public cocos2d::Layer
{
protected:
	Title();
	virtual ~Title();
	bool init() override;

public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(Title);
};

#endif // __TITLE_SCENE_H__
