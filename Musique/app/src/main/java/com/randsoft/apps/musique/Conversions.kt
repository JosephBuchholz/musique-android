package com.randsoft.apps.musique

import com.randsoft.apps.musique.renderdata.Scaling

object Conversions {
    fun millimetersToPoints(millimeters: Float): Float // converts from millimeters to points (1/72 inch)
    {
        return (millimeters / 25.4f) * 75.0f; // divide by 25.4 to get inches then multiply by 75 to get points
    }

    fun pointsToMillimeters(pt: Float): Float // converts from points (1/72 inch) to millimeters
    {
        return (pt / 75.0f) * 25.4f; // divide by 75 to get inches then multiply by 25.4 to get millimeters
    }

    fun pointsToTenths(pt: Float, scaling: Scaling): Float // converts from points (1/72 inch) to tenths (defined at top)
    {
        val millimeters = pointsToMillimeters(pt); // convert points to millimeters
        return (millimeters / scaling.millimeters) * scaling.tenths; // convert to tenths
    }

    fun tenthsToPoints(tenths: Float, scaling: Scaling): Float // converts from tenths (defined at top) to points (1/72 inch)
    {
        val millimeters = (tenths / scaling.tenths) * scaling.millimeters; // convert tenths to millimeters
        return millimetersToPoints(millimeters); // convert to points
    }
}