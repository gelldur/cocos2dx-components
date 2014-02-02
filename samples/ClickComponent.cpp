/*
 * ClickComponent.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: Dawid Drozd
 */

#include "app/components/click/ClickComponent.h"
#include "utils/TouchManager.h"
#include "app/components/Notifications.h"

namespace KoalaJump
{

ClickComponent::ClickComponent ( const Callback<void ( CCNode* ) >& callback,
								 const CCSize& touchMargin ) :
	m_marginForTouch ( touchMargin )
	, m_callback ( callback )
{
}

ClickComponent::~ClickComponent()
{
}

void ClickComponent::initComponent ( ComponentManager& manager )
{

	Notifier& notifier = manager.getNodeNotifier();

	//Add notification that we want receive notification that our Node is touched
	notifier.addNotification ( getNotificationOnTouchBegan(),
							   Utils::makeCallback ( this, &ClickComponent::onTouchBegan ) );

	//Add notification that we want receive notification that we stop touch
	notifier.addNotification ( getNotificationOnTouchEnded(),
							   Utils::makeCallback ( this, &ClickComponent::onTouchEnded ) );
}

void ClickComponent::onTouchBegan ( CCTouch* pTouch , bool* pReturnValue )
{
	//In pReturnValue we store what we should return for CCTouchDelegate
	if ( getWorkingNode()->isVisible() == false ||
			KoalaLib::TouchManager::detectTouchOnNode ( getWorkingNode(), pTouch ,
					m_marginForTouch ) == false )
	{
		*pReturnValue = false;
	}
	else if ( *pReturnValue == true )
	{
		getOwner().getNodeNotifier().notify ( getNotificationOnPress() );
	}
}

void ClickComponent::onTouchEnded ( CCTouch* pTouch )
{
	if ( getWorkingNode()->isVisible() && m_callback.isCallable() &&
			KoalaLib::TouchManager::detectTouchOnNode ( getWorkingNode(), pTouch ,
					m_marginForTouch ) )
	{
		m_callback.call ( getWorkingNode() );
	}

	getOwner().getNodeNotifier().notify ( getNotificationOnUnPress() );
}

void ClickComponent::setMarginForClick ( const CCSize& margin )
{
	m_marginForTouch = margin;
}

void ClickComponent::setOnClickCallback ( Callback<void ( CCNode* ) >
		callback )
{
	m_callback = callback;
}

} /* namespace KoalaJump */
