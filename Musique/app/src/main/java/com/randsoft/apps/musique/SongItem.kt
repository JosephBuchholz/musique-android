package com.randsoft.apps.musique

data class SongItem(
    var id: Int = 0,
    var title: String = "",
    var artists: List<SongArtist> = emptyList(),
    var instruments: List<String> = emptyList(),
    var files: List<String> = emptyList()
)

fun RawSongItemToSongItem(rawSongItem: RawSongItem): SongItem
{
    val songItem: SongItem = SongItem(id = rawSongItem.id,
        title = rawSongItem.title, instruments = rawSongItem.instruments, files = rawSongItem.files)

    val artists: MutableList<SongArtist> = mutableListOf()

    for (artistString in rawSongItem.artists)
    {
        val pair = artistString.split(':')
        var artist = SongArtist()

        if (pair.size == 2) {
            artist.name = pair[1]

            val typeStrings = pair[0].split('/')
            val artistTypes: MutableList<SongArtistType> = mutableListOf()

            for (typeString in typeStrings)
            {
                val artistType = GetSongArtistTypeFromString(typeString)
                artistTypes.add(artistType)
            }

            artist.artistTypes = artistTypes
        }
        else if  (pair.size == 1) {
            artist.name = pair[0]
        }

        artists.add(artist)
    }

    songItem.artists = artists

    return songItem
}