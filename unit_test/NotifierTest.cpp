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
//
//TEST( Notifier, TestOfNotification )
//{
//	Notifier notifier;
//	TestClazz testClazz;
//
//	notifier.addNotification( getNotificationTest1(),
//							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
//
//	EXPECT_EQ( 0, testClazz.deliveredCount );
//
//	notifier.notify( getNotificationTest1() );
//
//	EXPECT_EQ( 1, testClazz.deliveredCount );
//
//	notifier.notify( getNotificationTest1() );
//	notifier.notify( getNotificationTest1() );
//	EXPECT_EQ( 3, testClazz.deliveredCount );
//
//	notifier.removeAllForObject( &testClazz );
//	notifier.notify( getNotificationTest1() );
//	EXPECT_EQ( 3, testClazz.deliveredCount );
//
//	notifier.addNotification( getNotificationTest1(),
//							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
//	EXPECT_EQ( 3, testClazz.deliveredCount );
//
//	notifier.notify( getNotificationTest1() );
//	EXPECT_EQ( 4, testClazz.deliveredCount );
//
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 0u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	notifier.removeNotification( &testClazz, getNotificationTest1() );
//	notifier.notify( getNotificationTest1() );
//	EXPECT_EQ( 4, testClazz.deliveredCount );
//
//
//	//////////////////////////////////////////////////////////////////////////////////////
//
//	notifier.addNotification( getNotificationTest1(),
//							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
//	notifier.addNotification( getNotificationTest2(),
//							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
//
//	//Here listeners are not still added because before notifications we apply changes
//	EXPECT_EQ( 0u, notifier.getListenersCount( getNotificationTest1().tag ) );
//
//	notifier.notify( getNotificationTest1() );
//
//	//Changes should be applied
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	EXPECT_EQ( 5, testClazz.deliveredCount );
//
//	notifier.notify( getNotificationTest2() );
//	EXPECT_EQ( 6, testClazz.deliveredCount );
//
//	notifier.removeNotification( &testClazz, getNotificationTest1() );
//	notifier.notify( getNotificationTest1() );
//	EXPECT_EQ( 6, testClazz.deliveredCount );
//
//	notifier.notify( getNotificationTest2() );
//	EXPECT_EQ( 7, testClazz.deliveredCount );
//
//	EXPECT_EQ( 0u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	notifier.addNotification( getNotificationTest1(),
//							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
//
//	EXPECT_EQ( 0u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	notifier.notify( getNotificationTest1() );
//
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	EXPECT_EQ( 8, testClazz.deliveredCount );
//
//	notifier.removeAllForObject( &testClazz );
//
//	//Changes still not applied
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	notifier.notify( getNotificationTest1() );
//	EXPECT_EQ( 8, testClazz.deliveredCount );
//
//	EXPECT_EQ( 0u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 0u, notifier.getListenersCount( getNotificationTest2().tag ) );
//}
//
//TEST( Notifier, TestOfNotificationChangesStack )
//{
//	Notifier notifier;
//	TestClazz testClazz;
//
//	notifier.addNotification( getNotificationTest1(),
//							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
//	notifier.addNotification( getNotificationTest2(),
//							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
//
//	EXPECT_EQ( 0u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 0u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	EXPECT_EQ( 0, testClazz.deliveredCount );
//	notifier.notify( getNotificationTest1() );
//	EXPECT_EQ( 1, testClazz.deliveredCount );
//
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	notifier.removeAllForObject( &testClazz );
//
//	//Changes still not applied
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	notifier.removeAllForObject( &testClazz );
//
//	//Changes still not applied
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	notifier.notify( getNotificationTest3(), 0x01 );
//
//	//changes should be applied
//	EXPECT_EQ( 0u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 0u, notifier.getListenersCount( getNotificationTest2().tag ) );
//}
//
//TEST( Notifier, DISABLED_TestOfNotificationChangesStackDoubleCommands )
//{
//	Notifier notifier;
//	TestClazz testClazz;
//
//	notifier.addNotification( getNotificationTest1(),
//							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
//	notifier.addNotification( getNotificationTest2(),
//							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
//
//	EXPECT_EQ( 0u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 0u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	EXPECT_EQ( 0, testClazz.deliveredCount );
//	notifier.notify( getNotificationTest1() );
//	EXPECT_EQ( 1, testClazz.deliveredCount );
//
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	notifier.removeAllForObject( &testClazz );
//
//	//Changes still not applied
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	notifier.removeAllForObject( &testClazz );
//
//	//Changes still not applied
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	notifier.addNotification( getNotificationTest1(),
//							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
//	notifier.addNotification( getNotificationTest2(),
//							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
//
//	//Changes still not applied
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	notifier.notify( getNotificationTest3(), 0x01 );
//
//	//changes should be applied
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest2().tag ) );
//
//	EXPECT_EQ( 1, testClazz.deliveredCount );
//	notifier.notify( getNotificationTest1() );
//	EXPECT_EQ( 2, testClazz.deliveredCount );
//
//	notifier.addNotification( getNotificationTest1(),
//							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
//	//Now we have double notification
//	notifier.notify( getNotificationTest1() );
//	EXPECT_EQ( 4, testClazz.deliveredCount );
//	EXPECT_EQ( 2u, notifier.getListenersCount( getNotificationTest1().tag ) );
//
//	notifier.removeNotification( &testClazz, getNotificationTest1() );
//	notifier.notify( getNotificationTest1() );
//	EXPECT_EQ( 4, testClazz.deliveredCount );
//	EXPECT_EQ( 0u, notifier.getListenersCount( getNotificationTest1().tag ) );
//	EXPECT_EQ( 1u, notifier.getListenersCount( getNotificationTest2().tag ) );
//}


MAKE_NOTIFICATION( TestUseAfterRelease );

class TestClazzReleased : public CCNode
{
public:
	TestClazzReleased( Notifier& notifier ) :
		m_pNotifier( notifier )
	{
		notifier.addNotification( getNotificationTestUseAfterRelease(), Utils::makeCallback( this,
								  &TestClazzReleased::onCall ) );
	}

	virtual ~TestClazzReleased()
	{
		m_pNotifier.removeAllForObject( this );
	}

	void onCall()
	{
		EXPECT_GT( retainCount() , 0u );  //This object is released?
		CCLOG( "Hello!" );
	}

private:
	Notifier& m_pNotifier;
};

TEST( Notifier, TestOfNotificationUseAfterRelease )
{
	Notifier notifier;

	TestClazzReleased* pReleasedNode = new TestClazzReleased( notifier );
	pReleasedNode->init();

	CCNode* pHolder = new CCNode();
	pHolder->init();
	notifier.addNotification( getNotificationTestUseAfterRelease(),{pHolder,&CCNode::removeAllChildren } );

	EXPECT_EQ( 1u, pReleasedNode->retainCount() );
	EXPECT_EQ( 1u, pHolder->retainCount() );

	pHolder->addChild( pReleasedNode );

	EXPECT_EQ( 2u, pReleasedNode->retainCount() );
	EXPECT_EQ( 1u, pHolder->retainCount() );

	pReleasedNode->release();

	EXPECT_EQ( 1u, pReleasedNode->retainCount() );
	EXPECT_EQ( 1u, pHolder->retainCount() );

	//OLD: Now when we notify we have problem. We have use after release.
	//NEW: Now everything should work because we mark second notification as toRemove so we simply skip it :)
	notifier.notify( getNotificationTestUseAfterRelease() );

	pHolder->release();
}

