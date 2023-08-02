package com.randsoft.apps.musique

enum class SongArtistType {
    NONE, MAIN, COMPOSER, LYRICIST, ARRANGER, PERFORMER, FEATURING, PRODUCER, WRITER, WORSHIP_LEADER, VOCALIST, INSTRUMENTALIST
}

class SongArtist {
    var name: String = ""
    var artistTypes: List<SongArtistType> = emptyList()
}

fun GetSongArtistTypeFromString(string: String): SongArtistType
{
    if (string == "main") {
        return SongArtistType.MAIN
    }
    else if (string == "composer") {
        return SongArtistType.COMPOSER
    }
    else if (string == "lyricist") {
        return SongArtistType.LYRICIST
    }
    else if (string == "arranger") {
        return SongArtistType.ARRANGER
    }
    else if (string == "performer") {
        return SongArtistType.PERFORMER
    }
    else if (string == "featuring") {
        return SongArtistType.FEATURING
    }
    else if (string == "producer") {
        return SongArtistType.PRODUCER
    }
    else if (string == "writer") {
        return SongArtistType.WRITER
    }
    else if (string == "worshipleader") {
        return SongArtistType.WORSHIP_LEADER
    }
    else if (string == "vocalist") {
        return SongArtistType.VOCALIST
    }
    else if (string == "instrumentalist") {
        return SongArtistType.INSTRUMENTALIST
    }

    return SongArtistType.NONE
}