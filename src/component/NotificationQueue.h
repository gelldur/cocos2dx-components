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
#include <string>

namespace KoalaComponent
{

struct QueuedNotification
{
	QueuedNotification( Notifier* pNotifier, Notification<const std::string&> notification,const std::string& data ):
		pNotifier( pNotifier )
		, notification( notification )
		,data(data)
	{
	}

	QueuedNotification( QueuedNotification&& queueNotification ) :
		pNotifier( queueNotification.pNotifier )
		, notification( queueNotification.notification )
		,data(queueNotification.data)
	{
	}

	QueuedNotification& operator= ( QueuedNotification && queueNotification )
	{
		pNotifier = queueNotification.pNotifier;
		notification = std::move( queueNotification.notification );
		data = queueNotification.data;
		return *this;
	}

	Notifier* pNotifier;
	Notification<const std::string&> notification;
	std::string data;
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
	void queueNotification( Notifier* pNotifier, Notification<const std::string&> notification );
	void queueNotification( Notifier* pNotifier, Notification<const std::string&> notification, float time );
	void queueNotification( Notifier* pNotifier, Notification<const std::string&> notification,const std::string& data, float time);

private:
	int m_isScheduled = false;
	std::vector<float> m_times;//For performance (cache)
	std::vector<QueuedNotification> m_notifications;

	void onUpdate( float dt );
};

} /* namespace KoalaComponent */

#endif /* NOTIFICATIONQUEUE_H_ */
