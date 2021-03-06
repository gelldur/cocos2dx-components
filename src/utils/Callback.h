/*
* Callback.h
*
*  Created on: Dec 28, 2013
*      Author: Jakub 'kuhar' Kuderski , Dawid Drozd
*/

#pragma once

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

using BaseClass = cocos2d::CCObject;

template<typename T>
class Callback;

template<typename ReturnType, typename... Args>
class Callback<ReturnType( Args... )>
{
private:
	enum class PointerType : unsigned char
	{
		Unknown,
		NonConstMethod,
		ConstMethod,
		Function
	};

public:

	Callback() = default;

	template<typename ClassType, typename MethodType>
	Callback( ClassType* pObject, const MethodType& method ) :
		m_pointerType( PointerType::NonConstMethod )
	{
		static_assert( std::is_member_function_pointer<MethodType>::value,
					   "The second argument must be a pointer to a method" );
		set( pObject, method );
	}

	template<typename FunctionType>
	Callback( const FunctionType& functor ) :
		m_pointerType( PointerType::Function )
	{
		set( functor );
	}

	Callback( const Callback& otherCallback )
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
	Callback& set( ObjectType* pObject,
				   ClassReturnType( ClassType::*method )( ClassArgs... ) )
	{
		static_assert( std::is_base_of<BaseClass, ClassType>::value,
					   "CCObject must be the base of used class" );

		static_assert( std::is_base_of<ClassType, ObjectType>::value,
					   "Pointer to method must be the base of object's pointer" );

		m_pointerType = PointerType::NonConstMethod;
		MethodPointer methodPointer;
		methodPointer.nonConstMethod =
			static_cast<ReturnType( BaseClass::* )( Args... )> ( method );

		m_callableObject = std::make_pair( pObject, methodPointer );
		return *this;
	}

	template<typename ObjectType, typename ClassType, typename ClassReturnType, typename... ClassArgs>
	Callback& set( ObjectType* pObject,
				   ClassReturnType( ClassType::*method )( ClassArgs... ) const )
	{
		static_assert( std::is_base_of<BaseClass, ClassType>::value,
					   "CCObject must be the base of used class" );

		static_assert( std::is_base_of<ClassType, ObjectType>::value,
					   "Pointer to method must be the base of object's pointer" );

		m_pointerType = PointerType::ConstMethod;
		MethodPointer methodPointer;
		methodPointer.constMethod =
			static_cast<ReturnType( BaseClass::* )( Args... ) const> ( method );

		m_callableObject = std::make_pair( pObject, methodPointer );
		return *this;
	}

	template<typename FunctionType>
	Callback& set( const FunctionType& functor )
	{
		static_assert(
			std::is_convertible<FunctionType, std::function<ReturnType( Args... )>>::value,
			"Parameter must be either function (static method) pointer or non-capturing lambda. Or simply your arguments doesn't match." );

		m_pointerType = PointerType::Function;
		MethodPointer methodPointer;
		methodPointer.function = static_cast<ReturnType( * )( Args... )> ( functor );

		m_callableObject = std::make_pair( nullptr, methodPointer );
		return *this;
	}

	template<typename... CallArgs>
	ReturnType call( CallArgs&& ... params ) const
	{
		CALLBACK_ASSERT( isCallable() );
		CALLBACK_ASSERT( m_pointerType != PointerType::Unknown );

		if( m_pointerType == PointerType::NonConstMethod )
		{
			return ( m_callableObject.first->*m_callableObject.second.nonConstMethod )(
					   std::forward<CallArgs> ( params )... );
		}

		if( m_pointerType == PointerType::ConstMethod )
		{
			return ( m_callableObject.first->*m_callableObject.second.constMethod )(
					   std::forward<CallArgs> ( params )... );
		}


		return ( *m_callableObject.second.function )( std::forward<CallArgs> ( params )... );
	}

	template<typename... CallArgs>
	void callIfCallable( CallArgs&& ... params ) const
	{
		if( isCallable() )
		{
			call( std::forward<CallArgs> ( params )... );
		}
	}

#ifdef DEBUG
#pragma GCC diagnostic push  // require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"

	//I know this isn't good way to do this but i don't have any other option for now
	void* getFunctionPointer()const
	{
		if( m_pointerType == PointerType::NonConstMethod )
		{
			return ( void*& )m_callableObject.second.nonConstMethod;
		}

		if( m_pointerType == PointerType::ConstMethod )
		{
			return ( void*& )m_callableObject.second.nonConstMethod;
		}

		return ( void*& )m_callableObject.second.function;
	}
#pragma GCC diagnostic pop   // require GCC 4.6
#endif

private:

	union MethodPointer
	{
		ReturnType( BaseClass::*nonConstMethod )( Args... );
		ReturnType( BaseClass::*constMethod )( Args... ) const;
		ReturnType( *function )( Args... );
	};

	typedef std::pair<BaseClass*, MethodPointer> ObjectMethodPair;
	ObjectMethodPair m_callableObject;
	PointerType m_pointerType = PointerType::Unknown;
};


template<typename ReturnType, typename ObjectType, typename MethodType, typename... Args>
Callback<ReturnType( Args... )> makeCallback( ObjectType* pObject,
		ReturnType( MethodType::*method )( Args... ) )
{
	return Callback<ReturnType( Args... )> ( pObject, method );
}

template<typename ReturnType, typename ObjectType, typename MethodType, typename... Args>
Callback<ReturnType( Args... )> makeCallback( ObjectType* pObject,
		ReturnType( MethodType::*method )( Args... ) const )
{
	return Callback<ReturnType( Args... )> ( pObject, method );
}

template<typename ReturnType, typename... Args>
Callback<ReturnType( Args... )> makeCallback( ReturnType( *function )(
			Args... ) )
{
	return Callback<ReturnType( Args... )> ( function );
}

} // namespace Utils
