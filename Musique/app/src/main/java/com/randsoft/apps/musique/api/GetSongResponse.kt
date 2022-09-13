package com.randsoft.apps.musique.api

import com.google.gson.annotations.SerializedName
import com.randsoft.apps.musique.SongItem

class GetSongResponse {
    @SerializedName("song_count")
    var songCount: Int = 0
    lateinit var songs: List<SongItem>
}