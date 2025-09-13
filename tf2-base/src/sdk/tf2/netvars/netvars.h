#pragma once
#include "../cdll_int.h"
#include <cstdint>
#include "../../../utility/hash/hash.h"

namespace NetVars
{
	int GetOffset(RecvTable* pTable, const char* szNetVar);
	int GetNetVar(const char* szClass, const char* szNetVar);
}

#define NETVAR(_name, type, table, name) type &_name() \
{ \
	static int nOffset = NetVars::GetNetVar(table, name); \
	return *reinterpret_cast<type *>(std::uintptr_t(this) + nOffset); \
}