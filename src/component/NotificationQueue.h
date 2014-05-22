/*
 * NotificationQueue.h
 *
 *  Created on: May 2, 2014
 *      Author: dawid
 */

#ifndef NOTIFICATIONQUEUE_H_
#define NOTIFICATIONQUEUE_H_

#include "component/Notifier.h"
#include <vector>
#include <utility>

namespace KoalaComponent
{

struct QueuedNotification
{
	QueuedNotification( Notifier* pNotifier, Notification<> notification ):
		pNotifier( pNotifier )
		, notification( notification )
	{
	}

	QueuedNotification( QueuedNotification&& queueNotification ) :
		pNotifier( queueNotification.pNotifier )
		, notification( queueNotification.notification )
	{
	}

	QueuedNotification& operator= ( QueuedNotification && queueNotification )
	{
		pNotifier = queueNotification.pNotifier;
		notification = std::move( queueNotification.notification );
		return *this;
	}

	Notifier* pNotifier;
	Notification<> notification;
};

class NotificationQueue: public Utils::BaseClass
{
public:
	NotificationQueue();
	virtual ~NotificationQueue();

	/**
	 * @param pNotifier
	 * @param notification
	 * @param time default next frame
	 */
	void queueNotification( Notifier* pNotifier, Notification<> notification, float time = 1.F / 60.F );

private:
	int m_isScheduled = false;
	std::vector<float> m_times;//For performance (cache)
	std::vector<QueuedNotification> m_notifications;

	void onUpdate( float dt );
};

} /* namespace KoalaComponent */

#endif /* NOTIFICATIONQUEUE_H_ */
