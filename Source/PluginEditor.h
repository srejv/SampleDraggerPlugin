/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

#include <mutex>

#include "DataModel.h"
#include "Sample.h"
#include "AudioFileLoader.h"
#include "ScaleComponent.h"

//==============================================================================
/**
*/
class SampleDraggerPluginAudioProcessorEditor  
	: public AudioProcessorEditor, public Button::Listener, public Slider::Listener, public Timer
{
public:
    SampleDraggerPluginAudioProcessorEditor (SampleDraggerPluginAudioProcessor&);
    ~SampleDraggerPluginAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

	void timerCallback() override
	{
		repaint();
	}

private:
	void drawWaveform(Graphics& g, const Rectangle<int>& thumbnailBounds);

	void sliderValueChanged(Slider* slider) override;
	void buttonClicked(Button* btn) override;

	void generateFinalBuffer();

	void openButtonClicked();
	void saveButtonClicked();

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SampleDraggerPluginAudioProcessor& processor;
	ScopedPointer<TextButton> addSample, generateWaveform, saveGenerated, playButton;
	OwnedArray<Sample> samples;
	ScopedPointer<AudioThumbnail> specialBufferThumbnail;
	AudioFileLoader loader;

	Slider pixelsToSeconds;
	ScaleComponent scaleComponent;

	ScopedPointer<AudioSampleBuffer> xtrabuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleDraggerPluginAudioProcessorEditor)
};
