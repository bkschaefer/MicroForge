//
//  CustomLookAndFeel.hpp
//  MicoForge
//
//  Created by Ben on 26.01.26.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/**
 * Custom JUCE look-and-feel for the MicroForge plugin UI.
 *
 * Overrides the default JUCE V4 colour scheme and rotary slider rendering
 * to match the plugin's visual design: minimal line-style knobs with an
 * orange value arc and a white thumb dot.
 */
class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    /**
     * Sets the global colour scheme for all JUCE component types used
     * in the editor (sliders, combo boxes, labels, toggle buttons).
     */
    CustomLookAndFeel();

    /**
     * Draws a minimal rotary slider with a dark-grey background arc,
     * an orange value arc, and a small white thumb dot.
     * @param g          Graphics context
     * @param x          Bounding box x
     * @param y          Bounding box y
     * @param width      Bounding box width
     * @param height     Bounding box height
     * @param sliderPos  Normalised slider position (0.0–1.0)
     * @param startAngle Start angle of the arc in radians
     * @param endAngle   End angle of the arc in radians
     * @param slider     The slider component being drawn
     */
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          float startAngle, float endAngle, juce::Slider& slider) override;
};
