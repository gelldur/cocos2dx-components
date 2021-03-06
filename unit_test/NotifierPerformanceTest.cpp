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
MAKE_NOTIFICATION( Test3, int );
MAKE_NOTIFICATION( Test4, float );

//We use short notification names for better performance normally we have more.
#define OLD_NOTIFICATION_1 "a"
#define OLD_NOTIFICATION_2 "b"
#define OLD_NOTIFICATION_3 "c"
#define OLD_NOTIFICATION_4 "d"

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

	void onDeliverNotification2( float value )
	{
		deliveredCount += value;
	}

	void onDeliverNotification3( int value )
	{
		deliveredCount += value;
	}
};

TEST( Notifier, DISABLED_SpeedTestNotify )
{
	Notifier* pNotifier = new Notifier();
	//We want fresh notification center
	CCNotificationCenter::sharedNotificationCenter()->purgeNotificationCenter();
	CCNotificationCenter* pCenter =
		CCNotificationCenter::sharedNotificationCenter();

	int* pTab = new int[rand() % 1000 + 100];
	TestClazz* pTestClazz = new TestClazz();
	delete pTab;
	{
		CC_PROFILER_PURGE_ALL();
		pNotifier->addNotification( getNotificationTest4(),
									Utils::makeCallback( pTestClazz, &TestClazz::onDeliverNotification2 ) );
		pCenter->addObserver( pTestClazz,
							  callfuncO_selector( TestClazz::onOldNotificationFloat ), OLD_NOTIFICATION_4,
							  nullptr );

		CCFloat floatValue( 22.3F );

		for( int i = 0; i < 1000000; ++i )
		{
			CC_PROFILER_START( "Notifier_1kk_notify_to_1_listener" );
			pNotifier->notify( getNotificationTest4(), 22.3F );
			CC_PROFILER_STOP( "Notifier_1kk_notify_to_1_listener" );


			CC_PROFILER_START( "CCNotificationCenter_1kk_notify_to_1_listener" );
			pCenter->postNotification( OLD_NOTIFICATION_4, &floatValue );
			CC_PROFILER_STOP( "CCNotificationCenter_1kk_notify_to_1_listener" );
		}

		CC_PROFILER_DISPLAY_TIMERS();
	}
	CC_SAFE_DELETE( pTestClazz );
}

TEST( Notifier, DISABLED_SpeedTestNotifyMultipleListeners )
{
	const int listenersCount = 1000;
	Notifier* pNotifier = new Notifier();

	//We want fresh notification center
	CCNotificationCenter::sharedNotificationCenter()->purgeNotificationCenter();
	CCNotificationCenter* pCenter =
		CCNotificationCenter::sharedNotificationCenter();

	std::vector<TestClazz*> listeners;

	for( int i = 0; i < listenersCount; ++i )
	{
		int* pTab = new int[rand() % 1000 + 100];
		listeners.push_back( new TestClazz() );
		pNotifier->addNotification( getNotificationTest4(),
									Utils::makeCallback( listeners.back(), &TestClazz::onDeliverNotification2 ) );

		pCenter->addObserver( listeners.back(),
							  callfuncO_selector( TestClazz::onOldNotificationFloat ), OLD_NOTIFICATION_4,
							  nullptr );

		delete pTab;
	}

	CC_PROFILER_PURGE_ALL();
	CCFloat floatValue( 22.3F );

	for( int i = 0; i < 100000; ++i )
	{
		CC_PROFILER_START( "Notifier_100k_notify_to_1k_listeners" );
		pNotifier->notify( getNotificationTest4(), 22.3F );
		CC_PROFILER_STOP( "Notifier_100k_notify_to_1k_listeners" );

		CC_PROFILER_START( "CCNotificationCenter_100k_notify_to_1k_listeners" );
		pCenter->postNotification( OLD_NOTIFICATION_4, &floatValue );
		CC_PROFILER_STOP( "CCNotificationCenter_100k_notify_to_1k_listeners" );
	}

	CC_PROFILER_DISPLAY_TIMERS();

	for( int i = 0; i < listenersCount; ++i )
	{
		delete listeners[i];
	}
}

TEST( Notifier,
	  DISABLED_SpeedTestNotifyMultipleListenersMultipleNotifications )
{
	const int listenersCount = 1000;
	Notifier* pNotifier = new Notifier();

	//We want fresh notification center
	CCNotificationCenter::sharedNotificationCenter()->purgeNotificationCenter();
	CCNotificationCenter* pCenter =
		CCNotificationCenter::sharedNotificationCenter();

	std::vector<TestClazz*> listeners;

	for( int i = 0; i < listenersCount; ++i )
	{
		int* pTab = new int[rand() % 1000 + 100];
		listeners.push_back( new TestClazz() );
		int which = i % 4;

		if( which == 0 )
		{
			pNotifier->addNotification( getNotificationTest1(),
										Utils::makeCallback( listeners.back(), &TestClazz::onDeliverNotification ) );
			pCenter->addObserver( listeners.back(),
								  callfuncO_selector( TestClazz::onOldNotification ), OLD_NOTIFICATION_1,
								  nullptr );
		}
		else if( which == 1 )
		{
			pNotifier->addNotification( getNotificationTest2(),
										Utils::makeCallback( listeners.back(), &TestClazz::onDeliverNotification ) );
			pCenter->addObserver( listeners.back(),
								  callfuncO_selector( TestClazz::onOldNotification ), OLD_NOTIFICATION_2,
								  nullptr );
		}
		else if( which == 2 )
		{
			pNotifier->addNotification( getNotificationTest3(),
										Utils::makeCallback( listeners.back(), &TestClazz::onDeliverNotification3 ) );
			pCenter->addObserver( listeners.back(),
								  callfuncO_selector( TestClazz::onOldNotificationInt ), OLD_NOTIFICATION_3,
								  nullptr );
		}
		else if( which == 3 )
		{
			pNotifier->addNotification( getNotificationTest4(),
										Utils::makeCallback( listeners.back(), &TestClazz::onDeliverNotification2 ) );
			pCenter->addObserver( listeners.back(),
								  callfuncO_selector( TestClazz::onOldNotificationFloat ), OLD_NOTIFICATION_4,
								  nullptr );
		}


		delete pTab;
	}

	CC_PROFILER_PURGE_ALL();

	CCFloat floatValue( 22.3F );
	CCInteger intValue( 99 );

	for( int i = 0; i < 100000; ++i )
	{
		int which = i % 4;
		CC_PROFILER_START( "Notifier_100k_notify_to_1k_listener_multiple_notifications" );

		if( which == 0 )
		{
			pNotifier->notify( getNotificationTest1() );
		}
		else if( which == 1 )
		{
			pNotifier->notify( getNotificationTest2() );
		}
		else if( which == 2 )
		{
			pNotifier->notify( getNotificationTest3(), 99 );
		}
		else if( which == 3 )
		{
			pNotifier->notify( getNotificationTest4(), 23.3F );
		}

		CC_PROFILER_STOP( "Notifier_100k_notify_to_1k_listener_multiple_notifications" );
		CC_PROFILER_START( "CCNotificationCenter_100k_notify_to_1k_listener_multiple_notifications" );

		if( which == 0 )
		{
			pCenter->postNotification( OLD_NOTIFICATION_1 );
		}
		else if( which == 1 )
		{
			pCenter->postNotification( OLD_NOTIFICATION_2 );
		}
		else if( which == 2 )
		{
			pCenter->postNotification( OLD_NOTIFICATION_3, &intValue );
		}
		else if( which == 3 )
		{
			pCenter->postNotification( OLD_NOTIFICATION_4, &floatValue );
		}

		CC_PROFILER_STOP( "CCNotificationCenter_100k_notify_to_1k_listener_multiple_notifications" );
	}

	CC_PROFILER_DISPLAY_TIMERS();

	for( int i = 0; i < listenersCount; ++i )
	{
		delete listeners[i];
	}
}
