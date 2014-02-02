/*
 * StandardPressActionComponent.cpp
 *
 *  Created on: Jan 30, 2014
 *      Author: Dawid Drozd
 */

#include "app/components/click/StandardPressActionComponent.h"
#include "app/components/Notifications.h"

namespace KoalaJump
{

StandardPressActionComponent::StandardPressActionComponent()
{

}

StandardPressActionComponent::~StandardPressActionComponent()
{
}

void StandardPressActionComponent::initComponent ( ComponentManager& manager )
{
	//Those notifications will send ClickComponent but we don't know any thing about it because
	//whole notifications are stored in Notifications.h
	Notifier& notifier = manager.getNodeNotifier();
	notifier.addNotification ( getNotificationOnPress(),
							   Utils::makeCallback ( this, &StandardPressActionComponent::onPress ) );
	notifier.addNotification ( getNotificationOnUnPress(),
							   Utils::makeCallback ( this, &StandardPressActionComponent::onUnPress ) );
}

void StandardPressActionComponent::onPress ( )
{
	resetView();

	const float time = 0.25F;
	CCAction* pAction =  CCEaseExponentialOut::create ( CCScaleTo::create ( time,
						 0.88 ) );
	pAction->setTag ( BUMP_ACTION );

	getWorkingNode()->runAction ( pAction );
}

void StandardPressActionComponent::onUnPress ( )
{
	resetView();
}

void StandardPressActionComponent::resetView()
{
	CCAction* pAction = getWorkingNode()->getActionByTag ( BUMP_ACTION );

	if ( pAction != nullptr )
	{
		pAction->update ( 1 );
		getWorkingNode()->stopAction ( pAction );
		pAction = nullptr;
	}

	pAction = getWorkingNode()->getActionByTag ( RESET_ACTION );

	if ( pAction != nullptr )
	{
		pAction->update ( 1 );
		getWorkingNode()->stopAction ( pAction );
	}

	const float time = 0.15F;
	pAction =  CCEaseExponentialIn::create ( CCScaleTo::create ( time, 1 ) );
	pAction->setTag ( RESET_ACTION );
	getWorkingNode()->runAction ( pAction );
}

} /* namespace KoalaJump */
