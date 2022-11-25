package com.randsoft.apps.musique

import android.Manifest
import android.app.Activity
import android.content.ContentUris
import android.content.ContentValues
import android.content.Intent
import android.content.pm.PackageManager
import android.net.Uri
import android.os.*
import android.print.PrintAttributes
import android.provider.MediaStore
import androidx.appcompat.app.AppCompatActivity
import android.util.Log
import android.widget.Toast
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.core.content.ContextCompat
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.lifecycleScope
import com.randsoft.apps.musique.databinding.ActivityMainBinding
import com.randsoft.apps.musique.framedata.FrameData
import com.randsoft.apps.musique.renderdata.PrintRenderData
import com.randsoft.apps.musique.renderdata.RenderData
import com.randsoft.apps.musique.songdata.InstrumentInfo
import com.randsoft.apps.musique.songdata.SongData
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import org.billthefarmer.mididriver.MidiDriver
import java.io.IOException
import kotlin.concurrent.thread

private const val TAG = "MainActivity"

private const val TAG_SONG_LIST_FRAGMENT = "SongListFragment"
private const val TAG_MUSIC_DISPLAY_FRAGMENT = "MusicDisplayFragment"

private var StopMainLoopThread = false

private const val REQUEST_DOCUMENT = 0

class MainActivity : AppCompatActivity(), MusicDisplayFragment.Callbacks,
    SongListFragment.Callbacks {

    private lateinit var binding: ActivityMainBinding

    private var songListFragment: SongListFragment? = null
    private var musicDisplayFragment: MusicDisplayFragment? = null

    private lateinit var viewModel: MainActivityViewModel
    private lateinit var nativeViewModel: NativeViewModel

    private lateinit var midiDriver: MidiDriver
    private lateinit var config: IntArray

    private lateinit var mainLoopThread: Thread

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // initializing native c++
        initNative()

        // initializing midi driver
        midiDriver = MidiDriver.getInstance()
        midiDriver.setOnMidiStartListener {
            onMidiStart()
            Log.d(TAG, "driver has started")
        }

        // initializing native view model
        viewModel = ViewModelProvider(this).get(MainActivityViewModel::class.java)

        // initializing native view model
        nativeViewModel = ViewModelProvider(this).get(NativeViewModel::class.java)
        setViewModelData(nativeViewModel.viewModelData)

        // starting main loop thread
        if (StopMainLoopThread) {
            StopMainLoopThread = false
            //Log.i(TAG, "------------------------------ setting StopMainLoopThread to false")
        } else {
            //Log.i(TAG, "------------------------------ StopMainLoopThread is false already")
        }
        // main loop thread
        mainLoopThread = thread(start=true) {
            var dt: Double
            var lastFrameTime = SystemClock.elapsedRealtimeNanos()
            Log.d(TAG, "Thread started")
            while (!mainLoopThread.isInterrupted) {
                val timeNow = SystemClock.elapsedRealtimeNanos()
                dt = (timeNow - lastFrameTime).toDouble() / 1000000.0
                /*if (dt <= 0.0f) {
                    Log.d(TAG, "dt is <= 0.0f: $dt, $timeNow, $lastFrameTime, ${(timeNow - lastFrameTime)}")
                }*/
                lastFrameTime = timeNow

                /*if (StopMainLoopThread) {
                    Log.i(TAG, "------------------------------ Stopping thread")
                    break
                }*/

                onUpdate(dt)
            }
            Log.i(TAG, "--------------------------------- Stopping thread")
            /*var j = 0
            var i = 0
            while (true) {
                if (i >= 100000000) {
                    Log.d(TAG, "Hello from main loop in kotlin: $j")
                    j += 1
                    i = 0
                }
                i += 1
            }*/
        }
        Log.d(TAG, "Thread starting")

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // permissions
        permissionsLauncher = registerForActivityResult(ActivityResultContracts.RequestMultiplePermissions()) { permissions ->
            readPermissionGranted = permissions[Manifest.permission.READ_EXTERNAL_STORAGE] ?: readPermissionGranted
            writePermissionGranted = permissions[Manifest.permission.WRITE_EXTERNAL_STORAGE] ?: writePermissionGranted
        }
        updatePermissions()

        // creating fragment
        val isFragmentContainerEmpty = savedInstanceState == null
        if (isFragmentContainerEmpty) {
            songListFragment = SongListFragment.newInstance(readPermissionGranted)
            supportFragmentManager
                .beginTransaction()
                .add(R.id.fragment_container, songListFragment!!, TAG_SONG_LIST_FRAGMENT)
                .commit()
            viewModel.isShowingSongListFragment = true
        } else {
            if (viewModel.isShowingSongListFragment) {
                songListFragment = supportFragmentManager.findFragmentByTag(TAG_SONG_LIST_FRAGMENT) as SongListFragment
            }
            else if (viewModel.isShowingMusicDisplayFragment) {
                musicDisplayFragment = supportFragmentManager.findFragmentByTag(TAG_MUSIC_DISPLAY_FRAGMENT) as MusicDisplayFragment
            }
        }

        /*if (writePermissionGranted) {
            if (saveStringToExternalStorage("test3", "musicxml", "<body>0000abcdefg</body>"))
            {
                Toast.makeText(this, "sucsessfully created file!!", Toast.LENGTH_LONG).show()
            }
            else
            {
                Toast.makeText(this, "failed!!", Toast.LENGTH_SHORT).show()
            }
        }

        if (readPermissionGranted)
        {
            lifecycleScope.launch {
                val string = loadStringFromExternalStorage("tablature", "musicxml")
                if (string.isNotEmpty()) {
                    Log.w(TAG, "Loaded string: ->> $string")
                } else {
                    Log.w(TAG, "string is empty")
                }
            }
        }*/
    }

    override fun onSongOpened(string: String) {
        musicDisplayFragment = MusicDisplayFragment.newInstance()
        supportFragmentManager
            .beginTransaction()
            .replace(R.id.fragment_container, musicDisplayFragment!!, TAG_MUSIC_DISPLAY_FRAGMENT)
            .addToBackStack(null)
            .commit()
        viewModel.isShowingMusicDisplayFragment = true

        loadSongFromString(string)
    }

    override fun onDestroy() {
        super.onDestroy()
        //Log.i(TAG, "------------------------------ Setting to true")
        StopMainLoopThread = true
        //Log.i(TAG, "------------------------------ Set to true")
        Log.i(TAG, "--------------------------------- interrupting thread ${mainLoopThread.name}")
        mainLoopThread.interrupt()
        destroyNative()
        mainLoopThread.join()
    }

    override fun onResume() {
        super.onResume()
        midiDriver.start()
        config = midiDriver.config()
    }

    override fun onPause() {
        super.onPause()
        midiDriver.stop()
    }

    // ---- Music Display Fragment Callbacks ----

    override fun onStartRendering() {
        Log.d(TAG, "Calling on start rendering from activity")
        startRendering()
    }

    override fun onPlayButtonToggled(state: Boolean) {
        onPlayButtonToggledNative(state)
    }

    override fun onResetButtonPressed() {
        onResetButtonPressedNative()
    }

    override fun onPlayProgressChanged(progress: Float)
    {
        onPlayProgressChangedNative(progress)
    }

    override fun onUpdatePrintLayout(attributes: PrintAttributes): Boolean {
        return onUpdatePrintLayoutNative(attributes)
    }

    override fun onCalculateNumPages(): Int {
        return onCalculateNumPagesNative()
    }

    override fun updateInstrumentInfo(info: InstrumentInfo, index: Int) {
        updateInstrumentInfoNative(info, index)
    }

    // ---- Calls to Native C++ ----

    private external fun startRendering()
    private external fun initNative()
    private external fun destroyNative()
    private external fun onUpdate(dt: Double)
    private external fun loadSongFromString(string: String)

    private external fun onPlayButtonToggledNative(state: Boolean)
    private external fun onResetButtonPressedNative()
    private external fun onPlayProgressChangedNative(progress: Float)
    private external fun onUpdatePrintLayoutNative(attributes: PrintAttributes): Boolean
    private external fun onCalculateNumPagesNative(): Int
    private external fun updateInstrumentInfoNative(instrumentInfo: InstrumentInfo, index: Int)

    private external fun setViewModelData(viewModelData: ViewModelData)
    private external fun onMidiStart()

    // ---- Native C++ Callbacks ----

    private fun onUpdateRender(renderData: RenderData) {
        Handler(Looper.getMainLooper()).post(Runnable {
            if (musicDisplayFragment != null) {
                musicDisplayFragment?.onUpdateRenderData(renderData);
            }
        })
    }

    private fun onUpdatePrintRenderData(printRenderData: PrintRenderData) {
        Handler(Looper.getMainLooper()).post(Runnable {
            if (musicDisplayFragment != null) {
                musicDisplayFragment?.onUpdatePrintRenderData(printRenderData);
            }
        })
    }

    private fun onUpdateFrameData(frameData: FrameData) {
        Handler(Looper.getMainLooper()).post(Runnable {
            if (musicDisplayFragment != null) {
                musicDisplayFragment?.onUpdateFrameData(frameData);
            }
        })
    }

    private fun onUpdateSongData(songData: SongData) {
        Handler(Looper.getMainLooper()).post(Runnable {
            if (musicDisplayFragment != null) {
                musicDisplayFragment?.onUpdateSongData(songData);
            }
        })
    }

    private fun onUpdateViewModelData(viewModelData: ViewModelData) {
        nativeViewModel.viewModelData = viewModelData
    }

    // -- Callbacks For Midi --

    private fun writeMidi(byteArray: ByteArray) {
        midiDriver.write(byteArray)
    }

    private fun setMidiVolume(volume: Int) {
        midiDriver.setVolume(volume)
    }

    private fun setMidiReverb(reverb: Int) {
        midiDriver.setReverb(reverb)
    }

    // stroage tests

    private var readPermissionGranted = false
    private var writePermissionGranted = false
    private lateinit var permissionsLauncher: ActivityResultLauncher<Array<String>>

    private fun updatePermissions() {
        val hasReadPermission = ContextCompat.checkSelfPermission(
            this,
            Manifest.permission.READ_EXTERNAL_STORAGE
        ) == PackageManager.PERMISSION_GRANTED
        val hasWritePermission = ContextCompat.checkSelfPermission(
            this,
            Manifest.permission.WRITE_EXTERNAL_STORAGE
        ) == PackageManager.PERMISSION_GRANTED
        writePermissionGranted = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            true
        } else {
            hasWritePermission
        }
        readPermissionGranted = hasReadPermission

        val permissionsToRequest = mutableListOf<String>()
        if (!readPermissionGranted) {
            permissionsToRequest.add(Manifest.permission.READ_EXTERNAL_STORAGE)
        }
        if (!writePermissionGranted) {
            permissionsToRequest.add(Manifest.permission.WRITE_EXTERNAL_STORAGE)
        }

        if (permissionsToRequest.isNotEmpty()) {
            permissionsLauncher.launch(permissionsToRequest.toTypedArray()) // need to handle if permissions were declined by the user
        }
    }

    companion object {
        // Used to load the native c++, 'musique', library on application startup.
        init {
            System.loadLibrary("musique")
        }
    }
}