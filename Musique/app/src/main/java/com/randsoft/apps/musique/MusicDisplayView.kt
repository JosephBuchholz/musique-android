package com.randsoft.apps.musique

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.*
import android.os.Build
import android.util.AttributeSet
import android.util.DisplayMetrics
import android.util.Log
import android.view.MotionEvent
import android.view.View
import android.view.WindowManager
import androidx.core.content.ContextCompat
import androidx.core.graphics.drawable.DrawableCompat
import androidx.lifecycle.LiveData
import com.randsoft.apps.musique.framedata.FrameData
import com.randsoft.apps.musique.renderdata.Line
import com.randsoft.apps.musique.renderdata.RenderBitmap
import com.randsoft.apps.musique.renderdata.RenderData
import com.randsoft.apps.musique.renderdata.Text

private const val TAG = "MusicDisplayView"

// view that displays sheet music and tab
class MusicDisplayView(context: Context, attrs: AttributeSet? = null): View(context, attrs) {

    var renderData: RenderData? = null
    var frameData: FrameData? = null

    // equal to a tenth of a gap between two lines in standard notation
    var scale = 1.0f
    var bitmapSizeScale = scale

    private var positionX = 150.0f
    private var positionY = 150.0f

    var deviceWidth: Int = 0
    var deviceHeight: Int = 0
    var deviceDpi: Int = 0

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

    // for function see https://stackoverflow.com/questions/33696488/getting-bitmap-from-vector-drawable#:~:text=If%20you%20are%20willing%20to%20use%20Android%20KTX,or%20val%20bitmap%20%3D%20AppCompatResources.getDrawable%20%28context%2C%20drawableId%29.toBitmap%20%28%29
    private fun getBitmapFromVectorDrawable(drawableId: Int, sx: Float, sy: Float): Bitmap? {
        val density = resources.displayMetrics.density
        val drawable = ContextCompat.getDrawable(context, drawableId)
        val bitmap = Bitmap.createBitmap(((drawable!!.intrinsicWidth.toFloat() / density) * sx).toInt(),
            ((drawable.intrinsicHeight.toFloat() / density) * sy).toInt(), Bitmap.Config.ARGB_8888)
        val canvas = Canvas(bitmap)
        drawable.setBounds(0, 0, canvas.width, canvas.height)
        drawable.draw(canvas)
        return bitmap
    }

    @SuppressLint("DrawAllocation")
    override fun onDraw(canvas: Canvas) {
        canvas.drawPaint(backgroundPaint)

        if (frameData != null) {
            if ((frameData!!.playLinePosition*scale) + positionX >= deviceWidth.toFloat()/3.0f) {
                positionX += (deviceWidth.toFloat()/3.0f) - ((frameData!!.playLinePosition*scale) + positionX)
            } else if ((frameData!!.playLinePosition*scale) + positionX <= deviceWidth.toFloat()/8.0f) {
                positionX += (deviceWidth.toFloat()/8.0f) - ((frameData!!.playLinePosition*scale) + positionX)
            }
        }

        /*if (bitmap == null) {
            bitmap = BitmapFactory.decodeResource(resources, R.drawable.ic_launcher_foreground)
        }*/

        if (renderData != null) {

            for (line in renderData?.lines!!) {
                val paint = Paint().apply {
                    color = line.paint.color
                    strokeWidth = line.paint.strokeWidth * scale
                    isAntiAlias = true
                    strokeCap = Paint.Cap.values()[line.paint.strokeCap]
                }
                drawLine(canvas, line, paint)
            }

            for (text in renderData?.texts!!) {
                val paint = Paint().apply {
                    color = text.paint.color
                    textSize = text.paint.textSize // 30.0 text size =about= 22.0 normal size
                    textAlign = Paint.Align.CENTER
                    isAntiAlias = true
                }
                drawText(canvas, text, paint)
            }

            for (bitmap in renderData?.bitmaps!!) {
                val paint = Paint().apply {
                    color = bitmap.paint.color
                }

                val mat = Matrix()
                val resourceId = getResourceID(bitmap.assetId)
                val drawable = ContextCompat.getDrawable(context, resourceId)
                //Log.v(TAG, "w: ${drawable!!.intrinsicWidth}, h: ${drawable.intrinsicHeight}, d: ${getResources().getDisplayMetrics().density}")
                //mat.setRotate(50.0f)
                //mat.setScale(drawable!!.intrinsicWidth * bitmap.sx * bitmapSizeScale, drawable.intrinsicHeight.toFloat() * bitmap.sy * bitmapSizeScale)
                val density = resources.displayMetrics.density

                var centerX = (drawable!!.intrinsicWidth.toFloat() / density)
                var centerY = 0.0f
                if (bitmap.assetId == AssetID.QuarterNote.ordinal || bitmap.assetId == AssetID.EightNote.ordinal) {
                    centerX = 5.65f
                    centerY = 5.0f
                } else if (bitmap.assetId == AssetID.WholeNote.ordinal ||
                            bitmap.assetId == AssetID.HalfNoteNoteHead.ordinal ||
                            bitmap.assetId == AssetID.QuarterNoteNoteHead.ordinal) {
                    centerX = (drawable.intrinsicWidth.toFloat() / density)
                    centerY = (drawable.intrinsicHeight.toFloat() / density) / 2.0f
                } else if (bitmap.assetId == AssetID.Sharp.ordinal ||
                    bitmap.assetId == AssetID.Natural.ordinal) {
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

                mat.setTranslate(((bitmap.x) * scale) - (((drawable!!.intrinsicWidth.toFloat() / density) - centerX) * bitmapSizeScale) + positionX, ((bitmap.y) * scale) - (((drawable.intrinsicHeight.toFloat() / density) - centerY) * bitmapSizeScale) + positionY)

                //mat.setTranslate(((bitmap.x) * scale) - (drawable!!.intrinsicWidth.toFloat() - 7.149f) + positionX, ((bitmap.y) * scale) - (drawable.intrinsicHeight.toFloat() - 4.477f) + positionY)
                canvas.drawBitmap(getBitmapFromVectorDrawable(resourceId, bitmap.sx * bitmapSizeScale, bitmap.sy * bitmapSizeScale)!!, mat, paint)
                /*canvas.drawLine(10.0f, 10.0f, 10.0f, 47.2f, visiblePaint)
                canvas.drawLine(20.0f, 10.0f, 20.0f, 56.0f + 10.0f, visiblePaint)
                canvas.drawLine(20.0f, 10.0f, 20.0f + 23.0f, 10.0f, visiblePaint)*/
                //val w = bitmap.sx
               // val h = bitmap.sy
               // val dst = RectF(((bitmap.x) * scale) - (drawable!!.intrinsicWidth.toFloat() - 7.149f) + positionX,
                //    ((bitmap.y) * scale) - (drawable.intrinsicHeight.toFloat() - 4.477f)  + positionY,
                //    w,
                //    h)

                //canvas.drawBitmap(getBitmapFromVectorDrawable(resourceId)!!, Rect(0.0f,0.0f, drawable.intrinsicWidth.toFloat(), drawable.intrinsicHeight.toFloat()), dst, paint)

                /*val mat = Matrix()
                if (bitmap.assetId == AssetID.QuarterNote.ordinal) {
                    if (quarterNoteBitmap != null) {
                        val drawable = ContextCompat.getDrawable(context, R.drawable.quarter_note)
                        mat.setScale(drawable!!.intrinsicWidth * bitmap.sx * bitmapSizeScale, drawable.intrinsicHeight.toFloat() * bitmap.sy * bitmapSizeScale)
                        mat.setTranslate(((bitmap.x) * scale) - (drawable.intrinsicWidth.toFloat() - 7.149f) + positionX, ((bitmap.y) * scale) - (drawable.intrinsicHeight.toFloat() - 4.477f)  + positionY)
                        canvas.drawBitmap(quarterNoteBitmap!!, mat, paint)
                    } else {
                        quarterNoteBitmap = getBitmapFromVectorDrawable(R.drawable.quarter_note)
                        val drawable = ContextCompat.getDrawable(context, R.drawable.quarter_note)
                        mat.setScale(drawable!!.intrinsicWidth * bitmap.sx * bitmapSizeScale, drawable.intrinsicHeight.toFloat() * bitmap.sy * bitmapSizeScale)
                        mat.setTranslate(((bitmap.x) * scale) - (drawable.intrinsicWidth.toFloat() - 7.149f) + positionX, ((bitmap.y) * scale) - (drawable.intrinsicHeight.toFloat() - 4.477f)  + positionY)
                        canvas.drawBitmap(quarterNoteBitmap!!, mat, paint)
                    }
                }*/

                //canvas.drawRect((bitmap.x * scale) + positionX, (bitmap.y * scale) + positionY, ((bitmap.x * scale) + positionX) + 6.0f, ((bitmap.y * scale) + positionY) + 6.0f, visiblePaint)
            }

            /*val mat = Matrix()
            mat.setTranslate(1.0f * scale, 2.0f * scale)
            if (quarterNoteBitmap != null) {
                canvas.drawBitmap(quarterNoteBitmap!!, mat, null)
            } else {
                quarterNoteBitmap = getBitmapFromVectorDrawable(R.drawable.quarter_note)
                canvas.drawBitmap(quarterNoteBitmap!!, mat, null)
            }*/
        }

        if (frameData != null) {
            val paint = Paint().apply {
                color = 0xff0044dd.toInt()
                strokeWidth = 1.25f * scale
                isAntiAlias = true
                strokeCap = Paint.Cap.SQUARE
            }
            drawLine(canvas, Line(frameData!!.playLinePosition, frameData!!.playLinePositionY, frameData!!.playLinePosition, frameData!!.playLinePositionY + frameData!!.playLineHeight), paint)
        }

        //drawLine(canvas, PointF(1.0f, 1.0f), PointF(12.0f, 12.0f), linePaint)
        //drawLine(canvas, PointF(12.0f, 12.0f), PointF(12.0f, 6.0f), linePaint)
        //canvas.drawText("1", 12.0f * scale, 12.0f * scale, textPaint)
        //canvas.drawLine(1.0f, 1.0f, 12.0f, 12.0f, mainDisplayPaint)
        //canvas.drawLine(12.0f, 12.0f, 12.0f, 6.0f, mainDisplayPaint)
    }

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
            AssetID.Time6.ordinal -> { R.drawable.time_3 }
            AssetID.Time7.ordinal -> { R.drawable.time_3 }
            AssetID.Time8.ordinal -> { R.drawable.time_3 }
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
            AssetID.EightRest.ordinal -> { R.drawable.quarter_rest }
            AssetID.SixteenthRest.ordinal -> { R.drawable.quarter_rest }
            AssetID.KeySignature1Sharp.ordinal -> { R.drawable.key_signature_1_sharp } // key signatures
            AssetID.KeySignature2Sharps.ordinal -> { R.drawable.key_signature_2_sharps }
            AssetID.KeySignature3Sharps.ordinal -> { R.drawable.key_signature_3_sharps }
            AssetID.KeySignature4Sharps.ordinal -> { R.drawable.key_signature_4_sharps }
            AssetID.KeySignature5Sharps.ordinal -> { R.drawable.key_signature_5_sharps }
            AssetID.KeySignature6Sharps.ordinal -> { R.drawable.key_signature_6_sharps }
            AssetID.KeySignature7Sharps.ordinal -> { R.drawable.key_signature_7_sharps }
            AssetID.KeySignature1Flat.ordinal -> { R.drawable.quarter_rest }
            AssetID.KeySignature2Flats.ordinal -> { R.drawable.quarter_rest }
            AssetID.KeySignature3Flats.ordinal -> { R.drawable.key_signature_3_flats }
            AssetID.KeySignature4Flats.ordinal -> { R.drawable.quarter_rest }
            AssetID.KeySignature5Flats.ordinal -> { R.drawable.quarter_rest }
            AssetID.KeySignature6Flats.ordinal -> { R.drawable.quarter_rest }
            AssetID.KeySignature7Flats.ordinal -> { R.drawable.quarter_rest }
            else -> { R.drawable.ic_launcher_background }
        }
    }

    private fun drawLine(canvas: Canvas, line: Line, paint: Paint) {
        canvas.drawLine((line.startX * scale) + positionX, (line.startY * scale) + positionY, (line.endX * scale) + positionX, (line.endY * scale) + positionY, paint)
    }

    private fun drawText(canvas: Canvas, text: Text, paint: Paint) {
        canvas.drawText(text.text, (text.x * scale) + positionX, (text.y * scale) + positionY, paint)
        canvas.drawRect((text.x * scale) + positionX, (text.y * scale) + positionY, ((text.x * scale) + positionX) + 6.0f, ((text.y * scale) + positionY) + 6.0f, visiblePaint)
    }
}