package com.randsoft.apps.musique

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.SeekBar
import androidx.fragment.app.Fragment
import com.randsoft.apps.musique.framedata.FrameData
import com.randsoft.apps.musique.renderdata.RenderData

private const val TAG = "MusicDisplayFragment"

class MusicDisplayFragment : Fragment() {

    private var musicDisplayView: MusicDisplayView? = null

    private lateinit var playButton: Button
    private lateinit var restartButton: Button
    private lateinit var zoomSeekBar: SeekBar

    var playing = false; // needs to be in a ViewModel

    interface Callbacks {
        fun onStartRendering()
        fun onPlayButtonToggled(state: Boolean)
        fun onResetButtonPressed()
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
                playButton.text = "Pause"
            } else {
                playButton.text = "Play"
            }
        }

        restartButton = view.findViewById(R.id.restart_button)
        restartButton.setOnClickListener {
            if (playing) {
                playing = false;
                playButton.text = "Pause"
                callbacks?.onPlayButtonToggled(playing)
            }
            callbacks?.onResetButtonPressed()
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

        //bitmap = BitmapFactory.decodeResource(resources, R.drawable.quarter_note)

        return view
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
            }
            else
            {
                musicDisplayView?.renderDataChanged = true
            }
            musicDisplayView?.renderData = renderData
            musicDisplayView?.invalidate()
        }
    }

    fun onUpdateFrameData(frameData: FrameData) {
        if (musicDisplayView != null) {
            musicDisplayView?.frameData = frameData
            musicDisplayView?.invalidate()
        }
    }

    companion object {
        fun newInstance(): MusicDisplayFragment  {
            return MusicDisplayFragment()
        }
    }
}