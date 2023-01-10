package com.randsoft.apps.musique

import androidx.lifecycle.ViewModel

class MainActivityViewModel : ViewModel() {

    var isShowingMusicDisplayFragment = false
    var isShowingSongListFragment = false

    var songIsOpen = false
    var songString = ""
}