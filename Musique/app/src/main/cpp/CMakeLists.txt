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
        App.cpp
        MusicRenderer.cpp
        MusicPlayer.cpp
        RenderMeasurement.cpp
        MidiPlayer.cpp
        Metronome.cpp

        RenderData/RenderData.cpp
        RenderData/Line.cpp

        MusicData/Song.cpp
        MusicData/SongData.cpp
        MusicData/Staff.cpp
        MusicData/Instrument.cpp
        MusicData/Pitch.cpp
        MusicData/ComplexLine.cpp
        MusicData/InstrumentBracket.cpp

        MusicData/BaseElements/LineElement.cpp
        MusicData/BaseElements/TextualElement.cpp
        MusicData/BaseElements/VisibleElement.cpp
        MusicData/BaseElements/NumberElement.cpp
        MusicData/BaseElements/TextElement.cpp

        MusicData/Directions/Words.cpp
        MusicData/Directions/BracketDirection.cpp
        MusicData/Directions/Direction.cpp
        MusicData/Directions/DurationDirection.cpp
        MusicData/Directions/Dynamic.cpp
        MusicData/Directions/DynamicWedge.cpp
        MusicData/Directions/MetronomeMark.cpp
        MusicData/Directions/Rehearsal.cpp
        MusicData/Directions/Marker.cpp

        MusicData/Chords/Chord.cpp
        MusicData/Chords/ChordDiagram.cpp

        MusicData/Page/Page.cpp
        MusicData/Page/Credit.cpp
        MusicData/Page/PageNumber.cpp

        MusicData/Measures/Measure.cpp
        MusicData/Measures/KeySignature.cpp
        MusicData/Measures/Clef.cpp
        MusicData/Measures/Tuplet.cpp
        MusicData/Measures/MeasureNumber.cpp
        MusicData/Measures/TimeSignature.cpp
        MusicData/Measures/Arpeggio.cpp
        MusicData/Measures/Ending.cpp
        MusicData/Measures/MeasureRepeat.cpp
        MusicData/Measures/Barline.cpp
        MusicData/Measures/MultiMeasureRestSymbol.cpp

        MusicData/Notes/Note.cpp
        MusicData/Notes/Accidental.cpp
        MusicData/Notes/Lyric.cpp
        MusicData/Notes/AugmentationDot.cpp
        MusicData/Notes/NoteHead.cpp
        MusicData/Notes/Fermata.cpp
        MusicData/Notes/NoteChord.cpp
        MusicData/Notes/NoteFlag.cpp
        MusicData/Notes/NoteStem.cpp
        MusicData/Notes/TremoloSingle.cpp
        MusicData/Notes/Slur.cpp
        MusicData/Notes/TABSlur.cpp
        MusicData/Notes/Tie.cpp
        MusicData/Notes/GlissandoSlide.cpp

        MusicData/Notes/Beams/Beam.cpp
        MusicData/Notes/Beams/BeamGroup.cpp

        MusicData/Notes/Articulations/Accent.cpp
        MusicData/Notes/Articulations/Articulation.cpp
        MusicData/Notes/Articulations/BreathMark.cpp
        MusicData/Notes/Articulations/Caesura.cpp
        MusicData/Notes/Articulations/Slide.cpp
        MusicData/Notes/Articulations/StaccatoArticulation.cpp
        MusicData/Notes/Articulations/Stress.cpp

        MusicData/Notes/Techniques/Bowing.cpp
        MusicData/Notes/Techniques/GuitarTechnique.cpp
        MusicData/Notes/Techniques/Bend.cpp
        MusicData/Notes/Techniques/Technique.cpp

        MusicData/Notes/Ornaments/Ornaments.cpp
        MusicData/Notes/Ornaments/Mordent.cpp
        MusicData/Notes/Ornaments/TrillMark.cpp
        MusicData/Notes/Ornaments/Turn.cpp
        MusicData/Notes/Ornaments/Vibrato.cpp

        MusicXMLParser/MusicXMLHelper.cpp
        MusicXMLParser/MusicXMLParser.cpp
        MusicXMLParser/XMLHelper.cpp
        MusicXMLParser/ParseError.cpp
        MusicXMLParser/NoteElementParser.cpp
        MusicXMLParser/BaseElementParser.cpp

        Utils/Math.cpp
        Utils/Converters.cpp

        Collisions/BoundingBox.cpp

        JNIHelpers/JNIHelper.cpp

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