#ifndef __RESULT_SCENE_H__
#define __RESULT_SCENE_H__

#include "cocos2d.h"

class Result : public cocos2d::Layer
{
protected:
	Result();
	virtual ~Result();
	bool initWithResult(int winCount, int loseCount, int drawCount);

public:
	static cocos2d::Scene* createWithResult(int winCount, int loseCount, int drawCount);
};

#endif // __RESULT_SCENE_H__
