cocos2dx-components
===================

On <b>MIT<b> license

Components for cocos2dx

<b><h1>Hierarchy graph<b>

![alt tag](https://raw.github.com/gelldur/cocos2dx-components/master/Diagram.jpg)

Example use
===================

```cpp
ComponentNode<CCSprite>* pSprite = ComponentNode<CCSprite>::create();

pSprite->initWithFile ( R::Sprite::GUI_BUTTON_GREEN_9 );

pSprite->getComponentManager().addComponent ( TouchComponent::create() );
pSprite->getComponentManager().addComponent ( StandardPressActionComponent::create() );
pSprite->getComponentManager().addComponent (ClickComponent::create ( Utils::makeCallback ( this, &AchievementRow::onSkipClickedCallback ) ) );

//Or another way
TouchComponent* pTouch = TouchComponent::create();
ClickComponent* pClick = ClickComponent::create ( Utils::makeCallback ( this, &AchievementRow::onSkipClickedCallback ) );
StandardPressActionComponent* pPressAnimation = StandardPressActionComponent::create();


pTouch->getManager().addComponent( pClick );
pClick->getManager().addComponent( pPressAnimation );

pSprite->getComponentManager().addComponent ( pTouch );

/**
What is under hood
Now when TouchComponent recives touches he notify about them to NotificationCenter and ClickComponent decides is this pSprite (Owner) is being clicked. 
ClickComponent sends notification that we have click start so StandardPressActionComponent start animation of pressing button.
**/
```

You can see example components in /sample (snippets)
 - TouchComponent
 - ClickComponent
 - StandardPressActionComponent

More about callbacks you can read here: http://www.cocos2d-x.org/forums/6/topics/41358?r=43554#message-43554

How to create notifications
===================
Simply use macro

```cpp
MAKE_NOTIFICATION ( OnTouchBegan, CCTouch*, bool* );
MAKE_NOTIFICATION ( OnTouchEnded, CCTouch* );
MAKE_NOTIFICATION ( OnTouchMoved, CCTouch* );
MAKE_NOTIFICATION(OnPress);
MAKE_NOTIFICATION(OnUnPress);
```

Where first argument is name for notification. Automaticly macro will add something like "getNotification" as prefix so for example

getNotificationOnTouchBegan();
getNotificationOnPress();

rest of arguments are vargs that will be used for notifications. Types are really important here.

Look at Notifier.h for implementation.

Performance test
===================

Performance tests are disabled in unit_test/NotifierTest.cpp

On PC (DEBUG) (AMD Phenom 2 x6)
===================
            Notifier_1kk_notify_to_1_listener ::	avg1: 0µ,	avg2: 0µ,	min: 0µ,	max: 14µ,	total: 0.29s,	nr calls: 1000000
	    Notifier_1kk_notify_to_1_listener ::	avg1: 0µ,	avg2: 0µ,	min: 0µ,	max: 83µ,	total: 0.30s,	nr calls: 1000000
            Notifier_1kk_notify_to_1_listener ::	avg1: 0µ,	avg2: 0µ,	min: 0µ,	max: 45µ,	total: 0.30s,	nr calls: 1000000
	    Notifier_1kk_notify_to_1_listener ::	avg1: 0µ,	avg2: 0µ,	min: 0µ,	max: 18µ,	total: 0.28s,	nr calls: 1000000
CCNotificationCenter_1kk_notify_to_1_listener ::	avg1: 0µ,	avg2: 0µ,	min: 0µ,	max: 47µ,	total: 0.53s,	nr calls: 1000000
CCNotificationCenter_1kk_notify_to_1_listener ::	avg1: 0µ,	avg2: 0µ,	min: 0µ,	max: 69µ,	total: 0.54s,	nr calls: 1000000
CCNotificationCenter_1kk_notify_to_1_listener ::	avg1: 0µ,	avg2: 0µ,	min: 0µ,	max: 87µ,	total: 0.57s,	nr calls: 1000000
CCNotificationCenter_1kk_notify_to_1_listener ::	avg1: 0µ,	avg2: 0µ,	min: 0µ,	max: 41µ,	total: 0.52s,	nr calls: 1000000



            Notifier_100k_notify_to_1k_listeners ::	avg1: 42µ,	avg2: 38µ,	min: 36µ,	max: 296µ,	total: 3.89s,	nr calls: 100000
            Notifier_100k_notify_to_1k_listeners ::	avg1: 45µ,	avg2: 38µ,	min: 36µ,	max: 256µ,	total: 3.81s,	nr calls: 100000
	    Notifier_100k_notify_to_1k_listeners ::	avg1: 36µ,	avg2: 37µ,	min: 36µ,	max: 279µ,	total: 3.72s,	nr calls: 100000
CCNotificationCenter_100k_notify_to_1k_listeners ::	avg1: 77µ,	avg2: 78µ,	min: 75µ,	max: 733µ,	total: 7.83s,	nr calls: 100000
CCNotificationCenter_100k_notify_to_1k_listeners ::	avg1: 79µ,	avg2: 79µ,	min: 74µ,	max: 124µ,	total: 7.91s,	nr calls: 100000
CCNotificationCenter_100k_notify_to_1k_listeners ::	avg1: 91µ,	avg2: 83µ,	min: 75µ,	max: 129µ,	total: 8.37s,	nr calls: 100000



            Notifier_100k_notify_to_1k_listener_multiple_notifications ::	avg1: 9µ,	avg2: 9µ,	min: 9µ,	max: 277µ,	total: 0.98s,	nr calls: 100000
	    Notifier_100k_notify_to_1k_listener_multiple_notifications ::	avg1: 10µ,	avg2: 10µ,	min: 9µ,	max: 251µ,	total: 1.00s,	nr calls: 100000
CCNotificationCenter_100k_notify_to_1k_listener_multiple_notifications ::	avg1: 54µ,	avg2: 54µ,	min: 50µ,	max: 101µ,	total: 5.46s,	nr calls: 100000
CCNotificationCenter_100k_notify_to_1k_listener_multiple_notifications ::	avg1: 55µ,	avg2: 54µ,	min: 50µ,	max: 133µ,	total: 5.47s,	nr calls: 100000


On PC (RELEASE) (AMD Phenom 2 x6)
===================
	    Notifier_1kk_notify_to_1_listener ::	avg1: 0µ,	avg2: 0µ,	min: 0µ,	max: 9µ,	total: 0.08s,	nr calls: 1000000
	    Notifier_1kk_notify_to_1_listener ::	avg1: 0µ,	avg2: 0µ,	min: 0µ,	max: 20µ,	total: 0.08s,	nr calls: 1000000
CCNotificationCenter_1kk_notify_to_1_listener ::	avg1: 0µ,	avg2: 0µ,	min: 0µ,	max: 51µ,	total: 0.40s,	nr calls: 1000000
CCNotificationCenter_1kk_notify_to_1_listener ::	avg1: 0µ,	avg2: 0µ,	min: 0µ,	max: 49µ,	total: 0.41s,	nr calls: 1000000


	    Notifier_100k_notify_to_1k_listeners ::	avg1: 9µ,	avg2: 9µ,	min: 9µ,	max: 52µ,	total: 0.98s,	nr calls: 100000
	    Notifier_100k_notify_to_1k_listeners ::	avg1: 9µ,	avg2: 9µ,	min: 9µ,	max: 52µ,	total: 0.98s,	nr calls: 100000
CCNotificationCenter_100k_notify_to_1k_listeners ::	avg1: 31µ,	avg2: 31µ,	min: 29µ,	max: 77µ,	total: 3.19s,	nr calls: 100000
CCNotificationCenter_100k_notify_to_1k_listeners ::	avg1: 38µ,	avg2: 37µ,	min: 29µ,	max: 92µ,	total: 3.78s,	nr calls: 100000


	    Notifier_100k_notify_to_1k_listener_multiple_notifications ::	avg1: 2µ,	avg2: 2µ,	min: 2µ,	max: 42µ,	total: 0.26s,	nr calls: 100000
	    Notifier_100k_notify_to_1k_listener_multiple_notifications ::	avg1: 2µ,	avg2: 2µ,	min: 2µ,	max: 42µ,	total: 0.26s,	nr calls: 100000
CCNotificationCenter_100k_notify_to_1k_listener_multiple_notifications ::	avg1: 26µ,	avg2: 27µ,	min: 24µ,	max: 59µ,	total: 2.73s,	nr calls: 100000
CCNotificationCenter_100k_notify_to_1k_listener_multiple_notifications ::	avg1: 28µ,	avg2: 29µ,	min: 22µ,	max: 52µ,	total: 2.96s,	nr calls: 100000


On Nexus4 (RELEASE)
===================

(TBC)

