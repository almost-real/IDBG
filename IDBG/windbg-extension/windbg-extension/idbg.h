#pragma once
#include "stdafx.h"
#include <vector>

/*
note all of the functions in IDBG are base address independant
for ex if you pass 0x5000 in addbp it will set a bp to module_base_address+0x5000
*/
class IDBG
{
public:
	bool Init(PCSTR name_of_the_module);
	~IDBG();
	
	bool GetEip(PCSTR name_of_the_register, DEBUG_VALUE *val);
	bool AddBreakPoint(ULONG64 address);
	bool RemoveBreakpoint(ULONG64 address);

	
	bool IsInUse();
	bool Is64BitProcess();
	bool Is32BitProcess();


private:
	bool GetInterfaces();
	bool m_IsInUse = false;

	ULONG m_EIP_index = 0;
	ULONG64 m_baseaddr_of_module = 0;
	ULONG m_size_of_module = 0;

	IDebugControl4* gDebugControl4 = nullptr;

	IDebugSymbols*	  m_DebugSymbols = nullptr;
	IDebugControl*    m_DebugControl = nullptr;
	IDebugRegisters*  m_DebugRegisters = nullptr;
	IDebugClient*     m_DebugClient = nullptr;


	std::vector<IDebugBreakpoint*> m_Breakpoints;
};