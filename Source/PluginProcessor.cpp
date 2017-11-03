/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
SampleDraggerPluginAudioProcessor::SampleDraggerPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

SampleDraggerPluginAudioProcessor::~SampleDraggerPluginAudioProcessor()
{
}

//==============================================================================
const String SampleDraggerPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SampleDraggerPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SampleDraggerPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SampleDraggerPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SampleDraggerPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SampleDraggerPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SampleDraggerPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SampleDraggerPluginAudioProcessor::setCurrentProgram (int index)
{
	ignoreUnused(index);
}

const String SampleDraggerPluginAudioProcessor::getProgramName (int index)
{
	ignoreUnused(index);
    return {};
}

void SampleDraggerPluginAudioProcessor::changeProgramName (int index, const String& newName)
{
	ignoreUnused(index, newName);
}

//==============================================================================
void SampleDraggerPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	ignoreUnused(sampleRate, samplesPerBlock);
}

void SampleDraggerPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SampleDraggerPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void SampleDraggerPluginAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

	MidiBuffer processedMidi;
	int time;
	MidiMessage m;
	for (MidiBuffer::Iterator i(midiMessages); i.getNextEvent(m, time);)
	{
		if (m.isNoteOn())
		{
			isPlaying = m.getVelocity() > 0;
		}
		else if (m.isNoteOff())
		{
			isPlaying = false;
		}

		processedMidi.addEvent(m, time);
	}

	midiMessages.swapWith(processedMidi);

	if (!isPlaying) return;

	std::lock_guard<std::mutex> lock(g_i_mutex);
    if (mySpecialBuffer != nullptr) {
		const int numInputChannels = mySpecialBuffer->getNumChannels();
		const int numOutputChannels = buffer.getNumChannels();

		int outputSamplesRemaining = buffer.getNumSamples();
		while (outputSamplesRemaining > 0)
		{
			int bufferSamplesRemaining = mySpecialBuffer->getNumSamples() - position;                 // [10]
			int samplesThisTime = jmin(outputSamplesRemaining, bufferSamplesRemaining);        // [11]

			if (samplesThisTime < 0) continue;

			for (int channel = 0; channel < numOutputChannels; ++channel)
			{
				buffer.copyFrom(channel, 0,
					*mySpecialBuffer, channel % numInputChannels,
					position, samplesThisTime);
			}

			outputSamplesRemaining -= samplesThisTime;
			position += samplesThisTime;

			if (position == mySpecialBuffer->getNumSamples())
				position = 0;
		}
	}
}

//==============================================================================
bool SampleDraggerPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SampleDraggerPluginAudioProcessor::createEditor()
{
    return new SampleDraggerPluginAudioProcessorEditor (*this);
}

//==============================================================================
void SampleDraggerPluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	ignoreUnused(destData);
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SampleDraggerPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	ignoreUnused(data, sizeInBytes);
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SampleDraggerPluginAudioProcessor();
}
