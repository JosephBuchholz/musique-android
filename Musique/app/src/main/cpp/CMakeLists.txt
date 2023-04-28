# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.
project("musique")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        musique

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        Main.cpp
        Callbacks.cpp
        RenderData/RenderData.cpp
        RenderData/Line.cpp
        App.cpp
        AppRendering.cpp
        MusicData/LineElement.cpp
        MusicData/Song.cpp
        MusicData/SongData.cpp
        MusicData/Notes/Note.cpp
        MusicData/Measures/Measure.cpp
        MusicData/Staff.cpp
        MusicData/Instrument.cpp
        MusicData/Chords/Chord.cpp
        MusicData/Notes/Lyric.cpp
        MusicData/Directions/Words.cpp
        MusicData/Notes/AugmentationDot.cpp
        MusicData/Notes/Beam.cpp
        MusicData/Notes/BeamGroup.cpp

        MusicData/Directions/BracketDirection.cpp
        MusicData/Directions/Direction.cpp
        MusicData/Directions/Dynamic.cpp
        MusicData/Directions/DynamicWedge.cpp
        MusicData/Directions/MetronomeMark.cpp

        MusicData/Measures/KeySignature.cpp
        MusicData/Measures/Clef.cpp
        MusicData/Measures/Tuplet.cpp
        MusicData/Pitch.cpp
        MusicData/Measures/TimeSignature.cpp
        RenderMeasurement.cpp

        MusicXMLParser/MusicXMLHelper.cpp
        MusicXMLParser/MusicXMLParser.cpp
        MusicXMLParser/XMLHelper.cpp
        MusicXMLParser/ParseError.cpp
        MusicXMLParser/NoteElementParser.cpp
        MusicXMLParser/BaseElementParser.cpp

        MusicData/Notes/Articulations/Accent.cpp
        MusicData/Notes/Articulations/BreathMark.cpp
        MusicData/Notes/Articulations/Caesura.cpp
        MusicData/Notes/Articulations/Slide.cpp
        MusicData/Notes/Articulations/StaccatoArticulation.cpp
        MusicData/Notes/Articulations/Stress.cpp

        Collisions/BoundingBox.cpp

        JNIHelpers/JNIHelper.cpp
        Utils/Converters.cpp
        libs/tinyxml/tinystr.cpp
        libs/tinyxml/tinyxml.cpp
        libs/tinyxml/tinyxmlerror.cpp
        libs/tinyxml/tinyxmlparser.cpp
        libs/tinyxml/xmltest.cpp
        libs/tinyxml2/tinyxml2.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        musique

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})

#set(CMAKE_ANDROID_RTTI)