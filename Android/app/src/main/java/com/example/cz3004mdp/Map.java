package com.example.cz3004mdp;

import java.util.ArrayList;

/**
 * Map class
 * Date: 2021/08/20 - 2021/10/04
 * Android App Authors: Huang Chaoshan, Nicole Tan (Chen Jiaxin)
 */

public class Map {
    // Singleton pattern
    private static Map map;

    public static Map getInstance() {
        if (map == null) {
            map = new Map();
        }
        return map;
    }

    private Map() {
    }

    /**
     * Method to create array for obstacles created
     */
    private final ArrayList<Obstacle> obstacles = new ArrayList<>();

    /**
     * Method to get array for obstacles
     */
    public ArrayList<Obstacle> getObstacles() {
        return this.obstacles;
    }

    /**
     * Method to add new obstacle
     */
    public Obstacle addObstacle() {
        int number = this.obstacles.size() + 1;
        Obstacle newObstacle = new Obstacle(number);
        this.obstacles.add(newObstacle);
        return newObstacle;
    }

    /**
     * Method to remove existing obstacle
     */
    public void removeObstacle(Obstacle obstacle) {
        int indexToRemove = obstacle.getNumber() - 1;
        this.obstacles.remove(indexToRemove);
        for (int i = indexToRemove; i < this.obstacles.size(); i++) {
            this.obstacles.get(i).setNumber(i + 1);
        }
    }

    /**
     * Method to get x, y coordinate for obstacle
     */
    public ICoordinate findObstacle(int x, int y) {
        for (ICoordinate obstacle : this.obstacles) {
            if (obstacle.containsCoordinate(x, y)) {
                return obstacle;
            }
        }
        return null;
    }

    /**
     * Method to check the current coordinate for occupancy
     */
    public boolean isOccupied(int x, int y, Obstacle obstacle) {
        for (Obstacle o : this.obstacles) {
            if (o.containsCoordinate(x, y) && o != obstacle) {
                return true;
            }
        }
        return false;
    }
}
