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

	addAndMakeVisible(addSample = new TextButton("Load file -> Sound Pool"));
	addSample->addListener(this);

	addAndMakeVisible(saveGenerated = new TextButton("Save Generated Sample"));
	saveGenerated->addListener(this);

	addAndMakeVisible(pixelsToSeconds = new Slider());
	pixelsToSeconds->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight, false, 80, 20);
	pixelsToSeconds->setRange(2.0f, 2000.0f);
	pixelsToSeconds->setTextValueSuffix("px/s");
	pixelsToSeconds->setValue(100.0f);
	pixelsToSeconds->addListener(this);

	addAndMakeVisible(scaleComponent = new ScaleComponent());

	addAndMakeVisible(generateWaveform = new TextButton("Generate"));
	generateWaveform->addListener(this);

	addAndMakeVisible(playButton = new TextButton("Play"));
	playButton->setClickingTogglesState(true);
	playButton->addListener(this);

	addAndMakeVisible(comboSampleList = new ComboBox());
	comboSampleList->setTextWhenNothingSelected("No sample");
	comboSampleList->setTextWhenNoChoicesAvailable("Pool is empty");

	addAndMakeVisible(btnAddSprite = new TextButton("Add Sample"));
	btnAddSprite->addListener(this);

	startTimerHz(1);

	addKeyListener(this);

	setSize(800, 600);
}

SampleDraggerPluginAudioProcessorEditor::~SampleDraggerPluginAudioProcessorEditor()
{
	cmds.clear(true);

	removeAllChildren();

	addSample = nullptr;
	saveGenerated = nullptr;
	pixelsToSeconds = nullptr;
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
void SampleDraggerPluginAudioProcessorEditor::paint (Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(findColour(ResizableWindow::backgroundColourId));

	g.setColour(Colour(120, 120, 120));
	auto area(getLocalBounds().removeFromRight(200).withTrimmedTop(60));
	if (specialBufferThumbnail != nullptr) {
		double width = (processor.getNumSamples() / processor.getSampleRate()) * pixelsToSeconds->getValue();
		drawWaveform(g, getLocalBounds().removeFromBottom(100).withTrimmedBottom(20).withWidth(roundToInt(width)));
	}
}

void SampleDraggerPluginAudioProcessorEditor::resized()
{
	auto area(getLocalBounds());
	
	auto right = area.removeFromRight(100).withTrimmedBottom(20).withTrimmedBottom(20);
	saveGenerated->setBounds(right.removeFromTop(20));
	right.removeFromTop(20);
	addSample->setBounds(right.removeFromTop(100));
	right.removeFromTop(20);
	comboSampleList->setBounds(right.removeFromTop(20));
	btnAddSprite->setBounds(right.removeFromTop(20));
	right.removeFromTop(20);
	generateWaveform->setBounds(right.removeFromTop(20));
	playButton->setBounds(right.removeFromTop(20));

	pixelsToSeconds->setBounds(area.removeFromBottom(20));
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
	g.setColour(colourScheme.getUIColour(LookAndFeel_V4::ColourScheme::defaultFill));
	specialBufferThumbnail->drawChannels(g, thumbnailBounds, startTime, endTime, verticalZoom);
}
void SampleDraggerPluginAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	for (auto s : sampleComponents) {
		s->setPixelScale(slider->getValue());
	}
	scaleComponent->setPixelToSeconds(slider->getValue());
};

void SampleDraggerPluginAudioProcessorEditor::buttonClicked(Button* btn)
{
	if (btn == addSample) {
		openButtonClickedSoundPool();
	}
	if (btn == generateWaveform) {
		generateFinalBuffer();
	}
	if (btn == saveGenerated) {
		saveButtonClicked();
	}
	if (btn == playButton) {
		processor.setPlaying(playButton->getToggleState());
	}
	if (btn == btnAddSprite) {
		addSpriteButtonClicked();
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

    for (auto s : sampleComponents) {
        int index = s->getIndex();
        auto& source(processor.getSamplePool().getSample(index));
        if (source.name == String::empty) continue;

        auto startPos = static_cast<int>(s->getSampleStartPosition() - min);
		
        for (int i = 0; i < s->getNumChannels(); ++i) {
                workbuffer->addFrom(i, startPos,
                                    source.buffer, i, 0,
                                    source.buffer.getNumSamples());
                xtrabuffer->addFrom(i, startPos,
                                    source.buffer, i, 0,
                                    source.buffer.getNumSamples());
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
	newsample->setPixelScale(pixelsToSeconds->getValue());
	newsample->addListener(this);

	addAndMakeVisible(sampleComponents.add(newsample.release()));
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
