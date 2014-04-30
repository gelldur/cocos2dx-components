/*
 * TouchComponent.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: Dawid Drozd
 */

#include "app/components/TouchComponent.h"
#include "app/components/Notifications.h"
#include "component/ComponentNode.h"

namespace KoalaJump
{

TouchComponent::TouchComponent()
{
}

TouchComponent::~TouchComponent()
{
}

void TouchComponent::initComponent( ComponentManager& manager )
{
	//We register notifications for onEnter and onExit
	Notifier& notifier = manager.getNodeNotifier();
	notifier.addNotification( getNotificationOnEnter(),
							  Utils::makeCallback( this, &TouchComponent::onEnter ) );
	notifier.addNotification( getNotificationOnExit(),
							  Utils::makeCallback( this, &TouchComponent::onExit ) );
}

void TouchComponent::onEnter()
{
	//Register for touch information from cocos2dx
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate( this,
			INT_MIN, true );
}

void TouchComponent::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate( this );
}

bool TouchComponent::ccTouchBegan( CCTouch* pTouch, CCEvent* pEvent )
{
	bool returnValue = true;
	getOwner().getNodeNotifier().notify( getNotificationOnTouchBegan(), pTouch,
										 &returnValue );
	//If every listener says that we can consume click information we will return it here.
	return returnValue;
}

void TouchComponent::ccTouchMoved( CCTouch* pTouch, CCEvent* pEvent )
{
	getOwner().getNodeNotifier().notify( getNotificationOnTouchMoved(), pTouch );
}

void TouchComponent::ccTouchEnded( CCTouch* pTouch, CCEvent* pEvent )
{
	getOwner().getNodeNotifier().notify( getNotificationOnTouchEnded(), pTouch );
}

void TouchComponent::ccTouchCancelled( CCTouch* pTouch, CCEvent* pEvent )
{
	//we don't use it
}

} /* namespace KoalaJump */

