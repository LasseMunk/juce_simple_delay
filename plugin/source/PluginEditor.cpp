#include "SimpleDly/PluginProcessor.h"
#include "SimpleDly/PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (SimpleDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
    gainSliderAttachment (p.state, "GAIN", gainSlider), // attach the slider to the state
    feedbackSliderAttachment (p.state, "FEEDBACK", feedbackSlider),
    mixSliderAttachment (p.state, "MIX", mixSlider)
{
    for (auto* slider: { &gainSlider, &feedbackSlider, &mixSlider })
    {
        slider->setSliderStyle (juce::Slider::SliderStyle::Rotary);
        slider->setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::whitesmoke);
        slider->setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colours::whitesmoke);
        slider->setColour (juce::Slider::thumbColourId, juce::Colours::magenta);

        slider->setTextBoxStyle (juce::Slider::TextBoxBelow, true, 50, 30);
        slider->setColour (juce::Slider::textBoxTextColourId, juce::Colours::magenta);
        slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::magenta);
        
        addAndMakeVisible (slider);
    }

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll (juce::Colours::lightgrey);

    // g.setColour (juce::Colours::white);
    // g.setFont (15.0f);
    // g.drawFittedText ("SIMPLE DELAY", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    juce::Rectangle<int> bounds = getLocalBounds();
    int margin = 20;

    juce::Rectangle<int> gainBounds = bounds.removeFromRight ( getWidth() / 3 );
    gainSlider.setBounds (gainBounds.reduced (margin));

    juce::Rectangle<int> knobsBounds = bounds.removeFromRight ( getWidth() / 2 );
    juce::Rectangle<int> feedbackBounds = knobsBounds.removeFromLeft (knobsBounds.getWidth() / 2);
    feedbackSlider.setBounds (feedbackBounds.reduced (margin));
    mixSlider.setBounds (knobsBounds.reduced (margin));
}
