#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>

uintptr_t get_iat_slot(HMODULE mod, const char* dll, const char* name) {
    auto base = (uint8_t*)mod;
    auto dos = (IMAGE_DOS_HEADER*)base;
    auto nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);

    auto& dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!dir.VirtualAddress) return 0;

    auto imp = (IMAGE_IMPORT_DESCRIPTOR*)(base + dir.VirtualAddress);
    for (; imp->Name; ++imp)
    {
        if (_stricmp((char*)(base + imp->Name), dll) != 0)
            continue;

        auto thunk = (IMAGE_THUNK_DATA*)(base + imp->FirstThunk);
        auto orig = (IMAGE_THUNK_DATA*)(base + imp->OriginalFirstThunk);

        for (; orig && orig->u1.AddressOfData; ++orig, ++thunk)
        {
            if (orig->u1.Ordinal & IMAGE_ORDINAL_FLAG) continue;

            auto ibn = (IMAGE_IMPORT_BY_NAME*)(base + orig->u1.AddressOfData);
            if (strcmp((char*)ibn->Name, name) == 0)
                return (uintptr_t)&thunk->u1.Function;
        }
    }
    return 0;
}


// basically just iterate executable sections until we find calls to ShellExecute
// all of the retarded startup shit Ableton does is via ShellExecute. 
// while i agree noping the caller is more "elegant", I cba trying to do portability and this was good enough for me so cry about it or fix it yourself
std::vector<uintptr_t> get_xrefs() {
    std::vector<uintptr_t> out;

    HMODULE self = GetModuleHandleW(nullptr);
    HMODULE shell = GetModuleHandleW(L"shell32.dll");

    if (!shell) shell = LoadLibraryW(L"shell32.dll");

    uintptr_t target = (uintptr_t)GetProcAddress(shell, "ShellExecuteExW");
    uintptr_t iat_slot = get_iat_slot(self, "shell32.dll", "ShellExecuteExW");

    auto base = (uint8_t*)self;
    auto dos = (IMAGE_DOS_HEADER*)base;
    auto nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);

    auto sec = IMAGE_FIRST_SECTION(nt);

    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++sec) {
        if (!(sec->Characteristics & IMAGE_SCN_MEM_EXECUTE))
            continue;

        uint8_t* p = base + sec->VirtualAddress;
        uint8_t* end = p + sec->Misc.VirtualSize;

        for (; p < end - 6; ++p) {
            
            if (p[0] == 0xFF && p[1] == 0x15) {
                int32_t disp = *(int32_t*)(p + 2);
                uintptr_t* slot = (uintptr_t*)(p + 6 + disp);

                if ((uintptr_t)slot == iat_slot)
                    out.push_back((uintptr_t)p);
                else if (!IsBadReadPtr(slot, sizeof(uintptr_t)) && *slot == target)
                    out.push_back((uintptr_t)p);
            }
        }
    }

    return out;
}

void do_nop(BYTE* address, size_t size) {
    DWORD oldProtect;

    if (VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        memset(address, 0x90, size);
        VirtualProtect(address, size, oldProtect, &oldProtect);
    }
}

int main() {

    auto calls = get_xrefs();

    for (size_t i = 0; i < calls.size(); i++)
        do_nop((BYTE*)calls[i], 6);

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        main();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

