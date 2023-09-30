package com.randsoft.apps.musique

import android.content.Context
import android.graphics.RectF
import android.graphics.pdf.PdfDocument
import android.os.Bundle
import android.print.PrintAttributes
import android.text.method.LinkMovementMethod
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.SeekBar
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.core.text.HtmlCompat
import androidx.fragment.app.Fragment
import androidx.lifecycle.LiveData
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProvider
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.google.android.material.button.MaterialButton
import com.randsoft.apps.musique.event.InputEvent
import com.randsoft.apps.musique.framedata.FrameData
import com.randsoft.apps.musique.printing.PrintHandler
import com.randsoft.apps.musique.renderdata.*
import com.randsoft.apps.musique.songdata.InstrumentInfo
import com.randsoft.apps.musique.songdata.SongData
import java.time.temporal.Temporal

private const val TAG = "MusicDisplayFragment"

private const val ARG_SONG_ID = "SongId"

class MusicDisplayFragment : Fragment(), PrintHandler.Callbacks, SettingsDialogFragment.Callbacks, TransposeDialogFragment.Callbacks, MusicDisplayView.Callbacks, WebRepository.Callbacks {

    private var musicDisplayView: MusicDisplayView? = null

    private lateinit var titleTextView: TextView

    private lateinit var playButton: MaterialButton
    private lateinit var metronomeButton: MaterialButton
    private lateinit var restartButton: MaterialButton
    private lateinit var printButton: MaterialButton
    private lateinit var instrumentControlButton: MaterialButton
    private lateinit var settingsButton: MaterialButton
    private lateinit var volumeButton: MaterialButton
    private lateinit var tempoButton: MaterialButton
    private lateinit var transposeButton: MaterialButton

    private lateinit var volumeControl: View
    private lateinit var tempoControl: View

    private lateinit var secondaryControlBar: ConstraintLayout

    private lateinit var volumeControlSeekBar: SeekBar
    private lateinit var volumeControlTextView: TextView

    private lateinit var tempoControlSeekBar: SeekBar
    private lateinit var tempoControlProgressTextView: TextView
    private lateinit var tempoControlTempoTextView: TextView

    private lateinit var sideMenuButton: MaterialButton
    private lateinit var sideMenu: ConstraintLayout
    private lateinit var sideMenuSongDescTextView: TextView

    private lateinit var playSeekBar: SeekBar

    private lateinit var instrumentControlRecyclerView: RecyclerView

    private lateinit var printHandler: PrintHandler

    private lateinit var settingsDialogFragment: SettingsDialogFragment
    private lateinit var transposeDialogFragment: TransposeDialogFragment

    private var songData: SongData? = null

    private lateinit var viewModel: MusicDisplayFragmentViewModel
    private var sharedViewModel: SharedViewModel? = null

    private lateinit var webRepository: WebRepository

    private var songId: Int = 0

    private var currentTempo: Float = 120.0f

    interface Callbacks {
        fun onStartRendering()
        fun onPlayButtonToggled(state: Boolean)
        fun onMetronomeButtonToggled(state: Boolean)
        fun onResetButtonPressed()
        fun onPlayProgressChanged(progress: Float)
        fun onCalculateNumPages(): Int
        fun onUpdatePrintLayout(attributes: PrintAttributes): Boolean
        fun updateInstrumentInfo(info: InstrumentInfo, index: Int)
        fun onSettingsChanged(settings: SettingsDialogFragment.Settings)
        fun onVolumeChange(volume: Float)
        fun onTempoPercentageChange(tempoPercentage: Float)
        fun onTranspose(transpositionRequest: TranspositionRequest)

        fun onInputEvent(inputEvent: InputEvent)
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

        songId = requireArguments().getInt(ARG_SONG_ID)

        webRepository = WebRepository()
        webRepository.setCallbacks(this)

        printHandler = PrintHandler(requireContext())
        printHandler.setCallbacks(this)

        // initializing view models
        viewModel = ViewModelProvider(this)[MusicDisplayFragmentViewModel::class.java]
        sharedViewModel = activity?.run {
            ViewModelProvider(this)[SharedViewModel::class.java]
        }
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

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        val view = inflater.inflate(R.layout.music_display_fragment, container, false)

        settingsDialogFragment = SettingsDialogFragment.newInstance(this)
        transposeDialogFragment = TransposeDialogFragment.newInstance(this, TranspositionRequest())

        musicDisplayView = view.findViewById(R.id.music_display_view)
        musicDisplayView?.setCallbacks(this)
        musicDisplayView?.setFont(sharedViewModel!!.mainSettings.defaultFont, sharedViewModel!!.mainSettings.defaultMusicFont)

        secondaryControlBar = view.findViewById(R.id.secondary_control_bar)

        playButton = view.findViewById(R.id.play_button)
        if (viewModel.isPlaying) {
            playButton.setIconResource(R.drawable.pause_icon)
        }
        else {
            playButton.setIconResource(R.drawable.play_icon)
        }
        playButton.setOnClickListener {
            onPlayButtonClicked()
        }

        metronomeButton = view.findViewById(R.id.metronome_button)
        metronomeButton.setOnClickListener {
            viewModel.isMetronomeOn = !viewModel.isMetronomeOn

            if (viewModel.isMetronomeOn) {
                metronomeButton.iconTint = requireContext().resources.getColorStateList(R.color.main_500, requireContext().theme)
            } else {
                metronomeButton.iconTint = requireContext().resources.getColorStateList(R.color.gray_800, requireContext().theme)
            }

            callbacks?.onMetronomeButtonToggled(viewModel.isMetronomeOn)
        }

        restartButton = view.findViewById(R.id.restart_button)
        restartButton.setOnClickListener {
            if (viewModel.isPlaying) {
                viewModel.isPlaying = false
                playButton.setIconResource(R.drawable.pause_icon)
                callbacks?.onPlayButtonToggled(viewModel.isPlaying)
            }
            callbacks?.onResetButtonPressed()
        }

        printButton = view.findViewById(R.id.print_button)
        printButton.setOnClickListener {
            if (viewModel.isPlaying) { // stop the play back
                viewModel.isPlaying = false
                callbacks?.onPlayButtonToggled(viewModel.isPlaying)
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

        settingsButton = view.findViewById(R.id.settings_button)
        settingsButton.setOnClickListener {
            settingsDialogFragment.show(parentFragmentManager, "SettingsDialog")
        }

        volumeControl = view.findViewById(R.id.volume_control)

        volumeControlTextView = view.findViewById(R.id.volume_control_text_view)

        volumeControlSeekBar = view.findViewById(R.id.volume_control_seek_bar)
        volumeControlSeekBar.progress = 100
        volumeControlTextView.text = volumeControlSeekBar.progress.toString()
        volumeControlSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
                val progressFloat: Float = progress.toFloat() / 100.0f
                volumeControlTextView.text = progress.toString()
                callbacks?.onVolumeChange(progressFloat)
                setVolumeButtonLevel(progressFloat)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar) {
                val progressFloat: Float = seekBar.progress.toFloat() / 100.0f
                volumeControlTextView.text = volumeControlSeekBar.progress.toString()
                callbacks?.onVolumeChange(progressFloat)
                setVolumeButtonLevel(progressFloat)
            }
        })

        volumeButton = view.findViewById(R.id.volume_button)
        volumeButton.setOnClickListener {
            viewModel.isVolumeButtonOn = !viewModel.isVolumeButtonOn

            if (viewModel.isVolumeButtonOn) {
                showVolumeControl()
            }
            else {
                hideVolumeControl()
            }
        }

        tempoControl = view.findViewById(R.id.tempo_control)

        tempoControlProgressTextView = view.findViewById(R.id.tempo_control_progress_text_view)
        tempoControlTempoTextView = view.findViewById(R.id.tempo_control_tempo_text_view)

        tempoControlSeekBar = view.findViewById(R.id.tempo_control_seek_bar)
        tempoControlProgressTextView.text = getString(R.string.tempo_control_percentage, tempoControlSeekBar.progress)
        tempoControlTempoTextView.text = getString(R.string.tempo_control_tempo, tempoControlSeekBar.progress)
        tempoControlSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
                val progressFloat: Float = progress.toFloat()
                tempoControlProgressTextView.text = getString(R.string.tempo_control_percentage, progressFloat.toInt())
                tempoControlTempoTextView.text = getString(R.string.tempo_control_tempo, (currentTempo * (tempoControlSeekBar.progress / 100.0f)).toInt())
                callbacks?.onTempoPercentageChange(progressFloat / 100.0f)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar) {
                val progressFloat: Float = seekBar.progress.toFloat()
                tempoControlProgressTextView.text = getString(R.string.tempo_control_percentage, progressFloat.toInt())
                tempoControlTempoTextView.text = getString(R.string.tempo_control_tempo, (currentTempo * (tempoControlSeekBar.progress / 100.0f)).toInt())
                callbacks?.onTempoPercentageChange(progressFloat / 100.0f)
            }
        })

        tempoButton = view.findViewById(R.id.tempo_button)
        tempoButton.setOnClickListener {
            viewModel.isTempoButtonOn = !viewModel.isTempoButtonOn

            if (viewModel.isTempoButtonOn) {
                showTempoControl()
            }
            else {
                hideTempoControl()
            }
        }

        if (viewModel.isVolumeButtonOn)
            showVolumeControl()
        else
            hideVolumeControl()

        if (viewModel.isTempoButtonOn)
            showTempoControl()
        else
            hideTempoControl()

        transposeButton = view.findViewById(R.id.transpose_button)
        transposeButton.setOnClickListener {
            transposeDialogFragment.show(parentFragmentManager, "TransposeDialog")
        }

        playSeekBar = view.findViewById(R.id.play_seek_bar)
        playSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {

            }

            override fun onStartTrackingTouch(seekBar: SeekBar) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar) {
                val fProgress: Float = seekBar.progress.toFloat() / 100.0f // from 0.0 to 1.0
                callbacks?.onPlayProgressChanged(fProgress)
            }
        })

        // top control bar

        sideMenu = view.findViewById(R.id.side_menu)
        sideMenuSongDescTextView = view.findViewById(R.id.song_description_text_view)
        sideMenuSongDescTextView.movementMethod = LinkMovementMethod.getInstance() // allows links to be click in the text view

        val responseLiveData: LiveData<String> = webRepository.getDescriptionFile(songId)
        responseLiveData.observe(
            viewLifecycleOwner,
            Observer { descString ->
                if (descString != null)
                    sideMenuSongDescTextView.text = HtmlCompat.fromHtml(descString, 0)
            })

        sideMenuButton = view.findViewById(R.id.side_menu_button)
        sideMenuButton.setOnClickListener {
            Log.d(TAG, "button clicked")
            if (sideMenu.visibility == View.GONE)
                sideMenu.visibility = View.VISIBLE
            else
                sideMenu.visibility = View.GONE
        }

        titleTextView = view.findViewById(R.id.titleTextView)

        instrumentControlRecyclerView = view.findViewById(R.id.instrument_control_recycler_view)
        instrumentControlRecyclerView.layoutManager = LinearLayoutManager(context)
        instrumentControlRecyclerView.adapter = InstrumentControlAdapter(emptyList())

        return view
    }

    private fun showVolumeControl()
    {
        hideAllControls()
        viewModel.isVolumeButtonOn = true
        volumeButton.iconTint = requireContext().resources.getColorStateList(R.color.main_500, requireContext().theme)
        volumeControl.visibility = View.VISIBLE
    }

    private fun hideVolumeControl()
    {
        viewModel.isVolumeButtonOn = false
        volumeButton.iconTint = requireContext().resources.getColorStateList(R.color.gray_800, requireContext().theme)
        volumeControl.visibility = View.GONE
    }

    private fun showTempoControl()
    {
        hideAllControls()
        viewModel.isTempoButtonOn = true
        tempoButton.iconTint = requireContext().resources.getColorStateList(R.color.main_500, requireContext().theme)
        tempoControl.visibility = View.VISIBLE
    }

    private fun hideTempoControl()
    {
        viewModel.isTempoButtonOn = false
        tempoButton.iconTint = requireContext().resources.getColorStateList(R.color.gray_800, requireContext().theme)
        tempoControl.visibility = View.GONE
    }

    private fun hideAllControls()
    {
        hideVolumeControl()
        hideTempoControl()
    }

    fun onNativeInit()
    {
        callbacks?.onMetronomeButtonToggled(viewModel.isMetronomeOn)
        callbacks?.onPlayButtonToggled(viewModel.isPlaying)
    }

    private fun onPlayButtonClicked()
    {
        viewModel.isPlaying = !viewModel.isPlaying

        if (viewModel.isPlaying) {
            playButton.setIconResource(R.drawable.pause_icon)
        }
        else {
            playButton.setIconResource(R.drawable.play_icon)
        }

        callbacks?.onPlayButtonToggled(viewModel.isPlaying)
    }

    fun setVolumeButtonLevel(volume: Float)
    {
        if (volume > 0.70f) {
            volumeButton.setIconResource(R.drawable.volume_full_icon)
        }
        else if (volume > 0.40f) {
            volumeButton.setIconResource(R.drawable.volume_mid_icon)
        }
        else if (volume > 0.0f) {
            volumeButton.setIconResource(R.drawable.volume_low_icon)
        }
        else {
            volumeButton.setIconResource(R.drawable.volume_mute_icon)
        }
    }

    override fun onSettingsChanged(settings: SettingsDialogFragment.Settings) {
        Log.i(TAG, "SETTINGS CHANGED")
        callbacks?.onSettingsChanged(settings)
    }

    inner class InstrumentControlViewHolder(view: View) : RecyclerView.ViewHolder(view) {

        private lateinit var instrument: InstrumentInfo
        private var index: Int = 0

        private var name: TextView = view.findViewById(R.id.instrument_name)
        private var volume: TextView = view.findViewById(R.id.volume)
        private var isVisibleButton: Button = view.findViewById(R.id.is_visible_button)

        fun bind(instrument: InstrumentInfo, index: Int) {
            this.instrument = instrument
            this.index = index
            update()
            isVisibleButton.setOnClickListener {
                instrument.visible = !instrument.visible
                update()
                callbacks?.updateInstrumentInfo(instrument, index)
            }
        }

        private fun update() {
            name.text = instrument.name
            volume.text = "Volume " + instrument.volume.toString()
            if (instrument.visible)
                isVisibleButton.text = "Visible"
            else
                isVisibleButton.text = "Not Visible"
        }
    }

    inner class InstrumentControlAdapter(var instruments: List<InstrumentInfo>) : RecyclerView.Adapter<InstrumentControlViewHolder>() {
        override fun getItemCount(): Int {
            return instruments.size
        }

        override fun onBindViewHolder(holder: InstrumentControlViewHolder, position: Int) {
            holder.bind(instruments[position], position)
        }

        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): InstrumentControlViewHolder {
            val view = layoutInflater.inflate(R.layout.list_item_instrument_control, parent, false)
            return InstrumentControlViewHolder(view)
        }
    }

    private fun onUpdatePlayProgress(progress: Float)
    {
        playSeekBar.progress = (progress * 100.0f).toInt()
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
        else
        {
            Log.w(TAG, "musicDisplayView is null")
        }
    }

    fun onUpdatePrintRenderData(printRenderData: PrintRenderData) {
        if (musicDisplayView != null) {
            musicDisplayView?.printRenderData = printRenderData
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
        titleTextView.text = songData.songTitle
    }

    fun measureGlyph(glyph: SMuFLGlyph): Float {
        return if (musicDisplayView != null) {
            musicDisplayView!!.measureGlyph(glyph)
        } else {
            Log.w(TAG, "musicDisplayView is null")
            0.0f
        }
    }

    fun getGlyphBoundingBox(glyph: SMuFLGlyph): RectF {
        return if (musicDisplayView != null) {
            musicDisplayView!!.getGlyphBoundingBox(glyph)
        } else {
            Log.w(TAG, "musicDisplayView is null")
            RectF()
        }
    }

    fun measureText(text: Text): RectF {
        return if (musicDisplayView != null) {
            musicDisplayView!!.measureText(text)
        } else {
            Log.w(TAG, "musicDisplayView is null")
            RectF()
        }
    }

    fun measureSpannableText(text: SpannableText): RectF {
        return if (musicDisplayView != null) {
            musicDisplayView!!.measureSpannableText(text)
        } else {
            Log.w(TAG, "musicDisplayView is null")
            RectF()
        }
    }

    fun onTempoChangedCallback(tempo: Float) {
        currentTempo = tempo
        tempoControlTempoTextView.text = getString(R.string.tempo_control_tempo, (tempo * (tempoControlSeekBar.progress / 100.0f)).toInt())
    }

    override fun onInputEvent(inputEvent: InputEvent) {
        callbacks?.onInputEvent(inputEvent)
    }

    override fun onWebRequestFailed() {
        Log.e(TAG, "Web request failed")
    }

    override fun onTranspositionRequest(transpositionRequest: TranspositionRequest) {
       callbacks?.onTranspose(transpositionRequest)
    }

    companion object {
        fun newInstance(songId: Int): MusicDisplayFragment  {
            val args = Bundle().apply {
                putInt(ARG_SONG_ID, songId)
            }

            return MusicDisplayFragment().apply {
                arguments = args
            }
        }
    }
}