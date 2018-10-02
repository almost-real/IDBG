#include "stdafx.h"
#include "idbg.h"


#define EXT_RELEASE(_Unk) \
    ((_Unk) != NULL ? ((_Unk)->Release(), (void)((_Unk) = NULL)) : (void)NULL)

/*template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}*/

bool IDBG::Is64BitProcess()
{
	ULONG type;
	m_DebugControl->GetActualProcessorType(&type);
	return type == IMAGE_FILE_MACHINE_IA64;
}

bool IDBG::Is32BitProcess()
{
	ULONG type;
	m_DebugControl->GetActualProcessorType(&type);
	return type == IMAGE_FILE_MACHINE_I386;
}

bool IDBG::IsInUse()
{
	return m_IsInUse;
}

bool IDBG::GetInterfaces()
{
	HRESULT hResult = S_FALSE;
	if (hResult = DebugCreate(__uuidof(IDebugClient), (void**)&m_DebugClient) != S_OK)
	{
		dprintf("Acuqiring IDebugClient* Failled\n\n");
		return false;

	}

	if (hResult = m_DebugClient->QueryInterface(__uuidof(IDebugControl), (void**)&m_DebugControl) != S_OK)
	{

		dprintf("Acuqiring IDebugControl* Failled\n\n");
		return false;
	}

	if (hResult = m_DebugClient->QueryInterface(__uuidof(IDebugRegisters), (void**)&m_DebugRegisters) != S_OK)
	{

		dprintf("Acuqiring IDebugRegisters* Failled\n\n");
		return false;
	}

	if (hResult = m_DebugClient->QueryInterface(__uuidof(IDebugSymbols), (void**)&m_DebugSymbols) != S_OK)
	{
		dprintf("Acuqiring IDebugSymbols* Failled\n\n");
		return false;
	}

	return true;
}

bool IDBG::AddBreakPoint(ULONG64 address_base0)
{
	static int desiredid = 0;

	if (address_base0 > m_size_of_module)
		return false;

	ULONG64 address = address_base0 + m_baseaddr_of_module;

	IDebugBreakpoint *bp;
	if (m_DebugControl->AddBreakpoint(DEBUG_BREAKPOINT_CODE, desiredid, &bp) != S_OK) //c'est quoi DEBUG_BREAKPOINT_DATA??
	{
		dprintf("IDebugControl::AddBreakpoint failed %d ", GetLastError());
		return false;
	}
	bp->SetOffset(address);
	bp->SetFlags(DEBUG_BREAKPOINT_ENABLED);
	m_Breakpoints.push_back(bp);

	desiredid++;
	return true;
}

bool IDBG::RemoveBreakpoint(ULONG64 address_base0)
{
	ULONG64 address = address_base0 + m_baseaddr_of_module;

	for (size_t i = 0; i < m_Breakpoints.size(); i++)
	{
		ULONG64 address_of_bp;
		m_Breakpoints[i]->GetOffset(&address_of_bp);

		if (address_of_bp == address)
		{
			if (m_DebugControl->RemoveBreakpoint(m_Breakpoints[i]) != S_OK)
				return false;
			
			m_Breakpoints.erase(m_Breakpoints.begin() + i);
			return true;
		}
	}
	return false;
}

/*
Input:Takes in parameter the name of the module we are working with (passed as argument in !load )
*/
bool IDBG::Init(PCSTR name_of_the_module)
{
	if (!GetInterfaces())
		return false;
	
	ULONG index_of_module;
	if (m_DebugSymbols->GetModuleByModuleName(name_of_the_module, 0, &index_of_module, &m_baseaddr_of_module) != S_OK)
		return false;

	DEBUG_MODULE_PARAMETERS params;
	if (m_DebugSymbols->GetModuleParameters(1, NULL, index_of_module, &params) != S_OK)
		return false;

	m_size_of_module = params.Size;
	
	m_IsInUse = true;
	return true;
}


bool IDBG::GetEip(PCSTR name_of_the_register, DEBUG_VALUE *val)
{
	if (m_DebugRegisters->GetValue(m_EIP_index, val) == S_OK)
	{
		if (val->I32 >= m_baseaddr_of_module && val->I32 < m_baseaddr_of_module + m_size_of_module)
		{
			val->I32 -= m_baseaddr_of_module;
			return true;
		}
	}
	return false;
}

IDBG::~IDBG()
{
	for (size_t i = 0; i < m_Breakpoints.size(); i++)
	{
		EXT_RELEASE(m_Breakpoints[i]); 
	}

	EXT_RELEASE(m_DebugSymbols);
	EXT_RELEASE(m_DebugRegisters);
	EXT_RELEASE(m_DebugControl);
	EXT_RELEASE(m_DebugClient);
}




