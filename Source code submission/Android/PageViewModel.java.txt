package com.example.cz3004mdp;

import androidx.arch.core.util.Function;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.Transformations;
import androidx.lifecycle.ViewModel;

/**
 * PageViewModel class
 * Date: 2021/08/20 - 2021/10/04
 * Android App Authors: Huang Chaoshan, Nicole Tan (Chen Jiaxin)
 */
public class PageViewModel extends ViewModel {

    /**
     * Method to create a LiveData with Integer
     */
    private MutableLiveData<Integer> mIndex = new MutableLiveData<>();

    /**
     * Method to initialise the text with the page number
     * LiveData is an observable data holder class.
     */
    private LiveData<String> mText = Transformations.map(mIndex, new Function<Integer, String>() {
        @Override
        public String apply(Integer input) {
            return "Hello world from section: " + input;
        }
    });

    /**
     * Method to set index
     */
    public void setIndex(int index) {
        mIndex.setValue(index);
    }

    /**
     * Method to get text
     */
    public LiveData<String> getText() {
        return mText;
    }
}