package com.randsoft.apps.musique

enum class TransposeKey {
    None, C_FLAT, G_FLAT, D_FLAT, A_FLAT, E_FLAT, B_FLAT, F, C, G, D, A, E, B, F_SHARP, C_SHARP;

    companion object {
        fun getByValue(value: Int) = values().firstOrNull { it.ordinal == value }
    }
}

enum class TransposeDirection {
    None, Closest, Up, Down;

    companion object {
        fun getByValue(value: Int) = values().firstOrNull { it.ordinal == value }
    }
}

enum class TransposeTablatureType {
    None, Auto, UseCapo, NoCapo;

    companion object {
        fun getByValue(value: Int) = values().firstOrNull { it.ordinal == value }
    }
}

class TranspositionRequest {
    var key: TransposeKey = TransposeKey.C
    var direction: TransposeDirection = TransposeDirection.Closest
    var transposeTablatureType: TransposeTablatureType = TransposeTablatureType.Auto
}