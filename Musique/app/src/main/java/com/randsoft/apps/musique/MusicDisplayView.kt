package com.randsoft.apps.musique

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.*
import android.graphics.Paint
import android.graphics.pdf.PdfDocument
import android.os.Build
import android.text.Layout
import android.text.StaticLayout
import android.text.TextPaint
import android.util.AttributeSet
import android.util.DisplayMetrics
import android.util.Log
import android.view.*
import android.view.GestureDetector.SimpleOnGestureListener
import androidx.core.content.ContextCompat
import androidx.core.content.res.ResourcesCompat
import com.randsoft.apps.musique.event.InputEvent
import com.randsoft.apps.musique.framedata.FrameData
import com.randsoft.apps.musique.renderdata.*
import kotlinx.coroutines.internal.artificialFrame

private const val TAG = "MusicDisplayView"

const val SCALE_MAX = 4.0f
const val SCALE_MIN = 0.2f

/**
 * A view that displays Sheet Music and Tablature.
 */
class MusicDisplayView(context: Context, attrs: AttributeSet? = null): View(context, attrs) {

    var renderData: RenderData? = null
    var printRenderData: PrintRenderData? = null
    var frameData: FrameData? = null

    var renderDataChanged = false

    // equal to a tenth of a gap between two lines in standard notation
    var scale = 1.0f
    var scaling: Scaling = Scaling()

    var zoom = 1.0f

    var bitmapSizeScale = scale
    var mainScale = scale

    private var positionX = 150.0f
    private var positionY = 150.0f

    private var mainBitmapPositionX = 0.0f
    private var mainBitmapPositionY = 0.0f

    var deviceWidth: Int = 0
    var deviceHeight: Int = 0
    var deviceDpi: Int = 0

    private var typefacePlain = ResourcesCompat.getFont(context, R.font.times)
    private var typefaceItalic = ResourcesCompat.getFont(context, R.font.timesi)
    private var typefaceBold = ResourcesCompat.getFont(context, R.font.timesbd)
    private var typefaceBoldItalic = ResourcesCompat.getFont(context, R.font.timesbi)

    private var tablatureTypeface = ResourcesCompat.getFont(context, R.font.open_sans)

    var musicTypeface = ResourcesCompat.getFont(context, R.font.bravura)
    var musicTextTypeface = ResourcesCompat.getFont(context, R.font.bravura_text)

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

    interface Callbacks {
        fun onInputEvent(inputEvent: InputEvent)
    }

    private var callbacks: Callbacks? = null

    // touch/gesture variables

    private var scaleGestureDetector: ScaleGestureDetector = ScaleGestureDetector(context, object : ScaleGestureDetector.OnScaleGestureListener {
        override fun onScaleBegin(detector: ScaleGestureDetector): Boolean {
            return true;
        }

        override fun onScaleEnd(detector: ScaleGestureDetector) {
            /*scale = mainScale
            bitmapSizeScale = scale
            mainScale = 1.0f*/

            renderDataChanged = true
            invalidate()
        }

        override fun onScale(detector: ScaleGestureDetector): Boolean {
            Log.d(TAG, "scale")

            mainScale *= detector.scaleFactor
            mainScale = Math.max(SCALE_MIN, Math.min(mainScale, SCALE_MAX))

            invalidate()
            return true;
        }
    })

    private var gestureDetector: GestureDetector = GestureDetector(context, object : SimpleOnGestureListener() {
        override fun onScroll(
            e1: MotionEvent,
            e2: MotionEvent,
            distanceX: Float,
            distanceY: Float
        ): Boolean {

            positionX -= distanceX;
            positionY -= distanceY;

            invalidate()
            return true
        }

        override fun onSingleTapUp(e: MotionEvent): Boolean {

            if (callbacks != null) {
                val inputEvent = InputEvent()

                inputEvent.type = InputEvent.InputEventType.INPUT_EVENT_TAP

                val eventPosition = screenPositionToCanvasPosition(PointF(e.x, e.y))

                inputEvent.x = eventPosition.x
                inputEvent.y = eventPosition.y

                callbacks?.onInputEvent(inputEvent)
            }

            return super.onSingleTapUp(e)
        }
    })

    fun setCallbacks(cbs: Callbacks) {
        callbacks = cbs;
    }

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

    private fun convertPaint(oldPaint: com.randsoft.apps.musique.renderdata.Paint): Paint
    {
        val newPaint = Paint().apply {
            color = oldPaint.color.toInt()
            typeface = musicTypeface
            textAlign = Paint.Align.values()[oldPaint.align]
            isAntiAlias = oldPaint.isAntiAlias
        }

        newPaint.textSize = 40.0f * oldPaint.glyphSizeFactor * scale

        return newPaint
    }

    fun measureGlyph(glyph: SMuFLGlyph): Float
    {
        // create paint
        val paint = Paint().apply {
            color = glyph.paint.color.toInt()
            typeface = musicTypeface
        }

        paint.textSize = 40.0f * scale // text size equals the standard staff height (according to SMuFL specification)

        val width = paint.measureText(Character.toChars(glyph.codePoint), 0, 1) / scale

        return width
    }

    fun getGlyphBoundingBox(glyph: SMuFLGlyph): RectF {
        // create paint
        val paint = convertPaint(glyph.paint)

        val rect = Rect()
        paint.getTextBounds(Character.toChars(glyph.codePoint), 0, 1, rect)

        val rectF = RectF()
        rectF.top = rect.top / scale
        rectF.bottom = rect.bottom / scale
        rectF.right = rect.right / scale
        rectF.left = rect.left / scale

        return rectF
    }


    fun measureText(text: Text): RectF
    {
        // START OF ERROR

        // create paint
        val paint = Paint().apply {
            color = text.paint.color.toInt()
            textSize = text.paint.textSize
            textAlign = Paint.Align.values()[text.paint.align]
            isAntiAlias = true
        }

        paint.textSize = Conversions.pointsToTenths(text.paint.textSize, Scaling()) // LOTS OF ERRORS ON THIS LINE

        if (text.paint.isTablature) {
            paint.typeface = tablatureTypeface
            paint.textSize = text.paint.textSize
        }
        else if (text.paint.isItalic && text.paint.isBold) {
            paint.typeface = typefaceBoldItalic
        }
        else if (!text.paint.isItalic && text.paint.isBold) {
            paint.typeface = typefaceBold
        }
        else if (text.paint.isItalic) {
            paint.typeface = typefaceItalic
        }
        else {
            paint.typeface = typefacePlain
        }

        // END OF ERROR

        //val paint = Paint()

        val rect = Rect()
        //Log.e(TAG, "text: ${text.text}, t_length: ${text.text.length}");
        paint.getTextBounds(text.text, 0, text.text.length, rect) // this is the error line

        val rectF = RectF()
        rectF.top = rect.top / scale
        rectF.bottom = rect.bottom / scale
        rectF.right = rect.right / scale
        rectF.left = rect.left / scale

        //val width = paint.measureText(text.text, 0, text.text.length) / scale

        return rectF
    }

    fun measureSpannableText(text: SpannableText): RectF
    {
        // TODO: implement
        return RectF()
    }

    /**
     * Converts a com.randsoft.apps.musique.renderdata.Paint object to a android.graphics.Paint object.
     *
     * @param paint The native paint object to convert.
     */
    private fun getPaintFromNativePaintObject(paint: com.randsoft.apps.musique.renderdata.Paint): Paint
    {
        val newPaint = Paint().apply {
            color = paint.color.toInt()
            textSize = paint.textSize
            textAlign = Paint.Align.values()[paint.align]
            isAntiAlias = paint.isAntiAlias
            strokeWidth = paint.strokeWidth * scale
            strokeCap = Paint.Cap.values()[paint.strokeCap]
            style = Paint.Style.values()[paint.drawStyle]
        }

        return newPaint
    }

    /**
     * Draws a line on the given canvas.
     *
     * @param canvas The canvas to draw on.
     * @param line The line to draw.
     * @param offset An offset to the position of the object.
     */
    private fun drawLine(canvas: Canvas, line: Line, offset: PointF = PointF(0.0f, 0.0f))
    {
        val paint = Paint().apply {
            color = line.paint.color.toInt()
            strokeWidth = line.paint.strokeWidth * scale
            isAntiAlias = true
            strokeCap = Paint.Cap.values()[line.paint.strokeCap]
        }

        if (line.paint.verticalEnds) {
            val path = Path();
            path.moveTo((line.startX * scale) + offset.x, ((line.startY + (line.paint.strokeWidth / 2)) * scale) + offset.y)
            path.lineTo((line.startX * scale) + offset.x, ((line.startY - (line.paint.strokeWidth / 2)) * scale) + offset.y)
            path.lineTo((line.endX * scale) + offset.x, ((line.endY - (line.paint.strokeWidth / 2)) * scale) + offset.y)
            path.lineTo((line.endX * scale) + offset.x, ((line.endY + (line.paint.strokeWidth / 2)) * scale) + offset.y)
            path.lineTo((line.startX * scale) + offset.x, ((line.startY + (line.paint.strokeWidth / 2)) * scale) + offset.y)

            canvas.drawPath(path, paint)
        }
        else {
            if (line.paint.isDashedLine)
            {
                paint.pathEffect = DashPathEffect(floatArrayOf(line.paint.dashLength, line.paint.dashSpaceLength), 0.0f)
            }
            else if (line.paint.isDottedLine)
            {
                val dot = Path()
                dot.addCircle(0.0f, 0.0f, line.paint.dotRadius, Path.Direction.CW)

                paint.pathEffect = PathDashPathEffect(dot, line.paint.dashSpaceLength, 0.0f, PathDashPathEffect.Style.MORPH)
            }

            drawLine(canvas, line, paint, offset.x, offset.y)
        }
    }

    /**
     * Draws a text object on the given canvas.
     *
     * @param canvas The canvas to draw on.
     * @param text The text object to draw.
     * @param offset An offset to the position of the object.
     */
    private fun drawText(canvas: Canvas, text: Text, offset: PointF = PointF(0.0f, 0.0f))
    {
        val paint = Paint().apply {
            color = text.paint.color.toInt()
            textSize = text.paint.textSize //* 2.0f // 30.0 text size =about= 22.0 normal size
            textAlign = Paint.Align.values()[text.paint.align]
            isAntiAlias = true
        }


        /*if (text.paint.align == 0)
            paint.textAlign = Paint.Align.LEFT;
        else if (text.paint.align == 1)
            paint.textAlign = Paint.Align.CENTER;
        else if (text.paint.align == 2)
            paint.textAlign = Paint.Align.RIGHT;*/

        paint.textSize = Conversions.pointsToTenths(text.paint.textSize, Scaling())

        if (text.paint.isTablature) {
            paint.typeface = tablatureTypeface
            paint.textSize = text.paint.textSize
            paint.textAlign = Paint.Align.LEFT // ?? (should be CENTER?)

            // add a white rectangle behind the text so that it is more visible
            val rectPaint = Paint().apply {
                color = 0xFFFFFFFF.toInt()
                isAntiAlias = true
            }

            val rect = Rect()
            paint.getTextBounds(text.text, 0, text.text.length, rect)

            val rectX = (text.x * scale) + offset.x
            val rectY = (text.y * scale) + offset.y

            canvas.drawRect(rectX + rect.left * scale, rectY - (rect.height()/2.0f) * scale, rectX + rect.right * scale, rectY + (rect.height()/2.0f) * scale, rectPaint)
        }
        else if (text.paint.isItalic && text.paint.isBold) {
            paint.typeface = typefaceBoldItalic
        }
        else if (!text.paint.isItalic && text.paint.isBold) {
            paint.typeface = typefaceBold
        }
        else if (text.paint.isItalic) {
            paint.typeface = typefaceItalic
        }
        else {
            paint.typeface = typefacePlain
            //paint.typeface = musicTypeface
        }

        // draw
        canvas.save()
        canvas.rotate(-text.paint.rotateDegrees, text.x + offset.x, text.y + offset.y) // rotate canvas
        drawText(canvas, text, paint, offset.x, offset.y) // draw
        canvas.restore(); // reset rotation
    }

    /**
     * Draws a spannable text object on the given canvas.
     *
     * @param canvas The canvas to draw on.
     * @param text The spannable text object to draw.
     * @param offset An offset to the position of object.
     */
    private fun drawSpannableText(canvas: Canvas, text: SpannableText, offset: PointF = PointF(0.0f, 0.0f))
    {
        /*val mainPaint = Paint().apply {
                color = text.mainPaint.color.toInt()
                textSize = text.mainPaint.textSize
                textAlign = Paint.Align.values()[text.mainPaint.align]
                isAntiAlias = true
            }

            mainPaint.textSize = pointsToTenths(text.mainPaint.textSize, scaling)

            if (text.mainPaint.isTablature) {
                mainPaint.typeface = tablatureTypeface
                mainPaint.textSize = text.mainPaint.textSize
            }
            else if (text.mainPaint.isItalic && text.mainPaint.isBold) {
                mainPaint.typeface = typefaceBoldItalic
            }
            else if (!text.mainPaint.isItalic && text.mainPaint.isBold) {
                mainPaint.typeface = typefaceBold
            }
            else if (text.mainPaint.isItalic) {
                mainPaint.typeface = typefaceItalic
            }
            else {
                mainPaint.typeface = typefacePlain
                //paint.typeface = musicTypeface
            }*/

        var textX = text.x * scale
        val textY = text.y * scale

        //drawDebugPoint(canvas, PointF(text.x, text.y))

        for (span in text.spans)
        {
            val paint = Paint().apply {
                color = span.paint.color.toInt()
                textSize = span.paint.textSize //* 2.0f // 30.0 text size =about= 22.0 normal size
                textAlign = Paint.Align.values()[span.paint.align]
                isAntiAlias = true
            }

            paint.textSize = Conversions.pointsToTenths(span.paint.textSize, Scaling())

            if (span.paint.useMusicFont)
            {
                paint.typeface = musicTextTypeface
            }
            else if (span.paint.isTablature) {
                paint.typeface = tablatureTypeface
                paint.textSize = span.paint.textSize
            }
            else if (span.paint.isItalic && span.paint.isBold) {
                paint.typeface = typefaceBoldItalic
            }
            else if (!span.paint.isItalic && span.paint.isBold) {
                paint.typeface = typefaceBold
            }
            else if (span.paint.isItalic) {
                paint.typeface = typefaceItalic
            }
            else {
                paint.typeface = typefacePlain
            }

            paint.textSize *= scale

            var string = ""
            if (span.endIndex <= text.text.length)
                string = text.text.substring(span.startIndex, span.endIndex)
            else if (span.startIndex < text.text.length)
                string = text.text.substring(span.startIndex, text.text.length)

            canvas.drawText(string, textX + offset.x, textY + offset.y, paint)
            textX += (paint.measureText(string, 0, string.length) / scale);
        }
    }

    /**
     * Draws a glyph on the given canvas.
     *
     * @param canvas The canvas to draw on.
     * @param glyph The glyph to draw.
     * @param offset An offset to the position of the glyph.
     */
    private fun drawGlyph(canvas: Canvas, glyph: SMuFLGlyph, offset: PointF = PointF(0.0f, 0.0f))
    {
        // create paint
        val paint: Paint = getPaintFromNativePaintObject(glyph.paint)

        paint.typeface = musicTypeface
        paint.textAlign = Paint.Align.LEFT;

        // calculate correct size
        paint.textSize = 40.0f * glyph.paint.glyphSizeFactor * scale // text size equals the standard staff height (according to SMuFL specification)

        // position
        var x = ((glyph.x) * scale)
        var y = ((glyph.y) * scale)

        if (glyph.paint.centerHorizontally || glyph.paint.centerVertically || glyph.paint.hasBackground)
        {
            val bounds = Rect()

            paint.getTextBounds(Character.toChars(glyph.codePoint), 0, 1, bounds)

            if (glyph.paint.centerHorizontally)
            {
                x -= bounds.width() / 2.0f
            }

            if (glyph.paint.centerVertically)
            {
                y += bounds.height() / 2.0f
            }

            if (glyph.paint.hasBackground)
            {
                val backgroundPaint = Paint().apply {
                    color = glyph.paint.backgroundColor.toInt()
                    isAntiAlias = true
                }

                val backgroundRect = RectF()

                val rectX = x + offset.x
                val rectY = y + offset.y

                val padding = glyph.paint.backgroundPadding * scale

                backgroundRect.left = rectX - padding
                backgroundRect.right = rectX + bounds.width() + padding

                backgroundRect.top = rectY - bounds.height() - padding
                backgroundRect.bottom = rectY + padding

                canvas.drawRect(backgroundRect, backgroundPaint)
            }
        }

        // draw
        canvas.save()
        canvas.rotate(-glyph.paint.rotateDegrees, x + offset.x, y + offset.y) // rotate canvas
        canvas.drawText(Character.toChars(glyph.codePoint), 0, 1, x + offset.x, y + offset.y, paint) // draw
        canvas.restore(); // reset rotation
    }

    /**
     * Draws an array of bitmaps on the given canvas
     *
     * @param canvas the canvas to draw on
     * @param bitmaps the array of bitmaps to draw
     * @param offsetX how much to offset the bitmaps in the x direction
     * @param offsetY how much to offset the bitmaps in the y direction
     */
    private fun drawBitmaps(canvas: Canvas, bitmaps: Array<RenderBitmap>, offsetX: Float = 0.0f, offsetY: Float = 0.0f) // TODO: use offsetX and offsetY
    {
        val density = resources.displayMetrics.density

        for (bitmap in bitmaps) {

            if (bitmap.assetId == AssetID.TrebleClef.ordinal || bitmap.assetId == AssetID.TABClef.ordinal)
            {
                //resources.getString(R.raw.glyphnames)

                val paint = Paint().apply {
                    color = bitmap.paint.color.toInt()
                }

                val text: Text = Text("", ((bitmap.x) * scale), ((bitmap.y) * scale), bitmap.paint);
                //mainCanvas.drawText("", );
                //drawText(mainCanvas, text, paint);

                paint.textSize = 40.0f * scale // text size equals the standard staff height (according to SMuFL specification)
                //paint.setTextSize(TypedValue.COMPLEX_UNIT_SP, 66.6f) // scale textSize to the right size

                // calculate bounds of the text
                val textBounds = Rect()
                paint.getTextBounds(text.text, 0, text.text.length, textBounds)
                paint.typeface = musicTypeface;

                val textHeight = -textBounds.top // text height

                // draw
                val px = 0.0f;
                val py = 0.0f;
                var char = 0;
                val tab = 0xE06D;
                val treble = 0xE052;
                if (bitmap.assetId == AssetID.TABClef.ordinal)
                    char = tab
                else
                    char = treble

                canvas.drawText(Character.toChars(char), 0, 1, (text.x * scale) + px, (text.y * scale) /*+ (textHeight / 2.0f)*/ + py, paint)
            }
            else {
                val paint = Paint().apply {
                    color = bitmap.paint.color.toInt()
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
                drawable.draw(canvas)
            }
        }
    }

    /**
     * Draws a cubic curve on the given canvas.
     *
     * @param canvas The canvas to draw on.
     * @param curve The curve to draw.
     * @param offset An offset to the position of curve.
     */
    private fun drawCubicCurve(canvas: Canvas, curve: CubicCurve, offset: PointF = PointF(0.0f, 0.0f))
    {
        val paint = Paint().apply {
            color = curve.paint.color.toInt()
            strokeWidth = curve.paint.strokeWidth
            isAntiAlias = true
            strokeCap = Paint.Cap.values()[curve.paint.strokeCap]
            style = Paint.Style.STROKE
        }

        val path = Path()
        var s = 5.0f;
        val start = PointF(curve.x1 * scale + offset.x, curve.y1 * scale + offset.y)
        var point1 = PointF(curve.x2 * scale + offset.x, curve.y2 * scale + offset.y)
        var point2 = PointF(curve.x3 * scale + offset.x, curve.y3 * scale + offset.y)
        val end = PointF(curve.x4 * scale + offset.x, curve.y4 * scale + offset.y)

        //Log.i(TAG, "drawing cubic curve: ${}");

        path.moveTo(start.x, start.y)
        path.cubicTo(point1.x, point1.y, point2.x, point2.y, end.x, end.y)

        canvas.drawPath(path, paint)

        if (curve.paint.varyThickness)
        {
            var i = 0;
            while (i < 4) {
                point1 = PointF(point1.x, point1.y - 0.6f * scale)
                point2 = PointF(point2.x, point2.y - 0.6f * scale)
                path.reset()
                path.moveTo(start.x, start.y)
                path.cubicTo(point1.x, point1.y, point2.x, point2.y, end.x, end.y)

                canvas.drawPath(path, paint)
                i++
            }
        }
    }

    /**
     * Draws all the objects in the given
     * RenderData object onto the given Canvas.
     *
     * @param canvas The canvas to draw on.
     * @param renderData The RenderData object to draw.
     * @param offset An offset to the position of all the objects.
     */
    private fun drawObjects(canvas: Canvas, renderData: RenderData, offset: PointF = PointF(0.0f, 0.0f))
    {
        for (line in renderData.lines) {
            drawLine(canvas, line, offset)
        }

        for (text in renderData.texts) {
            drawText(canvas, text, offset)
        }

        for (text in renderData.spannableTexts) {
            drawSpannableText(canvas, text, offset)
        }

        for (glyph in renderData.glyphs) {
            drawGlyph(canvas, glyph, offset)
        }

        // note: bitmaps are not currently being used anymore
        drawBitmaps(canvas, renderData.bitmaps, offset.x, offset.y)

        for (curve in renderData.cubicCurves) {
            drawCubicCurve(canvas, curve, offset)
        }
    }

    private fun renderOnToBitmap()
    {
        if (renderData != null) {

            scaling = Scaling()
            scale = mainScale
            bitmapSizeScale = scale

            Log.v(TAG, "START RENDER $scale, $scaling, $mainScale")
            val density = resources.displayMetrics.density
            val sizeX = (renderData?.right!! - renderData?.left!!) * scale // in tenths
            val sizeY = (renderData?.bottom!! - renderData?.top!!) * scale // in tenths

            Log.v(TAG, "Calcualated sizes ${sizeX.toInt()}, ${sizeY.toInt()},  ${sizeX}, ${sizeY}")

            // create the bitmap
            mainBitmap = Bitmap.createBitmap(sizeX.toInt(), sizeY.toInt(), Bitmap.Config.ARGB_8888)

            // "connect/link" the canvas to the bitmap so that the canvas draws on the bitmap
            val mainCanvas = Canvas(mainBitmap!!)

            drawObjects(mainCanvas, renderData!!)
        }
        else
        {
            Log.e(TAG, "RENDER DATA IS NULL")
        }

        Log.v(TAG, "finnished")
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

    override fun onTouchEvent(event: MotionEvent): Boolean { // TODO: override perform click function
        scaleGestureDetector.onTouchEvent(event)
        gestureDetector.onTouchEvent(event)

        /*var x = event.x
        var y = event.y

        when (event.action) {s
            MotionEvent.ACTION_DOWN -> {

            }
            MotionEvent.ACTION_UP -> {

            }
            MotionEvent.ACTION_MOVE -> {

            }
        }*/

        return true
    }

    @SuppressLint("DrawAllocation")
    override fun onDraw(canvas: Canvas) {
        canvas.drawPaint(backgroundPaint) // fill canvas


        // calculate positionX and positionY (the position of the sheet music)
        if (frameData != null) {
            if (frameData!!.isPlaying)
            {
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
                mat.setScale((mainScale / scale), (mainScale / scale))
                mat.postTranslate(mainBitmapPositionX + positionX * mainScale, mainBitmapPositionY + positionY * mainScale)

                // draw the bitmap that has renderData rendered on it
                canvas.drawBitmap(mainBitmap!!, mat, visiblePaint)
            }
        }

        // draw frame data (includes: play line)
        if (frameData != null) {

            val paint = Paint().apply {
                color = context.resources.getColor(R.color.main_500, context.theme)
                color = 0xff0044dd.toInt()
                strokeWidth = 3.0f * mainScale
                isAntiAlias = true
                strokeCap = Paint.Cap.SQUARE
            }

            // draw play line
            //canvas.drawLine((0.0f * scale), (0.0f * scale), (frameData!!.playLinePosition * scale), ((frameData!!.playLinePositionY + frameData!!.playLineHeight) * scale), paint)
            drawLine(canvas, Line(frameData!!.playLinePosition * (mainScale / scale), frameData!!.playLinePositionY * (mainScale / scale), frameData!!.playLinePosition * (mainScale / scale), (frameData!!.playLinePositionY + frameData!!.playLineHeight) * (mainScale / scale)), paint)
            //drawLine(canvas, Line(0.0f, 0.0f, frameData!!.playLinePosition, frameData!!.playLinePositionY + frameData!!.playLineHeight), paint)
        }
    }

    private fun screenPositionToCanvasPosition(position: PointF): PointF
    {
        val newPosition = PointF()

        newPosition.x = (position.x / mainScale) - positionX
        newPosition.y = (position.y / mainScale) - positionY

        return newPosition
    }

    /*private fun pointsToSp(points: Float)
    {
        var px =
        return px / context.resources.displayMetrics.scaledDensity
    }*/

    /**
     * Draws a page of sheet music on the given page.
     *
     * @param page the page to render the sheet music on
     */
    fun drawPage(page: PdfDocument.Page)
    {
        page.canvas.drawPaint(backgroundPaint) // fill canvas
        val density = resources.displayMetrics.density

        if (printRenderData != null) {

            val mainCanvas = page.canvas
            //mainCanvas.drawPaint(backgroundPaint)

            Log.d(TAG, "drawing page ${page.info.pageNumber}");
            val pageRenderData: RenderData = printRenderData?.pages?.get(page.info.pageNumber)!!

            val pageWidth = Conversions.tenthsToPoints(1233.87f, pageRenderData.scaling)
            val pageHeight = Conversions.tenthsToPoints(1596.77f, pageRenderData.scaling)
            val pagePositionX: Float = (page.info.pageWidth - pageWidth) / 2.0f
            val pagePositionY: Float = (page.info.pageHeight - pageHeight) / 2.0f

            scale = Conversions.tenthsToPoints(1.0f, pageRenderData.scaling)
            bitmapSizeScale = scale
            scaling = pageRenderData.scaling

            drawObjects(mainCanvas, pageRenderData, PointF(pagePositionX, pagePositionY))
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
        canvas.drawLine((line.startX * scale) + positionX * mainScale, (line.startY * scale) + positionY * mainScale, (line.endX * scale) + positionX * mainScale, (line.endY * scale) + positionY * mainScale, paint)
    }

    private fun drawText(canvas: Canvas, text: Text, paint: Paint) {
        paint.textSize = paint.textSize * scale // scale textSize to the right size

        // calculate bounds of the text
        val textBounds = Rect()
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
        val textBounds = Rect()
        paint.getTextBounds(text.text, 0, text.text.length, textBounds)

        val textHeight = -textBounds.top // text height

        // draw
        canvas.drawText(text.text, (text.x * scale) + px, (text.y * scale) + (textHeight / 2.0f) + py, paint)
    }

    private fun drawDebugPoint(canvas: Canvas, point: PointF, paint: Paint = visiblePaint) {
        canvas.drawRect(RectF(point.x, point.y, point.x + 6.0f, point.y + 6.0f), paint)
    }

    fun setFont(defaultFont: FontType, defaultMusicFont: MusicFontType) {
        Log.w(TAG, "setting font")
        if (defaultFont == FontType.TimesNewRoman)
        {
            Log.w(TAG, "TimesNewRoman")
            typefacePlain = ResourcesCompat.getFont(context, R.font.times)
            typefaceItalic = ResourcesCompat.getFont(context, R.font.timesi)
            typefaceBold = ResourcesCompat.getFont(context, R.font.timesbd)
            typefaceBoldItalic = ResourcesCompat.getFont(context, R.font.timesbi)
        }
        else if (defaultFont == FontType.OpenSans)
        {
            Log.w(TAG, "OpenSans")
            typefacePlain = ResourcesCompat.getFont(context, R.font.open_sans)
            typefaceItalic = ResourcesCompat.getFont(context, R.font.open_sans_italic)
            typefaceBold = ResourcesCompat.getFont(context, R.font.open_sans_bold)
            typefaceBoldItalic = ResourcesCompat.getFont(context, R.font.open_sans_bold_italic)
        }
        else
        {
            Log.e(TAG, "That font does not exist")
        }

        when (defaultMusicFont) {
            MusicFontType.Bravura -> {
                musicTypeface = ResourcesCompat.getFont(context, R.font.bravura)
                musicTextTypeface = ResourcesCompat.getFont(context, R.font.bravura_text)
            }
            MusicFontType.Petaluma -> {
                musicTypeface = ResourcesCompat.getFont(context, R.font.petaluma)
                musicTextTypeface = ResourcesCompat.getFont(context, R.font.petaluma_text)
            }
            MusicFontType.Leland -> {
                musicTypeface = ResourcesCompat.getFont(context, R.font.leland)
                musicTextTypeface = ResourcesCompat.getFont(context, R.font.leland_text)
            }
            else -> {
                Log.e(TAG, "That music font does not exist")
            }
        }
    }
}