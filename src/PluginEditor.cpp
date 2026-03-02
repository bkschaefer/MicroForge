/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
MicroForgeAudioProcessorEditor::MicroForgeAudioProcessorEditor(MicroForgeAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , audioProcessor_(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    // Setup sliders
    densitySlider_.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    densitySlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
    addAndMakeVisible(densitySlider_);

    densityVarSlider_.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    densityVarSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 15);
    addAndMakeVisible(densityVarSlider_);

    distributionSlider_.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    distributionSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
    addAndMakeVisible(distributionSlider_);

    grainLengthSlider_.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    grainLengthSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
    addAndMakeVisible(grainLengthSlider_);

    grainLengthVarSlider_.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    grainLengthVarSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 15);
    addAndMakeVisible(grainLengthVarSlider_);

    delayTimeSlider_.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    delayTimeSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
    addAndMakeVisible(delayTimeSlider_);

    delayTimeVarSlider_.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    delayTimeVarSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 15);
    addAndMakeVisible(delayTimeVarSlider_);

    delayFeedbackSlider_.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    delayFeedbackSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
    addAndMakeVisible(delayFeedbackSlider_);

    playbackRateSlider_.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    playbackRateSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
    addAndMakeVisible(playbackRateSlider_);

    playbackRateVarSlider_.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    playbackRateVarSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 15);
    addAndMakeVisible(playbackRateVarSlider_);

    mixSlider_.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mixSlider_.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
    addAndMakeVisible(mixSlider_);

    // Setup slider labels
    densityLabel_.setText("Density", juce::dontSendNotification);
    densityLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(densityLabel_);

    densityVarLabel_.setText("Var", juce::dontSendNotification);
    densityVarLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(densityVarLabel_);

    distributionLabel_.setText("Distribution", juce::dontSendNotification);
    distributionLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(distributionLabel_);

    grainLengthLabel_.setText("Grain Length", juce::dontSendNotification);
    grainLengthLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(grainLengthLabel_);

    grainLengthVarLabel_.setText("Var", juce::dontSendNotification);
    grainLengthVarLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(grainLengthVarLabel_);

    delayTimeLabel_.setText("Delay Time", juce::dontSendNotification);
    delayTimeLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayTimeLabel_);

    delayTimeVarLabel_.setText("Var", juce::dontSendNotification);
    delayTimeVarLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayTimeVarLabel_);

    delayFeedbackLabel_.setText("Delay Feedback", juce::dontSendNotification);
    delayFeedbackLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayFeedbackLabel_);

    playbackRateLabel_.setText("Playback Rate", juce::dontSendNotification);
    playbackRateLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(playbackRateLabel_);

    playbackRateVarLabel_.setText("Var", juce::dontSendNotification);
    playbackRateVarLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(playbackRateVarLabel_);

    mixLabel_.setText("Mix", juce::dontSendNotification);
    mixLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(mixLabel_);

    // Setup toggle button and dropdown list
    granulationModeButton_.setButtonText("Asynchronous");
    addAndMakeVisible(granulationModeButton_);

    // Setup envelope ComboBox
    envelopeMenu_.addItem("Gaussian", 1);
    envelopeMenu_.addItem("Parabolic", 2);
    envelopeMenu_.setSelectedId(1);
    addAndMakeVisible(envelopeMenu_);

    // Setup envelope ComboBox label
    envelopeMenuLabel_.setText("Envelope", juce::dontSendNotification);
    envelopeMenuLabel_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(envelopeMenuLabel_);

    // Setup attachments
    densitySliderAttachment_ =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor_.apvts, "DENSITY", densitySlider_);
    densityVarSliderAttachment_ =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor_.apvts, "DENSITY VAR", densityVarSlider_);
    distributionSliderAttachment_ =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor_.apvts, "DISTRIBUTION", distributionSlider_);
    grainLengthSliderAttachment_ =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor_.apvts, "GRAIN LENGTH", grainLengthSlider_);
    grainLengthVarSliderAttachment_ =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor_.apvts, "GRAIN LENGTH VAR", grainLengthVarSlider_);
    delayTimeSliderAttachment_ =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor_.apvts, "DELAY TIME", delayTimeSlider_);
    delayTimeVarSliderAttachment_ =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor_.apvts, "DELAY TIME VAR", delayTimeVarSlider_);
    delayFeedbackSliderAttachment_ =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor_.apvts, "DELAY FEEDBACK", delayFeedbackSlider_);
    playbackRateSliderAttachment_ =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor_.apvts, "PLAYBACK RATE", playbackRateSlider_);
    playbackRateVarSliderAttachment_ =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor_.apvts, "PLAYBACK RATE VAR", playbackRateVarSlider_);

    mixSliderAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor_.apvts, "MIX", mixSlider_);

    granulationModeButtonAttachment_ =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            audioProcessor_.apvts, "MODE", granulationModeButton_);

    envelopeMenuAttachment_ =
        std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            audioProcessor_.apvts, "ENVELOPE", envelopeMenu_);

    setSize(600, 550);

    setLookAndFeel(&customLookAndFeel_);
}

MicroForgeAudioProcessorEditor::~MicroForgeAudioProcessorEditor() {}

//==============================================================================
void MicroForgeAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.fillAll(juce::Colours::lightgrey);
    g.setColour(juce::Colours::black);
    g.setFont(juce::FontOptions(24.0f));
    g.drawFittedText("MicroForge", juce::Rectangle<int>(0, 10, getWidth(), 30),
                     juce::Justification::centred, 1);
}

void MicroForgeAudioProcessorEditor::resized()
{
    int sliderWidth = 120;
    int sliderHeight = 120;
    int varSliderSize = 60;
    int labelHeight = 20;
    int margin = 30;

    // Calculate positions for 4-column grid
    int topY = 80;
    int varY = topY + sliderHeight + labelHeight;
    int bottomY = varY + varSliderSize + labelHeight + margin;
    int totalWidth = 4 * sliderWidth + 3 * margin;
    int leftX = (getWidth() - totalWidth) / 2;
    int centerLeftX = leftX + sliderWidth + margin;
    int centerRightX = centerLeftX + sliderWidth + margin;
    int rightX = centerRightX + sliderWidth + margin;

    // Top row - granular controls
    densityLabel_.setBounds(leftX, topY - labelHeight, sliderWidth, labelHeight);
    densitySlider_.setBounds(leftX, topY, sliderWidth, sliderHeight);

    grainLengthLabel_.setBounds(centerLeftX, topY - labelHeight, sliderWidth, labelHeight);
    grainLengthSlider_.setBounds(centerLeftX, topY, sliderWidth, sliderHeight);

    delayTimeLabel_.setBounds(centerRightX, topY - labelHeight, sliderWidth, labelHeight);
    delayTimeSlider_.setBounds(centerRightX, topY, sliderWidth, sliderHeight);

    playbackRateLabel_.setBounds(rightX, topY - labelHeight, sliderWidth, labelHeight);
    playbackRateSlider_.setBounds(rightX, topY, sliderWidth, sliderHeight);

    // Variation sliders row (smaller, below main sliders)
    int varOffset = (sliderWidth - varSliderSize) / 2;
    densityVarLabel_.setBounds(leftX + varOffset, varY, varSliderSize, labelHeight);
    densityVarSlider_.setBounds(leftX + varOffset, varY + labelHeight, varSliderSize,
                                varSliderSize);

    grainLengthVarLabel_.setBounds(centerLeftX + varOffset, varY, varSliderSize, labelHeight);
    grainLengthVarSlider_.setBounds(centerLeftX + varOffset, varY + labelHeight, varSliderSize,
                                    varSliderSize);

    delayTimeVarLabel_.setBounds(centerRightX + varOffset, varY, varSliderSize, labelHeight);
    delayTimeVarSlider_.setBounds(centerRightX + varOffset, varY + labelHeight, varSliderSize,
                                  varSliderSize);

    playbackRateVarLabel_.setBounds(rightX + varOffset, varY, varSliderSize, labelHeight);
    playbackRateVarSlider_.setBounds(rightX + varOffset, varY + labelHeight, varSliderSize,
                                     varSliderSize);

    // Bottom row - 3 controls centred
    int totalBottomWidth = 3 * sliderWidth + 2 * margin;
    int bottomLeftX = (getWidth() - totalBottomWidth) / 2;
    int bottomCenterX = bottomLeftX + sliderWidth + margin;
    int bottomRightX = bottomCenterX + sliderWidth + margin;

    distributionLabel_.setBounds(bottomLeftX, bottomY - labelHeight, sliderWidth, labelHeight);
    distributionSlider_.setBounds(bottomLeftX, bottomY, sliderWidth, sliderHeight);

    delayFeedbackLabel_.setBounds(bottomCenterX, bottomY - labelHeight, sliderWidth, labelHeight);
    delayFeedbackSlider_.setBounds(bottomCenterX, bottomY, sliderWidth, sliderHeight);

    mixLabel_.setBounds(bottomRightX, bottomY - labelHeight, sliderWidth, labelHeight);
    mixSlider_.setBounds(bottomRightX, bottomY, sliderWidth, sliderHeight);

    // Controls row
    granulationModeButton_.setBounds(leftX, bottomY + sliderHeight + 20, totalWidth, 30);

    int envelopeY = bottomY + sliderHeight + 60;
    envelopeMenuLabel_.setBounds(leftX, envelopeY - labelHeight, totalWidth, labelHeight);
    envelopeMenu_.setBounds(leftX, envelopeY, totalWidth, 30);
}
