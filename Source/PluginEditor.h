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
	void timerCallback() override { 
		if (autoGen->getToggleState()) { generateFinalBuffer(); }
		repaint(); 
	}
	
	bool keyPressed(const KeyPress& key, Component* originatingComponent) override;
	
	void undo();
	void redo();
	
	void sampleMoved(SampleComponent* caller, Command* cmd) override;
	void sampleRemoved(SampleComponent* sample) override;
	void sampleStartPointChanged(SampleComponent* caller, Command* cmd) override {
		ignoreUnused(caller);
		pushCmd(cmd);
	}
	void sampleEndPointChanged(SampleComponent* caller, Command* cmd) override {
		ignoreUnused(caller);
		pushCmd(cmd);
	}

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
	ScopedPointer<TextButton> addSample, generateWaveform, saveGenerated, playButton, autoGen;

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

	KeyPress undoKey{ 'z', ModifierKeys::commandModifier, juce_wchar('z') };
	KeyPress redoKey{ 'y', ModifierKeys::commandModifier, juce_wchar('y') };
    KeyPress genKey{ 'r', ModifierKeys::commandModifier, juce_wchar('r') };
    KeyPress loadSampleKey{'o', ModifierKeys::commandModifier, juce_wchar('o')};
    KeyPress addSpriteKey{'a', ModifierKeys::commandModifier, juce_wchar('a')};
    KeyPress saveKey{'s', ModifierKeys::commandModifier, juce_wchar('s')};
    KeyPress playKey{KeyPress::spaceKey};
    KeyPress autoGenKey{'g', ModifierKeys::commandModifier, juce_wchar('g')};
    
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleDraggerPluginAudioProcessorEditor)
};
