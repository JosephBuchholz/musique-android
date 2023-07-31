package com.randsoft.apps.musique

import android.content.Context
import android.os.Bundle
import android.util.Log
import android.view.*
import android.widget.Button
import android.widget.ImageButton
import android.widget.ProgressBar
import android.widget.TextView
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.widget.SearchView
import androidx.appcompat.widget.SearchView.OnQueryTextListener
import androidx.fragment.app.Fragment
import androidx.lifecycle.LiveData
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProvider
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.randsoft.apps.musique.api.GetSongApi
import retrofit2.Call
import retrofit2.Callback
import retrofit2.Response
import retrofit2.Retrofit
import retrofit2.converter.scalars.ScalarsConverterFactory

private const val TAG = "SongListFragment"

private const val ARG_READ_PERMISSION_GRANTED = "ReadPermissionGrated"

class SongListFragment() : Fragment(), WebRepository.Callbacks {

    interface Callbacks {
        fun onSongOpened(songId: Int, string: String)
    }

    private var readPermissionGranted: Boolean = false

    private var callbacks: Callbacks? = null

    private lateinit var openButton: ImageButton
    private lateinit var browseButton: Button
    private lateinit var searchView: SearchView
    private lateinit var songListRecyclerView: RecyclerView

    private var loadingProgressBar: ProgressBar? = null
    private var isLoading = false

    private lateinit var viewModel: SongListFragmentViewModel

    private val openDocument = registerForActivityResult(ActivityResultContracts.OpenDocument()) { uri ->
        if (readPermissionGranted) {
            if (uri != null) {
                callbacks?.onSongOpened(0, fileHandler.getStringFromFile(uri))
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
                songListRecyclerView.adapter = SongListAdapter(songItems)
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

        openButton = view.findViewById(R.id.open_button)
        openButton.setOnClickListener {
            Log.i(TAG, "Open button was clicked")
            openDocument.launch(arrayOf("*/*"))
        }

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
                    songListRecyclerView.adapter = SongListAdapter(songItems)
                })
        }

        searchView = view.findViewById(R.id.search_view)
        searchView.isIconified = false
        searchView.setOnQueryTextListener(object : OnQueryTextListener {
            override fun onQueryTextSubmit(query: String?): Boolean {

                if (query != null)
                {
                    onStartLoading()

                    val responseLiveData: LiveData<List<SongItem>> = webRepository.performSearch(query)
                    responseLiveData.observe(
                        viewLifecycleOwner,
                        Observer { songItems ->
                            onStopLoading()
                            Log.d(TAG, "Got data: $songItems")
                            viewModel.songItems = songItems
                            songListRecyclerView.adapter = SongListAdapter(songItems)
                        })
                }

                return true
            }

            override fun onQueryTextChange(newText: String?): Boolean {
                return false
            }
        })

        songListRecyclerView = view.findViewById(R.id.song_list_recycler_view)
        songListRecyclerView.layoutManager = LinearLayoutManager(context)
        songListRecyclerView.adapter = SongListAdapter(viewModel.songItems)

        return view
    }

    inner class SongListViewHolder(var view: View) : RecyclerView.ViewHolder(view), PartPickerDialogFragment.Callbacks {

        private lateinit var songItem: SongItem

        private var titleTextView: TextView = view.findViewById(R.id.song_item_title)
        private var artistsTextView: TextView = view.findViewById(R.id.song_item_artists)
        private var instrumentsTextView: TextView = view.findViewById(R.id.song_item_instruments)

        private var partPickerDialogFragment: PartPickerDialogFragment? = null

        fun bind(songItem: SongItem) {
            this.songItem = songItem
            titleTextView.text = songItem.title

            var artistsString = ""
            for ((index, artist) in songItem.artists.withIndex()) {
                artistsString += artist

                if (index != songItem.artists.size - 1) // add a comma if it is not the last one in the list
                    artistsString += ", "
            }
            artistsTextView.text = artistsString

            var instrumentsString = ""
            for ((index, instrument) in songItem.instruments.withIndex()) {
                instrumentsString += instrument

                if (index != songItem.instruments.size - 1) // add a comma if it is not the last one in the list
                    instrumentsString += ", "
            }
            instrumentsTextView.text = instrumentsString

            view.setOnClickListener {
                if (songItem.files.size >= 2) // 2 or more files, so give the user a choice
                {
                    partPickerDialogFragment = PartPickerDialogFragment.newInstance(this, songItem.files)
                    partPickerDialogFragment?.show(parentFragmentManager, "PartPickerDialog")
                }
                else // else just open the song
                {
                    openSong(0)
                }
            }
        }

        override fun onPartPicked(partIndex: Int) {
            openSong(partIndex)
        }

        private fun openSong(partIndex: Int)
        {
            onStartLoading()

            val responseLiveData: LiveData<String> = webRepository.getFile(songItem.id, partIndex)
            responseLiveData.observe(
                this@SongListFragment,
                Observer { string ->
                    onStopLoading()
                    callbacks?.onSongOpened(songItem.id, string)
                })
        }
    }

    inner class SongListAdapter(private var songItems: List<SongItem>) : RecyclerView.Adapter<SongListViewHolder>() {
        override fun getItemCount(): Int {
            return songItems.size
        }

        override fun onBindViewHolder(holder: SongListViewHolder, position: Int) {
            holder.bind(songItems[position])
        }

        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): SongListViewHolder {
            val view = layoutInflater.inflate(R.layout.list_item_song, parent, false)
            return SongListViewHolder(view)
        }
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