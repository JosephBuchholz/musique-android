package com.randsoft.apps.musique

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.*
import android.graphics.Paint
import android.graphics.pdf.PdfDocument
import android.os.Build
import android.util.AttributeSet
import android.util.DisplayMetrics
import android.util.Log
import android.util.TypedValue
import android.view.MotionEvent
import android.view.View
import android.view.WindowManager
import androidx.core.content.ContextCompat
import androidx.core.content.res.ResourcesCompat
import androidx.core.graphics.drawable.DrawableCompat
import androidx.lifecycle.LiveData
import com.randsoft.apps.musique.framedata.FrameData
import com.randsoft.apps.musique.renderdata.*
import java.util.*

private const val TAG = "MusicDisplayView"

// view that displays sheet music and tab
class MusicDisplayView(context: Context, attrs: AttributeSet? = null): View(context, attrs) {

    var renderData: RenderData? = null
    var printRenderData: PrintRenderData? = null
    var frameData: FrameData? = null

    var renderDataChanged = false

    // equal to a tenth of a gap between two lines in standard notation
    var scale = 1.0f
    var bitmapSizeScale = scale

    private var positionX = 150.0f
    private var positionY = 150.0f

    private var mainBitmapPositionX = 0.0f
    private var mainBitmapPositionY = 0.0f

    var deviceWidth: Int = 0
    var deviceHeight: Int = 0
    var deviceDpi: Int = 0

    var typefacePlain = ResourcesCompat.getFont(context, R.font.open_sans)
    var typefaceItalic = ResourcesCompat.getFont(context, R.font.open_sans_italic)

    private val backgroundPaint = Paint().apply {
        color = 0xffffffff.toInt()
    }
    private val blackPaint = Paint().apply {
        color = 0xff000000.toInt()
    }
    private val visiblePaint = Paint().apply {
        color = 0xffff00ff.toInt()
    }
    private val linePaint = Paint().apply {
        color = 0xff000000.toInt()
        strokeWidth = 3.0f
    }
    private val textPaint = Paint().apply {
        color = 0xffff0000.toInt()
        textSize = 30.0f
        textAlign = Paint.Align.CENTER
    }

    private var quarterNoteBitmap: Bitmap? = null

    private var mainBitmap: Bitmap? = null
    private var firstRender = true;

    private var bitmaps: MutableMap<Int, Bitmap?> = mutableMapOf()

    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
        Log.d(TAG, "onAttachedToWindow----------------------------------")
        calculateDeviceDisplayMetrics()
        Log.d(TAG, "Device Display Metrics: w: $deviceWidth, h: $deviceHeight, dpi: $deviceDpi")
    }

    private fun calculateDeviceDisplayMetrics() {
        // getting device width and height
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            val windowsManager = context?.getSystemService(Context.WINDOW_SERVICE) as WindowManager
            val bounds = windowsManager.currentWindowMetrics.bounds
            deviceWidth = bounds.width()
            deviceHeight = bounds.height()
            deviceDpi = resources.configuration.densityDpi
        } else {
            val displayMetrics = DisplayMetrics()
            val windowsManager = context?.getSystemService(Context.WINDOW_SERVICE) as WindowManager
            windowsManager.defaultDisplay.getMetrics(displayMetrics)
            deviceWidth = displayMetrics.widthPixels
            deviceHeight = displayMetrics.heightPixels
            deviceDpi = displayMetrics.densityDpi
        }
    }

    private fun renderOnToBitmap()
    {
        if (renderData != null) {
            Log.w(TAG, "START RENDER ${renderData?.lines?.size}")
            val density = resources.displayMetrics.density
            val sizeX = 5000 * scale // in tenths
            val sizeY = 5000 * scale // in tenths

            // create the bitmap
            mainBitmap = Bitmap.createBitmap(sizeX.toInt(), sizeY.toInt(), Bitmap.Config.ARGB_8888)

            // "connect/link" the canvas to the bitmap so that the canvas draws on the bitmap
            val mainCanvas = Canvas(mainBitmap!!)
            //mainCanvas.drawPaint(backgroundPaint)

            for (line in renderData?.lines!!) {
                val paint = Paint().apply {
                    color = line.paint.color
                    strokeWidth = line.paint.strokeWidth * scale
                    isAntiAlias = true
                    strokeCap = Paint.Cap.values()[line.paint.strokeCap]
                }
                drawLine(mainCanvas, line, paint, 0.0f, 0.0f)
            }

            for (text in renderData?.texts!!) {
                //typeface.style = Typeface.BOLD
                val paint = Paint().apply {
                    color = text.paint.color
                    textSize = text.paint.textSize // 30.0 text size =about= 22.0 normal size
                    textAlign = Paint.Align.values()[text.paint.align]
                    isAntiAlias = true
                }

                if (text.paint.isItalic) {
                    paint.typeface = typefaceItalic
                }
                else {
                    paint.typeface = typefacePlain
                }
                drawText(mainCanvas, text, paint, 0.0f, 0.0f)
            }

            for (bitmap in renderData?.bitmaps!!) {
                val paint = Paint().apply {
                    color = bitmap.paint.color
                }

                val resourceId = getResourceID(bitmap.assetId) // resource id of the bitmap
                val drawable =
                    ContextCompat.getDrawable(context, resourceId) // the drawable of the bitmap

                // calculate centers of bitmaps
                var centerX = (drawable!!.intrinsicWidth.toFloat() / density)
                var centerY = 0.0f
                if (bitmap.assetId == AssetID.QuarterNote.ordinal || bitmap.assetId == AssetID.EightNote.ordinal) {
                    centerX = 5.65f
                    centerY = 5.0f
                } else if (bitmap.assetId == AssetID.WholeNote.ordinal ||
                    bitmap.assetId == AssetID.HalfNoteNoteHead.ordinal ||
                    bitmap.assetId == AssetID.QuarterNoteNoteHead.ordinal
                ) {
                    centerX = (drawable.intrinsicWidth.toFloat() / density)
                    centerY = (drawable.intrinsicHeight.toFloat() / density) / 2.0f
                } else if (bitmap.assetId == AssetID.Sharp.ordinal ||
                    bitmap.assetId == AssetID.Natural.ordinal
                ) {
                    centerX = (drawable.intrinsicWidth.toFloat() / density)
                    centerY = (drawable.intrinsicHeight.toFloat() / density) / 2.0f
                } else if (bitmap.assetId == AssetID.Flat.ordinal) {
                    centerX = (drawable.intrinsicWidth.toFloat() / density)
                    centerY = (drawable.intrinsicHeight.toFloat() / density) * 0.25f
                } else if (bitmap.assetId == AssetID.TrebleClef.ordinal) {
                    centerY = 17.0f
                } else if (bitmap.assetId == AssetID.QuarterRest.ordinal) {
                    centerX = (drawable.intrinsicWidth.toFloat() / density)
                    centerY = (drawable.intrinsicHeight.toFloat() / density) / 2.0f
                } else if (bitmap.assetId == AssetID.WholeRest.ordinal) {
                    centerX = (drawable.intrinsicWidth.toFloat() / density)
                    centerY = (drawable.intrinsicHeight.toFloat() / density)
                }

                val left =
                    (((bitmap.x) * scale) - (((drawable.intrinsicWidth.toFloat() / density) - centerX) * bitmapSizeScale) + 0.0f).toInt()
                val top =
                    (((bitmap.y) * scale) - (((drawable.intrinsicHeight.toFloat() / density) - centerY) * bitmapSizeScale) + 0.0f).toInt()
                val right =
                    ((drawable.intrinsicWidth.toFloat() / density) * bitmap.sx * bitmapSizeScale).toInt() + left
                val bottom =
                    ((drawable.intrinsicHeight.toFloat() / density) * bitmap.sy * bitmapSizeScale).toInt() + top

                drawable.setTint(paint.color)
                drawable.setBounds(left, top, right, bottom)
                drawable.draw(mainCanvas)
            }

            // render cubic bezier curves
            for (curve in renderData?.cubicCurves!!) {
                val paint = Paint().apply {
                    color = curve.paint.color
                    strokeWidth = curve.paint.strokeWidth
                    isAntiAlias = true
                    strokeCap = Paint.Cap.values()[curve.paint.strokeCap]
                    style = Paint.Style.STROKE
                }

                var path = Path()
                var s = 5.0f;
                var start = PointF(curve.x1 * scale + positionX, curve.y1 * scale + positionY)
                var point1 = PointF(curve.x2 * scale + positionX, curve.y2 * scale + positionY)
                var point2 = PointF(curve.x3 * scale + positionX, curve.y3 * scale + positionY)
                var end = PointF(curve.x4 * scale + positionX, curve.y4 * scale + positionY)

                path.moveTo(start.x, start.y)
                path.cubicTo(point1.x, point1.y, point2.x, point2.y, end.x, end.y)

                mainCanvas.drawPath(path, paint)

                var i = 0;
                while (i < 2) {
                    point1 = PointF(point1.x, point1.y - 1.0f * scale)
                    point2 = PointF(point2.x, point2.y - 1.0f * scale)
                    path.reset()
                    path.moveTo(start.x, start.y)
                    path.cubicTo(point1.x, point1.y, point2.x, point2.y, end.x, end.y)

                    mainCanvas.drawPath(path, paint)
                    i++
                }
            }
        }
        else
        {
            Log.e(TAG, "RENDER DATA IS NULL")
        }
    }

    fun renderDataIsDifferent(previousRenderData: RenderData, newRenderData: RenderData): Boolean
    {
        if (previousRenderData.lines.size != newRenderData.lines.size ||
            previousRenderData.bitmaps.size != newRenderData.bitmaps.size ||
            previousRenderData.texts.size != newRenderData.texts.size) { // they are different
            return true
        }

        return false // they are not different
    }

    @SuppressLint("DrawAllocation")
    override fun onDraw(canvas: Canvas) {
        canvas.drawPaint(backgroundPaint) // fill canvas

        // calculate positionX (the position of the sheet music)
        if (frameData != null) {
            if ((frameData!!.playLinePosition * scale) + positionX >= deviceWidth.toFloat() / 3.0f) {
                positionX += (deviceWidth.toFloat() / 3.0f) - ((frameData!!.playLinePosition * scale) + positionX)
            } else if ((frameData!!.playLinePosition * scale) + positionX <= deviceWidth.toFloat() / 8.0f) {
                positionX += (deviceWidth.toFloat() / 8.0f) - ((frameData!!.playLinePosition * scale) + positionX)
            }

            if ((frameData!!.playLinePositionY * scale) + positionY >= deviceHeight.toFloat() / 3.0f) {
                positionY += (deviceHeight.toFloat() / 3.0f) - ((frameData!!.playLinePositionY * scale) + positionY)
            } else if ((frameData!!.playLinePositionY * scale) + positionY <= deviceHeight.toFloat() / 8.0f) {
                positionY += (deviceHeight.toFloat() / 8.0f) - ((frameData!!.playLinePositionY * scale) + positionY)
            }
        }

        if (renderData != null) {

            if (renderDataChanged) { // render renderData on to a bitmap if the render data has changed from last time
                renderOnToBitmap()
                renderDataChanged = false
            }

            if (mainBitmap != null)
            {
                // translate bitmap
                val mat = Matrix()
                mat.setTranslate(mainBitmapPositionX + positionX, mainBitmapPositionY + positionY)

                // draw the bitmap that has renderData rendered on it
                canvas.drawBitmap(mainBitmap!!, mat, visiblePaint)
            }
        }

        // draw frame data (includes: play line)
        if (frameData != null) {
            val paint = Paint().apply {
                color = 0xff0044dd.toInt()
                strokeWidth = 1.25f * scale
                isAntiAlias = true
                strokeCap = Paint.Cap.SQUARE
            }

            // draw play line
            //canvas.drawLine((0.0f * scale), (0.0f * scale), (frameData!!.playLinePosition * scale), ((frameData!!.playLinePositionY + frameData!!.playLineHeight) * scale), paint)
            drawLine(canvas, Line(frameData!!.playLinePosition, frameData!!.playLinePositionY, frameData!!.playLinePosition, frameData!!.playLinePositionY + frameData!!.playLineHeight), paint)
            //drawLine(canvas, Line(0.0f, 0.0f, frameData!!.playLinePosition, frameData!!.playLinePositionY + frameData!!.playLineHeight), paint)
        }
    }

    private fun millimetersToPoints(millimeters: Float): Float // converts from millimeters to points (1/72 inch)
    {
        return (millimeters / 25.4f) * 75.0f; // divide by 25.4 to get inches then multiply by 75 to get points
    }

    private fun pointsToMillimeters(pt: Float): Float // converts from points (1/72 inch) to millimeters
    {
        return (pt / 75.0f) * 25.4f; // divide by 75 to get inches then multiply by 25.4 to get millimeters
    }

    private fun pointsToTenths(pt: Float, scaling: Scaling): Float // converts from points (1/72 inch) to tenths (defined at top)
    {
        val millimeters = pointsToMillimeters(pt); // convert points to millimeters
        return (millimeters / scaling.millimeters) * scaling.tenths; // convert to tenths
    }

    private fun tenthsToPoints(tenths: Float, scaling: Scaling): Float // converts from tenths (defined at top) to points (1/72 inch)
    {
        val millimeters = (tenths / scaling.tenths) * scaling.millimeters; // convert tenths to millimeters
        return millimetersToPoints(millimeters); // convert to points
    }

    private var margin = 72.0f; // in points (1/72 inch)

    fun drawPage(page: PdfDocument.Page)
    {
        page.canvas.drawPaint(backgroundPaint) // fill canvas
        val density = resources.displayMetrics.density

        if (printRenderData != null) {

            val mainCanvas = page.canvas
            //mainCanvas.drawPaint(backgroundPaint)

            val pageRenderData: RenderData = printRenderData?.pages?.get(page.info.pageNumber)!!

            scale = tenthsToPoints(1.0f, pageRenderData.scaling)
            bitmapSizeScale = scale

            for (line in pageRenderData.lines) {
                val paint = Paint().apply {
                    color = line.paint.color
                    strokeWidth = line.paint.strokeWidth * scale
                    isAntiAlias = true
                    strokeCap = Paint.Cap.values()[line.paint.strokeCap]
                }
                drawLine(mainCanvas, line, paint, 0.0f, 0.0f)
            }

            for (text in pageRenderData.texts) {
                //typeface.style = Typeface.BOLD
                val paint = Paint().apply {
                    color = text.paint.color
                    textSize = text.paint.textSize // 30.0 text size =about= 22.0 normal size
                    textAlign = Paint.Align.values()[text.paint.align]
                    isAntiAlias = true
                }

                if (text.paint.isItalic) {
                    paint.typeface = typefaceItalic
                }
                else {
                    paint.typeface = typefacePlain
                }
                drawText(mainCanvas, text, paint, 0.0f, 0.0f)
            }

            for (bitmap in pageRenderData.bitmaps) {
                val paint = Paint().apply {
                    color = bitmap.paint.color
                }

                val resourceId = getResourceID(bitmap.assetId) // resource id of the bitmap
                val drawable =
                    ContextCompat.getDrawable(context, resourceId) // the drawable of the bitmap

                // calculate centers of bitmaps
                var centerX = (drawable!!.intrinsicWidth.toFloat() / density)
                var centerY = 0.0f
                if (bitmap.assetId == AssetID.QuarterNote.ordinal || bitmap.assetId == AssetID.EightNote.ordinal) {
                    centerX = 5.65f
                    centerY = 5.0f
                } else if (bitmap.assetId == AssetID.WholeNote.ordinal ||
                    bitmap.assetId == AssetID.HalfNoteNoteHead.ordinal ||
                    bitmap.assetId == AssetID.QuarterNoteNoteHead.ordinal
                ) {
                    centerX = (drawable.intrinsicWidth.toFloat() / density)
                    centerY = (drawable.intrinsicHeight.toFloat() / density) / 2.0f
                } else if (bitmap.assetId == AssetID.Sharp.ordinal ||
                    bitmap.assetId == AssetID.Natural.ordinal
                ) {
                    centerX = (drawable.intrinsicWidth.toFloat() / density)
                    centerY = (drawable.intrinsicHeight.toFloat() / density) / 2.0f
                } else if (bitmap.assetId == AssetID.Flat.ordinal) {
                    centerX = (drawable.intrinsicWidth.toFloat() / density)
                    centerY = (drawable.intrinsicHeight.toFloat() / density) * 0.25f
                } else if (bitmap.assetId == AssetID.TrebleClef.ordinal) {
                    centerY = 17.0f
                } else if (bitmap.assetId == AssetID.QuarterRest.ordinal) {
                    centerX = (drawable.intrinsicWidth.toFloat() / density)
                    centerY = (drawable.intrinsicHeight.toFloat() / density) / 2.0f
                } else if (bitmap.assetId == AssetID.WholeRest.ordinal) {
                    centerX = (drawable.intrinsicWidth.toFloat() / density)
                    centerY = (drawable.intrinsicHeight.toFloat() / density)
                }

                val left =
                    (((bitmap.x) * scale) - (((drawable.intrinsicWidth.toFloat() / density) - centerX) * bitmapSizeScale) + 0.0f).toInt()
                val top =
                    (((bitmap.y) * scale) - (((drawable.intrinsicHeight.toFloat() / density) - centerY) * bitmapSizeScale) + 0.0f).toInt()
                val right =
                    ((drawable.intrinsicWidth.toFloat() / density) * bitmap.sx * bitmapSizeScale).toInt() + left
                val bottom =
                    ((drawable.intrinsicHeight.toFloat() / density) * bitmap.sy * bitmapSizeScale).toInt() + top

                drawable.setTint(paint.color)
                drawable.setBounds(left, top, right, bottom)
                drawable.draw(mainCanvas)
            }

            // render cubic bezier curves
            for (curve in pageRenderData.cubicCurves) {
                val paint = Paint().apply {
                    color = curve.paint.color
                    strokeWidth = curve.paint.strokeWidth
                    isAntiAlias = true
                    strokeCap = Paint.Cap.values()[curve.paint.strokeCap]
                    style = Paint.Style.STROKE
                }

                var path = Path()
                var s = 5.0f;
                var start = PointF(curve.x1 * scale + positionX, curve.y1 * scale + positionY)
                var point1 = PointF(curve.x2 * scale + positionX, curve.y2 * scale + positionY)
                var point2 = PointF(curve.x3 * scale + positionX, curve.y3 * scale + positionY)
                var end = PointF(curve.x4 * scale + positionX, curve.y4 * scale + positionY)

                path.moveTo(start.x, start.y)
                path.cubicTo(point1.x, point1.y, point2.x, point2.y, end.x, end.y)

                mainCanvas.drawPath(path, paint)

                var i = 0;
                while (i < 2) {
                    point1 = PointF(point1.x, point1.y - 1.0f * scale)
                    point2 = PointF(point2.x, point2.y - 1.0f * scale)
                    path.reset()
                    path.moveTo(start.x, start.y)
                    path.cubicTo(point1.x, point1.y, point2.x, point2.y, end.x, end.y)

                    mainCanvas.drawPath(path, paint)
                    i++
                }
            }
        }
    }

    // gets the resourceID for the given assetID
    private fun getResourceID(assetID: Int): Int {
        return when (assetID) {
            AssetID.WholeNote.ordinal -> { R.drawable.whole_note } // notes
            AssetID.HalfNote.ordinal -> { R.drawable.whole_note }
            AssetID.QuarterNote.ordinal -> { R.drawable.quarter_note }
            AssetID.EightNote.ordinal -> { R.drawable.quarter_note }
            AssetID.SixteenthNote.ordinal -> { R.drawable.quarter_note }
            AssetID.Time1.ordinal -> { R.drawable.time_3 } // time signatures
            AssetID.Time2.ordinal -> { R.drawable.time_2 }
            AssetID.Time3.ordinal -> { R.drawable.time_3 }
            AssetID.Time4.ordinal -> { R.drawable.time_4 }
            AssetID.Time5.ordinal -> { R.drawable.time_3 }
            AssetID.Time6.ordinal -> { R.drawable.time_6 }
            AssetID.Time7.ordinal -> { R.drawable.time_3 }
            AssetID.Time8.ordinal -> { R.drawable.time_8 }
            AssetID.Time9.ordinal -> { R.drawable.time_3 }
            AssetID.Time0.ordinal -> { R.drawable.time_3 }
            AssetID.TrebleClef.ordinal -> { R.drawable.treble_clef } // clefs
            AssetID.BassClef.ordinal -> { R.drawable.bass_clef }
            AssetID.AltoClef.ordinal -> { R.drawable.treble_clef }
            AssetID.TABClef.ordinal -> { R.drawable.tab_clef }
            AssetID.PercussionClef.ordinal -> { R.drawable.treble_clef }
            AssetID.QuarterNoteNoteHead.ordinal -> { R.drawable.quarter_note_notehead } // noteheads
            AssetID.HalfNoteNoteHead.ordinal -> { R.drawable.half_note_notehead }
            AssetID.XNoteHead.ordinal -> { R.drawable.quarter_note_notehead }
            AssetID.Sharp.ordinal -> { R.drawable.sharp } // accidentals
            AssetID.Flat.ordinal -> { R.drawable.flat }
            AssetID.Natural.ordinal -> { R.drawable.natural }
            AssetID.DoubleSharp.ordinal -> { R.drawable.sharp }
            AssetID.DoubleFlat.ordinal -> { R.drawable.flat }
            AssetID.WholeRest.ordinal -> { R.drawable.whole_and_half_rest } // rests
            AssetID.HalfRest.ordinal -> { R.drawable.whole_and_half_rest }
            AssetID.QuarterRest.ordinal -> { R.drawable.quarter_rest }
            AssetID.EightRest.ordinal -> { R.drawable.eighth_rest }
            AssetID.SixteenthRest.ordinal -> { R.drawable.sixteenth_rest }
            AssetID.ThirtySecondRest.ordinal -> { R.drawable.thirty_second_rest }
            AssetID.KeySignature1Sharp.ordinal -> { R.drawable.key_signature_1_sharp } // key signatures
            AssetID.KeySignature2Sharps.ordinal -> { R.drawable.key_signature_2_sharps }
            AssetID.KeySignature3Sharps.ordinal -> { R.drawable.key_signature_3_sharps }
            AssetID.KeySignature4Sharps.ordinal -> { R.drawable.key_signature_4_sharps }
            AssetID.KeySignature5Sharps.ordinal -> { R.drawable.key_signature_5_sharps }
            AssetID.KeySignature6Sharps.ordinal -> { R.drawable.key_signature_6_sharps }
            AssetID.KeySignature7Sharps.ordinal -> { R.drawable.key_signature_7_sharps }
            AssetID.KeySignature1Flat.ordinal -> { R.drawable.thirty_second_rest }
            AssetID.KeySignature2Flats.ordinal -> { R.drawable.thirty_second_rest }
            AssetID.KeySignature3Flats.ordinal -> { R.drawable.key_signature_3_flats }
            AssetID.KeySignature4Flats.ordinal -> { R.drawable.thirty_second_rest }
            AssetID.KeySignature5Flats.ordinal -> { R.drawable.thirty_second_rest }
            AssetID.KeySignature6Flats.ordinal -> { R.drawable.thirty_second_rest }
            AssetID.KeySignature7Flats.ordinal -> { R.drawable.thirty_second_rest }
            else -> { R.drawable.ic_launcher_background }
        }
    }

    private fun drawLine(canvas: Canvas, line: Line, paint: Paint) {
        canvas.drawLine((line.startX * scale) + positionX, (line.startY * scale) + positionY, (line.endX * scale) + positionX, (line.endY * scale) + positionY, paint)
    }

    private fun drawText(canvas: Canvas, text: Text, paint: Paint) {
        paint.textSize = paint.textSize * scale // scale textSize to the right size

        // calculate bounds of the text
        var textBounds = Rect()
        paint.getTextBounds(text.text, 0, text.text.length, textBounds)

        val textHeight = -textBounds.top // text height

        // draw
        canvas.drawText(text.text, (text.x * scale) + positionX, (text.y * scale) + (textHeight / 2.0f) + positionY, paint)
    }

    private fun drawLine(canvas: Canvas, line: Line, paint: Paint, px: Float, py: Float) {
        canvas.drawLine((line.startX * scale) + px, (line.startY * scale) + py, (line.endX * scale) + px, (line.endY * scale) + py, paint)
    }

    private fun drawText(canvas: Canvas, text: Text, paint: Paint, px: Float, py: Float) {
        paint.textSize = paint.textSize * scale // scale textSize to the right size

        // calculate bounds of the text
        var textBounds = Rect()
        paint.getTextBounds(text.text, 0, text.text.length, textBounds)

        val textHeight = -textBounds.top // text height

        // draw
        canvas.drawText(text.text, (text.x * scale) + px, (text.y * scale) + (textHeight / 2.0f) + py, paint)
    }

    private fun drawDebugPoint(canvas: Canvas, point: PointF, paint: Paint = visiblePaint) {
        canvas.drawRect(RectF(point.x, point.y, point.x + 6.0f, point.y + 6.0f), paint)
    }
}