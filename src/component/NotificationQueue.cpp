/*
 * NotificationQueue.cpp
 *
 *  Created on: May 2, 2014
 *      Author: dawid
 */

#include "component/NotificationQueue.h"
USING_NS_CC;

namespace KoalaComponent
{

NotificationQueue::NotificationQueue()
{
}

NotificationQueue::~NotificationQueue()
{
}


void NotificationQueue::queueNotification( Notifier* pNotifier, Notification<const std::string&> notification )
{
	queueNotification(pNotifier,notification,"",1.F/60.F);
}

void NotificationQueue::queueNotification( Notifier* pNotifier, Notification<const std::string&> notification ,
		float time )
{
	queueNotification(pNotifier,notification,"",time);
}

void NotificationQueue::queueNotification( Notifier* pNotifier, Notification<const std::string&> notification ,
		const std::string& data,float time )
{
	m_notifications.emplace_back( pNotifier, notification,data );
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
			element.pNotifier->notify( element.notification,element.data );

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
