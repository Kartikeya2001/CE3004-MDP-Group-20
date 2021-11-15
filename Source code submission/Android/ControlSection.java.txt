package com.example.cz3004mdp;

import static android.content.Context.SENSOR_SERVICE;

import android.content.Context;
import android.content.SharedPreferences;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageButton;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProviders;

import java.nio.charset.Charset;

/**
 * Control Section class
 * Date: 2021/08/20 - 2021/10/04
 * Android App Authors: Huang Chaoshan, Nicole Tan (Chen Jiaxin)
 */
public class ControlSection extends Fragment implements SensorEventListener {
    // Init
    private static final String ARG_SECTION_NUMBER = "section_number";
    private static final String TAG = "ControlFragment";
    private PageViewModel pageViewModel;
    private String sentText = "";
    // Declaration Variable
    // Shared Preferences
    SharedPreferences sharedPreferences;
    SharedPreferences.Editor editor;
    public static TextView xAxisTextView;
    public static TextView yAxisTextView;

    // Control Button
    ImageButton moveForwardImageBtn, turnRightImageBtn, moveBackImageBtn, turnLeftImageBtn, exploreResetButton, fastestResetButton;
    private static long exploreTimer, fastestTimer;
    ToggleButton exploreButton, fastestButton;
    TextView exploreTimeTextView, fastestTimeTextView, robotStatusTextView;
    Switch phoneTiltSwitch;
    static Button calibrateButton;
    private static GridMap gridMap;

    private Sensor mSensor;
    private SensorManager mSensorManager;

    // Timer
    static Handler timerHandler = new Handler();

    Runnable timerRunnableExplore = new Runnable() {
        @Override
        public void run() {
            long millisExplore = System.currentTimeMillis() - exploreTimer;
            int secondsExplore = (int) (millisExplore / 1000);
            int minutesExplore = secondsExplore / 60;
            secondsExplore = secondsExplore % 60;

            exploreTimeTextView.setText(String.format("%02d:%02d", minutesExplore, secondsExplore));

            timerHandler.postDelayed(this, 500);
        }
    };

    Runnable timerRunnableFastest = new Runnable() {
        @Override
        public void run() {
            long millisFastest = System.currentTimeMillis() - fastestTimer;
            int secondsFastest = (int) (millisFastest / 1000);
            int minutesFastest = secondsFastest / 60;
            secondsFastest = secondsFastest % 60;

            fastestTimeTextView.setText(String.format("%02d:%02d", minutesFastest, secondsFastest));

            timerHandler.postDelayed(this, 500);
        }
    };

    /**
     * Fragment Constructor
     *
     * @param index
     */
    public static ControlSection newInstance(int index) {
        ControlSection fragment = new ControlSection();
        Bundle bundle = new Bundle();
        bundle.putInt(ARG_SECTION_NUMBER, index);
        fragment.setArguments(bundle);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        pageViewModel = ViewModelProviders.of(this).get(PageViewModel.class);
        int index = 1;
        if (getArguments() != null) {
            index = getArguments().getInt(ARG_SECTION_NUMBER);
        }
        pageViewModel.setIndex(index);
    }

    /**
     * View Creation for the Control Section
     *
     * @param inflater
     * @param container
     * @param savedInstanceState
     */
    @Override
    public View onCreateView(
            @NonNull LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        // inflate
        View root = inflater.inflate(R.layout.activity_control, container, false);

        // get shared preferences
        sharedPreferences = getActivity().getSharedPreferences("Shared Preferences", Context.MODE_PRIVATE);

        // variable initialization
        moveForwardImageBtn = root.findViewById(R.id.forwardImageBtn);
        turnRightImageBtn = root.findViewById(R.id.rightImageBtn);
        moveBackImageBtn = root.findViewById(R.id.backImageBtn);
        turnLeftImageBtn = root.findViewById(R.id.leftImageBtn);
        exploreTimeTextView = root.findViewById(R.id.exploreTimeTextView);
        fastestTimeTextView = root.findViewById(R.id.fastestTimeTextView);
        exploreButton = root.findViewById(R.id.exploreToggleBtn);
        fastestButton = root.findViewById(R.id.fastestToggleBtn);
        exploreResetButton = root.findViewById(R.id.exploreResetImageBtn);
        fastestResetButton = root.findViewById(R.id.fastestResetImageBtn);
        phoneTiltSwitch = root.findViewById(R.id.phoneTiltSwitch);
        calibrateButton = root.findViewById(R.id.calibrateButton);

        robotStatusTextView = MainActivity.getRobotStatusTextView();
        fastestTimer = 0;
        exploreTimer = 0;

        mSensorManager = (SensorManager) getActivity().getSystemService(SENSOR_SERVICE);
        mSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);

        gridMap = MainActivity.getGridMap();

        // Button Listener
        moveForwardImageBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainActivity.robot.moveRobotForward();
                gridMap.invalidate();
                MainActivity.printMessage("f");
                updateStatus("moving forward");
                if (MainActivity.robot.getX() != -1 && MainActivity.robot.getY() != -1) {
                    MainActivity.xAxisTextView.setText(String.valueOf(MainActivity.robot.getX()));
                    MainActivity.yAxisTextView.setText(String.valueOf(MainActivity.robot.getY()));
                    MainActivity.directionAxisTextView.setText(String.valueOf(MainActivity.robot.getDirection()));
                } else {
                    MainActivity.xAxisTextView.setText("-");
                    MainActivity.yAxisTextView.setText("-");
                    MainActivity.directionAxisTextView.setText("-");
                }
                showLog("Exiting moveForwardImageBtn");
            }
        });

        turnRightImageBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainActivity.robot.moveRobotTurnRight();
                gridMap.invalidate();
                MainActivity.printMessage("r");
                updateStatus("turn right");
                if (MainActivity.robot.getX() != -1 && MainActivity.robot.getY() != -1) {
                    MainActivity.xAxisTextView.setText(String.valueOf(MainActivity.robot.getX()));
                    MainActivity.yAxisTextView.setText(String.valueOf(MainActivity.robot.getY()));
                    MainActivity.directionAxisTextView.setText(String.valueOf(MainActivity.robot.getDirection()));
                } else {
                    MainActivity.xAxisTextView.setText("-");
                    MainActivity.yAxisTextView.setText("-");
                    MainActivity.directionAxisTextView.setText("-");
                }
                showLog("Exiting turnRightImageBtn");
            }
        });

        moveBackImageBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainActivity.robot.moveRobotBackward();
                gridMap.invalidate();
                MainActivity.printMessage("b");
                updateStatus("moving backward");
                if (MainActivity.robot.getX() != -1 && MainActivity.robot.getY() != -1) {
                    MainActivity.xAxisTextView.setText(String.valueOf(MainActivity.robot.getX()));
                    MainActivity.yAxisTextView.setText(String.valueOf(MainActivity.robot.getY()));
                    MainActivity.directionAxisTextView.setText(String.valueOf(MainActivity.robot.getDirection()));
                } else {
                    MainActivity.xAxisTextView.setText("-");
                    MainActivity.yAxisTextView.setText("-");
                    MainActivity.directionAxisTextView.setText("-");
                }
                showLog("Exiting turnRightImageBtn");
            }
        });

        turnLeftImageBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                MainActivity.robot.moveRobotTurnLeft();
                gridMap.invalidate();
                MainActivity.printMessage("l");
                updateStatus("turn left");
                if (MainActivity.robot.getX() != -1 && MainActivity.robot.getY() != -1) {
                    MainActivity.xAxisTextView.setText(String.valueOf(MainActivity.robot.getX()));
                    MainActivity.yAxisTextView.setText(String.valueOf(MainActivity.robot.getY()));
                    MainActivity.directionAxisTextView.setText(String.valueOf(MainActivity.robot.getDirection()));
                } else {
                    MainActivity.xAxisTextView.setText("-");
                    MainActivity.yAxisTextView.setText("-");
                    MainActivity.directionAxisTextView.setText("-");
                }
                showLog("Exiting turnRightImageBtn");
            }
        });

        exploreButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showLog("Clicked exploreToggleBtn");
                ToggleButton exploreToggleBtn = (ToggleButton) v;
                if (exploreToggleBtn.getText().equals("EXPLORE")) {
                    showToast("Exploration timer stop!");
                    robotStatusTextView.setText("Exploration Stopped");
                    timerHandler.removeCallbacks(timerRunnableExplore);
                } else if (exploreToggleBtn.getText().equals("STOP")) {
                    showToast("Exploration timer start!");
                    MainActivity.printMessage("beginExplore");
                    robotStatusTextView.setText("Exploration Started");
                    exploreTimer = System.currentTimeMillis();
                    timerHandler.postDelayed(timerRunnableExplore, 0);
                } else {
                    showToast("Else statement: " + exploreToggleBtn.getText());
                }
                showLog("Exiting exploreToggleBtn");
            }
        });

        fastestButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showLog("Clicked fastestToggleBtn");
                ToggleButton fastestToggleBtn = (ToggleButton) v;
                if (fastestToggleBtn.getText().equals("FASTEST")) {
                    showToast("Fastest timer stop!");
                    robotStatusTextView.setText("Fastest Path Stopped");
                    timerHandler.removeCallbacks(timerRunnableFastest);
                } else if (fastestToggleBtn.getText().equals("STOP")) {
                    showToast("Fastest timer start!");
                    MainActivity.printMessage("beginFastest");
                    robotStatusTextView.setText("Fastest Path Started");
                    fastestTimer = System.currentTimeMillis();
                    timerHandler.postDelayed(timerRunnableFastest, 0);
                } else
                    showToast(fastestToggleBtn.getText().toString());
                showLog("Exiting fastestToggleBtn");
            }
        });

        exploreResetButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showLog("Clicked exploreResetImageBtn");
                showToast("Reseting exploration time...");
                exploreTimeTextView.setText("00:00");
                robotStatusTextView.setText("Not Available");
                if (exploreButton.isChecked())
                    exploreButton.toggle();
                timerHandler.removeCallbacks(timerRunnableExplore);
                showLog("Exiting exploreResetImageBtn");
            }
        });

        fastestResetButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showLog("Clicked fastestResetImageBtn");
                showToast("Reseting fastest time...");
                fastestTimeTextView.setText("00:00");
                robotStatusTextView.setText("Not Available");
                if (fastestButton.isChecked())
                    fastestButton.toggle();
                timerHandler.removeCallbacks(timerRunnableFastest);
                showLog("Exiting fastestResetImageBtn");
            }
        });

        calibrateButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showLog("Clicked Calibrate Button");
                MainActivity.printMessage("SS|");
                //MapSection.manualUpdateRequest = true;
                showLog("Exiting Calibrate Button");
            }
        });
        return root;
    }

    /**
     * Show Log
     */
    private static void showLog(String message) {
        Log.d(TAG, message);
    }

    /**
     * Show Toast
     */
    private void showToast(String message) {
        Toast.makeText(getContext(), message, Toast.LENGTH_SHORT).show();
    }

    Handler sensorHandler = new Handler();
    boolean sensorFlag = false;

    private final Runnable sensorDelay = new Runnable() {
        @Override
        public void run() {
            sensorFlag = true;
            sensorHandler.postDelayed(this, 1000);
        }
    };

    /**
     * Detection of changes to the sensor
     *
     * @param event
     */
    @Override
    public void onSensorChanged(SensorEvent event) {
        float x = event.values[0];
        float y = event.values[1];
        float z = event.values[2];
        showLog("SensorChanged X: " + x);
        showLog("SensorChanged Y: " + y);
        showLog("SensorChanged Z: " + z);

//        if(sensorFlag) {
//            if (y < -2) {
//                showLog("Sensor Move Forward Detected");
//                gridMap.moveRobot("forward");
//                MainActivity.refreshLabel();
//                MainActivity.printMessage("f");
//            } else if (y > 2) {
//                showLog("Sensor Move Backward Detected");
//                gridMap.moveRobot("back");
//                MainActivity.refreshLabel();
//                MainActivity.printMessage("r");
//            } else if (x > 2) {
//                showLog("Sensor Move Left Detected");
//                gridMap.moveRobot("left");
//                MainActivity.refreshLabel();
//                MainActivity.printMessage("sl");
//            } else if (x < -2) {
//                showLog("Sensor Move Right Detected");
//                gridMap.moveRobot("right");
//                MainActivity.refreshLabel();
//                MainActivity.printMessage("sr");
//            }
//        }
        sensorFlag = false;
    }

    /*
     * Accuracy Changed
     * */
    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {
    }

    /*
     * Destroy
     * */
    @Override
    public void onDestroy() {
        super.onDestroy();
        try {
            mSensorManager.unregisterListener(ControlSection.this);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        }
    }

    /*
     * Update Status with message
     * */
    private void updateStatus(String message) {
        Toast toast = Toast.makeText(getContext(), message, Toast.LENGTH_SHORT);
        toast.setGravity(Gravity.TOP, 0, 0);
        toast.show();
    }

    /*
     * Calibration Button
     * */
    public static Button getCalibrateButton() {
        return calibrateButton;
    }
}
