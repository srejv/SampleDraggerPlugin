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

	KeyPress undoKey{ 'z', ModifierKeys::ctrlModifier, juce_wchar('z') };
	KeyPress redoKey{ 'y', ModifierKeys::ctrlModifier, juce_wchar('y') };

	bool keyPressed(const KeyPress& key, Component* originatingComponent) override {
		ignoreUnused(originatingComponent);
		if (key == undoKey) {
			undo();
			return true;
		} 
		if(key == redoKey) {
			redo();
			return true;
		}
		return false;
	}
	
	void undo() {
		if (cmdIndex < 0) return;
		Command* c = cmds[cmdIndex--];
		if (c != nullptr)
			c->undo();
	}
	void redo() {
		if (cmdIndex >= cmds.size()) return;
		Command* c = cmds[cmdIndex++];
		if(c != nullptr)
			c->execute();
	}

	void sampleMoved(SampleComponent* caller, const Point<float>& from, const Point<float>& to) override
	{
		pushCmd(new MoveCommand(caller, from, to));
	}

	void pushCmd(Command* cmd) {
		cmds.set(++cmdIndex, cmd);
	}
	

	OwnedArray<Command>& getCommandList() {
		return cmds;
	}

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

	MyLookAndFeel lookAndFeel;

	int cmdIndex = -1;
	OwnedArray<Command> cmds;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleDraggerPluginAudioProcessorEditor)
};
