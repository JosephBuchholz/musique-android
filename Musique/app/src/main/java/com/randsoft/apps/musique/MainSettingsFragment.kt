package com.randsoft.apps.musique

import android.content.Context
import android.nfc.Tag
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.*
import androidx.fragment.app.Fragment
import androidx.lifecycle.ViewModelProvider

private const val TAG = "MainSettingsFragment"

class MainSettingsFragment : Fragment() {

    private lateinit var settingsPanel1: LinearLayout
    private lateinit var settingsPanel2: LinearLayout

    private lateinit var preferencesButton: Button
    private lateinit var fontsColorsButton: Button

    private lateinit var chordSymbolStyleDropdown: Spinner
    private lateinit var showChordDiagramsDropdown: Spinner

    private lateinit var defaultFontDropdown: Spinner

    private var sharedViewModel: SharedViewModel? = null

    interface Callbacks {
        fun onSettingsChanged(mainSettings: MainSettings)
    }

    private var callbacks: Callbacks? = null

    override fun onAttach(context: Context) {
        super.onAttach(context)
        callbacks = context as Callbacks
    }

    override fun onDetach() {
        super.onDetach()
        callbacks = null
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        sharedViewModel = activity?.run {
            ViewModelProvider(this)[SharedViewModel::class.java]
        }
    }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        val view = inflater.inflate(R.layout.main_settings_fragment, container, false)

        settingsPanel1 = view.findViewById(R.id.settings_panel1)
        settingsPanel2 = view.findViewById(R.id.settings_panel2)

        preferencesButton = view.findViewById(R.id.preferences_button)
        preferencesButton.setOnClickListener {
            settingsPanel1.visibility = View.VISIBLE
            settingsPanel2.visibility = View.GONE

            preferencesButton.setTextColor(requireContext().getColor(R.color.main_500))
            fontsColorsButton.setTextColor(requireContext().getColor(R.color.gray_900))
        }

        fontsColorsButton = view.findViewById(R.id.fonts_colors_button)
        fontsColorsButton.setOnClickListener {
            settingsPanel1.visibility = View.GONE
            settingsPanel2.visibility = View.VISIBLE

            preferencesButton.setTextColor(requireContext().getColor(R.color.gray_900))
            fontsColorsButton.setTextColor(requireContext().getColor(R.color.main_500))
        }

        chordSymbolStyleDropdown = view.findViewById(R.id.chord_symbol_style_dropdown) as Spinner
        chordSymbolStyleDropdown.setSelection(sharedViewModel!!.mainSettings.chordSymbolStyle.ordinal - 1)
        chordSymbolStyleDropdown.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(adapterView: AdapterView<*>?, view: View?, position: Int, id: Long) {
                val type = ChordSymbolStyleType.getByValue(position+1)
                if (type != null)
                    sharedViewModel!!.mainSettings = sharedViewModel!!.mainSettings.copy(chordSymbolStyle = type)

                callbacks?.onSettingsChanged(sharedViewModel!!.mainSettings)
            }

            override fun onNothingSelected(p0: AdapterView<*>?) {

            }
        }

        showChordDiagramsDropdown = view.findViewById(R.id.show_chord_diagrams_dropdown) as Spinner
        showChordDiagramsDropdown.setSelection(sharedViewModel!!.mainSettings.showChordDiagrams.ordinal - 1)
        showChordDiagramsDropdown.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(adapterView: AdapterView<*>?, view: View?, position: Int, id: Long) {
                val type = ShowChordDiagramsType.getByValue(position+1)
                if (type != null)
                    sharedViewModel!!.mainSettings = sharedViewModel!!.mainSettings.copy(showChordDiagrams = type)

                callbacks?.onSettingsChanged(sharedViewModel!!.mainSettings)
            }

            override fun onNothingSelected(p0: AdapterView<*>?) {

            }
        }

        // Panel 2

        defaultFontDropdown = view.findViewById(R.id.default_font_dropdown) as Spinner
        defaultFontDropdown.setSelection(sharedViewModel!!.mainSettings.defaultFont.ordinal - 1)
        defaultFontDropdown.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(adapterView: AdapterView<*>?, view: View?, position: Int, id: Long) {
                val type = FontType.getByValue(position+1)
                if (type != null)
                    sharedViewModel!!.mainSettings = sharedViewModel!!.mainSettings.copy(defaultFont = type)

                callbacks?.onSettingsChanged(sharedViewModel!!.mainSettings)
            }

            override fun onNothingSelected(p0: AdapterView<*>?) {

            }
        }

        return view
    }

    companion object {
        fun newInstance(): MainSettingsFragment  {
            return MainSettingsFragment()
        }
    }
}