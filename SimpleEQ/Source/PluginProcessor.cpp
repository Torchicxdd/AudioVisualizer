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
    
    // Make coefficients
    auto chainSettings = getChainSettings(apvts);
    
    // The gainFactor needs to be in gain units which is somehow different from db??
    // dBFS is like what you see in ableton (0 being max)
    // gain level is a measurement of the strength of an audio signal in true decibels 1.0 is neutral, over 1 is an increase in voltage and bellow 1 is a decrease in voltage
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
                                                                                chainSettings.peakFreq,
                                                                                chainSettings.peakQuality,
                                                                                juce::Decibels::decibelsToGain(chainSettings.peakGainInDecibels));
    // give ChainPositions::Peak as index for the .get
    // do not ask idk how this works or why we need to dereference on both sides
    // I think its cus we need to copy over the values and not the references
    *leftChain.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
    *rightChain.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
    
    // basically this returns an array of IIR::Coefficients that we will use for the lowcut filter
    // 1. frequency of the cut filter
    // 2. sampleRate (maybe this could be made a singleton idk
    // 3. filter order which is basically the intensity of the cut (aka the 12db/oct, 24db/oct, etc)
    // we convert our selected lowcutslope 0,1,2,3 to our even order 2,4,6,8
    // there is some nerd shit behind even vs odd orders in IIR but we go with even cus they are overall a safer choice
    auto cutCoefficient = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq,
                                                                                                      sampleRate,
                                                                                                      2 * (chainSettings.lowCutSlope + 1));
    // get the address of the leftChain lowCut and rightChain lowCut
    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();
    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();
    
    // first, bypass all the links in the chain (the CutFilter is a chain of 4 Filter objects)
    leftLowCut.setBypassed<0>(true);
    leftLowCut.setBypassed<1>(true);
    leftLowCut.setBypassed<2>(true);
    leftLowCut.setBypassed<3>(true);
    
    rightLowCut.setBypassed<0>(true);
    rightLowCut.setBypassed<1>(true);
    rightLowCut.setBypassed<2>(true);
    rightLowCut.setBypassed<3>(true);
    
    // then, setup our switch statement so that changing the slope will have an effect on audio
    // remember the way filters work is that based on the order, we add more coefficients to steepen the cutoff
    switch( chainSettings.lowCutSlope )
    {
        case Slope_12:
            *leftLowCut.get<0>().coefficients = *cutCoefficient[0];
            leftLowCut.setBypassed<0>(false);
            
            *rightLowCut.get<0>().coefficients = *cutCoefficient[0];
            rightLowCut.setBypassed<0>(false);
            break;
        case Slope_24:
            *leftLowCut.get<0>().coefficients = *cutCoefficient[0];
            leftLowCut.setBypassed<0>(false);
            *leftLowCut.get<1>().coefficients = *cutCoefficient[1];
            leftLowCut.setBypassed<1>(false);
            
            *rightLowCut.get<0>().coefficients = *cutCoefficient[0];
            rightLowCut.setBypassed<0>(false);
            *rightLowCut.get<1>().coefficients = *cutCoefficient[1];
            rightLowCut.setBypassed<1>(false);
            break;
        case Slope_36:
            *leftLowCut.get<0>().coefficients = *cutCoefficient[0];
            leftLowCut.setBypassed<0>(false);
            *leftLowCut.get<1>().coefficients = *cutCoefficient[1];
            leftLowCut.setBypassed<1>(false);
            *leftLowCut.get<2>().coefficients = *cutCoefficient[2];
            leftLowCut.setBypassed<2>(false);
            
            *rightLowCut.get<0>().coefficients = *cutCoefficient[0];
            rightLowCut.setBypassed<0>(false);
            *rightLowCut.get<1>().coefficients = *cutCoefficient[1];
            rightLowCut.setBypassed<1>(false);
            *rightLowCut.get<2>().coefficients = *cutCoefficient[2];
            rightLowCut.setBypassed<2>(false);
            
            break;
        case Slope_48:
            *leftLowCut.get<0>().coefficients = *cutCoefficient[0];
            leftLowCut.setBypassed<0>(false);
            *leftLowCut.get<1>().coefficients = *cutCoefficient[1];
            leftLowCut.setBypassed<1>(false);
            *leftLowCut.get<2>().coefficients = *cutCoefficient[2];
            leftLowCut.setBypassed<2>(false);
            *leftLowCut.get<3>().coefficients = *cutCoefficient[3];
            leftLowCut.setBypassed<3>(false);
            
            *rightLowCut.get<0>().coefficients = *cutCoefficient[0];
            rightLowCut.setBypassed<0>(false);
            *rightLowCut.get<1>().coefficients = *cutCoefficient[1];
            rightLowCut.setBypassed<1>(false);
            *rightLowCut.get<2>().coefficients = *cutCoefficient[2];
            rightLowCut.setBypassed<2>(false);
            *rightLowCut.get<3>().coefficients = *cutCoefficient[3];
            rightLowCut.setBypassed<3>(false);
            break;
    }
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
    
    // Always update parameters BEFORE you process audio
    auto chainSettings = getChainSettings(apvts);
    
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
                                                                                chainSettings.peakFreq,
                                                                                chainSettings.peakQuality,
                                                                                juce::Decibels::decibelsToGain(chainSettings.peakGainInDecibels));

    *leftChain.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
    *rightChain.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
    
    // look at prepare to play for more comments
    auto cutCoefficient = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq,
                                                                                                      getSampleRate(),
                                                                                                      2 * (chainSettings.lowCutSlope + 1));
    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();
    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();
    
    leftLowCut.setBypassed<0>(true);
    leftLowCut.setBypassed<1>(true);
    leftLowCut.setBypassed<2>(true);
    leftLowCut.setBypassed<3>(true);
    
    rightLowCut.setBypassed<0>(true);
    rightLowCut.setBypassed<1>(true);
    rightLowCut.setBypassed<2>(true);
    rightLowCut.setBypassed<3>(true);
    
    switch( chainSettings.lowCutSlope )
    {
        case Slope_12:
            *leftLowCut.get<0>().coefficients = *cutCoefficient[0];
            leftLowCut.setBypassed<0>(false);
            
            *rightLowCut.get<0>().coefficients = *cutCoefficient[0];
            rightLowCut.setBypassed<0>(false);
            break;
        case Slope_24:
            *leftLowCut.get<0>().coefficients = *cutCoefficient[0];
            leftLowCut.setBypassed<0>(false);
            *leftLowCut.get<1>().coefficients = *cutCoefficient[1];
            leftLowCut.setBypassed<1>(false);
            
            *rightLowCut.get<0>().coefficients = *cutCoefficient[0];
            rightLowCut.setBypassed<0>(false);
            *rightLowCut.get<1>().coefficients = *cutCoefficient[1];
            rightLowCut.setBypassed<1>(false);
            break;
        case Slope_36:
            *leftLowCut.get<0>().coefficients = *cutCoefficient[0];
            leftLowCut.setBypassed<0>(false);
            *leftLowCut.get<1>().coefficients = *cutCoefficient[1];
            leftLowCut.setBypassed<1>(false);
            *leftLowCut.get<2>().coefficients = *cutCoefficient[2];
            leftLowCut.setBypassed<2>(false);
            
            *rightLowCut.get<0>().coefficients = *cutCoefficient[0];
            rightLowCut.setBypassed<0>(false);
            *rightLowCut.get<1>().coefficients = *cutCoefficient[1];
            rightLowCut.setBypassed<1>(false);
            *rightLowCut.get<2>().coefficients = *cutCoefficient[2];
            rightLowCut.setBypassed<2>(false);
            
            break;
        case Slope_48:
            *leftLowCut.get<0>().coefficients = *cutCoefficient[0];
            leftLowCut.setBypassed<0>(false);
            *leftLowCut.get<1>().coefficients = *cutCoefficient[1];
            leftLowCut.setBypassed<1>(false);
            *leftLowCut.get<2>().coefficients = *cutCoefficient[2];
            leftLowCut.setBypassed<2>(false);
            *leftLowCut.get<3>().coefficients = *cutCoefficient[3];
            leftLowCut.setBypassed<3>(false);
            
            *rightLowCut.get<0>().coefficients = *cutCoefficient[0];
            rightLowCut.setBypassed<0>(false);
            *rightLowCut.get<1>().coefficients = *cutCoefficient[1];
            rightLowCut.setBypassed<1>(false);
            *rightLowCut.get<2>().coefficients = *cutCoefficient[2];
            rightLowCut.setBypassed<2>(false);
            *rightLowCut.get<3>().coefficients = *cutCoefficient[3];
            rightLowCut.setBypassed<3>(false);
            break;
    }

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

// Helper function to get all our parameter values in the chain
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;
    
    // the arrow -> operater is used as a shortcut for accessing a member of a pointer
    // so  -> is equivalent to dereferencing with * and calling .load()
    settings.lowCutFreq = apvts.getRawParameterValue("LowCut Freq")->load();
    settings.highCutFreq = apvts.getRawParameterValue("HighCut Freq")->load();
    settings.peakFreq = apvts.getRawParameterValue("Peak Freq")->load();
    settings.peakGainInDecibels = apvts.getRawParameterValue("Peak Gain")->load();
    settings.peakQuality = apvts.getRawParameterValue("Peak Quality")->load();
    // expects an int so we have to cast our Slope enum
    settings.lowCutSlope = static_cast<Slope>(apvts.getRawParameterValue("LowCut Slope")->load());
    settings.highCutFreq = static_cast<Slope>(apvts.getRawParameterValue("HighCut Slope")->load());
    
    return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleEQAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	// Add a parameter for the low cut frequency
	// 1: ID of the parameter
	// 2: Name of the parameter
	// 3: Range of the parameter (20 Hz to 20 kHz, 1 Hz steps, skewed to bottom)
	// 4: Default value of the parameter (20 Hz since this is lowcut so off by default)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "LowCut Freq",  1 },
        "LowCut Freq",
		juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.3),
        20.f));

	// Add a parameter for the high cut frequency
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "HighCut Freq", 1},
        "HighCut Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.3),
        20000.f));

	// Add a parameter for the peak frequency
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "Peak Freq", 1},
        "Peak Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.3),
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
