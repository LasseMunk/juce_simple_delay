#include "SimpleDly/PluginProcessor.h"
#include "SimpleDly/PluginEditor.h"

//==============================================================================
SimpleDelayAudioProcessor::SimpleDelayAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     
                       )
                #endif
                        // initializer list, initializing the statevaluetree
                        // *this dereferences the pointer to the AudioPluginAudioProcessor
                        // nullptr is the undo manager
                       , state (*this, nullptr, "STATE", {
    std::make_unique<juce::AudioParameterFloat>("GAIN",     "Gain",     0.0f, 1.0f, 1.0f),
    std::make_unique<juce::AudioParameterFloat>("FEEDBACK", "Feedback", 0.0f, 1.0f, 0.35f),
    std::make_unique<juce::AudioParameterFloat>("MIX",      "Dry/Wet",  0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("TIME",     "Time",     0.0f, 1.0f, 0.5f)
})
{
    // constructor start
}


SimpleDelayAudioProcessor::~SimpleDelayAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleDelayAudioProcessor::getName() const
{
    return "SimpleDly";
}

bool SimpleDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleDelayAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String SimpleDelayAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void SimpleDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void SimpleDelayAudioProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock */)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    int delayMilliseconds = 200;
    auto delaySamples = (int) std::round (sampleRate * delayMilliseconds / 1000.0); // sizes delayBuffer according to samplerate
    delayBuffer.setSize (2, delaySamples);
    delayBuffer.clear();
    delayBufferPos = 0;
    // juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void SimpleDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool SimpleDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void SimpleDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    float gain      = state.getParameter ("GAIN") ->getValue();
    float feedback  = state.getParameter ("FEEDBACK")->getValue();
    float mix       = state.getParameter ("MIX") ->getValue();

    int delayBufferSize = delayBuffer.getNumSamples();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        int delayPos = delayBufferPos; 
        
        for(int i = 0; i < buffer.getNumSamples(); i++ ) 
        {
            float drySample = channelData[i];

            float delaySample = delayBuffer.getSample (channel, delayPos) * feedback;
            delayBuffer.setSample (channel, delayPos, drySample + delaySample);

            delayPos++;
            if (delayPos == delayBufferSize)
                delayPos = 0;

            channelData[i] = (drySample * (1.0f - mix)) + (delaySample * mix);
            channelData[i] *= gain;
        }
    }

    // ciruclar buffer
    delayBufferPos += buffer.getNumSamples();  
    if (delayBufferPos >= delayBufferSize)
        delayBufferPos -= delayBufferSize;
}

//==============================================================================
bool SimpleDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleDelayAudioProcessor::createEditor()
{
    // Auto create sliders for parameters
    // return new juce::GenericAudioProcessorEditor (*this);
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this metshod to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    // state is the AudioProcessorValueTreeState
    if ( auto xmlState = state.copyState().createXml() )
    // destdata is the block of memory provided by the host
    // it is nice to encode into xml instead of binary so you can debug the state
        copyXmlToBinary (*xmlState, destData);
}

void SimpleDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    if ( auto xmlState = getXmlFromBinary (data, sizeInBytes) )
    // create valuetree from xml and set state to it
        state.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleDelayAudioProcessor();
}

// juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameters()
// {
//     // A vector is an arry that can grow and shrink, pointers of ranged parameters. 
//     std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

//     // Add a parameter to the vector
//     // "GAIN" is the ID of the parameter, "Gain" is the name, min, max, default value
//     // This works because AudioParameterFloat inherits from RangedAudioParameter
//     parameters.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", 0.0f, 1.0f, 0.5f));

//     // return the vector
//     return { parameters.begin(), parameters.end()};
// }
