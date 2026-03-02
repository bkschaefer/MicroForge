/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "CustomLookAndFeel.h"
#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
/**
 */
class MicroForgeAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    MicroForgeAudioProcessorEditor(MicroForgeAudioProcessor&);
    ~MicroForgeAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider densitySlider_;
    juce::Slider densityVarSlider_;

    juce::Slider distributionSlider_;

    juce::Slider grainLengthSlider_;
    juce::Slider grainLengthVarSlider_;

    juce::Slider delayTimeSlider_;
    juce::Slider delayTimeVarSlider_;

    juce::Slider delayFeedbackSlider_;

    juce::Slider playbackRateSlider_;
    juce::Slider playbackRateVarSlider_;

    juce::Slider mixSlider_;
    juce::ToggleButton granulationModeButton_;
    juce::ComboBox envelopeMenu_;

    juce::Label densityLabel_;
    juce::Label densityVarLabel_;

    juce::Label distributionLabel_;

    juce::Label grainLengthLabel_;
    juce::Label grainLengthVarLabel_;

    juce::Label delayTimeLabel_;
    juce::Label delayTimeVarLabel_;

    juce::Label delayFeedbackLabel_;

    juce::Label playbackRateLabel_;
    juce::Label playbackRateVarLabel_;

    juce::Label mixLabel_;
    juce::ToggleButton granulationModeLabel_;
    juce::Label envelopeMenuLabel_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> densitySliderAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        densityVarSliderAttachment_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        distributionSliderAttachment_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        grainLengthSliderAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        grainLengthVarSliderAttachment_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        delayTimeSliderAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        delayTimeVarSliderAttachment_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        delayFeedbackSliderAttachment_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        playbackRateSliderAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        playbackRateVarSliderAttachment_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixSliderAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        granulationModeButtonAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> envelopeMenuAttachment_;

    CustomLookAndFeel customLookAndFeel_;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MicroForgeAudioProcessor& audioProcessor_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MicroForgeAudioProcessorEditor)
};
