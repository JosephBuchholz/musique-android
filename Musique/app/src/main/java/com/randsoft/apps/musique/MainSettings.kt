package com.randsoft.apps.musique

import kotlinx.serialization.Serializable

enum class ChordSymbolStyleType {
    None, Auto, Normal, Jazzy;

    companion object {
        fun getByValue(value: Int) = values().firstOrNull { it.ordinal == value }
    }
}

enum class ShowChordDiagramsType {
    None, Auto, Never, Always;

    companion object {
        fun getByValue(value: Int) = values().firstOrNull { it.ordinal == value }
    }
}

enum class FontType {
    None, TimesNewRoman, OpenSans;

    companion object {
        fun getByValue(value: Int) = values().firstOrNull { it.ordinal == value }
    }
}

@Serializable
data class MainSettings(
    val chordSymbolStyle: ChordSymbolStyleType = ChordSymbolStyleType.Auto,
    val showChordDiagrams: ShowChordDiagramsType = ShowChordDiagramsType.Auto,
    val defaultFont: FontType = FontType.TimesNewRoman
)