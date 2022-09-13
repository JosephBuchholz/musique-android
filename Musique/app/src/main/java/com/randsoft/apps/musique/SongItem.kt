package com.randsoft.apps.musique

data class SongItem(
    var id: Int = 0,
    var title: String = "",
    var artists: List<String> = emptyList(),
    var instruments: List<String> = emptyList(),
    var files: List<String> = emptyList()
)