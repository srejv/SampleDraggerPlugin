/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <mutex>

#include "Helpers/SamplePool.h"

//==============================================================================
/**
*/
class SampleDraggerPluginAudioProcessor  : public AudioProcessor
{
public:
	//==============================================================================
	SampleDraggerPluginAudioProcessor();
	~SampleDraggerPluginAudioProcessor();

	//==============================================================================
	void prepareToPlay (double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

	#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
	#endif

	void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

	//==============================================================================
	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect () const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram (int index) override;
	const String getProgramName (int index) override;
	void changeProgramName (int index, const String& newName) override;

	//==============================================================================
	void getStateInformation (MemoryBlock& destData) override;
	void setStateInformation (const void* data, int sizeInBytes) override;

	void swapBuffer(AudioSampleBuffer* buffer) {
        position = 0;
		std::lock_guard<std::mutex> lock(g_i_mutex);
		mySpecialBuffer = buffer;
	}

	void setPlaying(bool state) {
		isPlaying = state;
	}

	int getNumSamples() {
		if (mySpecialBuffer == nullptr) return 0;
		return mySpecialBuffer->getNumSamples();
	}

	AudioSampleBuffer& getBuffer() {
		return *mySpecialBuffer;
	}

public:

	SamplePool& getSamplePool() {
		return samplePool;
	}

private:
    //==============================================================================
   	ScopedPointer<AudioSampleBuffer> mySpecialBuffer;
	std::mutex g_i_mutex;
	bool isPlaying = false;
	int position = 0;

	SamplePool samplePool;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleDraggerPluginAudioProcessor)
};
