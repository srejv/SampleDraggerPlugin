#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

#include <mutex>

#include "Helpers/AudioFileLoader.h"

#include "Components/MyLookAndFeel.h"
#include "Components/ScaleComponent.h"
#include "Components/SampleComponent.h"

#include "Commands/Command.h"

class SampleDraggerPluginAudioProcessorEditor
	: public AudioProcessorEditor
	, public Button::Listener
	, public Slider::Listener
	, public Timer
	, public SampleComponent::Listener
	, public KeyListener
{
public:
	SampleDraggerPluginAudioProcessorEditor (SampleDraggerPluginAudioProcessor&);
	~SampleDraggerPluginAudioProcessorEditor();

	//==============================================================================
	void paint (Graphics&) override;
	void resized() override;

	// Is this really needed?
	void timerCallback() override { repaint(); }
	
	bool keyPressed(const KeyPress& key, Component* originatingComponent) override;
	
	void undo();
	void redo();
	
	void sampleMoved(SampleComponent* caller, Command* cmd) override;
	void sampleRemoved(SampleComponent* sample) override;

	void pushCmd(Command* cmd);

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

	// UI
	OwnedArray<SampleComponent> sampleComponents;
	ScopedPointer<TextButton> addSample, generateWaveform, saveGenerated, playButton;

	ScopedPointer<ComboBox> comboSampleList;
	ScopedPointer<TextButton> btnAddSprite;

	ScopedPointer<AudioThumbnail> specialBufferThumbnail;

	ScopedPointer<Slider> pixelsToSeconds, viewPosition;
	ScopedPointer<ScaleComponent> scaleComponent;

	MyLookAndFeel lookAndFeel;

	// File loading
	AudioFileLoader loader;

	// Extra buffer
	ScopedPointer<AudioSampleBuffer> xtrabuffer;

	int cmdIndex = -1;
	OwnedArray<Command> cmds;

	KeyPress undoKey{ 'z', ModifierKeys::ctrlModifier, juce_wchar('z') };
	KeyPress redoKey{ 'y', ModifierKeys::ctrlModifier, juce_wchar('y') };


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleDraggerPluginAudioProcessorEditor)
};
