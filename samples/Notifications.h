/*
 * Notifications.h
 *
 *  Created on: Feb 2, 2014
 *      Author: Dawid Drozd
 */

#ifndef COMPONENTS_NOTIFICATIONS_H_
#define COMPONENTS_NOTIFICATIONS_H_

#include "component/Notifier.h"

/**
 * @param bool* return value
 */
MAKE_NOTIFICATION ( OnTouchBegan, CCTouch*, bool* );
MAKE_NOTIFICATION ( OnTouchEnded, CCTouch* );
MAKE_NOTIFICATION ( OnTouchMoved, CCTouch* );
MAKE_NOTIFICATION(OnPress);
MAKE_NOTIFICATION(OnUnPress);


#endif /* COMPONENTS_NOTIFICATIONS_H_ */
