package com.randsoft.apps.musique.ui

import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.material.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.randsoft.apps.musique.SongItem

@Composable
fun SongList(songItems: List<SongItem>, onClick: (songItem: SongItem) -> Unit) {
    LazyColumn() {
        items(songItems.size) {
            val songItem = songItems[it]

            var artistsString = ""
            for ((index, artist) in songItem.artists.withIndex()) {

                artistsString += artist.name

                if (index != songItem.artists.size - 1) // add a comma if it is not the last one in the list
                    artistsString += ", "
            }

            var instrumentsString = ""
            for ((index, instrument) in songItem.instruments.withIndex()) {
                instrumentsString += instrument

                if (index != songItem.instruments.size - 1) // add a comma if it is not the last one in the list
                    instrumentsString += ", "
            }

            SongListItem(songItem.title, artistsString, instrumentsString, Modifier.clickable {
                onClick(songItem)
            })
        }
    }
}

@Composable
fun SongListItem(title: String, artists: String, instruments: String, modifier: Modifier = Modifier) {
    Column(modifier.padding(start = 15.dp, top = 8.dp, bottom = 8.dp)) {
        Text(text = title, fontWeight = FontWeight.Bold, fontSize = 20.sp)
        Text(text = artists)
        Text(text = instruments)
    }
}