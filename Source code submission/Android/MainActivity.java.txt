package com.example.cz3004mdp;

import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.MutableLiveData;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;
import androidx.viewpager.widget.ViewPager;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.Bundle;

import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.material.tabs.TabLayout;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.math.BigInteger;
import java.nio.charset.Charset;
import java.util.UUID;

/**
 * Date: 2021/08/20 - 2021/10/04
 * Android App Authors: Huang Chaoshan, Nicole Tan (Chen Jiaxin)
 */

public class MainActivity extends AppCompatActivity {
    // Variable declaration
    static Robot robot = new Robot();
    MutableLiveData<String> listen = new MutableLiveData<>();
    public static TextView xAxisTextView;
    public static TextView yAxisTextView;
    public static TextView directionAxisTextView;
    static TextView robotStatusTextView;


    public boolean tiltChk = false;
    private static GridMap gridMap;
    private static SharedPreferences sharedPreferences;
    private static SharedPreferences.Editor editor;
    private static Context context;


    BluetoothConnectionService mBluetoothConnection;
    BluetoothDevice mBTDevice;
    private static UUID myUUID;
    ProgressDialog myDialog;

    private static final String TAG = "Main Activity";

    /**
     * Start the onCreate activity
     * Create a ViewModel the first time the system calls an activity's onCreate() method.
     * Re-created activities receive the same MyViewModel instance created by the first activity.
     */

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        // Initialization
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //Map
        gridMap = findViewById(R.id.map);

        //Update Robot Pos
        xAxisTextView = findViewById(R.id.xAxisTextView);
        yAxisTextView = findViewById(R.id.yAxisTextView);

        //Update Robot Direction
        directionAxisTextView = findViewById(R.id.directionAxisTextView);

        //Update Robot Status
        robotStatusTextView = findViewById(R.id.robotStatusTextView);

        //section pages
        SectionsPagerAdapter sectionsPagerAdapter = new SectionsPagerAdapter(this, getSupportFragmentManager());
        ViewPager viewPager = findViewById(R.id.view_pager);
        viewPager.setAdapter(sectionsPagerAdapter);
        viewPager.setOffscreenPageLimit(9999);
        TabLayout tabs = findViewById(R.id.tabs);
        tabs.setupWithViewPager(viewPager);
        LocalBroadcastManager.getInstance(this).registerReceiver(messageReceiver, new IntentFilter("incomingMessage"));

        // Set up sharedPreferences
        MainActivity.context = getApplicationContext();
        this.sharedPreferences();
        editor.putString("message", "");
        editor.putString("direction", "None");
        editor.putString("connStatus", "Disconnected");
        editor.commit();

        // Set layout to shift up when soft keyboard is open
        getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_PAN);

        // Bluetooth button
        Button bluetoothButton = (Button) findViewById(R.id.bluetoothButton);
        bluetoothButton.setOnClickListener(v -> {
            Intent popup = new Intent(MainActivity.this, BluetoothPopUpWindow.class);
            startActivity(popup);
        });

        // Listening to the bluetooth connection
        myDialog = new ProgressDialog(MainActivity.this);
        myDialog.setMessage("Waiting for other device to reconnect...");
        myDialog.setCancelable(false);
        myDialog.setButton(DialogInterface.BUTTON_NEGATIVE, "Cancel", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });

    }

    /**
     * Get the gridMap
     */
    public static GridMap getGridMap() {
        return gridMap;
    }

    /**
     * Get the RobotStatusText
     */
    public static TextView getRobotStatusTextView() {
        return robotStatusTextView;
    }

    /**
     * Method call the pop up window for obstacle and set side for obstacle
     *
     * @param c        The UI Activity Context
     * @param view     The view of the UI displayed
     * @param obstacle The obstacle
     */
    public static void showObstaclePopup(Context c, View view, Obstacle obstacle) {
        // inflate the layout of the popup window
        LayoutInflater inflater = (LayoutInflater) c.getSystemService(LAYOUT_INFLATER_SERVICE);
        View popupView = inflater.inflate(R.layout.obstacle_popup, null);

        // create the popup window
        int width = LinearLayout.LayoutParams.WRAP_CONTENT;
        int height = LinearLayout.LayoutParams.WRAP_CONTENT;
        final PopupWindow popupWindow = new PopupWindow(popupView, width, height, true);

        Button btnN = (Button) popupView.findViewById(R.id.obstacleN);
        Button btnS = (Button) popupView.findViewById(R.id.obstacleS);
        Button btnE = (Button) popupView.findViewById(R.id.obstacleE);
        Button btnW = (Button) popupView.findViewById(R.id.obstacleW);

        // show the popup window
        popupWindow.showAtLocation(view, Gravity.CENTER, 0, 0);

        GridMap mapGrid = view.findViewById(R.id.map);

        btnN.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mapGrid.setObstacleSide(obstacle, 'N');
                popupWindow.dismiss();
            }
        });

        btnS.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mapGrid.setObstacleSide(obstacle, 'S');
                popupWindow.dismiss();
            }
        });

        btnE.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mapGrid.setObstacleSide(obstacle, 'E');
                popupWindow.dismiss();
            }
        });

        btnW.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mapGrid.setObstacleSide(obstacle, 'W');
                popupWindow.dismiss();
            }
        });

        // dismiss the popup window when touched
        popupView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                popupWindow.dismiss();
                return true;
            }
        });
    }

    /**
     * Method set the position of robot
     *
     * @param x         Coordinate x of robot
     * @param y         Coordinate y of robot
     * @param direction direction faced of robot
     */

    public static boolean setRobotPosition(int x, int y, char direction) {
        if (1 <= x && x <= 18 && 1 <= y && y <= 18 && (direction == 'N' || direction == 'S' || direction == 'E' || direction == 'W')) {
            robot.setCoordinates(x, y);
            robot.setDirection(direction);
            xAxisTextView.setText(String.valueOf(robot.getX()));
            yAxisTextView.setText(String.valueOf(robot.getY()));
            directionAxisTextView.setText(String.valueOf(robot.getDirection()));
            gridMap.invalidate();
            return true;
        }
        return false;
    }

    /**
     * Method call sharedPreferences
     */
    public static void sharedPreferences() {
        sharedPreferences = MainActivity.getSharedPreferences(MainActivity.context);
        editor = sharedPreferences.edit();
    }

    /**
     * Method to display the target ID for obstacle
     *
     * @param obstacleNumber The obstacle number on the grid map
     * @param targetID       The target ID explored and replace the obstacle number
     */
    public static boolean exploreTarget(int obstacleNumber, int targetID) {
        // if obstacle number exists in map
        if (1 <= obstacleNumber && obstacleNumber <= Map.getInstance().getObstacles().size()) {
            Obstacle obstacle = Map.getInstance().getObstacles().get(obstacleNumber - 1);
            obstacle.explore(targetID);
            gridMap.invalidate();
            return true;
        }

        return false;
    }

    /**
     * Method to updated the status of robot
     *
     * @param status The current status of robot
     */

    public static void updateRobotStatus(String status) {
        robot.setStatus(status);
        robotStatusTextView.setText(robot.getStatus());
    }

    /**
     * Method to print the received message
     *
     * @param message The message to print
     */
    public static void printMessage(String message) {
        showLog("Entering printMessage");
        editor = sharedPreferences.edit();

        if (BluetoothConnectionService.BluetoothConnectionStatus == true) {
            byte[] bytes = message.getBytes(Charset.defaultCharset());
            BluetoothConnectionService.write(bytes);
        }
        showLog(message);
        editor.putString("message", CommandSection.getMessageReceivedTextView().getText() + "\n" + message);
        editor.commit();

        refreshMessageReceived();
        showLog("Exiting printMessage");
    }

    /**
     * Method to send message via bluetooth
     *
     * @param message The message to send
     */
    public static void sendMsg(String message) {
        // Check that we're actually connected before trying anything
        if (BluetoothConnectionService.BluetoothConnectionStatus == true && message.length() > 0) {
            byte[] send = message.getBytes();
            BluetoothConnectionService.write(send);
        }
        editor.putString("message", CommandSection.getMessageReceivedTextView().getText() + "\n" + message);
        editor.commit();
        refreshMessageReceived();
    }

    /**
     * Method to refresh the ui for message received
     */
    public static void refreshMessageReceived() {
        CommandSection.getMessageReceivedTextView().setText(sharedPreferences.getString("message", ""));
    }

    /**
     * Method to show the log message on console
     */
    private static void showLog(String message) {
        Log.d(TAG, message);
    }

    /**
     * Method to get shared preferences context
     *
     * @param context The UI activity Context
     */
    private static SharedPreferences getSharedPreferences(Context context) {
        return context.getSharedPreferences("Shared Preferences", Context.MODE_PRIVATE);
    }

    /**
     * Method to create the broadcast receiver for the bluetooth connection event
     */
    private BroadcastReceiver mBroadcastReceiver5 = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            BluetoothDevice mDevice = intent.getParcelableExtra("Device");
            String status = intent.getStringExtra("Status");
            sharedPreferences();

            if (status.equals("connected")) {
                try {
                    myDialog.dismiss();
                } catch (NullPointerException e) {
                    e.printStackTrace();
                }

                Log.d(TAG, "mBroadcastReceiver5: Device now connected to " + mDevice.getName());
                Toast.makeText(MainActivity.this, "Device now connected to " + mDevice.getName(), Toast.LENGTH_LONG).show();
                editor.putString("connStatus", "Connected to " + mDevice.getName());

            } else if (status.equals("disconnected")) {
                Log.d(TAG, "mBroadcastReceiver5: Disconnected from " + mDevice.getName());
                Toast.makeText(MainActivity.this, "Disconnected from " + mDevice.getName(), Toast.LENGTH_LONG).show();
                editor.putString("connStatus", "Disconnected");

                myDialog.show();
            }
            editor.commit();
        }
    };

    /**
     * Method to create the broadcast receiver for the message received through Bluetooth
     */
    BroadcastReceiver messageReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String message = intent.getStringExtra("receivedMessage");
            String commandType = "";
            showLog("receivedMessage: message --- " + message);
            try {
                if (message.length() > 7 && message.split(",")[0].equals("ROBOT")) {
                    String[] splitString = message.split(",");
                    if (splitString.length == 4 && isInteger(splitString[1]) && isInteger(splitString[2]) && splitString[3].length() == 1) {
                        setRobotPosition(Integer.parseInt(splitString[1]), Integer.parseInt(splitString[2]), splitString[3].charAt(0));
                        commandType = "T";
                    } else if (splitString.length == 2) {
                        updateRobotStatus(splitString[1]);
                        commandType = "T";
                    }
                } else if (message.length() > 7 && message.split(",")[0].equals("TARGET")) {
                    String[] splitString = message.split(",");
                    if (splitString.length == 3 && isInteger(splitString[1]) && isInteger(splitString[2])) {
                        exploreTarget(Integer.parseInt(splitString[1]), Integer.parseInt(splitString[2]));
                        commandType = "T";

                    }
                }
                if (!commandType.equalsIgnoreCase("T")) {
                    String amdString = message;
                    showLog("amdString: " + amdString);
                    byte[] amdBytes = amdString.getBytes();
                    BigInteger hexBigIntegerExplored = new BigInteger(amdBytes);
                    String exploredString = hexBigIntegerExplored.toString(16);
                    showLog(exploredString);
                }

                showLog("Executed for AMD message, message: " + message);

            } catch (ArrayIndexOutOfBoundsException e) {
                e.printStackTrace();
            }

            sharedPreferences();
            String receivedText = sharedPreferences.getString("message", "") + "\n" + message;
            editor.putString("message", receivedText);
            editor.commit();
            refreshMessageReceived();
        }
    };

    /**
     * Method to check bluetooth connection activity
     *
     * @param requestCode The activity request code
     * @param resultCode  The activity result code
     * @param data        The data gent from intent request
     */
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        switch (requestCode) {
            case 1:
                if (resultCode == Activity.RESULT_OK) {
                    mBTDevice = (BluetoothDevice) data.getExtras().getParcelable("mBTDevice");
                    myUUID = (UUID) data.getSerializableExtra("myUUID");
                }
        }
    }

    /**
     * Method to close connection and unregister the broadcast receiver for bluetooth connection and message
     */
    @Override
    protected void onDestroy() {
        super.onDestroy();
        try {
            LocalBroadcastManager.getInstance(this).unregisterReceiver(messageReceiver);
            LocalBroadcastManager.getInstance(this).unregisterReceiver(mBroadcastReceiver5);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        }
    }

    /**
     * Method to pause connection and unregister the broadcast receiver
     */
    @Override
    protected void onPause() {
        super.onPause();
        try {
            LocalBroadcastManager.getInstance(this).unregisterReceiver(mBroadcastReceiver5);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        }
    }

    /**
     * Method to resume the bluetooth connection
     */
    @Override
    protected void onResume() {
        super.onResume();
        try {
            IntentFilter filter2 = new IntentFilter("ConnectionStatus");

            LocalBroadcastManager.getInstance(this).registerReceiver(mBroadcastReceiver5, filter2);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        }
    }

    /**
     * Method to create onsaveinstance state
     */
    @Override
    public void onSaveInstanceState(Bundle outState) {
        showLog("Entering onSaveInstanceState");
        super.onSaveInstanceState(outState);

        outState.putString(TAG, "onSaveInstanceState");
        showLog("Exiting onSaveInstanceState");
    }

    /**
     * Method to convert string input to integer
     */
    private boolean isInteger(String input) {
        try {
            Integer.parseInt(input);
            return true;
        } catch (NumberFormatException e) {
            return false;
        }
    }

}