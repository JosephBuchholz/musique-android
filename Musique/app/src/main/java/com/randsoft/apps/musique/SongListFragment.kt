package com.randsoft.apps.musique

import android.content.Context
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.TextView
import androidx.activity.result.contract.ActivityResultContracts
import androidx.fragment.app.Fragment
import androidx.lifecycle.LiveData
import androidx.lifecycle.Observer
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

class SongListFragment() : Fragment() {

    interface Callbacks {
        fun onSongOpened(string: String)
    }

    private var readPermissionGranted: Boolean = false

    private var callbacks: Callbacks? = null

    private lateinit var openButton: Button
    private lateinit var songListRecyclerView: RecyclerView

    private val openDocument = registerForActivityResult(ActivityResultContracts.OpenDocument()) { uri ->
        if (readPermissionGranted) {
            if (uri != null) {
                callbacks?.onSongOpened(fileHandler.getStringFromFile(uri))
            }
        }
    }

    private lateinit var fileHandler: FileHandler

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        readPermissionGranted = requireArguments().getBoolean(ARG_READ_PERMISSION_GRANTED)

        fileHandler = FileHandler(requireContext())

        val responseLiveData2: LiveData<List<SongItem>> = WebRepository().getAll()
        responseLiveData2.observe(
            this,
            Observer { songItems ->
                Log.d(TAG, "Got data: $songItems")
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

        openButton = view.findViewById(R.id.open_button)
        openButton.setOnClickListener {
            Log.i(TAG, "Open button was clicked")
            openDocument.launch(arrayOf("*/*"))
        }

        songListRecyclerView = view.findViewById(R.id.song_list_recycler_view)
        songListRecyclerView.layoutManager = LinearLayoutManager(context)
        songListRecyclerView.adapter = SongListAdapter(emptyList())

        return view
    }

    inner class SongListViewHolder(var view: View) : RecyclerView.ViewHolder(view) {

        private lateinit var songItem: SongItem

        private var titleTextView: TextView = view.findViewById(R.id.song_item_title)
        private var artistsTextView: TextView = view.findViewById(R.id.song_item_artists)
        private var instrumentsTextView: TextView = view.findViewById(R.id.song_item_instruments)

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
                val responseLiveData: LiveData<String> = WebRepository().getFile(songItem.id)
                responseLiveData.observe(
                       this@SongListFragment,
                        Observer { string ->
                            callbacks?.onSongOpened(string)
                        })
            }
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