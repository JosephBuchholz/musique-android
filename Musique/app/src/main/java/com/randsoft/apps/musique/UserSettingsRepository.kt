package com.randsoft.apps.musique

import android.content.Context
import android.util.Log
import androidx.datastore.core.DataStore
import androidx.datastore.dataStore
import androidx.datastore.preferences.core.Preferences
import androidx.datastore.preferences.core.edit
import androidx.datastore.preferences.core.intPreferencesKey
import androidx.datastore.preferences.preferencesDataStore
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.collect
import kotlinx.coroutines.flow.combine
import kotlinx.coroutines.flow.map

private const val TAG = "UserSettingsRepository"

private const val USER_SETTINGS_NAME = "settings"

private val Context.dataStore by dataStore("main-settings.json", MainSettingsSerializer)

class UserSettingsRepository(context: Context) {

    val dataStore = context.dataStore

    suspend fun readMainSettings(): MainSettings
    {
        var mainSettings = MainSettings()

        dataStore.data.collect {
            mainSettings = it
        }

        return mainSettings
    }

    suspend fun writeMainSettings(mainSettings: MainSettings) {
        dataStore.updateData {
            it.copy(chordSymbolStyle = mainSettings.chordSymbolStyle,
                showChordDiagrams = mainSettings.showChordDiagrams,
                defaultFont = mainSettings.defaultFont,
                defaultMusicFont = mainSettings.defaultMusicFont)
        }
    }
}

/*private const val CHORD_SYMBOL_STYLE = "chord_symbol_style"
private const val SHOW_CHORD_DIAGRAMS = "show_chord_diagrams"

private val Context.dataStore: DataStore<Preferences> by preferencesDataStore(name = USER_SETTINGS_NAME)

class UserSettingsRepository(context: Context) {

    private val dataStore = context.dataStore

    private val chordSymbolStyleKey = intPreferencesKey(CHORD_SYMBOL_STYLE)
    val chordSymbolStyleFlow: Flow<Int> = dataStore.data
        .map { preferences ->
            // No type safety.
            preferences[chordSymbolStyleKey] ?: 0
        }

    suspend fun readMainSettings(): MainSettings
    {
        val mainSettings = MainSettings()

        Log.d(TAG, "going to read: ${mainSettings.chordSymbolStyle}")

        chordSymbolStyleFlow.collect {
            Log.d(TAG, "before: readMainSettings: ${mainSettings.chordSymbolStyle}, $it")
            mainSettings.chordSymbolStyle = ChordSymbolStyleType.getByValue(it)!!
            Log.d(TAG, "after: readMainSettings: ${mainSettings.chordSymbolStyle}, $it")
        }

        return mainSettings
    }

    suspend fun writeMainSettings(mainSettings: MainSettings)
    {
        dataStore.edit { settings ->
            settings[chordSymbolStyleKey] = mainSettings.chordSymbolStyle.ordinal
        }

        Log.d(TAG, "writeMainSettings: ${mainSettings.chordSymbolStyle}, ${mainSettings.chordSymbolStyle.ordinal}")
    }
}*/