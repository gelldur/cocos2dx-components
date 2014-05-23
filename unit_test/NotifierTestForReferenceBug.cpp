/*
 * NotifierTest.cpp
 *
 *  Created on: 03.02.2014
 *      Author: Dawid Drozd
 */

#include "gtest/gtest.h"
#include "component/Notifier.h"

/**
 * Test case. Because now (23.05.2014) we have reference to vector and during some notifications
 * vector can reallocate so we have reference to bad address
 *
 * Notifier line 102
 * https://github.com/gelldur/cocos2dx-components/blob/master/src/component/Notifier.h#L102
 *
 * const auto& localVector = m_callbacks[notification.tag];
 *
 * here we pass through this vector and one of notifications call applyChanges and in line 236
 * https://github.com/gelldur/cocos2dx-components/blob/master/src/component/Notifier.h#L236
 *
 * we add another element
 * m_callbacks[elementChanges.tag].emplace_back( elementChanges );
 *
 * so vector reallocates and we have bad reference.
 *
 * below we have test case for this behavior
 *
 */

using namespace KoalaComponent;
USING_NS_CC;

MAKE_NOTIFICATION( Test1 );
MAKE_NOTIFICATION( Test2 );

struct TestClazzRef : Utils::BaseClass
{
	TestClazzRef() {}

	void onDeliverNotification()
	{
		pNotifier->addNotification(getNotificationTest1(),{this,&TestClazzRef::onDeliverNotification3});
		pNotifier->addNotification(getNotificationTest1(),{this,&TestClazzRef::onDeliverNotification4});
		pNotifier->notify(getNotificationTest2());

		CCLog("%s",__FUNCTION__);
	}

	void onDeliverNotification2()
	{
		//Calling this should cause error in current version
		CCLog("%s",__FUNCTION__);
	}
	void onDeliverNotification3()
	{
		CCLog("%s",__FUNCTION__);
	}
	void onDeliverNotification4()
	{
		CCLog("%s",__FUNCTION__);
	}

	Notifier* pNotifier = nullptr;
};

TEST( Notifier, DISABLED_TestOfNotificationBugWithReference )
{
	Notifier notifier;
	TestClazzRef testClazz;
	testClazz.pNotifier = &notifier;

	//It should crash
	notifier.addNotification(getNotificationTest1(),{&testClazz,&TestClazzRef::onDeliverNotification});

	notifier.notify(getNotificationTest1());
	notifier.notify(getNotificationTest2());
}

