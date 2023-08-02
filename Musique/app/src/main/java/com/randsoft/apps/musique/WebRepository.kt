package com.randsoft.apps.musique

import android.content.Context
import android.print.PrintAttributes
import android.util.Log
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import com.randsoft.apps.musique.api.GetSongApi
import com.randsoft.apps.musique.api.GetSongResponse
import com.randsoft.apps.musique.songdata.InstrumentInfo
import retrofit2.Call
import retrofit2.Callback
import retrofit2.Response
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory
import retrofit2.converter.scalars.ScalarsConverterFactory

private const val TAG = "WebRepositiory"

private const val KEY = "ABC123"

private const val METHOD_ALL = "all"
private const val METHOD_SEARCH = "search"
private const val METHOD_GET_FILE = "get_file"
private const val METHOD_GET_DESCRIPTION_FILE = "get_desc"

// a repository that gets data from the web
class WebRepository {

    private val getSongApi: GetSongApi

    interface Callbacks {
        fun onWebRequestFailed()
    }

    private var callbacks: Callbacks? = null

    fun setCallbacks(callbacks: Callbacks)
    {
        this.callbacks = callbacks
    }

    init {

        // retrofit networking stuff
        val retrofit: Retrofit = Retrofit.Builder()
            .baseUrl("https://music.escriben.org/")
            //.baseUrl("http://192.168.1.173:8000/")
            .addConverterFactory(ScalarsConverterFactory.create())
            .addConverterFactory(GsonConverterFactory.create())
            .build()

        // create getSong api
        getSongApi = retrofit.create(GetSongApi::class.java)
    }

    fun performSearch(query: String): LiveData<List<SongItem>> {
        val responseLiveData: MutableLiveData<List<SongItem>> = MutableLiveData()
        val searchRequest: Call<GetSongResponse> = getSongApi.performSearch("song/get/?key=$KEY&method=$METHOD_SEARCH&query=$query") // request to get a file of a particular song

        searchRequest.enqueue(object : Callback<GetSongResponse> {
            override fun onFailure(call: Call<GetSongResponse>, t: Throwable) {
                Log.e(TAG, "Failed to call method 'search' on web request $t")
                if (callbacks != null)
                    callbacks?.onWebRequestFailed()
            }

            override fun onResponse(call: Call<GetSongResponse>, response: Response<GetSongResponse>) {
                Log.d(TAG, "Got a response")
                val songResponse = response.body()
                val rawSongItems: List<RawSongItem> = songResponse?.songs ?: mutableListOf()
                val songItems: MutableList<SongItem> = mutableListOf()
                for (songItem in rawSongItems) {
                    songItems.add(RawSongItemToSongItem(songItem))
                }
                responseLiveData.value = songItems
            }
        })

        return responseLiveData
    }

    fun getAll(): LiveData<List<SongItem>> {
        val responseLiveData: MutableLiveData<List<SongItem>> = MutableLiveData()
        val getFileRequest: Call<GetSongResponse> = getSongApi.getAll("song/get/?key=$KEY&method=$METHOD_ALL") // request to get a file of a particular song

        getFileRequest.enqueue(object : Callback<GetSongResponse> {
            override fun onFailure(call: Call<GetSongResponse>, t: Throwable) {
                Log.e(TAG, "Failed to call method 'all' on web request $t")

                if (callbacks != null)
                    callbacks?.onWebRequestFailed()
            }

            override fun onResponse(call: Call<GetSongResponse>, response: Response<GetSongResponse>) {
                Log.d(TAG, "Got a response")
                val getSongResponse = response.body()
                val rawSongItems: List<RawSongItem> = getSongResponse?.songs ?: mutableListOf()
                val songItems: MutableList<SongItem> = mutableListOf()
                for (songItem in rawSongItems) {
                    songItems.add(RawSongItemToSongItem(songItem))
                }
                responseLiveData.value = songItems
            }
        })

        return responseLiveData
    }

    fun getFile(id: Int, file_index: Int = 0): LiveData<String> {
        val responseLiveData: MutableLiveData<String> = MutableLiveData()
        val getFileRequest: Call<String> = getSongApi.get("song/get/?key=$KEY&method=$METHOD_GET_FILE&id=$id&file_index=$file_index") // request to get a file of a particular song

        getFileRequest.enqueue(object : Callback<String> {
            override fun onFailure(call: Call<String>, t: Throwable) {
                Log.e(TAG, "Failed to call method 'get_file' on web request", t)

                if (callbacks != null)
                    callbacks?.onWebRequestFailed()
            }

            override fun onResponse(call: Call<String>, response: Response<String>) {
                Log.d(TAG, "Got a response")
                responseLiveData.value = response.body()
            }
        })

        return responseLiveData
    }

    fun getDescriptionFile(id: Int): LiveData<String> {
        val responseLiveData: MutableLiveData<String> = MutableLiveData()
        val getFileRequest: Call<String> = getSongApi.get("song/get/?key=$KEY&method=$METHOD_GET_DESCRIPTION_FILE&id=$id") // request to get the description file for a particular song

        getFileRequest.enqueue(object : Callback<String> {
            override fun onFailure(call: Call<String>, t: Throwable) {
                Log.e(TAG, "Failed to call method 'get_desc' on web request", t)

                if (callbacks != null)
                    callbacks?.onWebRequestFailed()
            }

            override fun onResponse(call: Call<String>, response: Response<String>) {
                Log.d(TAG, "Got a response")
                responseLiveData.value = response.body()
            }
        })

        return responseLiveData
    }
}