package com.example.apkmanager

import android.graphics.drawable.Drawable

data class ApkItem(
    val name: String,
    val packageName: String,
    val apkPath: String,
    val icon: Drawable
)

