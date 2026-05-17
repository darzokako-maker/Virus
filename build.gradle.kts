// Üst düzey build dosyası; eklentileri alt modüllere (app) dağıtır.
plugins {
    alias(libs.plugins.android.application) apply false
    alias(libs.plugins.kotlin.android) apply false
}

