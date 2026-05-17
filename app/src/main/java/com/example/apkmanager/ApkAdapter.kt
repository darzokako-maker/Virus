package com.example.apkmanager

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView

class ApkAdapter(
    private val apkList: List<ApkItem>,
    private val onItemClick: (ApkItem) -> Unit
) : RecyclerView.Adapter<ApkAdapter.ApkViewHolder>() {

    class ApkViewHolder(view: View) : RecyclerView.ViewHolder(view) {
        val imgIcon: ImageView = view.findViewById(R.id.img_android_icon)
        val txtName: TextView = view.findViewById(R.id.txt_app_name)
        val txtPackage: TextView = view.findViewById(R.id.txt_package_name)
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ApkViewHolder {
        val view = LayoutInflater.from(parent.context).inflate(R.layout.item_apk, parent, false)
        return ApkViewHolder(view)
    }

    override fun onBindViewHolder(holder: ApkViewHolder, position: Int) {
        val item = apkList[position]
        holder.txtName.text = item.name
        holder.txtPackage.text = item.packageName
        holder.imgIcon.setImageDrawable(item.icon)

        holder.itemView.setOnClickListener { onItemClick(item) }
    }

    override fun getItemCount(): Int = apkList.size
}

