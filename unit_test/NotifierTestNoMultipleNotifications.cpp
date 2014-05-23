/*
 * NotifierTest.cpp
 *
 *  Created on: 03.02.2014
 *      Author: Dawid Drozd
 */

#include "gtest/gtest.h"
#include "component/Notifier.h"

using namespace KoalaComponent;
USING_NS_CC;

MAKE_NOTIFICATION( Test1 );
MAKE_NOTIFICATION( Test2 );

struct TestClazz : Utils::BaseClass
{
	TestClazz() {}

	int deliveredCount = 0;

	void onOldNotification( Utils::BaseClass* pObject )
	{
		++deliveredCount;
	}

	void onOldNotificationFloat( Utils::BaseClass* pObject )
	{
		//We use static cast for better performance
		CCFloat* pFloat = static_cast<CCFloat*>( pObject );
		deliveredCount += pFloat->getValue();
	}

	void onOldNotificationInt( Utils::BaseClass* pObject )
	{
		//We use static cast for better performance
		CCInteger* pInteger = static_cast<CCInteger*>( pObject );
		deliveredCount += pInteger->getValue();
	}

	void onDeliverNotification()
	{
		++deliveredCount;
	}

	void onDeliverNotificationVerB()
	{
		++deliveredCount;
	}

	int onDeliverNotificationVerC(int a,int b)
	{
		return ++deliveredCount;
	}

	void onDeliverNotification2( float value )
	{
		deliveredCount += value;
	}

	void onDeliverNotification3( int value )
	{
		deliveredCount += value;
	}
};

TEST( Notifier, TestOfNotificationNoMultipleNotificationsForSingleObjectCase2 )
{
	Notifier notifier;
	TestClazz testClazz;

	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.removeNotification( &testClazz, getNotificationTest1() );
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
}

TEST( Notifier, TestOfNotificationNoMultipleNotificationsForSingleObjectCase12 )
{
	Notifier notifier;
	TestClazz testClazz;

	//If we have conflict we must specify third argument
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotificationVerB } );
}

TEST( Notifier, TestOfNotificationNoMultipleNotificationsForSingleObjectCase3 )
{
	Notifier notifier;
	TestClazz testClazz;

	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.addNotification( getNotificationTest2(), { &testClazz, &TestClazz::onDeliverNotification } );
}

TEST( Notifier, TestOfNotificationNoMultipleNotificationsForSingleObjectCase5 )
{
	Notifier notifier;
	TestClazz testClazz;

	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.removeNotification( &testClazz, getNotificationTest1() );
	notifier.removeNotification( &testClazz, getNotificationTest1() );
	notifier.removeNotification( &testClazz, getNotificationTest1() );
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.removeNotification( &testClazz, getNotificationTest1() );
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
}

TEST( Notifier, TestOfNotificationNoMultipleNotificationsForSingleObjectCase6 )
{
	Notifier notifier;
	TestClazz testClazz;

	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.removeAllForObject( &testClazz );
	notifier.removeNotification( &testClazz, getNotificationTest1() );
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.removeAllForObject( &testClazz );
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
}

TEST( Notifier, TestOfNotificationNoMultipleNotificationsForSingleObjectCase7 )
{
	Notifier notifier;
	TestClazz testClazz;

	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.addNotification( getNotificationTest2(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.removeAllForObject( &testClazz );
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.addNotification( getNotificationTest2(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.removeAllForObject( &testClazz );
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.addNotification( getNotificationTest2(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.removeAllForObject( &testClazz );
	notifier.removeAllForObject( &testClazz );
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.addNotification( getNotificationTest2(), { &testClazz, &TestClazz::onDeliverNotification } );
}

TEST( Notifier, TestOfNotificationNoMultipleNotificationsForSingleObjectCase9 )
{
	Notifier notifier;
	TestClazz testClazz;

	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.notify(getNotificationTest1());

	notifier.removeAllForObject( &testClazz );
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
}

TEST( Notifier, TestOfNotificationNoMultipleNotificationsForSingleObjectCase10 )
{
	Notifier notifier;
	TestClazz testClazz;

	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.notify(getNotificationTest1());

	notifier.removeAllForObject( &testClazz );
	notifier.notify(getNotificationTest1());
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
}

TEST( Notifier, TestOfNotificationNoMultipleNotificationsForSingleObjectCase11 )
{
	Notifier notifier;
	TestClazz testClazz;

	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.notify(getNotificationTest1());

	notifier.removeAllForObject( &testClazz );
	notifier.notify(getNotificationTest1());
	notifier.removeAllForObject( &testClazz );
	notifier.notify(getNotificationTest1());
	notifier.removeAllForObject( &testClazz );
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );

	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
}

TEST( Notifier, DISABLED_TestOfNotificationNoMultipleNotificationsForSingleObjectCase8 )
{
	Notifier notifier;
	TestClazz testClazz;

	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.notify(getNotificationTest1());

	//It should crash
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
}

TEST( Notifier, DISABLED_TestOfNotificationNoMultipleNotificationsForSingleObjectCase1 )
{
	Notifier notifier;
	TestClazz testClazz;

	//It should crash
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
}

TEST( Notifier, DISABLED_TestOfNotificationNoMultipleNotificationsForSingleObjectCase4 )
{
	Notifier notifier;
	TestClazz testClazz;

	//It should crash
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.removeNotification( &testClazz, getNotificationTest1() );
	notifier.removeNotification( &testClazz, getNotificationTest1() );
	notifier.removeNotification( &testClazz, getNotificationTest1() );
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
	notifier.addNotification( getNotificationTest1(), { &testClazz, &TestClazz::onDeliverNotification } );
}
