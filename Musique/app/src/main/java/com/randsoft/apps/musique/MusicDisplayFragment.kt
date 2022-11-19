package com.randsoft.apps.musique

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.pdf.PdfDocument
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.print.PrintAttributes
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.ImageButton
import android.widget.SeekBar
import android.widget.TextView
import androidx.fragment.app.Fragment
import androidx.lifecycle.LiveData
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.randsoft.apps.musique.framedata.FrameData
import com.randsoft.apps.musique.printing.PrintHandler
import com.randsoft.apps.musique.renderdata.PrintRenderData
import com.randsoft.apps.musique.renderdata.RenderData
import com.randsoft.apps.musique.songdata.InstrumentInfo
import com.randsoft.apps.musique.songdata.SongData

private const val TAG = "MusicDisplayFragment"

class MusicDisplayFragment : Fragment(), PrintHandler.Callbacks {

    private var musicDisplayView: MusicDisplayView? = null

    private lateinit var playButton: ImageButton
    private lateinit var restartButton: ImageButton
    private lateinit var printButton: Button
    private lateinit var instrumentControlButton: Button

    private lateinit var zoomSeekBar: SeekBar
    private lateinit var playSeekBar: SeekBar

    private lateinit var instrumentControlRecyclerView: RecyclerView

    private lateinit var printHandler: PrintHandler

    private var songData: SongData? = null

    var playing = false; // needs to be in a ViewModel

    interface Callbacks {
        fun onStartRendering()
        fun onPlayButtonToggled(state: Boolean)
        fun onResetButtonPressed()
        fun onPlayProgressChanged(progress: Float)
        fun onCalculateNumPages(): Int
        fun onUpdatePrintLayout(attributes: PrintAttributes): Boolean
    }

    private var callbacks: Callbacks? = null

    override fun onAttach(context: Context) {
        super.onAttach(context)
        callbacks = context as Callbacks
    }

    override fun onDetach() {
        super.onDetach()
        callbacks = null
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        printHandler = PrintHandler(requireContext())
        printHandler.setCallbacks(this)
    }

    override fun onCalculateNumPages(): Int {
        return callbacks?.onCalculateNumPages()!!
    }

    override fun onDrawPage(page: PdfDocument.Page) {
        musicDisplayView?.drawPage(page)
    }

    override fun onUpdateLayout(attributes: PrintAttributes): Boolean {
        return callbacks?.onUpdatePrintLayout(attributes)!!
    }

    private var bitmap: Bitmap? = null

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        val view = inflater.inflate(R.layout.music_display_fragment, container, false)

        musicDisplayView = view.findViewById(R.id.music_display_view)

        playButton = view.findViewById(R.id.play_button)
        playButton.setOnClickListener {
            playing = !playing;
            callbacks?.onPlayButtonToggled(playing)
            if (playing) {
                playButton.setImageDrawable(resources.getDrawable(R.drawable.pause_icon))
            } else {
                playButton.setImageDrawable(resources.getDrawable(R.drawable.play_icon))
            }
        }

        restartButton = view.findViewById(R.id.restart_button)
        restartButton.setOnClickListener {
            if (playing) {
                playing = false;
                playButton.setImageDrawable(resources.getDrawable(R.drawable.pause_icon))
                callbacks?.onPlayButtonToggled(playing)
            }
            callbacks?.onResetButtonPressed()
        }

        printButton = view.findViewById(R.id.print_button)
        printButton.setOnClickListener {
            if (playing) { // stop the play back
                playing = false;
                callbacks?.onPlayButtonToggled(playing)
            }

            printHandler.print()
        }

        instrumentControlButton = view.findViewById(R.id.instrument_control_button)
        instrumentControlButton.setOnClickListener {
            if (instrumentControlRecyclerView.visibility == View.GONE)
                instrumentControlRecyclerView.visibility = View.VISIBLE
            else
                instrumentControlRecyclerView.visibility = View.GONE
        }

        zoomSeekBar = view.findViewById(R.id.zoom_seek_bar)
        zoomSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                var fProgress: Float = progress.toFloat() / 100.0f
                if (musicDisplayView != null) {
                    musicDisplayView?.scale = 0.2f + (fProgress * 10.0f)
                    musicDisplayView?.bitmapSizeScale = musicDisplayView!!.scale
                    musicDisplayView?.invalidate()
                }
            }

            override fun onStartTrackingTouch(p0: SeekBar?) {

            }

            override fun onStopTrackingTouch(p0: SeekBar?) {

            }
        })

        playSeekBar = view.findViewById(R.id.play_seek_bar)
        playSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {

            }

            override fun onStartTrackingTouch(seekBar: SeekBar) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar) {
                val fProgress: Float = seekBar.progress.toFloat() / 100.0f // from 0.0 to 1.0
                callbacks?.onPlayProgressChanged(fProgress)
            }
        })

        instrumentControlRecyclerView = view.findViewById(R.id.instrument_control_recycler_view)
        instrumentControlRecyclerView.layoutManager = LinearLayoutManager(context)
        instrumentControlRecyclerView.adapter = InstrumentControlAdapter(emptyList())

        //bitmap = BitmapFactory.decodeResource(resources, R.drawable.quarter_note)

        return view
    }

    inner class InstrumentControlViewHolder(view: View) : RecyclerView.ViewHolder(view) {

        private lateinit var instrument: InstrumentInfo

        private var name: TextView = view.findViewById(R.id.instrument_name)
        private var volume: TextView = view.findViewById(R.id.volume)

        fun bind(instrument: InstrumentInfo) {
            this.instrument = instrument
            name.text = instrument.name
            volume.text = instrument.volume.toString()
        }
    }

    inner class InstrumentControlAdapter(var instruments: List<InstrumentInfo>) : RecyclerView.Adapter<InstrumentControlViewHolder>() {
        override fun getItemCount(): Int {
            return instruments.size
        }

        override fun onBindViewHolder(holder: InstrumentControlViewHolder, position: Int) {
            holder.bind(instruments[position])
        }

        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): InstrumentControlViewHolder {
            val view = layoutInflater.inflate(R.layout.list_item_instrument_control, parent, false)
            return InstrumentControlViewHolder(view)
        }
    }

    private fun onUpdatePlayProgress(progress: Float)
    {
        playSeekBar.progress = (progress * 100.0f).toInt();
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        Log.d(TAG, "Calling on start rendering from fragment")
        callbacks?.onStartRendering()
    }

    fun onUpdateRenderData(renderData: RenderData) {
        if (musicDisplayView != null) {
            if (musicDisplayView?.renderData != null) {
                if (musicDisplayView?.renderDataIsDifferent(
                        musicDisplayView?.renderData!!, renderData) == true) { // if the has renderData changed from last time
                    musicDisplayView?.renderDataChanged = true
                }
                musicDisplayView?.renderDataChanged = true
            }
            else
            {
                musicDisplayView?.renderDataChanged = true
            }
            musicDisplayView?.renderData = renderData
            musicDisplayView?.invalidate()
        }
    }

    fun onUpdatePrintRenderData(printRenderData: PrintRenderData) {
        if (musicDisplayView != null) {
            musicDisplayView?.printRenderData = printRenderData;
        }
    }

    fun onUpdateFrameData(frameData: FrameData) {
        if (musicDisplayView != null) {
            musicDisplayView?.frameData = frameData
            onUpdatePlayProgress(frameData.playProgress)
            musicDisplayView?.invalidate()
        }
    }

    fun onUpdateSongData(songData: SongData) {
        this.songData = songData
        val adapter = instrumentControlRecyclerView.adapter as InstrumentControlAdapter
        adapter.instruments = songData.instruments.toList()
    }

    companion object {
        fun newInstance(): MusicDisplayFragment  {
            return MusicDisplayFragment()
        }
    }
}