package com.randsoft.apps.musique

import android.app.AlertDialog
import android.app.Dialog
import android.os.Bundle
import android.view.View
import android.widget.AdapterView
import android.widget.Button
import android.widget.Spinner
import androidx.fragment.app.DialogFragment

class TransposeDialogFragment : DialogFragment() {

    private lateinit var keyDropdown: Spinner
    private lateinit var directionDropdown: Spinner
    private lateinit var tablatureDropdown: Spinner

    private lateinit var cancelButton: Button
    private lateinit var okButton: Button

    private var transpositionRequest: TranspositionRequest = TranspositionRequest()

    interface Callbacks {
        fun onTranspositionRequest(transpositionRequest: TranspositionRequest)
    }

    private var callbacks: Callbacks? = null

    override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {
        return activity?.let {
            val builder = AlertDialog.Builder(it)

            val view = layoutInflater.inflate(R.layout.transpose_dialog, null)

            keyDropdown = view.findViewById(R.id.key_dropdown)
            keyDropdown.setSelection(transpositionRequest.key.ordinal - 1)
            keyDropdown.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
                override fun onItemSelected(adapterView: AdapterView<*>?, view: View?, position: Int, id: Long) {
                    val type = TransposeKey.getByValue(position+1)
                    if (type != null)
                        transpositionRequest.key = type
                }

                override fun onNothingSelected(p0: AdapterView<*>?) {

                }
            }

            directionDropdown = view.findViewById(R.id.direction_dropdown)
            directionDropdown.setSelection(transpositionRequest.direction.ordinal - 1)
            directionDropdown.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
                override fun onItemSelected(adapterView: AdapterView<*>?, view: View?, position: Int, id: Long) {
                    val type = TransposeDirection.getByValue(position+1)
                    if (type != null)
                        transpositionRequest.direction = type
                }

                override fun onNothingSelected(p0: AdapterView<*>?) {

                }
            }

            tablatureDropdown = view.findViewById(R.id.tablature_dropdown)
            tablatureDropdown.setSelection(transpositionRequest.transposeTablatureType.ordinal - 1)
            tablatureDropdown.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
                override fun onItemSelected(adapterView: AdapterView<*>?, view: View?, position: Int, id: Long) {
                    val type = TransposeTablatureType.getByValue(position+1)
                    if (type != null)
                        transpositionRequest.transposeTablatureType = type
                }

                override fun onNothingSelected(p0: AdapterView<*>?) {

                }
            }

            cancelButton = view.findViewById(R.id.cancel_button)
            okButton = view.findViewById(R.id.ok_button)

            builder.setView(view)
            val dialog = builder.create()

            okButton.setOnClickListener {
                callbacks?.onTranspositionRequest(transpositionRequest)
                dialog.cancel()
            }

            cancelButton.setOnClickListener {
                dialog.cancel()
            }

            // return dialog
            dialog
        } ?: throw IllegalStateException("Activity cannot be null")
    }

    companion object  {

        fun newInstance(callbacks: Callbacks, defaultTranspositionRequest: TranspositionRequest): TransposeDialogFragment  {
            val dialog = TransposeDialogFragment()
            dialog.callbacks = callbacks
            dialog.transpositionRequest = defaultTranspositionRequest
            return dialog
        }
    }
}
