package com.example.cz3004mdp;

import android.util.Log;

/**
 * Robot class
 * Date: 2021/08/20 - 2021/10/04
 * Android App Authors: Huang Chaoshan, Nicole Tan (Chen Jiaxin)
 */
public class Robot implements ICoordinate {
    private int x;
    private int y;
    private String status;
    private char direction;

    /**
     * Initialise Robot
     */
    public Robot() {
        direction = 'N';
        x = -1;
        y = -1;
    }

    /**
     * Method to get x coordinate of robot
     */
    @Override
    public int getX() {
        return this.x;
    }

    /**
     * Method to set x coordinate of robot
     *
     * @param x x coordinate received
     */
    public void setX(int x) {
        this.x = x;
    }

    /**
     * Method to get y coordinate of robot
     */
    @Override
    public int getY() {
        return this.y;
    }

    /**
     * Method to set y coordinate of robot
     *
     * @param y y coordinate received
     */
    public void setY(int y) {
        this.y = y;
    }

    /**
     * Method to set x,y coordinate of robot
     *
     * @param x x coordinate received
     * @param y y coordinate received
     */
    @Override
    public void setCoordinates(int x, int y) {
        this.x = x;
        this.y = y;
    }

    /**
     * Method to check x,y coordinate for grid map
     *
     * @param x x coordinate received
     * @param y y coordinate received
     */
    @Override
    public boolean containsCoordinate(int x, int y) {
        Log.d("ROBOT:", "" + this.x + ", " + this.y);
        if (this.x <= x && x <= (this.x + 2) && this.y <= y && y <= (this.y + 2)) {
            return true;
        }
        return false;
    }

    /**
     * Method to get robot status
     */
    public String getStatus() {
        return status;
    }

    /**
     * Method to set robot status
     *
     * @param status Robot status
     */
    public void setStatus(String status) {
        this.status = status;
    }

    /**
     * Method to get robot faced direction
     */
    public char getDirection() {
        return direction;
    }

    /**
     * Method to set robot faced direction
     *
     * @param direction Robot faced direction e.g N,S,E,W
     */
    public void setDirection(char direction) {
        this.direction = direction;
    }

    /**
     * Method to set robot forward movement
     */
    public void moveRobotForward() {
        char robotDir = getDirection();
        if (this.x != -1 && this.y != -1) {
            if (robotDir == 'N') {
                int newY = this.y + 1;
                if (newY <= 18) {
                    this.setY(newY);
                }
            } else if (robotDir == 'S') {
                int newY = this.y - 1;
                if (newY >= 1) {
                    this.setY(newY);
                }
            } else if (robotDir == 'E') {
                int newX = this.x + 1;
                if (newX <= 18) {
                    this.setX(newX);
                }
            } else {
                //W
                int newX = this.x - 1;
                if (newX >= 1) {
                    this.setX(newX);
                }
            }
        }
    }

    /**
     * Method to set robot backward movement
     */
    public void moveRobotBackward() {
        char robotDir = getDirection();
        if (this.x != -1 && this.y != -1) {
            if (robotDir == 'N') {
                int newY = this.y - 1;
                if (newY >= 1) {
                    this.setY(newY);
                }
            } else if (robotDir == 'S') {
                int newY = this.y + 1;
                if (newY <= 18) {
                    this.setY(newY);
                }
            } else if (robotDir == 'E') {
                int newX = this.x - 1;
                if (newX >= 1) {
                    this.setX(newX);
                }
            } else {
                //W
                int newX = this.x + 1;
                if (newX <= 18) {
                    this.setX(newX);
                }
            }
        }
    }

    /**
     * Method to set robot turn left movement
     */
    public void moveRobotTurnLeft() {
        if (this.x != -1 && this.y != -1) {
            char robotDir = this.getDirection();
            if (robotDir == 'N') {
                this.setDirection('W');
            } else if (robotDir == 'S') {
                this.setDirection('E');
            } else if (robotDir == 'E') {
                this.setDirection('N');
            } else {
                //W
                this.setDirection('S');
            }
        }
    }

    /**
     * Method to set robot turn right movement
     */
    public void moveRobotTurnRight() {
        if (this.x != -1 && this.y != -1) {
            char robotDir = this.getDirection();
            if (robotDir == 'N') {
                this.setDirection('E');
            } else if (robotDir == 'S') {
                this.setDirection('W');
            } else if (robotDir == 'E') {
                this.setDirection('S');
            } else {
                //W
                this.setDirection('N');
            }
        }
    }

    /**
     * Method to set robot robot position on grid map
     */
    public void reset() {
        setCoordinates(-1, -1);
        this.direction = 'N';
    }

}
