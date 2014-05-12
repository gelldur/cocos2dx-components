/*
 * NotificationQueue.cpp
 *
 *  Created on: May 2, 2014
 *      Author: dawid
 */

#include "component/NotificationQueue.h"

namespace KoalaComponent
{

NotificationQueue::NotificationQueue()
{
}

NotificationQueue::~NotificationQueue()
{
}

void NotificationQueue::queueNotification( Notifier* pNotifier, Notification<> notification ,
		float time )
{
	m_notifications.emplace_back( pNotifier, notification );
	m_times.emplace_back( time );

	if( m_isScheduled == false )
	{
		m_isScheduled = true;
		CCDirector::sharedDirector()->getScheduler()->scheduleSelector( schedule_selector(
					NotificationQueue::onUpdate ), this, 0, false );
	}
}

void NotificationQueue::onUpdate( float dt )
{
	assert( m_isScheduled );

	for( int i = static_cast<int>( m_times.size() ) - 1; i > -1; --i )
	{
		if( ( m_times[i] -= dt ) < FLT_EPSILON )
		{
			std::swap( m_times[i], m_times.back() );
			m_times.pop_back();

			auto&& element = m_notifications[i];
			element.pNotifier->notify( element.notification );

			std::swap( m_notifications[i], m_notifications.back() );
			m_notifications.pop_back();

			assert( m_times.size() == m_notifications.size() );

			if( m_times.empty() )
			{
				CCDirector::sharedDirector()->getScheduler()->unscheduleAllForTarget( this );
				m_isScheduled = false;
			}
		}
	}
}

} /* namespace KoalaComponent */
