//
//  CustomLookAndFeel.cpp
//  MicoForge
//
//  Created by Ben on 26.01.26.
//

#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
    // closed dropdown menu
    setColour(juce::ComboBox::backgroundColourId, juce::Colours::lightgrey);
    setColour(juce::ComboBox::textColourId, juce::Colours::black);
    setColour(juce::ComboBox::arrowColourId, juce::Colours::black);

    // open dropdown menu
    setColour(juce::PopupMenu::backgroundColourId, juce::Colours::lightgrey);
    setColour(juce::PopupMenu::textColourId, juce::Colours::black);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colours::blue);
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::black);

    // labels
    setColour(juce::Label::textColourId, juce::Colours::black);

    // slider value text boxes
    setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::darkgrey);

    // toggle button text and tick box
    setColour(juce::ToggleButton::textColourId, juce::Colours::black);
    setColour(juce::ToggleButton::tickColourId, juce::Colours::black);
    setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::black);
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float startAngle, float endAngle,
                                         juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
    float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    float centreX = bounds.getCentreX();
    float centreY = bounds.getCentreY();

    float trackWidth = 2.0f;
    float thumbSize = 5.0f;

    // background ring
    juce::Path bgArc;
    bgArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, startAngle, endAngle, true);
    g.setColour(juce::Colours::darkgrey);
    g.strokePath(bgArc, juce::PathStrokeType(trackWidth, juce::PathStrokeType::curved,
                                             juce::PathStrokeType::rounded));

    // value ring
    float angle = startAngle + sliderPos * (endAngle - startAngle);
    juce::Path valueArc;
    valueArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, startAngle, angle, true);
    g.setColour(juce::Colours::darkorange);
    g.strokePath(valueArc, juce::PathStrokeType(trackWidth, juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));

    // value thumb
    float thumbX = centreX + radius * std::cos(angle - juce::MathConstants<float>::halfPi);
    float thumbY = centreY + radius * std::sin(angle - juce::MathConstants<float>::halfPi);
    g.setColour(juce::Colours::white);
    g.fillEllipse(thumbX - thumbSize * 0.5f, thumbY - thumbSize * 0.5f, thumbSize, thumbSize);
}
