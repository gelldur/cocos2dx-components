/*
* Callback.h
*
*  Created on: Dec 28, 2013
*      Author: Jakub 'kuhar' Kuderski
*/

#ifndef KOALA_CALLBACK_H_
#define KOALA_CALLBACK_H_

#include <vector>
#include <utility>
#include <type_traits>
#include <functional>
#include "cocos2d.h"

#define CALLBACK_ENABLE_ASSERTIONS 1

#if CALLBACK_ENABLE_ASSERTIONS == 1
#include <cassert>
#endif

#if CALLBACK_ENABLE_ASSERTIONS == 1
#define CALLBACK_ASSERT( CONDITION ) assert( CONDITION )
#else
#define CALLBACK_ASSERT( CONDITION )
#endif

namespace Utils
{
/**
* WARNING FOR VISUAL C++: set additional /vmg flag, otherwise generated code may not work!
*
* Example usage:
*
* Callback<void(int)> callback; // void --> return type, other types --> argument types
* callback.set(pObject ,&CCObject::methodWithInt);
*
* or Callback<void(int)> callback(pObject, &CCObject::methodWithInt);
*
* or Callback<int(int, float)> callback = makeCallback( pObject, &SomeClass::someMethod );
*
* or even: auto callback = makeCallback( pObject, &SomeClass::someMethod );
*
* then call methods with some args: int result = callback.call ( some, args );
*
* You can also use functions/static methods and non-capturing lambdas.
* Ex. auto callback = Callback<void(int)>( []( int n ){ cout << n; } );
*
* Please note that the following implementation uses chakra magic - be careful when modifying.
* If you are out of mana, see previous revisions that used std::function.
*/

typedef cocos2d::CCObject BaseClass;

template<typename T>
class Callback;

#ifndef _MSC_VER

template<typename ReturnType, typename... Args>
class Callback<ReturnType ( Args... ) >
{

public:

	Callback() :
		m_pointerType ( PointerType::NonConstMethod )
	{
	}

	template<typename ClassType, typename MethodType>
	Callback ( ClassType* pObject, const MethodType& method ) :
		m_pointerType ( PointerType::NonConstMethod )
	{
		static_assert ( std::is_member_function_pointer<MethodType>::value,
						"The second argument must be a pointer to a method" );
		set ( pObject, method );
	}

	template<typename FunctionType>
	Callback ( const FunctionType& functor ) :
		m_pointerType ( PointerType::Function )
	{
		set ( functor );
	}

	Callback ( const Callback& otherCallback )
	{
		m_callableObject = otherCallback.m_callableObject;
		m_pointerType = otherCallback.m_pointerType;
	}

	Callback& operator= ( const Callback& otherCallback )
	{
		m_callableObject = otherCallback.m_callableObject;
		m_pointerType = otherCallback.m_pointerType;
		return *this;
	}

	inline bool isCallable() const
	{
		return m_callableObject.first != nullptr ||
			   m_pointerType == PointerType::Function;
	}

	inline BaseClass* getObject() const
	{
		return m_callableObject.first;
	}

	template<typename ObjectType, typename ClassType, typename ClassReturnType, typename... ClassArgs>
	Callback& set ( ObjectType* pObject,
					 ClassReturnType ( ClassType::*method ) ( ClassArgs... ) )
	{
		static_assert ( std::is_base_of<BaseClass, ClassType>::value,
						"CCObject must be the base of used class" );

		static_assert ( std::is_base_of<ClassType, ObjectType>::value,
						"Pointer to method must be the base of object's pointer" );

		m_pointerType = PointerType::NonConstMethod;
		MethodPointer methodPointer;
		methodPointer.nonConstMethod =
			static_cast<ReturnType ( BaseClass::* ) ( Args... ) > ( method );

		m_callableObject = std::make_pair ( pObject, methodPointer );
		return *this;
	}

	template<typename ObjectType, typename ClassType, typename ClassReturnType, typename... ClassArgs>
	Callback& set ( ObjectType* pObject,
					 ClassReturnType ( ClassType::*method ) ( ClassArgs... ) const )
	{
		static_assert ( std::is_base_of<BaseClass, ClassType>::value,
						"CCObject must be the base of used class" );

		static_assert ( std::is_base_of<ClassType, ObjectType>::value,
						"Pointer to method must be the base of object's pointer" );

		m_pointerType = PointerType::ConstMethod;
		MethodPointer methodPointer;
		methodPointer.constMethod =
			static_cast<ReturnType ( BaseClass::* ) ( Args... ) const> ( method );

		m_callableObject = std::make_pair ( pObject, methodPointer );
		return *this;
	}

	template<typename FunctionType>
	Callback& set ( const FunctionType& functor )
	{
		static_assert (
			std::is_convertible<FunctionType, std::function<ReturnType ( Args... ) >>::value,
			"Parameter must be either function (static method) pointer or non-capturing lambda" );

		m_pointerType = PointerType::Function;
		MethodPointer methodPointer;
		methodPointer.function = static_cast<ReturnType ( * ) ( Args... ) > ( functor );

		m_callableObject = std::make_pair ( nullptr, methodPointer );
		return *this;
	}

	ReturnType call ( Args... params ) const
	{
		CALLBACK_ASSERT ( isCallable() );

		if ( m_pointerType == PointerType::ConstMethod )
		{
			return ( m_callableObject.first->*m_callableObject.second.constMethod ) (
					   params... );
		}

		else if ( m_pointerType == PointerType::NonConstMethod )
		{
			return ( m_callableObject.first->*m_callableObject.second.nonConstMethod ) (
					   params... );
		}

		return ( *m_callableObject.second.function ) ( params... );
	}

private:

	union MethodPointer
	{
		ReturnType ( BaseClass::*nonConstMethod ) ( Args... );
		ReturnType ( BaseClass::*constMethod ) ( Args... ) const;
		ReturnType ( *function ) ( Args... );
	};

	enum class PointerType : unsigned char
	{
		NonConstMethod,
		ConstMethod,
		Function
	};

	typedef std::pair<BaseClass*, MethodPointer> ObjectMethodPair;
	ObjectMethodPair m_callableObject;
	PointerType m_pointerType;
};


template<typename ReturnType, typename ObjectType, typename MethodType, typename... Args>
Callback<ReturnType ( Args... ) > makeCallback ( ObjectType* pObject,
		ReturnType ( MethodType::*method ) ( Args... ) )
{
	return Callback<ReturnType ( Args... ) > ( pObject, method );
}

template<typename ReturnType, typename ObjectType, typename MethodType, typename... Args>
Callback<ReturnType ( Args... ) > makeCallback ( ObjectType* pObject,
		ReturnType ( MethodType::*method ) ( Args... ) const )
{
	return Callback<ReturnType ( Args... ) > ( pObject, method );
}

template<typename ReturnType, typename... Args>
Callback<ReturnType ( Args... ) > makeCallback ( ReturnType ( *function ) (
			Args... ) )
{
	return Callback<ReturnType ( Args... ) > ( function );
}

#else

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

template<typename ReturnType>
class Callback<ReturnType() >
{

public:

	Callback() :
		m_pointerType ( PointerType::NonConstMethod )
	{
	}

	template<typename ClassType, typename MethodType>
	Callback ( ClassType* pObject, const MethodType& method ) :
		m_pointerType ( PointerType::NonConstMethod )
	{
		static_assert ( std::is_member_function_pointer<MethodType>::value,
						"The second argument must be a pointer to a method" );
		set ( pObject, method );
	}

	template<typename FunctionType>
	Callback ( const FunctionType& functor ) :
		m_pointerType ( PointerType::Function )
	{
		set ( functor );
	}

	Callback ( const Callback& otherCallback )
	{
		m_callableObject = otherCallback.m_callableObject;
		m_pointerType = otherCallback.m_pointerType;
	}

	Callback& operator= ( const Callback& otherCallback )
	{
		m_callableObject = otherCallback.m_callableObject;
		m_pointerType = otherCallback.m_pointerType;
		return *this;
	}

	inline bool isCallable() const
	{
		return m_callableObject.first || m_pointerType == PointerType::Function;
	}

	inline BaseClass* getObject() const
	{
		return m_callableObject.first;
	}

	template<typename ObjectType, typename ClassType, typename ClassReturnType>
	Callback& set ( ObjectType* pObject,
					 ClassReturnType ( ClassType::*method ) ( ) )
	{
		static_assert ( std::is_base_of<BaseClass, ClassType>::value,
						"CCObject must be the base of used class" );

		static_assert ( std::is_base_of<ClassType, ObjectType>::value,
						"Pointer to method must be the base of object's pointer" );

		m_pointerType = PointerType::NonConstMethod;
		MethodPointer methodPointer;
		methodPointer.nonConstMethod = static_cast<ReturnType ( BaseClass::* ) ( ) >
									   ( method );

		m_callableObject = std::make_pair ( pObject, methodPointer );
		return *this;
	}

	template<typename ObjectType, typename ClassType, typename ClassReturnType>
	Callback& set ( ObjectType* pObject,
					 ClassReturnType ( ClassType::*method ) ( ) const )
	{
		static_assert ( std::is_base_of<BaseClass, ClassType>::value,
						"CCObject must be the base of used class" );

		static_assert ( std::is_base_of<ClassType, ObjectType>::value,
						"Pointer to method must be the base of object's pointer" );

		m_pointerType = PointerType::ConstMethod;
		MethodPointer methodPointer;
		methodPointer.constMethod = static_cast<ReturnType ( BaseClass::* ) ( ) const>
									( method );

		m_callableObject = std::make_pair ( pObject, methodPointer );
		return *this;
	}

	template<typename FunctionType>
	Callback& set ( const FunctionType& functor )
	{
		static_assert (
			std::is_convertible<FunctionType, std::function<ReturnType() >>::value,
			"Parameter must be either function (static method) pointer or non-capturing lambda" );

		m_pointerType = PointerType::Function;
		MethodPointer methodPointer;
		methodPointer.function = static_cast<ReturnType ( * ) ( ) > ( functor );

		m_callableObject = std::make_pair ( nullptr, methodPointer );
		return *this;
	}

	ReturnType call() const
	{
		CALLBACK_ASSERT ( isCallable() );

		if ( m_pointerType == PointerType::ConstMethod )
		{
			return ( m_callableObject.first->*m_callableObject.second.constMethod ) ( );
		}

		else if ( m_pointerType == PointerType::NonConstMethod )
		{
			return ( m_callableObject.first->*m_callableObject.second.nonConstMethod ) ( );
		}

		return ( *m_callableObject.second.function ) ( );
	}

private:

	union MethodPointer
	{
		ReturnType ( BaseClass::*nonConstMethod ) ( );
		ReturnType ( BaseClass::*constMethod ) ( ) const;
		ReturnType ( *function ) ( );
	};

	enum class PointerType : unsigned char
	{
		NonConstMethod,
		ConstMethod,
		Function
	};

	typedef std::pair<BaseClass*, MethodPointer> ObjectMethodPair;
	ObjectMethodPair m_callableObject;
	PointerType m_pointerType;
};


template<typename ReturnType, typename ObjectType, typename MethodType>
Callback<ReturnType() > makeCallback ( ObjectType* pObject,
										ReturnType ( MethodType::*method ) ( ) )
{
	return Callback<ReturnType() > ( pObject, method );
}

template<typename ReturnType, typename ObjectType, typename MethodType>
Callback<ReturnType() > makeCallback ( ObjectType* pObject,
										ReturnType ( MethodType::*method ) ( ) const )
{
	return Callback<ReturnType() > ( pObject, method );
}

template<typename ReturnType>
Callback<ReturnType() > makeCallback ( ReturnType ( *function ) ( ) )
{
	return Callback<ReturnType() > ( function );
}

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

template<typename ReturnType, typename T1>
class Callback<ReturnType ( T1 ) >
{

public:

	Callback() :
		m_pointerType ( PointerType::NonConstMethod )
	{
	}

	template<typename ClassType, typename MethodType>
	Callback ( ClassType* pObject, const MethodType& method ) :
		m_pointerType ( PointerType::NonConstMethod )
	{
		static_assert ( std::is_member_function_pointer<MethodType>::value,
						"The second argument must be a pointer to a method" );
		set ( pObject, method );
	}

	template<typename FunctionType>
	Callback ( const FunctionType& functor ) :
		m_pointerType ( PointerType::Function )
	{
		set ( functor );
	}

	Callback ( const Callback& otherCallback )
	{
		m_callableObject = otherCallback.m_callableObject;
		m_pointerType = otherCallback.m_pointerType;
	}

	Callback& operator= ( const Callback& otherCallback )
	{
		m_callableObject = otherCallback.m_callableObject;
		m_pointerType = otherCallback.m_pointerType;
		return *this;
	}

	inline bool isCallable() const
	{
		return m_callableObject.first || m_pointerType == PointerType::Function;
	}

	inline BaseClass* getObject() const
	{
		return m_callableObject.first;
	}

	template<typename ObjectType, typename ClassType, typename ClassReturnType, typename CT1>
	Callback& set ( ObjectType* pObject,
					 ClassReturnType ( ClassType::*method ) ( CT1 ) )
	{
		static_assert ( std::is_base_of<BaseClass, ClassType>::value,
						"CCObject must be the base of used class" );

		static_assert ( std::is_base_of<ClassType, ObjectType>::value,
						"Pointer to method must be the base of object's pointer" );

		m_pointerType = PointerType::NonConstMethod;
		MethodPointer methodPointer;
		methodPointer.nonConstMethod =
			static_cast<ReturnType ( BaseClass::* ) ( T1 ) > ( method );

		m_callableObject = std::make_pair ( pObject, methodPointer );
		return *this;
	}

	template<typename ObjectType, typename ClassType, typename ClassReturnType, typename CT1 >
	Callback& set ( ObjectType* pObject,
					 ClassReturnType ( ClassType::*method ) ( CT1 ) const )
	{
		static_assert ( std::is_base_of<BaseClass, ClassType>::value,
						"CCObject must be the base of used class" );

		static_assert ( std::is_base_of<ClassType, ObjectType>::value,
						"Pointer to method must be the base of object's pointer" );

		m_pointerType = PointerType::ConstMethod;
		MethodPointer methodPointer;
		methodPointer.constMethod =
			static_cast<ReturnType ( BaseClass::* ) ( T1 ) const> ( method );

		m_callableObject = std::make_pair ( pObject, methodPointer );
		return *this;
	}

	template<typename FunctionType>
	Callback& set ( const FunctionType& functor )
	{
		static_assert (
			std::is_convertible<FunctionType, std::function<ReturnType ( T1 ) >>::value,
			"Parameter must be either function (static method) pointer or non-capturing lambda" );

		m_pointerType = PointerType::Function;
		MethodPointer methodPointer;
		methodPointer.function = static_cast<ReturnType ( * ) ( T1 ) > ( functor );

		m_callableObject = std::make_pair ( nullptr, methodPointer );
		return *this;
	}

	ReturnType call ( T1 param1 ) const
	{
		CALLBACK_ASSERT ( isCallable() );

		if ( m_pointerType == PointerType::ConstMethod )
		{
			return ( m_callableObject.first->*m_callableObject.second.constMethod ) (
					   param1 );
		}

		else if ( m_pointerType == PointerType::NonConstMethod )
		{
			return ( m_callableObject.first->*m_callableObject.second.nonConstMethod ) (
					   param1 );
		}

		return ( *m_callableObject.second.function ) ( param1 );
	}

private:

	union MethodPointer
	{
		ReturnType ( BaseClass::*nonConstMethod ) ( T1 );
		ReturnType ( BaseClass::*constMethod ) ( T1 ) const;
		ReturnType ( *function ) ( T1 );
	};

	enum class PointerType : unsigned char
	{
		NonConstMethod,
		ConstMethod,
		Function
	};

	typedef std::pair<BaseClass*, MethodPointer> ObjectMethodPair;
	ObjectMethodPair m_callableObject;
	PointerType m_pointerType;
};


template<typename ReturnType, typename ObjectType, typename MethodType, typename T1>
Callback<ReturnType ( T1 ) > makeCallback ( ObjectType* pObject,
		ReturnType ( MethodType::*method ) ( T1 ) )
{
	return Callback<ReturnType ( T1 ) > ( pObject, method );
}

template<typename ReturnType, typename ObjectType, typename MethodType, typename T1>
Callback<ReturnType ( T1 ) > makeCallback ( ObjectType* pObject,
		ReturnType ( MethodType::*method ) ( T1 ) const )
{
	return Callback<ReturnType ( T1 ) > ( pObject, method );
}

template<typename ReturnType, typename T1>
Callback<ReturnType ( T1 ) > makeCallback ( ReturnType ( *function ) ( T1 ) )
{
	return Callback<ReturnType ( T1 ) > ( function );
}

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

template<typename ReturnType, typename T1, typename T2>
class Callback<ReturnType ( T1, T2 ) >
{

public:

	Callback() :
		m_pointerType ( PointerType::NonConstMethod )
	{
	}

	template<typename ClassType, typename MethodType>
	Callback ( ClassType* pObject, const MethodType& method ) :
		m_pointerType ( PointerType::NonConstMethod )
	{
		static_assert ( std::is_member_function_pointer<MethodType>::value,
						"The second argument must be a pointer to a method" );
		set ( pObject, method );
	}

	template<typename FunctionType>
	Callback ( const FunctionType& functor ) :
		m_pointerType ( PointerType::Function )
	{
		set ( functor );
	}

	Callback ( const Callback& otherCallback )
	{
		m_callableObject = otherCallback.m_callableObject;
		m_pointerType = otherCallback.m_pointerType;
	}

	Callback& operator= ( const Callback& otherCallback )
	{
		m_callableObject = otherCallback.m_callableObject;
		m_pointerType = otherCallback.m_pointerType;
		return *this;
	}

	inline bool isCallable() const
	{
		return m_callableObject.first || m_pointerType == PointerType::Function;
	}

	inline BaseClass* getObject() const
	{
		return m_callableObject.first;
	}

	template<typename ObjectType, typename ClassType, typename ClassReturnType, typename CT1, typename CT2>
	Callback& set ( ObjectType* pObject,
					 ClassReturnType ( ClassType::*method ) ( CT1, CT2 ) )
	{
		static_assert ( std::is_base_of<BaseClass, ClassType>::value,
						"CCObject must be the base of used class" );

		static_assert ( std::is_base_of<ClassType, ObjectType>::value,
						"Pointer to method must be the base of object's pointer" );

		m_pointerType = PointerType::NonConstMethod;
		MethodPointer methodPointer;
		methodPointer.nonConstMethod =
			static_cast<ReturnType ( BaseClass::* ) ( T1, T2 ) > ( method );

		m_callableObject = std::make_pair ( pObject, methodPointer );
		return *this;
	}

	template<typename ObjectType, typename ClassType, typename ClassReturnType, typename CT1, typename CT2 >
	Callback& set ( ObjectType* pObject,
					 ClassReturnType ( ClassType::*method ) ( CT1, CT2 ) const )
	{
		static_assert ( std::is_base_of<BaseClass, ClassType>::value,
						"CCObject must be the base of used class" );

		static_assert ( std::is_base_of<ClassType, ObjectType>::value,
						"Pointer to method must be the base of object's pointer" );

		m_pointerType = PointerType::ConstMethod;
		MethodPointer methodPointer;
		methodPointer.constMethod =
			static_cast<ReturnType ( BaseClass::* ) ( T1, T2 ) const> ( method );

		m_callableObject = std::make_pair ( pObject, methodPointer );
		return *this;
	}

	template<typename FunctionType>
	Callback& set ( const FunctionType& functor )
	{
		static_assert (
			std::is_convertible<FunctionType, std::function<ReturnType ( T1, T2 ) >>::value,
			"Parameter must be either function (static method) pointer or non-capturing lambda" );

		m_pointerType = PointerType::Function;
		MethodPointer methodPointer;
		methodPointer.function = static_cast<ReturnType ( * ) ( T1, T2 ) > ( functor );

		m_callableObject = std::make_pair ( nullptr, methodPointer );
		return *this;
	}

	ReturnType call ( T1 param1, T2 param2 ) const
	{
		CALLBACK_ASSERT ( isCallable() );

		if ( m_pointerType == PointerType::ConstMethod )
		{
			return ( m_callableObject.first->*m_callableObject.second.constMethod ) (
					   param1, param2 );
		}

		else if ( m_pointerType == PointerType::NonConstMethod )
		{
			return ( m_callableObject.first->*m_callableObject.second.nonConstMethod ) (
					   param1, param2 );
		}

		return ( *m_callableObject.second.function ) ( param1, param2 );
	}

private:

	union MethodPointer
	{
		ReturnType ( BaseClass::*nonConstMethod ) ( T1, T2 );
		ReturnType ( BaseClass::*constMethod ) ( T1, T2 ) const;
		ReturnType ( *function ) ( T1, T2 );
	};

	enum class PointerType : unsigned char
	{
		NonConstMethod,
		ConstMethod,
		Function
	};

	typedef std::pair<BaseClass*, MethodPointer> ObjectMethodPair;
	ObjectMethodPair m_callableObject;
	PointerType m_pointerType;
};


template<typename ReturnType, typename ObjectType, typename MethodType, typename T1, typename T2>
Callback<ReturnType ( T1, T2 ) > makeCallback ( ObjectType* pObject,
		ReturnType ( MethodType::*method ) ( T1, T2 ) )
{
	return Callback<ReturnType ( T1, T2 ) > ( pObject, method );
}

template<typename ReturnType, typename ObjectType, typename MethodType, typename T1, typename T2>
Callback<ReturnType ( T1, T2 ) > makeCallback ( ObjectType* pObject,
		ReturnType ( MethodType::*method ) ( T1, T2 ) const )
{
	return Callback<ReturnType ( T1, T2 ) > ( pObject, method );
}

template<typename ReturnType, typename T1, typename T2>
Callback<ReturnType ( T1, T2 ) > makeCallback ( ReturnType ( *function ) ( T1,
		T2 ) )
{
	return Callback<ReturnType ( T1, T2 ) > ( function );
}

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

template<typename ReturnType, typename T1, typename T2, typename T3>
class Callback<ReturnType ( T1, T2, T3 ) >
{

public:

	Callback() :
		m_pointerType ( PointerType::NonConstMethod )
	{
	}

	template<typename ClassType, typename MethodType>
	Callback ( ClassType* pObject, const MethodType& method ) :
		m_pointerType ( PointerType::NonConstMethod )
	{
		static_assert ( std::is_member_function_pointer<MethodType>::value,
						"The second argument must be a pointer to a method" );
		set ( pObject, method );
	}

	template<typename FunctionType>
	Callback ( const FunctionType& functor ) :
		m_pointerType ( PointerType::Function )
	{
		set ( functor );
	}

	Callback ( const Callback& otherCallback )
	{
		m_callableObject = otherCallback.m_callableObject;
		m_pointerType = otherCallback.m_pointerType;
	}

	Callback& operator= ( const Callback& otherCallback )
	{
		m_callableObject = otherCallback.m_callableObject;
		m_pointerType = otherCallback.m_pointerType;
		return *this;
	}

	inline bool isCallable() const
	{
		return m_callableObject.first || m_pointerType == PointerType::Function;
	}

	inline BaseClass* getObject() const
	{
		return m_callableObject.first;
	}

	template<typename ObjectType, typename ClassType, typename ClassReturnType, typename CT1, typename CT2, typename CT3>
	Callback& set ( ObjectType* pObject,
					 ClassReturnType ( ClassType::*method ) ( CT1, CT2, CT3 ) )
	{
		static_assert ( std::is_base_of<BaseClass, ClassType>::value,
						"CCObject must be the base of used class" );

		static_assert ( std::is_base_of<ClassType, ObjectType>::value,
						"Pointer to method must be the base of object's pointer" );

		m_pointerType = PointerType::NonConstMethod;
		MethodPointer methodPointer;
		methodPointer.nonConstMethod =
			static_cast<ReturnType ( BaseClass::* ) ( T1, T2, T3 ) > ( method );

		m_callableObject = std::make_pair ( pObject, methodPointer );
		return *this;
	}

	template<typename ObjectType, typename ClassType, typename ClassReturnType, typename CT1, typename CT2, typename CT3 >
	Callback& set ( ObjectType* pObject,
					 ClassReturnType ( ClassType::*method ) ( CT1, CT2, CT3 ) const )
	{
		static_assert ( std::is_base_of<BaseClass, ClassType>::value,
						"CCObject must be the base of used class" );

		static_assert ( std::is_base_of<ClassType, ObjectType>::value,
						"Pointer to method must be the base of object's pointer" );

		m_pointerType = PointerType::ConstMethod;
		MethodPointer methodPointer;
		methodPointer.constMethod =
			static_cast<ReturnType ( BaseClass::* ) ( T1, T2, T3 ) const> ( method );

		m_callableObject = std::make_pair ( pObject, methodPointer );
		return *this;
	}

	template<typename FunctionType>
	Callback& set ( const FunctionType& functor )
	{
		static_assert (
			std::is_convertible<FunctionType, std::function<ReturnType ( T1, T2, T3 ) >>::value,
			"Parameter must be either function (static method) pointer or non-capturing lambda" );

		m_pointerType = PointerType::Function;
		MethodPointer methodPointer;
		methodPointer.function = static_cast<ReturnType ( * ) ( T1, T2, T3 ) >
								 ( functor );

		m_callableObject = std::make_pair ( nullptr, methodPointer );
		return *this;
	}

	ReturnType call ( T1 param1, T2 param2, T3 param3 ) const
	{
		CALLBACK_ASSERT ( isCallable() );

		if ( m_pointerType == PointerType::ConstMethod )
		{
			return ( m_callableObject.first->*m_callableObject.second.constMethod ) (
					   param1, param2, param3 );
		}

		else if ( m_pointerType == PointerType::NonConstMethod )
		{
			return ( m_callableObject.first->*m_callableObject.second.nonConstMethod ) (
					   param1, param2, param3 );
		}

		return ( *m_callableObject.second.function ) ( param1, param2, param3 );
	}

private:

	union MethodPointer
	{
		ReturnType ( BaseClass::*nonConstMethod ) ( T1, T2, T3 );
		ReturnType ( BaseClass::*constMethod ) ( T1, T2, T3 ) const;
		ReturnType ( *function ) ( T1, T2, T3 );
	};

	enum class PointerType : unsigned char
	{
		NonConstMethod,
		ConstMethod,
		Function
	};

	typedef std::pair<BaseClass*, MethodPointer> ObjectMethodPair;
	ObjectMethodPair m_callableObject;
	PointerType m_pointerType;
};


template<typename ReturnType, typename ObjectType, typename MethodType, typename T1, typename T2, typename T3>
Callback<ReturnType ( T1, T2, T3 ) > makeCallback ( ObjectType* pObject,
		ReturnType ( MethodType::*method ) ( T1, T2, T3 ) )
{
	return Callback<ReturnType ( T1, T2, T3 ) > ( pObject, method );
}

template<typename ReturnType, typename ObjectType, typename MethodType, typename T1, typename T2, typename T3>
Callback<ReturnType ( T1, T2, T3 ) > makeCallback ( ObjectType* pObject,
		ReturnType ( MethodType::*method ) ( T1, T2, T3 ) const )
{
	return Callback<ReturnType ( T1, T2, T3 ) > ( pObject, method );
}

template<typename ReturnType, typename T1, typename T2, typename T3>
Callback<ReturnType ( T1, T2, T3 ) > makeCallback ( ReturnType ( *function ) (
			T1, T2, T3 ) )
{
	return Callback<ReturnType ( T1, T2, T3 ) > ( function );
}

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

template<typename ReturnType, typename T1, typename T2, typename T3, typename T4>
class Callback<ReturnType ( T1, T2, T3, T4 ) >
{

public:

	Callback() :
		m_pointerType ( PointerType::NonConstMethod )
	{
	}

	template<typename ClassType, typename MethodType>
	Callback ( ClassType* pObject, const MethodType& method ) :
		m_pointerType ( PointerType::NonConstMethod )
	{
		static_assert ( std::is_member_function_pointer<MethodType>::value,
						"The second argument must be a pointer to a method" );
		set ( pObject, method );
	}

	template<typename FunctionType>
	Callback ( const FunctionType& functor ) :
		m_pointerType ( PointerType::Function )
	{
		set ( functor );
	}

	Callback ( const Callback& otherCallback )
	{
		m_callableObject = otherCallback.m_callableObject;
		m_pointerType = otherCallback.m_pointerType;
	}

	Callback& operator= ( const Callback& otherCallback )
	{
		m_callableObject = otherCallback.m_callableObject;
		m_pointerType = otherCallback.m_pointerType;
		return *this;
	}

	inline bool isCallable() const
	{
		return m_callableObject.first || m_pointerType == PointerType::Function;
	}

	inline BaseClass* getObject() const
	{
		return m_callableObject.first;
	}

	template<typename ObjectType, typename ClassType, typename ClassReturnType, typename CT1, typename CT2, typename CT3, typename CT4>
	Callback& set ( ObjectType* pObject,
					 ClassReturnType ( ClassType::*method ) ( CT1, CT2, CT3, CT4 ) )
	{
		static_assert ( std::is_base_of<BaseClass, ClassType>::value,
						"CCObject must be the base of used class" );

		static_assert ( std::is_base_of<ClassType, ObjectType>::value,
						"Pointer to method must be the base of object's pointer" );

		m_pointerType = PointerType::NonConstMethod;
		MethodPointer methodPointer;
		methodPointer.nonConstMethod =
			static_cast<ReturnType ( BaseClass::* ) ( T1, T2, T3, T4 ) > ( method );

		m_callableObject = std::make_pair ( pObject, methodPointer );
		return *this;
	}

	template<typename ObjectType, typename ClassType, typename ClassReturnType, typename CT1, typename CT2, typename CT3, typename CT4 >
	Callback& set ( ObjectType* pObject,
					 ClassReturnType ( ClassType::*method ) ( CT1, CT2, CT3, CT4 ) const )
	{
		static_assert ( std::is_base_of<BaseClass, ClassType>::value,
						"CCObject must be the base of used class" );

		static_assert ( std::is_base_of<ClassType, ObjectType>::value,
						"Pointer to method must be the base of object's pointer" );

		m_pointerType = PointerType::ConstMethod;
		MethodPointer methodPointer;
		methodPointer.constMethod =
			static_cast<ReturnType ( BaseClass::* ) ( T1, T2, T3, T4 ) const> ( method );

		m_callableObject = std::make_pair ( pObject, methodPointer );
		return *this;
	}

	template<typename FunctionType>
	Callback& set ( const FunctionType& functor )
	{
		static_assert (
			std::is_convertible<FunctionType, std::function<ReturnType ( T1, T2, T3, T4 ) >>::value,
			"Parameter must be either function (static method) pointer or non-capturing lambda" );

		m_pointerType = PointerType::Function;
		MethodPointer methodPointer;
		methodPointer.function = static_cast<ReturnType ( * ) ( T1, T2, T3, T4 ) >
								 ( functor );

		m_callableObject = std::make_pair ( nullptr, methodPointer );
		return *this;
	}

	ReturnType call ( T1 param1, T2 param2, T3 param3, T4 param4 ) const
	{
		CALLBACK_ASSERT ( isCallable() );

		if ( m_pointerType == PointerType::ConstMethod )
		{
			return ( m_callableObject.first->*m_callableObject.second.constMethod ) (
					   param1, param2, param3, param4 );
		}

		else if ( m_pointerType == PointerType::NonConstMethod )
		{
			return ( m_callableObject.first->*m_callableObject.second.nonConstMethod ) (
					   param1, param2, param3, param4 );
		}

		return ( *m_callableObject.second.function ) ( param1, param2, param3, param4 );
	}

private:

	union MethodPointer
	{
		ReturnType ( BaseClass::*nonConstMethod ) ( T1, T2, T3, T4 );
		ReturnType ( BaseClass::*constMethod ) ( T1, T2, T3, T4 ) const;
		ReturnType ( *function ) ( T1, T2, T3, T4 );
	};

	enum class PointerType : unsigned char
	{
		NonConstMethod,
		ConstMethod,
		Function
	};

	typedef std::pair<BaseClass*, MethodPointer> ObjectMethodPair;
	ObjectMethodPair m_callableObject;
	PointerType m_pointerType;
};


template<typename ReturnType, typename ObjectType, typename MethodType, typename T1, typename T2, typename T3, typename T4>
Callback<ReturnType ( T1, T2, T3, T4 ) > makeCallback ( ObjectType* pObject,
		ReturnType ( MethodType::*method ) ( T1, T2, T3, T4 ) )
{
	return Callback<ReturnType ( T1, T2, T3, T4 ) > ( pObject, method );
}

template<typename ReturnType, typename ObjectType, typename MethodType, typename T1, typename T2, typename T3, typename T4>
Callback<ReturnType ( T1, T2, T3, T4 ) > makeCallback ( ObjectType* pObject,
		ReturnType ( MethodType::*method ) ( T1, T2, T3, T4 ) const )
{
	return Callback<ReturnType ( T1, T2, T3, T4 ) > ( pObject, method );
}

template<typename ReturnType, typename T1, typename T2, typename T3, typename T4>
Callback<ReturnType ( T1, T2, T3, T4 ) > makeCallback ( ReturnType (
			*function ) ( T1, T2, T3, T4 ) )
{
	return Callback<ReturnType ( T1, T2, T3, T4 ) > ( function );
}

#endif // msvc++


} // namespace Utils


#endif /* KOALA_CALLBACK_H_ */
