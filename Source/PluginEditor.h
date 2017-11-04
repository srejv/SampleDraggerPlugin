#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

#include <mutex>

#include "Helpers/AudioFileLoader.h"

#include "Components/ScaleComponent.h"
#include "Components/SampleComponent.h"

class SampleDraggerPluginAudioProcessorEditor
	: public AudioProcessorEditor, public Button::Listener, public Slider::Listener, public Timer
{
public:
	SampleDraggerPluginAudioProcessorEditor (SampleDraggerPluginAudioProcessor&);
	~SampleDraggerPluginAudioProcessorEditor();

	//==============================================================================
	void paint (Graphics&) override;
	void resized() override;

	// Is this really needed?
	void timerCallback() override { repaint(); }

private:
	void drawWaveform(Graphics& g, const Rectangle<int>& thumbnailBounds);

	void sliderValueChanged(Slider* slider) override;
	void buttonClicked(Button* btn) override;

	void generateFinalBuffer();

	void addSpriteButtonClicked();
	void addFromIndex(int sampleIndex);

	void openButtonClickedSoundPool();
	void saveButtonClicked();

	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	SampleDraggerPluginAudioProcessor& processor;
	ScopedPointer<TextButton> addSample, generateWaveform, saveGenerated, playButton;
	//OwnedArray<Sample> samples;
	OwnedArray<SampleComponent> sampleComponents;

	ComboBox comboSampleList;
	ScopedPointer<TextButton> btnAddSprite;

	ScopedPointer<AudioThumbnail> specialBufferThumbnail;
	AudioFileLoader loader;

	Slider pixelsToSeconds;
	ScaleComponent scaleComponent;

	ScopedPointer<AudioSampleBuffer> xtrabuffer;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleDraggerPluginAudioProcessorEditor)
};
