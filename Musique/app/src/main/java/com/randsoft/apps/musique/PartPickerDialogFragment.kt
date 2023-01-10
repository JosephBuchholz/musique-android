package com.randsoft.apps.musique

import android.app.AlertDialog
import android.app.Dialog
import android.os.Bundle
import android.util.Log
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.RadioButton
import android.widget.RadioGroup
import android.widget.TextView
import androidx.fragment.app.DialogFragment
import androidx.lifecycle.LiveData
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView

private const val TAG = "PartPickerDialogFragment"

class PartPickerDialogFragment : DialogFragment() {

    private lateinit var cancelButton: Button
    private lateinit var okButton: Button

    private lateinit var radioGroup: RadioGroup
    private lateinit var radioButtonRecyclerView: RecyclerView

    interface Callbacks {
        fun onPartPicked(partIndex: Int)
    }

    private lateinit var optionNames: List<String>
    private var partIndex = 0

    private var callbacks: Callbacks? = null

    override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {
        return activity?.let {
            val builder = AlertDialog.Builder(it)

            val view = layoutInflater.inflate(R.layout.part_picker_dialog, null)

            radioGroup = view.findViewById(R.id.radioGroup)

            for (name in optionNames) {
                val radioButton = RadioButton(context)
                radioButton.text = name
                radioGroup.addView(radioButton)
            }

            cancelButton = view.findViewById(R.id.cancel_button)
            okButton = view.findViewById(R.id.ok_button)

            builder.setView(view)
            val dialog = builder.create()

            okButton.setOnClickListener {
                dialog.cancel()
                partIndex = radioGroup.indexOfChild(radioGroup.findViewById(radioGroup.checkedRadioButtonId))
                Log.d(TAG, "index: ${partIndex}")
                callbacks?.onPartPicked(partIndex)
            }

            cancelButton.setOnClickListener {
                dialog.cancel()
            }

            // return dialog
            dialog
        } ?: throw IllegalStateException("Activity cannot be null")
    }

    companion object  {

        fun newInstance(callbacks: Callbacks, optionNames: List<String>): PartPickerDialogFragment  {
            val dialog = PartPickerDialogFragment()
            dialog.callbacks = callbacks
            dialog.optionNames = optionNames
            return dialog
        }
    }
}
