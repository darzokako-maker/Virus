package com.example.apkmanager

import android.content.ClipData
import android.content.ClipboardManager
import android.content.Context
import android.content.pm.ApplicationInfo
import android.content.pm.PackageManager
import android.os.Bundle
import android.widget.Button
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import java.io.File
import kotlin.concurrent.thread

class MainActivity : AppCompatActivity() {

    private lateinit var recyclerView: RecyclerView
    private lateinit var btnInstalledApks: Button
    private lateinit var btnFileManager: Button
    private val masterApkList = ArrayList<ApkItem>()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        recyclerView = findViewById(R.id.recyclerView)
        btnInstalledApks = findViewById(R.id.btn_installed_apks)
        btnFileManager = findViewById(R.id.btn_file_manager)

        recyclerView.layoutManager = LinearLayoutManager(this)

        // Buton Dinleyicileri
        btnInstalledApks.setOnClickListener {
            loadInstalledAppsAsync()
        }

        btnFileManager.setOnClickListener {
            Toast.makeText(this, "Dosya Sistemi Entegrasyonu Hazırlanıyor...", Toast.LENGTH_SHORT).show()
        }
    }

    /**
     * Cihazı yormamak adına asenkron (Arka plan iş parçacığı) APK taraması yapar.
     */
    private fun loadInstalledAppsAsync() {
        Toast.makeText(this, "Uygulamalar taranıyor...", Toast.LENGTH_SHORT).show()

        thread {
            val pm: PackageManager = packageManager
            val apps = pm.getInstalledApplications(PackageManager.GET_META_DATA)
            masterApkList.clear()

            for (app in apps) {
                // Sadece kullanıcı uygulamalarını filtrele (Temiz liste için)
                if ((app.flags and ApplicationInfo.FLAG_SYSTEM) == 0) {
                    val appName = app.loadLabel(pm).toString()
                    val packageName = app.packageName
                    val apkPath = app.sourceDir
                    val icon = app.loadIcon(pm)

                    masterApkList.add(ApkItem(appName, packageName, apkPath, icon))
                }
            }

            // Arayüz güncellemeleri ana iş parçacığında tetiklenir
            runOnUiThread {
                val adapter = ApkAdapter(masterApkList) { selectedItem ->
                    showApkActionMenu(selectedItem)
                }
                recyclerView.adapter = adapter
                Toast.makeText(this, "${masterApkList.size} Uygulama Hazır.", Toast.LENGTH_SHORT).show()
            }
        }
    }

    /**
     * Öğe seçildiğinde açılan MT Manager stili interaktif menü
     */
    private fun showApkActionMenu(item: ApkItem) {
        val options = arrayOf("APK Düzenle / Yedekle", "Dosya Yolunu Kopyala", "Detayları İncele")

        AlertDialog.Builder(this)
            .setTitle(item.name)
            .setItems(options) { _, which ->
                when (which) {
                    0 -> exportApkFile(item)
                    1 -> {
                        val clipboard = getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
                        val clip = ClipData.newPlainText("APK Path", item.apkPath)
                        clipboard.setPrimaryClip(clip)
                        Toast.makeText(this, "Yol panoya kopyalandı!", Toast.LENGTH_SHORT).show()
                    }
                    2 -> showAppDetailsDialog(item)
                }
            }
            .show()
    }

    /**
     * Seçilen uygulamanın orijinal base.apk dosyasını güvenli bir dizine kopyalar.
     */
    private fun exportApkFile(item: ApkItem) {
        Toast.makeText(this, "Dışa aktarılıyor...", Toast.LENGTH_SHORT).show()
        
        thread {
            try {
                val srcFile = File(item.apkPath)
                val destFolder = getExternalFilesDir(null) ?: filesDir
                val destFile = File(destFolder, "${item.name.replace(" ", "_")}_extracted.apk")

                srcFile.inputStream().use { input ->
                    destFile.outputStream().use { output ->
                        input.copyTo(output)
                    }
                }

                runOnUiThread {
                    Toast.makeText(this, "Başarıyla Kopyalandı:\n${destFile.absolutePath}", Toast.LENGTH_LONG).show()
                }
            } catch (e: Exception) {
                runOnUiThread {
                    Toast.makeText(this, "Hata Oluştu: ${e.message}", Toast.LENGTH_SHORT).show()
                }
            }
        }
    }

    private fun showAppDetailsDialog(item: ApkItem) {
        AlertDialog.Builder(this)
            .setTitle("Sistem Verisi")
            .setMessage("Paket Kimliği:\n${item.packageName}\n\nKaynak Dizini:\n${item.apkPath}")
            .setPositiveButton("Kapat", null)
            .show()
    }
}

