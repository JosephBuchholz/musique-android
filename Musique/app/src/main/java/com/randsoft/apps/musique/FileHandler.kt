package com.randsoft.apps.musique

import android.content.ContentResolver
import android.content.ContentValues
import android.content.Context
import android.net.Uri
import android.os.Build
import android.provider.MediaStore
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.io.IOException

private const val TAG = "FileHandler"

class FileHandler(private val context: Context) {

    // EXTERNAL STORAGE

    private inline fun <T> sdk29AndUp(onSdk29: () -> T): T? {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            onSdk29()
        } else null
    }

    fun getStringFromFile(uri: Uri): String {
        try {
            context.contentResolver.openInputStream(uri).use { inputStream ->
                val bytes = inputStream?.readBytes()
                val s: String = bytes?.decodeToString() ?: ""
                return s
            }
        } catch(e: IOException) {
            e.printStackTrace()
            return ""
        }
    }

    suspend fun loadStringFromExternalStorage(filename: String, fileExtension: String): String {
        return withContext(Dispatchers.IO) {
            val fileCollection = sdk29AndUp {
                MediaStore.Files.getContentUri(MediaStore.VOLUME_EXTERNAL)
            } ?: MediaStore.Files.getContentUri(MediaStore.VOLUME_EXTERNAL)

            val projection = arrayOf(
                MediaStore.Files.FileColumns._ID,
                MediaStore.Files.FileColumns.DISPLAY_NAME,
                MediaStore.Files.FileColumns.MIME_TYPE,
                MediaStore.Files.FileColumns.MEDIA_TYPE
            )

            var contentUri: Uri = Uri.EMPTY
            context.contentResolver.query(
                fileCollection,
                projection,
                /*MediaStore.Files.FileColumns.DISPLAY_NAME + " = ?",
                arrayOf("$filename.$fileExtension"),*/
                null,
                null,
                null
            )?.use { cursor ->
                val nameColumn =
                    cursor.getColumnIndexOrThrow(MediaStore.Files.FileColumns.DISPLAY_NAME)
                val idColumn = cursor.getColumnIndexOrThrow(MediaStore.Files.FileColumns._ID)
                val mimeTypeColumn = cursor.getColumnIndexOrThrow(MediaStore.Files.FileColumns.MIME_TYPE)
                val mediaTypeColumn = cursor.getColumnIndexOrThrow(MediaStore.Files.FileColumns.MEDIA_TYPE)

                cursor.moveToFirst()
                Log.w(TAG, "count: ${cursor.count}")

                if (cursor.count > 0) {
                    while (cursor.moveToNext()) {
                        val name = cursor.getString(nameColumn)
                        val mimeType = cursor.getString(mimeTypeColumn)
                        val mediaType = cursor.getInt(mediaTypeColumn)
                        //Log.w(TAG, "filename: $name, mimeType: $mimeType, mediaType: $mediaType")
                    }
                    /*contentUri = ContentUris.withAppendedId(
                    MediaStore.Files.getContentUri(MediaStore.VOLUME_EXTERNAL),
                    id
                )*/
                    //val mimeType = cursor.getString(idColumn)
                    //Log.d(TAG, "mimeType: $mimeType")
                    /*val id = cursor.getLong(idColumn)
                    contentUri = ContentUris.withAppendedId(
                        MediaStore.Files.getContentUri(MediaStore.VOLUME_EXTERNAL),
                        id
                    )*/
                }
            }

            try {
                context.contentResolver.openInputStream(contentUri).use { inputStream ->
                    val bytes = inputStream?.readBytes()
                    Log.d(TAG, "-------------------------------------------- BYTES LOADED: $bytes, ${bytes?.decodeToString()}")
                    val s: String = bytes?.decodeToString() ?: ""
                    Log.w(TAG, "s: $s")
                    return@withContext s
                }
            } catch(e: IOException) {
                e.printStackTrace()
            }

            ""
        }
    }

    fun saveStringToExternalStorage(filename: String, fileExtension: String, string: String): Boolean {
        val fileCollection = sdk29AndUp {
            MediaStore.Files.getContentUri(MediaStore.VOLUME_EXTERNAL_PRIMARY)
        } ?: MediaStore.Files.getContentUri(MediaStore.VOLUME_EXTERNAL)

        val contentValues = ContentValues().apply {
            put(MediaStore.Files.FileColumns.DISPLAY_NAME, "$filename.$fileExtension")
            put(MediaStore.Files.FileColumns.MIME_TYPE, "application/vnd.recordare.musicxml+xml")
        }

        return try {

            context.contentResolver.insert(fileCollection, contentValues)?.also { uri ->
                context.contentResolver.openOutputStream(uri).use { outputStream ->
                    Log.d(TAG, "-------------------------------------------- BYTES SAVED: ${string.toByteArray()}, ${string.encodeToByteArray()}, $string")
                    outputStream?.write(string.toByteArray())
                }
            } ?: throw IOException("Could not create MediaStore entry")
            true
        } catch(e: IOException) {
            e.printStackTrace()
            false
        }
    }

    // INTERNAL STORAGE

    // internal storage gets deleted when app gets deleted

    suspend fun loadAllMusicFilesFromInternalStorage(): List<String> {
        return withContext(Dispatchers.IO) {
            val files = context.filesDir.listFiles()
            files?.filter { it.canRead() && it.isFile && it.name.endsWith(".musicxml") }?.map {
                val bytes = it.readBytes()
                val string = bytes.toString()
                string
            } ?: listOf()
        }
    }

    fun loadMusicFileFromInternalStorage(filename: String, fileExtension: String): String {
        return try {
            context.openFileInput("$filename.$fileExtension").use { stream ->
                val bytes = stream.readBytes()
                val string = bytes.toString()
                string
            }
        } catch(e: IOException) {
            e.printStackTrace()
            ""
        }
    }

    fun saveMusicFileToInternalStorage(filename: String, fileExtension: String, string: String): Boolean {
        return try {
            context.openFileOutput("$filename.$fileExtension", AppCompatActivity.MODE_PRIVATE).use { stream ->
                stream.write(string.toByteArray())
            }
            true
        } catch(e: IOException) {
            e.printStackTrace()
            false
        }
    }

    fun deleteFileFromInternalStorage(filename: String, fileExtension: String): Boolean {
        return try {
            context.deleteFile("$filename.$fileExtension")
        } catch(e: IOException) {
            e.printStackTrace()
            false
        }
    }
}