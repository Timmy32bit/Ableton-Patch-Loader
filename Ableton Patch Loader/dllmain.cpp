#include <Windows.h>
#include <iostream>

// if some retard makes a push request to make this more "std compliant", i will osint your name and bdsm your anus :)
void load_patches() {
    wchar_t base_path[MAX_PATH];
    GetModuleFileNameW(nullptr, base_path, MAX_PATH);

    wchar_t* last_slash = wcsrchr(base_path, L'\\');
    
    if (!last_slash) return;

    *(last_slash + 1) = L'\0';

    std::wstring search_path = std::wstring(base_path) + L"patches\\*.dll";

    WIN32_FIND_DATAW find_data;
    HANDLE hFind = FindFirstFileW(search_path.c_str(), &find_data);

    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do {
        if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            std::wstring full_path = std::wstring(base_path) + L"patches\\" + find_data.cFileName;
            LoadLibraryW(full_path.c_str());
        }
    } while (FindNextFileW(hFind, &find_data));

    FindClose(hFind);
}

int main() {
    
    load_patches();

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

