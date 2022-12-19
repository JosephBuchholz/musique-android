package com.randsoft.apps.musique

import android.app.AlertDialog
import android.app.Dialog
import android.content.Context
import android.content.DialogInterface
import android.os.Bundle
import android.view.View
import android.widget.AdapterView
import android.widget.Button
import android.widget.Spinner
import androidx.fragment.app.DialogFragment

class SettingsDialogFragment : DialogFragment() {

    private lateinit var layoutDropdown: Spinner

    private lateinit var cancelButton: Button
    private lateinit var okButton: Button

    class Settings() {
        enum class MusicLayout {
            None, Horizontal, Vertical, Paged;

            companion object {
                fun getByValue(value: Int) = values().firstOrNull { it.ordinal == value }
            }
        }

        var musicLayout: MusicLayout = MusicLayout.Vertical
    }

    private var settings: Settings = Settings()

    interface Callbacks {
        fun onSettingsChanged(settings: Settings)
    }

    private var callbacks: Callbacks? = null

    override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {
        return activity?.let {
            val builder = AlertDialog.Builder(it)

            val view = layoutInflater.inflate(R.layout.settings_dialog, null)

            layoutDropdown = view.findViewById(R.id.layout_dropdown) as Spinner
            layoutDropdown.setSelection(settings.musicLayout.ordinal-1)
            layoutDropdown.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
                override fun onItemSelected(adapterView: AdapterView<*>?, view: View?, position: Int, id: Long) {
                    val type = Settings.MusicLayout.getByValue(position+1)
                    if (type != null)
                        settings.musicLayout = type
                }

                override fun onNothingSelected(p0: AdapterView<*>?) {

                }
            }

            /*layoutButton.setOnClickListener {
                settings.musicLayout = Settings.MusicLayout.Horizontal
            }*/

            cancelButton = view.findViewById(R.id.cancel_button)
            okButton = view.findViewById(R.id.ok_button)

            builder.setView(view)
            val dialog = builder.create()

            okButton.setOnClickListener {
                dialog.cancel()
                callbacks?.onSettingsChanged(settings)
            }

            cancelButton.setOnClickListener {
                dialog.cancel()
            }

            // return dialog
            dialog
        } ?: throw IllegalStateException("Activity cannot be null")
    }

    companion object  {

        fun newInstance(callbacks: Callbacks): SettingsDialogFragment  {
            val dialog = SettingsDialogFragment()
            dialog.callbacks = callbacks
            return dialog
        }
    }
}
