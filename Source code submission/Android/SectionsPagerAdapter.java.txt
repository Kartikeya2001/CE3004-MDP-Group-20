package com.example.cz3004mdp;

import android.content.Context;

import androidx.annotation.Nullable;
import androidx.annotation.StringRes;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentPagerAdapter;

/**
 * A [FragmentPagerAdapter] that returns a fragment corresponding to
 * one of the sections/tabs/pages.
 * Date: 2021/08/20 - 2021/10/04
 * Android App Authors: Huang Chaoshan, Nicole Tan (Chen Jiaxin)
 */
public class SectionsPagerAdapter extends FragmentPagerAdapter {

    /**
     * Method to initialise tabs
     */
    @StringRes
    private static final int[] TAB_TITLES = new int[]{R.string.tab_text_2, R.string.tab_text_4};
    private final Context mContext;

    public SectionsPagerAdapter(Context context, FragmentManager fm) {
        super(fm);
        mContext = context;
    }

    /**
     * Method to getItem is called to instantiate the fragment for the given page.
     * Return a PlaceholderFragment (defined as a static inner class below).
     *
     * @param position Tab position
     */
    @Override
    public Fragment getItem(int position) {
        switch (position) {
            case 0:
                return CommandSection.newInstance(position + 1);
            case 1:
                return ControlSection.newInstance(position + 1);
            default:
                return PlaceholderFragment.newInstance(position + 1);
        }
    }

    /**
     * Method to get page title for fragment
     *
     * @param position Tab position
     */
    @Nullable
    @Override
    public CharSequence getPageTitle(int position) {
        return mContext.getResources().getString(TAB_TITLES[position]);
    }

    /**
     * Method to return the number of pages for fragment
     */
    @Override
    public int getCount() {
        // Show 4 total pages.
        return 2;
    }
}