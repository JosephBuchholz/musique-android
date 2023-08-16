package com.randsoft.apps.musique

import android.util.Log
import androidx.datastore.core.Serializer
import kotlinx.serialization.SerializationException
import kotlinx.serialization.json.Json
import java.io.InputStream
import java.io.OutputStream

private const val TAG = "MainSettingsSerializer"

object MainSettingsSerializer : Serializer<MainSettings> {

    override val defaultValue: MainSettings
        get() = MainSettings()

    override suspend fun readFrom(input: InputStream): MainSettings {
        return try {
            Json.decodeFromString(
                deserializer = MainSettings.serializer(),
                string = input.readBytes().decodeToString()
            )
        } catch (e: SerializationException) {
            e.printStackTrace()
            defaultValue
        }
    }

    override suspend fun writeTo(t: MainSettings, output: OutputStream) {
        output.write(
            Json.encodeToString(
                serializer = MainSettings.serializer(),
                value = t
            ).encodeToByteArray()
        )

        Log.e(TAG, Json.encodeToString(
            serializer = MainSettings.serializer(),
            value = t
        ))
    }
}