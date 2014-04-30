/*
 * TouchComponent.h
 *
 *  Created on: Dec 17, 2013
 *      Author: Dawid Drozd
 */

#ifndef TOUCHCOMPONENT_H_
#define TOUCHCOMPONENT_H_

#include "component/Component.h"
#include "utils/Utils.h"
using namespace KoalaComponent;
namespace KoalaJump
{

/**
 * We must inherit from CCTouchDelegate because we want receive touch information.
 */
class TouchComponent : public Component, public CCTouchDelegate
{
public:
	//KK_CREATE_METHOD is similar to CREATE_FUNC but we don't call init()
	KK_CREATE_METHOD( TouchComponent );
	virtual ~TouchComponent();

	/**
	 * Initialize your component
	 * @param manager manager that is owner of this Component
	 */
	virtual void initComponent( ComponentManager& manager ) override;

	virtual bool ccTouchBegan( CCTouch* pTouch, CCEvent* pEvent ) override;

	virtual void ccTouchMoved( CCTouch* pTouch, CCEvent* pEvent ) override;
	virtual void ccTouchEnded( CCTouch* pTouch, CCEvent* pEvent ) override;
	virtual void ccTouchCancelled( CCTouch* pTouch, CCEvent* pEvent ) override;

private:
	void onEnter();
	void onExit();

	TouchComponent();
};

} /* namespace KoalaJump */

#endif /* TOUCHCOMPONENT_H_ */
