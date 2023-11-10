package com.randsoft.apps.musique

import android.content.Context
import android.os.Build
import android.os.Bundle
import android.util.Log
import android.view.*
import android.view.inputmethod.EditorInfo
import android.view.inputmethod.InputMethodManager
import android.widget.Button
import android.widget.EditText
import android.widget.PopupMenu
import android.widget.ProgressBar
import android.widget.TextView
import android.widget.TextView.OnEditorActionListener
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.ui.platform.ComposeView
import androidx.core.content.ContextCompat.getSystemService
import androidx.fragment.app.Fragment
import androidx.lifecycle.LiveData
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProvider
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.randsoft.apps.musique.ui.SongList


private const val TAG = "SongListFragment"

private const val ARG_READ_PERMISSION_GRANTED = "ReadPermissionGrated"

class SongListFragment() : Fragment(), WebRepository.Callbacks {

    interface Callbacks {
        fun onSongOpened(songId: Int, extension: String, string: String)
        fun onMainSettingsOpened()
    }

    private var readPermissionGranted: Boolean = false

    private var callbacks: Callbacks? = null

    private lateinit var openButton: Button
    private lateinit var browseButton: Button
    private lateinit var searchButton: Button
    private lateinit var searchField: EditText
    private lateinit var songListRecyclerView: ComposeView
    private lateinit var mainSettingsButton: Button
    private lateinit var moreOptionsMenuButton: Button

    private var loadingProgressBar: ProgressBar? = null
    private var isLoading = false

    private var partPickerDialogFragment: PartPickerDialogFragment? = null

    private lateinit var viewModel: SongListFragmentViewModel

    private val openDocument = registerForActivityResult(ActivityResultContracts.OpenDocument()) { uri ->
        if (readPermissionGranted) {
            if (uri != null) {
                val filePath = uri.path
                var extension = ""
                if (filePath != null)
                    extension = filePath.substring(filePath.lastIndexOf(".") + 1); // gets the file extension
                Log.e(TAG, "uri.path: ${uri.path}, ${uri.lastPathSegment}, ${extension}")
                callbacks?.onSongOpened(0, extension, fileHandler.getStringFromFile(uri))
            }
        }
    }

    private lateinit var fileHandler: FileHandler

    private lateinit var webRepository: WebRepository

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        viewModel = ViewModelProvider(this).get(SongListFragmentViewModel::class.java)

        readPermissionGranted = requireArguments().getBoolean(ARG_READ_PERMISSION_GRANTED)

        fileHandler = FileHandler(requireContext())

        webRepository = WebRepository()
        webRepository.setCallbacks(this)

        val responseLiveData2: LiveData<List<SongItem>> = webRepository.getAll()
        onStartLoading()

        responseLiveData2.observe(
            this,
            Observer { songItems ->
                onStopLoading()
                Log.d(TAG, "Got data: $songItems")
                viewModel.songItems = songItems
                songListRecyclerView.setContent {
                    SongList(songItems = songItems) { songItem ->
                        onItemClicked(songItem)
                    }
                }
            })
    }

    override fun onAttach(context: Context) {
        super.onAttach(context)
        callbacks = context as Callbacks
    }

    override fun onDetach() {
        super.onDetach()
        callbacks = null
    }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        val view = inflater.inflate(R.layout.song_list_fragment, container, false)

        loadingProgressBar = view.findViewById(R.id.loading_progress_bar)
        onUpdateLoading()

        browseButton = view.findViewById(R.id.browse_button)
        browseButton.setOnClickListener {
            Log.i(TAG, "Browse button was clicked")

            onStartLoading()

            val responseLiveData: LiveData<List<SongItem>> = webRepository.getAll()
            responseLiveData.observe(
                viewLifecycleOwner,
                Observer { songItems ->
                    Log.d(TAG, "Got data: $songItems")
                    onStopLoading()

                    viewModel.songItems = songItems
                    songListRecyclerView.setContent {
                        SongList(songItems = songItems) { songItem ->
                            onItemClicked(songItem)
                        }
                    }
                })
        }

        searchField = view.findViewById(R.id.search_field)
        searchField.setOnEditorActionListener(object : OnEditorActionListener {
            override fun onEditorAction(v: TextView?, actionId: Int, event: KeyEvent?): Boolean {
                if (actionId == EditorInfo.IME_ACTION_GO) {
                    searchField.clearFocus()
                    onSearch(searchField.text.toString())
                    return true;
                }
                return false;
            }
        })

        searchButton = view.findViewById(R.id.search_button)
        searchButton.setOnClickListener {
            if (searchField.text.isNotEmpty()) {
                searchField.clearFocus()
                val inputMethodManager: InputMethodManager? = getSystemService(requireContext(), InputMethodManager::class.java)
                inputMethodManager?.showSoftInput(searchField, InputMethodManager.HIDE_IMPLICIT_ONLY)

                onSearch(searchField.text.toString())
            }
            else {
                if (searchField.requestFocus()) {
                    val inputMethodManager: InputMethodManager? = getSystemService(requireContext(), InputMethodManager::class.java)
                    inputMethodManager?.showSoftInput(searchField, InputMethodManager.SHOW_IMPLICIT)
                }
            }
        }

        songListRecyclerView = view.findViewById(R.id.song_list_recycler_view)
        songListRecyclerView.setContent {
            SongList(songItems = viewModel.songItems) { songItem ->
                onItemClicked(songItem)
            }
        }

        moreOptionsMenuButton = view.findViewById(R.id.more_options_menu_button)
        moreOptionsMenuButton.setOnClickListener {
            val popupMenu = PopupMenu(requireContext(), moreOptionsMenuButton)
            popupMenu.inflate(R.menu.more_options_menu)
            popupMenu.setOnMenuItemClickListener {
                when (it.itemId) {
                    R.id.open_item -> {
                        Log.i(TAG, "Open button was clicked")
                        openDocument.launch(arrayOf("*/*"))
                        true
                    }
                    R.id.settings_item -> {
                        callbacks?.onMainSettingsOpened()
                        true
                    }
                    else -> true
                }
            }

            if (Build.VERSION.SDK_INT >= 29)
                popupMenu.setForceShowIcon(true)

            popupMenu.show()
        }

        return view
    }

    private fun onSearch(query: String) {
        onStartLoading()

        val responseLiveData: LiveData<List<SongItem>> = webRepository.performSearch(query)
        responseLiveData.observe(
            viewLifecycleOwner,
            Observer { songItems ->
                onStopLoading()
                Log.d(TAG, "Got data: $songItems")
                viewModel.songItems = songItems
                songListRecyclerView.setContent {
                    SongList(songItems = songItems) { songItem ->
                        onItemClicked(songItem)
                    }
                }
            })
    }

    private fun onItemClicked(songItem: SongItem) {
        if (songItem.files.size >= 2) // 2 or more files, so give the user a choice
        {
            partPickerDialogFragment = PartPickerDialogFragment.newInstance(object : PartPickerDialogFragment.Callbacks {
                override fun onPartPicked(partIndex: Int) {
                    openSong(songItem, partIndex)
                }
            }, songItem.files)
            partPickerDialogFragment?.show(parentFragmentManager, "PartPickerDialog")
        }
        else // else just open the song
        {
            openSong(songItem, 0)
        }
    }

    private fun openSong(songItem: SongItem, partIndex: Int)
    {
        onStartLoading()

        val responseLiveData: LiveData<String> = webRepository.getFile(songItem.id, partIndex)
        responseLiveData.observe(
            viewLifecycleOwner,
            Observer { string ->
                onStopLoading()
                if (string != null)
                    callbacks?.onSongOpened(songItem.id, "musicxml", string)
                else
                    Log.e(TAG, "string is null!!")
            })
    }

    private fun onStartLoading()
    {
        isLoading = true

        if (loadingProgressBar != null)
            loadingProgressBar?.visibility = View.VISIBLE
    }

    private fun onUpdateLoading()
    {
        if (loadingProgressBar != null)
        {
            if (isLoading)
                loadingProgressBar?.visibility = View.VISIBLE
            else
                loadingProgressBar?.visibility = View.GONE
        }
    }

    private fun onStopLoading()
    {
        isLoading = false

        if (loadingProgressBar != null)
            loadingProgressBar?.visibility = View.GONE
    }

    override fun onWebRequestFailed() {
        Log.e(TAG, "Web request failed")
        onStopLoading()
    }

    companion object {
        fun newInstance(readPermissionGranted: Boolean): SongListFragment  {
            val args = Bundle().apply {
                putBoolean(ARG_READ_PERMISSION_GRANTED, readPermissionGranted)
            }

            return SongListFragment().apply {
                arguments = args
            }
        }
    }
}