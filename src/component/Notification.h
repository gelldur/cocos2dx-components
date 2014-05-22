#pragma once

namespace KoalaComponent
{

enum NotificationConst
{
	UNUSED_TAG = 0
};

inline int getUniqueId()
{
	static int id = NotificationConst::UNUSED_TAG + 1;
	return ++id;
}

template<typename... Args>
class Notification
{
public:

	Notification( int dummyInt ):
		//dummyInt used only for prevent creating unused notifications you should use MAKE_NOTIFICATION
		tag( getUniqueId() )
	{
		assert( dummyInt == 691283 );
	}

	Notification& operator= ( const Notification& ) = delete;
	Notification& operator= ( Notification && notification )
	{
		tag = notification.tag;
		return *this;
	}

	Notification( const Notification& notification ) :
		tag( notification.tag )
	{
	}

	Notification( Notification&& notification ) :
		tag( notification.tag )
	{
	}

	int tag;
};

template<typename NotificationType>
struct notification_traits;

template<typename... Args>
struct notification_traits<Notification<Args...>>
{
	using callback_type = typename Utils::Callback<void ( Args... )>;
};

}
