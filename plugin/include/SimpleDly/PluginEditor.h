#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (SimpleDelayAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleDelayAudioProcessor& processorRef;

    juce::Slider gainSlider, timeSlider, feedbackSlider, mixSlider;
    juce::Label gainLabel, feedbackLabel, mixLabel;
    // the attachment must be placed after the slider is created 
    juce::AudioProcessorValueTreeState::SliderAttachment    gainSliderAttachment, 
                                                            timeSliderAttachment,
                                                            feedbackSliderAttachment, 
                                                            mixSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
