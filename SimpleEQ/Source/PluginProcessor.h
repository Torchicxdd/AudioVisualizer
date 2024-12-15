/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class SimpleEQAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SimpleEQAudioProcessor();
    ~SimpleEQAudioProcessor() override;

    //==============================================================================
    // prepare to play is what gets called by the host when its about to start playback
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

	// processBlock is what happens when you press "play" in the transport control
    // NEVER INTERUPT THE PROCESS BLOCK AT ALL COSTS
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	//==============================================================================
	static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

	juce::AudioProcessorValueTreeState apvts{*this, nullptr, "Parameters", createParameterLayout()};
private:
	// Aliases to make the code more readable
    using Filter = juce::dsp::IIR::Filter<float>;
	// the CutFilter is a chain of 4 Filter objects
	using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
	// the MonoChain is a chain of 3 (LowCut, Peak, HighCut)
	using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

	// By default basically everything in juce is mono so we need to make it stereo
	MonoChain leftChain, rightChain;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessor)
};

