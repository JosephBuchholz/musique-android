package com.randsoft.apps.musique.api

import retrofit2.Call
import retrofit2.http.GET
import retrofit2.http.Url

interface GetSongApi
{
    @GET
    fun getAll(@Url url: String): Call<GetSongResponse>

    @GET
    fun performSearch(@Url url: String): Call<GetSongResponse>

    @GET
    fun get(@Url url: String): Call<String>
}