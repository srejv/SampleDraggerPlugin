/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
SampleDraggerPluginAudioProcessorEditor::SampleDraggerPluginAudioProcessorEditor (SampleDraggerPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
	setLookAndFeel(&lookAndFeel);

	// Buttons
	addAndMakeVisible(addSample = new TextButton("Load file -> Sound Pool"));
	addSample->addListener(this);
	addSample->addShortcut(loadSampleKey);
	addSample->setConnectedEdges(Button::ConnectedOnRight);

	addAndMakeVisible(saveGenerated = new TextButton("Save Generated Sample"));
	saveGenerated->addListener(this);
	saveGenerated->addShortcut(saveKey);
	
	addAndMakeVisible(generateWaveform = new TextButton("Generate"));
	generateWaveform->addShortcut(genKey);
	generateWaveform->addListener(this);
	generateWaveform->setConnectedEdges(Button::ConnectedOnRight);

	addAndMakeVisible(playButton = new TextButton("Play/Pause"));
	playButton->setClickingTogglesState(true);
	playButton->addShortcut(playKey);
	playButton->addListener(this);
	playButton->setConnectedEdges(Button::ConnectedOnLeft);

	addAndMakeVisible(btnAddSprite = new TextButton("Add Sample"));
	btnAddSprite->addListener(this);
	btnAddSprite->addShortcut(addSpriteKey);
	btnAddSprite->setConnectedEdges(Button::ConnectedOnRight | Button::ConnectedOnLeft);

	addAndMakeVisible(btnClear = new TextButton("Clear"));
	btnClear->addListener(this);
	btnClear->setConnectedEdges(Button::ConnectedOnLeft);

	addAndMakeVisible(autoGen = new TextButton("AutoGen"));
	autoGen->setClickingTogglesState(true);
	autoGen->addShortcut(autoGenKey);
	autoGen->setConnectedEdges(Button::ConnectedOnRight | Button::ConnectedOnLeft);

	// Combobboxes
	addAndMakeVisible(comboSampleList = new ComboBox());
	comboSampleList->setTextWhenNothingSelected("Select a to load");
	comboSampleList->setTextWhenNoChoicesAvailable("Pool is empty");

	// Scale component
	addAndMakeVisible(scaleComponent = new ScaleComponent());
	scaleComponent->addListener(this);

	addKeyListener(this);

	startTimerHz(20);
	setSize(800, 600);
}

SampleDraggerPluginAudioProcessorEditor::~SampleDraggerPluginAudioProcessorEditor()
{
	cmds.clear(true);

	removeAllChildren();

	addSample = nullptr;
	saveGenerated = nullptr;
	//pixelsToSeconds = nullptr;
	scaleComponent = nullptr;
	generateWaveform = nullptr;
	playButton = nullptr;
	comboSampleList = nullptr;
	btnAddSprite = nullptr;

	stopTimer();
	specialBufferThumbnail = nullptr;
	sampleComponents.clear(true);

	setLookAndFeel(nullptr);
}

//==============================================================================
void SampleDraggerPluginAudioProcessorEditor::paint (Graphics& g) {
	g.fillAll(findColour(ResizableWindow::backgroundColourId));
	g.setColour(Colour(120, 120, 120));
	if (specialBufferThumbnail != nullptr) {
		double width = (processor.getNumSamples() / processor.getSampleRate()) * scaleComponent->getPixelToSeconds();
		auto area(getLocalBounds().removeFromTop(120).withTrimmedTop(40).withWidth(roundToInt(width)).withX(-scaleComponent->getViewPosition() * scaleComponent->getPixelToSeconds()));
		drawWaveform(g, area);
	}
	else {
		auto area(getLocalBounds().removeFromTop(120).withTrimmedTop(40));
		g.setColour(Colour(36, 36, 36));
		g.fillRect(area);
		g.setColour(Colour(240, 240, 240));
		g.drawRect(area, 1);
		g.drawText("No buffer yet (Add samples to the workspace. Generate or ctrl+g/cmd+g)", area, Justification::centred);
	}
}

void SampleDraggerPluginAudioProcessorEditor::resized() {
	auto area(getLocalBounds());
	
	auto topButtonRow = area.removeFromTop(20);

	saveGenerated->setBounds(topButtonRow.removeFromRight(80));

	addSample->setBounds(topButtonRow.removeFromLeft(60));
	comboSampleList->setBounds(topButtonRow.removeFromLeft(160));
	btnAddSprite->setBounds(topButtonRow.removeFromLeft(60));
	btnClear->setBounds(topButtonRow.removeFromLeft(60));
	
	topButtonRow.removeFromLeft(20);
	generateWaveform->setBounds(topButtonRow.removeFromLeft(60));
	autoGen->setBounds(topButtonRow.removeFromLeft(60));
	playButton->setBounds(topButtonRow.removeFromLeft(60));

	scaleComponent->setBounds(area.removeFromTop(20));
}

void SampleDraggerPluginAudioProcessorEditor::drawWaveform(Graphics& g, const Rectangle<int>& thumbnailBounds)
{
	const auto colourScheme(static_cast<LookAndFeel_V4&>(getLookAndFeel()).getCurrentColourScheme());
	g.setColour(colourScheme.getUIColour(LookAndFeel_V4::ColourScheme::widgetBackground));
	g.fillRect(thumbnailBounds);
	g.setColour(colourScheme.getUIColour(LookAndFeel_V4::ColourScheme::outline));
	g.drawRect(thumbnailBounds, 1);

	double startTime = 0.0f;
	double endTime = processor.getNumSamples() / processor.getSampleRate();
	float verticalZoom = 1.0f;
    
    g.saveState();
	g.setColour(colourScheme.getUIColour(LookAndFeel_V4::ColourScheme::defaultFill));
	specialBufferThumbnail->drawChannels(g, thumbnailBounds, startTime, endTime, verticalZoom);
    
    auto pposition = processor.getPosition() / processor.getSampleRate();
    g.drawVerticalLine(roundToInt(pposition * scaleComponent->getPixelToSeconds()), thumbnailBounds.getY(), thumbnailBounds.getBottom());
    
    g.restoreState();
}

void SampleDraggerPluginAudioProcessorEditor::buttonClicked(Button* btn) {
	if (btn == addSample) {
		openButtonClickedSoundPool();
	}
	else if (btn == generateWaveform) {
		generateFinalBuffer();
	}
	else if (btn == saveGenerated) {
		saveButtonClicked();
	}
	else if (btn == playButton) {
		processor.setPlaying(playButton->getToggleState());
	}
	else if (btn == btnAddSprite) {
		addSpriteButtonClicked();
	}
	else if (btn == btnClear) {
		for (auto s : sampleComponents) {
			removeChildComponent(s);
		}
		sampleComponents.clear();
	}
}

void SampleDraggerPluginAudioProcessorEditor::generateFinalBuffer() {
	int min = INT32_MAX;
	int max = 0;
	for (auto s : sampleComponents) {
		auto start = s->getSampleStartPosition();
		auto len = s->getSampleLength();
		if (start < min) { min = start; }
		if ((start + len) > max) { max = (start + len); }
	}
	int length = max - min;

    if(length < 0) return;
    
    ScopedPointer<AudioSampleBuffer> workbuffer = new AudioSampleBuffer(2, length);
    xtrabuffer = new AudioSampleBuffer(2, length);
    
    if (specialBufferThumbnail == nullptr) specialBufferThumbnail = loader.createThumbnail();
    specialBufferThumbnail->reset(2, processor.getSampleRate());

    workbuffer->clear();
    xtrabuffer->clear();

    ScopedPointer< std::vector<Ramp>> ramps;
    
    for (auto s : sampleComponents) {
		int index = s->getIndex();
		auto& source(processor.getSamplePool().getSample(index));
		if (source.name == String::empty) continue;
        
		auto startPos = static_cast<int>(s->getSampleStartPosition() - min);
        
        auto internalStart = s->getInternalSampleStart();
        auto internalLength = s->getSampleLength();

        ramps = s->getRamps();
        
        int rampAccumulator = 0;
        
        for(auto r : *ramps) {
            std::cout << r.numSamples << " " << r.startGain << " " << r.endGain << std::endl;
            
            for (int i = 0; i < s->getNumChannels(); ++i) {
                int destChannel = i;
                int destStartSample = startPos + rampAccumulator;
                const float* sauce = source.buffer.getReadPointer(i) + (rampAccumulator);
                int numSamples = r.numSamples;
                float startGain = r.startGain;
                float endGain = r.endGain;
                
                workbuffer->addFromWithRamp(destChannel, destStartSample, sauce, numSamples, startGain, endGain);
                xtrabuffer->addFromWithRamp(destChannel, destStartSample, sauce, numSamples, startGain, endGain);
            }
            rampAccumulator += r.numSamples;
        }
    }

	specialBufferThumbnail->addBlock(0, *xtrabuffer, 0, workbuffer->getNumSamples());
	processor.swapBuffer(workbuffer.release());
}

void SampleDraggerPluginAudioProcessorEditor::saveButtonClicked()
{
	FileChooser myChooser("Saving as wav...",
		File::getSpecialLocation(File::userHomeDirectory),
		"*.wav");
	if (myChooser.browseForFileToSave(true))
	{
		StringArray metadata;
		WavAudioFormat wav;
		File wavFile(myChooser.getResult());
		FileOutputStream *outputStream = wavFile.createOutputStream();

		ScopedPointer<AudioFormatWriter> writer = wav.createWriterFor(outputStream, processor.getSampleRate(), 2, 16, StringPairArray(), 0);
		if (writer != nullptr)
		{
			if (writer->writeFromAudioSampleBuffer(processor.getBuffer(), 0, processor.getNumSamples()))
			{
				std::cout << "Saving .wav succeeded" << std::endl;
			}
			else
			{
				std::cout << "Saving .wav failed" << std::endl;
			}
		}
	}
}

void SampleDraggerPluginAudioProcessorEditor::openButtonClickedSoundPool() {
	int sampleIndex = loader.loadAudioFileIntoPool(processor.getSamplePool());
	if (sampleIndex < 0) return;
	comboSampleList->addItem(processor.getSamplePool().getSample(sampleIndex).name, sampleIndex + 1);
	comboSampleList->setSelectedId(sampleIndex + 1, dontSendNotification);
}

void SampleDraggerPluginAudioProcessorEditor::addSpriteButtonClicked() {
	auto index = comboSampleList->getSelectedId() - 1;
	if (index < 0) return;
	addFromIndex(index);
}

void SampleDraggerPluginAudioProcessorEditor::addFromIndex(int sampleIndex) {
	if (sampleIndex < 0) return; // Couldn't load for some readon
	auto s = processor.getSamplePool().getSample(sampleIndex);

	ScopedPointer<AudioThumbnail> t = loader.createThumbnail();
	t->reset(2, processor.getSampleRate());
	t->addBlock(0, s.buffer, 0, s.buffer.getNumSamples());

	ScopedPointer<SampleComponent> newsample = new SampleComponent();
	newsample->setThumbnail(t.release());
	newsample->setName(s.name);
	newsample->setIndex(sampleIndex);
	newsample->setNumSamples(s.buffer.getNumSamples());
	newsample->setPixelScale(scaleComponent->getPixelToSeconds());
    newsample->addListener(this);
	newsample->setTopLeftPosition(0, 200);

	addAndMakeVisible(sampleComponents.add(newsample.release()));
    for(auto s : sampleComponents) {
        s->setPixelScale(scaleComponent->getPixelToSeconds());
    }
}

bool SampleDraggerPluginAudioProcessorEditor::keyPressed(const KeyPress& key, Component* originatingComponent)  {
	ignoreUnused(originatingComponent);
	if (key == undoKey) {
		undo();
		return true;
	}
	if (key == redoKey) {
		redo();
		return true;
	}
	if (key == genKey) {
		generateFinalBuffer();
		return true;
	}
	return false;
}

void SampleDraggerPluginAudioProcessorEditor::undo() {
	if (cmdIndex < 0) return;
	Command* c = cmds[cmdIndex--];
	if (c != nullptr)
		c->undo();
}
void SampleDraggerPluginAudioProcessorEditor::redo() {
	if (cmdIndex >= cmds.size()) return;
	Command* c = cmds[cmdIndex++];
	if (c != nullptr)
		c->execute();
}

void SampleDraggerPluginAudioProcessorEditor::sampleMoved(SampleComponent* caller, Command* cmd) {
	ignoreUnused(caller);
	pushCmd(cmd);
}

void SampleDraggerPluginAudioProcessorEditor::sampleRemoved(SampleComponent* sample) {
	removeChildComponent(sample);
	auto indexOf = sampleComponents.indexOf(sample);
	sampleComponents.remove(indexOf, true);
}

void SampleDraggerPluginAudioProcessorEditor::pushCmd(Command* cmd) {
	cmds.set(++cmdIndex, cmd);
}

void SampleDraggerPluginAudioProcessorEditor::scaleChanged(ScaleComponent*, double newPixelToSeconds_, double viewPosition_) {
	for (auto s : sampleComponents) {
		s->setPixelScale(newPixelToSeconds_);
		s->setTransform(AffineTransform::translation(-viewPosition_ * newPixelToSeconds_, 0.0f));
	}
	repaint();
}

void SampleDraggerPluginAudioProcessorEditor::sampleStartPointChanged(SampleComponent* caller, Command* cmd) {
	ignoreUnused(caller);
	pushCmd(cmd);
}

void SampleDraggerPluginAudioProcessorEditor::sampleEndPointChanged(SampleComponent* caller, Command* cmd) {
	ignoreUnused(caller);
	pushCmd(cmd);
}

void SampleDraggerPluginAudioProcessorEditor::timerCallback() {
	if (autoGen->getToggleState()) { generateFinalBuffer(); }
	repaint();
}
