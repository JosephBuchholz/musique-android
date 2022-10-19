package com.randsoft.apps.musique.printing

import android.content.Context
import android.graphics.Color
import android.graphics.Paint
import android.graphics.pdf.PdfDocument
import android.os.Bundle
import android.os.CancellationSignal
import android.os.ParcelFileDescriptor
import android.print.PageRange
import android.print.PrintAttributes
import android.print.PrintDocumentAdapter
import android.print.PrintDocumentInfo
import android.print.pdf.PrintedPdfDocument
import android.util.Log
import java.io.FileOutputStream
import java.io.IOException

private const val TAG = "PrintAdapter"

class PrintAdapter(private val context: Context) : PrintDocumentAdapter()
{
    private var pdfDocument: PrintedPdfDocument? = null
    private var totalPages = 1
    private var writtenPages: MutableList<Int> = mutableListOf()

    interface Callbacks {
        fun onDrawPage(page: PdfDocument.Page)
        fun onCalculateNumPages(): Int
    }

    private var callbacks: Callbacks? = null

    fun setCallbacks(c: Callbacks?) {
        callbacks = c
    }

    override fun onStart() {
        super.onStart()
        Log.i(TAG, "Starting printing");
    }

    override fun onFinish() {
        super.onFinish()
        Log.i(TAG, "Finished printing");
    }

    override fun onLayout(
        oldAttributes: PrintAttributes?,
        newAttributes: PrintAttributes,
        cancellationSignal: CancellationSignal?,
        callback: LayoutResultCallback,
        extras: Bundle?
    ) {
        Log.i(TAG, "onLayout");
        var layoutChanged = true
        pdfDocument = PrintedPdfDocument(context, newAttributes) // create document

        // if the job was canceled
        if (cancellationSignal?.isCanceled == true)
        {
            callback.onLayoutCancelled()
            return
        }

        var pages = callbacks?.onCalculateNumPages()!! // calculate num pages here
        if (pages > 0) {
            PrintDocumentInfo.Builder("print_output.pdf")
                .setContentType(PrintDocumentInfo.CONTENT_TYPE_DOCUMENT)
                .setPageCount(pages)
                .build().also { info ->
                    callback.onLayoutFinished(info, layoutChanged) // done
                }
        }
        else { // there is an error
            callback.onLayoutFailed("Failed to calculate the number of pages.")
        }
    }

    override fun onWrite(
        pageRanges: Array<out PageRange>,
        destination: ParcelFileDescriptor,
        cancellationSignal: CancellationSignal?,
        callback: WriteResultCallback
    ) {
        Log.i(TAG, "onWrite");

        for (i in 0 until totalPages) {
            if (isInPageRanges(pageRanges, i)) {
                writtenPages.add(writtenPages.size, i)
                pdfDocument?.startPage(i)?.also { page ->

                    // if the job was canceled
                    if (cancellationSignal?.isCanceled == true) {
                        callback.onWriteCancelled()
                        pdfDocument?.close()
                        pdfDocument = null
                        return
                    }

                    // draw the page
                    //drawPage(page);
                    callbacks?.onDrawPage(page)

                    // finished with pdf
                    pdfDocument?.finishPage(page)
                }
            }
        }

        // write the pdf to file
        try {
            pdfDocument?.writeTo(FileOutputStream(destination.fileDescriptor))
        } catch (e: IOException) {
            callback.onWriteFailed(e.toString())
            return
        } finally {
            pdfDocument?.close()
            pdfDocument = null
        }

        callback.onWriteFinished(pageRanges) // done
    }

    private fun drawPage(page: PdfDocument.Page) {
        page.canvas.apply {

            // units are in points (1/72 of an inch)
            val titleBaseLine = 200f
            val leftMargin = 54f

            val paint = Paint()
            paint.color = Color.BLACK
            paint.textSize = 36f
            drawText("Test Title .............. ", leftMargin, titleBaseLine, paint)

            paint.textSize = 11f
            drawText("Test paragraph", leftMargin, titleBaseLine + 25, paint)

            paint.color = Color.RED
            drawRect(100f, 100f, 172f, 172f, paint)
            paint.color = Color.GREEN
            drawRect(0.0f, 0.0f, 72f, 72f, paint)
        }
    }

    private fun isInPageRanges(pageRanges: Array<out PageRange>, page: Int): Boolean
    {
        for (i in pageRanges)
        {
            if (page >= i.start && page <= i.end)
            {
                return true
            }
        }

        return false
    }
}