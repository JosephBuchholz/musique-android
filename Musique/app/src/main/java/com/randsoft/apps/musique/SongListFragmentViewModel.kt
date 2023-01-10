package com.randsoft.apps.musique

import androidx.lifecycle.ViewModel

class SongListFragmentViewModel : ViewModel() {

    var songItems: List<SongItem> = emptyList()
}