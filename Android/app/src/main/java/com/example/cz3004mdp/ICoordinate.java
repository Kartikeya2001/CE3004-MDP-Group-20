package com.example.cz3004mdp;

public interface ICoordinate {
    int getX();

    int getY();

    void setCoordinates(int x, int y);

    boolean containsCoordinate(int x, int y);
}
