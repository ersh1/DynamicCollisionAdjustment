#include "Havok.h"

#include "Offsets.h"

RE::hkMemoryRouter& hkGetMemoryRouter()
{
	return *(RE::hkMemoryRouter*)(uintptr_t)TlsGetValue(*g_dwTlsIndex);
}
