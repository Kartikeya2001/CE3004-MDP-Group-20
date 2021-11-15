package com.example.cz3004mdp;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.SyncStateContract;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;
import androidx.lifecycle.ViewModelProvider;

import com.google.android.material.floatingactionbutton.FloatingActionButton;

import java.nio.charset.Charset;

/**
 * Command Section class
 * Date: 2021/08/20 - 2021/10/04
 * Android App Authors: Huang Chaoshan, Nicole Tan (Chen Jiaxin)
 */
public class CommandSection extends Fragment {

    // Variable declaration
    private static final String ARG_SECTION_NUMBER = "section_number";
    private static final String TAG = "CommandSection";

    private PageViewModel pageViewModel;

    SharedPreferences sharedPreferences;

    FloatingActionButton send;
    private static TextView messageReceivedTextView;
    private EditText typeBoxEditText;

    /**
     * Command Section Instance
     *
     * @param index
     */
    public static CommandSection newInstance(int index) {
        CommandSection fragment = new CommandSection();
        Bundle bundle = new Bundle();
        bundle.putInt(ARG_SECTION_NUMBER, index);
        fragment.setArguments(bundle);
        return fragment;
    }

    /**
     * Command Section
     *
     * @param savedInstanceState
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        pageViewModel = new ViewModelProvider(this).get(PageViewModel.class);
        int index = 1;
        if (getArguments() != null) {
            index = getArguments().getInt(ARG_SECTION_NUMBER);
        }
        pageViewModel.setIndex(index);
    }

    /**
     * View Creation for the Command Section
     *
     * @param container
     * @param inflater
     * @param savedInstanceState
     */
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.activity_comms, container, false);

        send = (FloatingActionButton) root.findViewById(R.id.messageButton);

        messageReceivedTextView = (TextView) root.findViewById(R.id.messageReceivedTextView);
        messageReceivedTextView.setMovementMethod(new ScrollingMovementMethod());
        typeBoxEditText = (EditText) root.findViewById(R.id.typeBoxEditText);

        // get shared preferences
        sharedPreferences = getActivity().getSharedPreferences("Shared Preferences", Context.MODE_PRIVATE);

        send.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                showLog("Clicked sendTextBtn");
                String sentText = "" + typeBoxEditText.getText().toString();

                SharedPreferences.Editor editor = sharedPreferences.edit();
                editor.putString("message", sharedPreferences.getString("message", "") + '\n' + sentText);
                editor.commit();
                messageReceivedTextView.setText(sharedPreferences.getString("message", ""));
                typeBoxEditText.setText("");

                if (BluetoothConnectionService.BluetoothConnectionStatus == true) {
                    byte[] bytes = sentText.getBytes(Charset.defaultCharset());
                    BluetoothConnectionService.write(bytes);
                }
                showLog("Exiting sendTextBtn");
            }
        });
        return root;
    }

    /*
     * Show Log
     * */
    private static void showLog(String message) {
        Log.d(TAG, message);
    }

    /*
     * Message Received Text View
     * */
    public static TextView getMessageReceivedTextView() {
        return messageReceivedTextView;
    }
}
