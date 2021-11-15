package com.example.cz3004mdp;

/**
 * Obstacle class
 * Date: 2021/08/20 - 2021/10/04
 * Android App Authors: Huang Chaoshan, Nicole Tan (Chen Jiaxin)
 */
public class Obstacle implements ICoordinate {
    private int x;
    private int y;
    private int number;
    private int targetID;
    private char side;
    private boolean isExplored;

    /**
     * Method to initialise Obstacle
     *
     * @param number obstacle number received
     */
    public Obstacle(int number) {
        this.x = -1;
        this.y = -1;
        this.number = number;
        this.isExplored = false;
    }

    /**
     * Method to get x coordinate of obstacle
     */
    public int getX() {
        return this.x;
    }

    /**
     * Method to get y coordinate of obstacle
     */
    public int getY() {
        return this.y;
    }

    /**
     * Method to get obstacle number
     */
    public int getNumber() {
        return this.number;
    }

    /**
     * Method to set obstacle number
     *
     * @param number obstacle number received
     */
    public void setNumber(int number) {
        this.number = number;
    }

    /**
     * Method to get Target ID explored
     */
    public int getTargetID() {
        return this.targetID;
    }

    /**
     * Method to get image side
     */
    public char getSide() {
        return this.side;
    }

    /**
     * Method to set image side
     *
     * @param side obstacle side received, eg.N,S,E,W
     */
    public boolean setSide(char side) {
        if (side != 'N' && side != 'S' && side != 'E' && side != 'W') {
            return false;
        } else {
            this.side = side;
            return true;
        }
    }

    /**
     * Method to set explore status and set targetID
     *
     * @param targetID TargetID explored
     */
    public void explore(int targetID) {
        this.targetID = targetID;
        isExplored = true;
    }

    /**
     * Method to check the explored status
     */
    public boolean isExplored() {
        return this.isExplored;
    }

    /**
     * Method to set x,y coordinate of obstacle
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
     * Method to check x,y coordinate of obstacle exist in the grid map
     *
     * @param x x coordinate received
     * @param y y coordinate received
     */
    @Override
    public boolean containsCoordinate(int x, int y) {
        if (this.x == x && this.y == y) {
            return true;
        }
        return false;
    }


    /**
     * Method to set the obstacle instances
     * * @param o obstacle Object
     */
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;

        if (o instanceof Obstacle) {
            Obstacle obstacle = (Obstacle) o;
            return x == obstacle.getX() && y == obstacle.getY();
        } else {
            return false;
        }
    }

}
