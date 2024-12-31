/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQAudioProcessor::SimpleEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

SimpleEQAudioProcessor::~SimpleEQAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleEQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

	// a spec is a struct that holds info about the audio processing context
	// you pass this spec to other objects that need to know about the audio processing context
    juce::dsp::ProcessSpec spec;

	spec.maximumBlockSize = samplesPerBlock;

	spec.numChannels = 1;

	spec.sampleRate = sampleRate;

	// Prepare the chains with the spec
    leftChain.prepare(spec);
    rightChain.prepare(spec);
}

void SimpleEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
#endif

void SimpleEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
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
    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    //{
    //    auto* channelData = buffer.getWritePointer (channel);

    //    // ..do something to the data...
    //}

    juce::dsp::AudioBlock<float> block(buffer);

	// We need to create a block for each channel so we can process them
    auto leftBlock = block.getSingleChannelBlock(0);
	auto rightBlock = block.getSingleChannelBlock(1);

	// Context wrappers for the blocks so the chain can process them
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

	leftChain.process(leftContext);
	rightChain.process(rightContext);

}

//==============================================================================
bool SimpleEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleEQAudioProcessor::createEditor()
{
	return new juce::GenericAudioProcessorEditor(*this);
    //return new SimpleEQAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SimpleEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleEQAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	// Add a parameter for the low cut frequency
	// 1: ID of the parameter
	// 2: Name of the parameter
	// 3: Range of the parameter (20 Hz to 20 kHz, 1 Hz steps, no skew)
	// 4: Default value of the parameter (20 Hz since this is lowcut so off by default) 
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "LowCut Freq",  1 },
        "LowCut Freq",
		juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), 
        20.f));

	// Add a parameter for the high cut frequency
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "HighCut Freq", 1},
        "HighCut Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
        20000.f));

	// Add a parameter for the peak frequency
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "Peak Freq", 1},
        "Peak Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
        750.f));

	// Add a parameter for the peak gain
	// The range is -24 dB to 24 dB, with 0.5 dB steps, and a skew of 1
	// Default value is +/- 0 dB
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "Peak Gain", 1},
        "Peak Gain",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
        0.0f));

	// Add a parameter for the peak quality (Q)
	// The range is 0.1 to 10, with 0.05 steps, and a skew of 1 (values are kinda arbitrary)
	// This is a Q factor for the peak filter (how wide the filter is)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "Peak Quality", 1},
        "Peak Quality",
        juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
        1.f));

	// Add the parameters for low cut and high cut slopes
	// The choices are 12, 24, 36, and 48 dB/octave
	// A bit of a complicated way to do it but it's c++ practice
	juce::StringArray slopeChoices;
    for (int i = 0; i < 4; i++)
    {
		juce::String str;
		str << (12 + i * 12);
		str << " db/Oct";
		slopeChoices.add(str);
    }

	// This is a ParameterChoice, which is a parameter that teakes in a set of choices
	// Default value is 0 (12 dB/octave)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "LowCut Slope", 1},
        "LowCut Slope",
        slopeChoices,
        0));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "HighCut Slope", 1},
        "HighCut Slope",
        slopeChoices,
        0));


    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleEQAudioProcessor();
}
