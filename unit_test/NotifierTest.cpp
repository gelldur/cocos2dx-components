/*
 * NotifierTest.cpp
 *
 *  Created on: 03.02.2014
 *      Author: Dawid Drozd
 */

#include "gtest/gtest.h"
#include "component/Notifier.h"

using namespace KoalaComponent;

MAKE_NOTIFICATION ( Test1 );
MAKE_NOTIFICATION ( Test2 );
MAKE_NOTIFICATION ( Test3, int );
MAKE_NOTIFICATION ( Test4, float );

//We use short notification names for better performance normally we have more.
#define OLD_NOTIFICATION_1 "a"
#define OLD_NOTIFICATION_2 "b"
#define OLD_NOTIFICATION_3 "c"
#define OLD_NOTIFICATION_4 "d"

struct TestClazz : CCObject
{
	TestClazz() :
		deliveredCount ( 0 )
	{
	}

	int deliveredCount;

	void onOldNotification ( CCObject* pObject )
	{
		++deliveredCount;
	}

	void onOldNotificationFloat ( CCObject* pObject )
	{
		//We use static cast for better performance
		CCFloat* pFloat = static_cast<CCFloat*> ( pObject );
		deliveredCount += pFloat->getValue();
	}

	void onOldNotificationInt ( CCObject* pObject )
	{
		//We use static cast for better performance
		CCInteger* pInteger = static_cast<CCInteger*> ( pObject );
		deliveredCount += pInteger->getValue();
	}

	void onDeliverNotification()
	{
		++deliveredCount;
	}

	void onDeliverNotification2 ( float value )
	{
		deliveredCount += value;
	}

	void onDeliverNotification3 ( int value )
	{
		deliveredCount += value;
	}

};

TEST ( Notifier, TestOfNotification )
{
	Notifier notifier;
	TestClazz testClazz;

	notifier.addNotification ( getNotificationTest1(),
							   Utils::makeCallback ( &testClazz, &TestClazz::onDeliverNotification ) );

	EXPECT_EQ ( 0, testClazz.deliveredCount );

	notifier.notify ( getNotificationTest1() );

	EXPECT_EQ ( 1, testClazz.deliveredCount );

	notifier.notify ( getNotificationTest1() );
	notifier.notify ( getNotificationTest1() );
	EXPECT_EQ ( 3, testClazz.deliveredCount );

	notifier.removeAllForObject ( &testClazz );
	notifier.notify ( getNotificationTest1() );
	EXPECT_EQ ( 3, testClazz.deliveredCount );

	notifier.addNotification ( getNotificationTest1(),
							   Utils::makeCallback ( &testClazz, &TestClazz::onDeliverNotification ) );
	EXPECT_EQ ( 3, testClazz.deliveredCount );

	notifier.notify ( getNotificationTest1() );
	EXPECT_EQ ( 4, testClazz.deliveredCount );

	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 0, notifier.getListenersCount ( getNotificationTest2().tag ) );

	notifier.removeNotification ( &testClazz, getNotificationTest1() );
	notifier.notify ( getNotificationTest1() );
	EXPECT_EQ ( 4, testClazz.deliveredCount );


	//////////////////////////////////////////////////////////////////////////////////////

	notifier.addNotification ( getNotificationTest1(),
							   Utils::makeCallback ( &testClazz, &TestClazz::onDeliverNotification ) );
	notifier.addNotification ( getNotificationTest2(),
							   Utils::makeCallback ( &testClazz, &TestClazz::onDeliverNotification ) );

	//Here listeners are not still added because before notifications we apply changes
	EXPECT_EQ ( 0, notifier.getListenersCount ( getNotificationTest1().tag ) );

	notifier.notify ( getNotificationTest1() );

	//Changes should be applied
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest2().tag ) );

	EXPECT_EQ ( 5, testClazz.deliveredCount );

	notifier.notify ( getNotificationTest2() );
	EXPECT_EQ ( 6, testClazz.deliveredCount );

	notifier.removeNotification ( &testClazz, getNotificationTest1() );
	notifier.notify ( getNotificationTest1() );
	EXPECT_EQ ( 6, testClazz.deliveredCount );

	notifier.notify ( getNotificationTest2() );
	EXPECT_EQ ( 7, testClazz.deliveredCount );

	EXPECT_EQ ( 0, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest2().tag ) );

	notifier.addNotification ( getNotificationTest1(),
							   Utils::makeCallback ( &testClazz, &TestClazz::onDeliverNotification ) );

	EXPECT_EQ ( 0, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest2().tag ) );

	notifier.notify ( getNotificationTest1() );

	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest2().tag ) );

	EXPECT_EQ ( 8, testClazz.deliveredCount );

	notifier.removeAllForObject ( &testClazz );

	//Changes still not applied
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest2().tag ) );

	notifier.notify ( getNotificationTest1() );
	EXPECT_EQ ( 8, testClazz.deliveredCount );

	EXPECT_EQ ( 0, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 0, notifier.getListenersCount ( getNotificationTest2().tag ) );
}

TEST ( Notifier, TestOfNotificationChangesStack )
{
	Notifier notifier;
	TestClazz testClazz;

	notifier.addNotification ( getNotificationTest1(),
							   Utils::makeCallback ( &testClazz, &TestClazz::onDeliverNotification ) );
	notifier.addNotification ( getNotificationTest2(),
							   Utils::makeCallback ( &testClazz, &TestClazz::onDeliverNotification ) );

	EXPECT_EQ ( 0, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 0, notifier.getListenersCount ( getNotificationTest2().tag ) );

	EXPECT_EQ ( 0, testClazz.deliveredCount );
	notifier.notify ( getNotificationTest1() );
	EXPECT_EQ ( 1, testClazz.deliveredCount );

	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest2().tag ) );

	notifier.removeAllForObject ( &testClazz );

	//Changes still not applied
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest2().tag ) );

	notifier.removeAllForObject ( &testClazz );

	//Changes still not applied
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest2().tag ) );

	notifier.notify ( getNotificationTest3(), 0x01 );

	//changes should be applied
	EXPECT_EQ ( 0, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 0, notifier.getListenersCount ( getNotificationTest2().tag ) );
}


MAKE_NOTIFICATION ( TestUseAfterRelease );

class TestClazzReleased : public CCNode
{
public:
	TestClazzReleased ( Notifier& notifier ) :
		m_pNotifier ( notifier )
	{
		notifier.addNotification ( getNotificationTestUseAfterRelease(), Utils::makeCallback ( this,
								   &TestClazzReleased::onCall ) );
	}

	virtual ~TestClazzReleased()
	{
		m_pNotifier.removeAllForObject ( this );
	}

	void onCall()
	{
		EXPECT_GT ( retainCount() , 0 ); //This object is released?
		CCLOG ( "Hello!" );
	}

private:
	Notifier& m_pNotifier;
};

TEST ( Notifier, DISABLED_TestOfNotificationUseAfterRelease )
{
	Notifier notifier;

	CCNode* pHolder = new CCNode();
	pHolder->init();
	notifier.addNotification ( getNotificationTestUseAfterRelease(), Utils::makeCallback ( pHolder,
							   &CCNode::removeAllChildren ) );

	TestClazzReleased* pReleasedNode = new TestClazzReleased ( notifier );
	pReleasedNode->init();

	EXPECT_EQ ( 1, pReleasedNode->retainCount() );
	EXPECT_EQ ( 1, pHolder->retainCount() );

	pHolder->addChild ( pReleasedNode );

	EXPECT_EQ ( 2, pReleasedNode->retainCount() );
	EXPECT_EQ ( 1, pHolder->retainCount() );

	pReleasedNode->release();

	EXPECT_EQ ( 1, pReleasedNode->retainCount() );
	EXPECT_EQ ( 1, pHolder->retainCount() );

	EXPECT_EQ ( 1, pReleasedNode->retainCount() );
	EXPECT_EQ ( 1, pHolder->retainCount() );

	//Now when we notify we have problem. We have use after release.
	//It should crash
	notifier.notify ( getNotificationTestUseAfterRelease() );

	pHolder->release();
}

TEST ( Notifier, TestOfNotificationChangesStackDoubleCommands )
{
	Notifier notifier;
	TestClazz testClazz;

	notifier.addNotification ( getNotificationTest1(),
							   Utils::makeCallback ( &testClazz, &TestClazz::onDeliverNotification ) );
	notifier.addNotification ( getNotificationTest2(),
							   Utils::makeCallback ( &testClazz, &TestClazz::onDeliverNotification ) );

	EXPECT_EQ ( 0, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 0, notifier.getListenersCount ( getNotificationTest2().tag ) );

	EXPECT_EQ ( 0, testClazz.deliveredCount );
	notifier.notify ( getNotificationTest1() );
	EXPECT_EQ ( 1, testClazz.deliveredCount );

	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest2().tag ) );

	notifier.removeAllForObject ( &testClazz );

	//Changes still not applied
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest2().tag ) );

	notifier.removeAllForObject ( &testClazz );

	//Changes still not applied
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest2().tag ) );

	notifier.addNotification ( getNotificationTest1(),
							   Utils::makeCallback ( &testClazz, &TestClazz::onDeliverNotification ) );
	notifier.addNotification ( getNotificationTest2(),
							   Utils::makeCallback ( &testClazz, &TestClazz::onDeliverNotification ) );

	//Changes still not applied
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest2().tag ) );

	notifier.notify ( getNotificationTest3(), 0x01 );

	//changes should be applied
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest2().tag ) );

	EXPECT_EQ ( 1, testClazz.deliveredCount );
	notifier.notify ( getNotificationTest1() );
	EXPECT_EQ ( 2, testClazz.deliveredCount );

	notifier.addNotification ( getNotificationTest1(),
							   Utils::makeCallback ( &testClazz, &TestClazz::onDeliverNotification ) );
	//Now we have double notification
	notifier.notify ( getNotificationTest1() );
	EXPECT_EQ ( 4, testClazz.deliveredCount );
	EXPECT_EQ ( 2, notifier.getListenersCount ( getNotificationTest1().tag ) );

	notifier.removeNotification ( &testClazz, getNotificationTest1() );
	notifier.notify ( getNotificationTest1() );
	EXPECT_EQ ( 4, testClazz.deliveredCount );
	EXPECT_EQ ( 0, notifier.getListenersCount ( getNotificationTest1().tag ) );
	EXPECT_EQ ( 1, notifier.getListenersCount ( getNotificationTest2().tag ) );
}

TEST ( Notifier, DISABLED_SpeedTestNotify )
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
		pNotifier->addNotification ( getNotificationTest4(),
									 Utils::makeCallback ( pTestClazz, &TestClazz::onDeliverNotification2 ) );
		pCenter->addObserver ( pTestClazz,
							   callfuncO_selector ( TestClazz::onOldNotificationFloat ), OLD_NOTIFICATION_4,
							   nullptr );

		CCFloat floatValue ( 22.3F );

		for ( int i = 0; i < 1000000; ++i )
		{
			CC_PROFILER_START ( "Notifier_1kk_notify_to_1_listener" );
			pNotifier->notify ( getNotificationTest4(), 22.3F );
			CC_PROFILER_STOP ( "Notifier_1kk_notify_to_1_listener" );


			CC_PROFILER_START ( "CCNotificationCenter_1kk_notify_to_1_listener" );
			pCenter->postNotification ( OLD_NOTIFICATION_4, &floatValue );
			CC_PROFILER_STOP ( "CCNotificationCenter_1kk_notify_to_1_listener" );
		}

		CC_PROFILER_DISPLAY_TIMERS();
	}
	CC_SAFE_DELETE ( pTestClazz );
}

TEST ( Notifier, DISABLED_SpeedTestNotifyMultipleListeners )
{
	const int listenersCount = 1000;
	Notifier* pNotifier = new Notifier();

	//We want fresh notification center
	CCNotificationCenter::sharedNotificationCenter()->purgeNotificationCenter();
	CCNotificationCenter* pCenter =
		CCNotificationCenter::sharedNotificationCenter();

	vector<TestClazz*> listeners;

	for ( int i = 0; i < listenersCount; ++i )
	{
		int* pTab = new int[rand() % 1000 + 100];
		listeners.push_back ( new TestClazz() );
		pNotifier->addNotification ( getNotificationTest4(),
									 Utils::makeCallback ( listeners.back(), &TestClazz::onDeliverNotification2 ) );

		pCenter->addObserver ( listeners.back(),
							   callfuncO_selector ( TestClazz::onOldNotificationFloat ), OLD_NOTIFICATION_4,
							   nullptr );

		delete pTab;
	}

	CC_PROFILER_PURGE_ALL();
	CCFloat floatValue ( 22.3F );

	for ( int i = 0; i < 100000; ++i )
	{
		CC_PROFILER_START ( "Notifier_100k_notify_to_1k_listeners" );
		pNotifier->notify ( getNotificationTest4(), 22.3F );
		CC_PROFILER_STOP ( "Notifier_100k_notify_to_1k_listeners" );

		CC_PROFILER_START ( "CCNotificationCenter_100k_notify_to_1k_listeners" );
		pCenter->postNotification ( OLD_NOTIFICATION_4, &floatValue );
		CC_PROFILER_STOP ( "CCNotificationCenter_100k_notify_to_1k_listeners" );
	}

	CC_PROFILER_DISPLAY_TIMERS();

	for ( int i = 0; i < listenersCount; ++i )
	{
		delete listeners[i];
	}
}

TEST ( Notifier,
	   DISABLED_SpeedTestNotifyMultipleListenersMultipleNotifications )
{
	const int listenersCount = 1000;
	Notifier* pNotifier = new Notifier();

	//We want fresh notification center
	CCNotificationCenter::sharedNotificationCenter()->purgeNotificationCenter();
	CCNotificationCenter* pCenter =
		CCNotificationCenter::sharedNotificationCenter();

	vector<TestClazz*> listeners;

	for ( int i = 0; i < listenersCount; ++i )
	{
		int* pTab = new int[rand() % 1000 + 100];
		listeners.push_back ( new TestClazz() );
		int which = i % 4;

		if ( which == 0 )
		{
			pNotifier->addNotification ( getNotificationTest1(),
										 Utils::makeCallback ( listeners.back(), &TestClazz::onDeliverNotification ) );
			pCenter->addObserver ( listeners.back(),
								   callfuncO_selector ( TestClazz::onOldNotification ), OLD_NOTIFICATION_1,
								   nullptr );
		}
		else if ( which == 1 )
		{
			pNotifier->addNotification ( getNotificationTest2(),
										 Utils::makeCallback ( listeners.back(), &TestClazz::onDeliverNotification ) );
			pCenter->addObserver ( listeners.back(),
								   callfuncO_selector ( TestClazz::onOldNotification ), OLD_NOTIFICATION_2,
								   nullptr );
		}
		else if ( which == 2 )
		{
			pNotifier->addNotification ( getNotificationTest3(),
										 Utils::makeCallback ( listeners.back(), &TestClazz::onDeliverNotification3 ) );
			pCenter->addObserver ( listeners.back(),
								   callfuncO_selector ( TestClazz::onOldNotificationInt ), OLD_NOTIFICATION_3,
								   nullptr );
		}
		else if ( which == 3 )
		{
			pNotifier->addNotification ( getNotificationTest4(),
										 Utils::makeCallback ( listeners.back(), &TestClazz::onDeliverNotification2 ) );
			pCenter->addObserver ( listeners.back(),
								   callfuncO_selector ( TestClazz::onOldNotificationFloat ), OLD_NOTIFICATION_4,
								   nullptr );
		}


		delete pTab;
	}

	CC_PROFILER_PURGE_ALL();

	CCFloat floatValue ( 22.3F );
	CCInteger intValue ( 99 );

	for ( int i = 0; i < 100000; ++i )
	{
		int which = i % 4;
		CC_PROFILER_START ( "Notifier_100k_notify_to_1k_listener_multiple_notifications" );

		if ( which == 0 )
		{
			pNotifier->notify ( getNotificationTest1() );
		}
		else if ( which == 1 )
		{
			pNotifier->notify ( getNotificationTest2() );
		}
		else if ( which == 2 )
		{
			pNotifier->notify ( getNotificationTest3(), 99 );
		}
		else if ( which == 3 )
		{
			pNotifier->notify ( getNotificationTest4(), 23.3F );
		}

		CC_PROFILER_STOP ( "Notifier_100k_notify_to_1k_listener_multiple_notifications" );
		CC_PROFILER_START ( "CCNotificationCenter_100k_notify_to_1k_listener_multiple_notifications" );

		if ( which == 0 )
		{
			pCenter->postNotification ( OLD_NOTIFICATION_1 );
		}
		else if ( which == 1 )
		{
			pCenter->postNotification ( OLD_NOTIFICATION_2 );
		}
		else if ( which == 2 )
		{
			pCenter->postNotification ( OLD_NOTIFICATION_3, &intValue );
		}
		else if ( which == 3 )
		{
			pCenter->postNotification ( OLD_NOTIFICATION_4, &floatValue );
		}

		CC_PROFILER_STOP ( "CCNotificationCenter_100k_notify_to_1k_listener_multiple_notifications" );
	}

	CC_PROFILER_DISPLAY_TIMERS();

	for ( int i = 0; i < listenersCount; ++i )
	{
		delete listeners[i];
	}
}
