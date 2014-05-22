#pragma once

namespace KoalaComponent
{

inline int _UniqueComponentIdNonDeterministic()
{
	static int id = 1001;
	return ++id;
}

template <typename Type>
inline int getIdForType()
{
	static int id_for_type = _UniqueComponentIdNonDeterministic();
	return id_for_type;
}

}

