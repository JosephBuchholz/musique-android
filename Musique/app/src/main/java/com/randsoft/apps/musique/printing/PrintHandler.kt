package com.randsoft.apps.musique.printing

import android.content.Context
import android.graphics.pdf.PdfDocument
import android.print.PrintAttributes
import android.print.PrintManager
import com.randsoft.apps.musique.MusicDisplayFragment
import com.randsoft.apps.musique.R

// followed the tutorial here: https://developer.android.com/training/printing

class PrintHandler(private val context: Context) : PrintAdapter.Callbacks {

    interface Callbacks {
        fun onDrawPage(page: PdfDocument.Page)
        fun onCalculateNumPages(): Int
        fun onUpdateLayout(attributes: PrintAttributes): Boolean
    }

    private var callbacks: Callbacks? = null

    fun setCallbacks(c: Callbacks?) {
        callbacks = c
    }

    override fun onDrawPage(page: PdfDocument.Page) {
        callbacks?.onDrawPage(page)
    }

    override fun onCalculateNumPages(): Int {
        return callbacks?.onCalculateNumPages()!!
    }

    override fun onUpdateLayout(attributes: PrintAttributes): Boolean {
        return callbacks?.onUpdateLayout(attributes)!!
    }

    fun print()
    {
        val printManager = context.getSystemService(Context.PRINT_SERVICE) as PrintManager // get print manager
        val jobName = "${context.getString(R.string.app_name)} Music Document" // the name of the print job
        val printAdapter = PrintAdapter(context); // create print adapter
        printAdapter.setCallbacks(this);
        printManager.print(jobName, printAdapter, null); // start the print job
    }
}