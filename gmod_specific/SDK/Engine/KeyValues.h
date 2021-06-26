#pragma once
#include "../../../include/stdafx.h"



//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//


#ifdef _WIN32
#pragma once
#endif

// #include <vgui/VGUI.h>

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#include "utlvector.h"

#define FOR_EACH_SUBKEY( kvRoot, kvSubKey ) \
	for ( KeyValues * kvSubKey = kvRoot->GetFirstSubKey(); kvSubKey != NULL; kvSubKey = kvSubKey->GetNextKey() )

#define FOR_EACH_TRUE_SUBKEY( kvRoot, kvSubKey ) \
	for ( KeyValues * kvSubKey = kvRoot->GetFirstTrueSubKey(); kvSubKey != NULL; kvSubKey = kvSubKey->GetNextTrueSubKey() )

#define FOR_EACH_VALUE( kvRoot, kvValue ) \
	for ( KeyValues * kvValue = kvRoot->GetFirstValue(); kvValue != NULL; kvValue = kvValue->GetNextValue() )

class IBaseFileSystem;
class CUtlBuffer;
class Color;
typedef void * FileHandle_t;
class CKeyValuesGrowableStringTable;


class KeyValuess {
public:
	void Init();
	PVOID Operator_New();

	PVOID Constructor(PVOID kv, const char *command);

	void SetInt(PVOID kv, const char *Name, int Value);//

	bool LoadFromBuffer(PVOID kv, const char* resourceName, const char* pBuffer, void* pFileSystem = nullptr, const char* pPathID = nullptr);
private:
	PVOID Operator_NewPtr;
	PVOID ConstructorPtr;
	PVOID SetIntPtr;
	PVOID LoadFromBufferPtr;
};

extern KeyValuess g_KeyValues;