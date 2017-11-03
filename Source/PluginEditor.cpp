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
	addAndMakeVisible(addSample = new TextButton("Add Sample"));
	addSample->addListener(this);

	addAndMakeVisible(saveGenerated = new TextButton("Save Generated"));
	saveGenerated->addListener(this);

	addAndMakeVisible(pixelsToSeconds);
	pixelsToSeconds.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight, true, 80, 20);
	pixelsToSeconds.setRange(2.0f, 12000.0f);
	pixelsToSeconds.setTextValueSuffix("px/s");
	pixelsToSeconds.setValue(100.0f);
	pixelsToSeconds.addListener(this);

	addAndMakeVisible(scaleComponent);

	addAndMakeVisible(generateWaveform = new TextButton("Generate"));
	generateWaveform->addListener(this);

	addAndMakeVisible(playButton = new TextButton("Play"));
	playButton->setClickingTogglesState(true);
	playButton->addListener(this);

	startTimer(1);

	setSize(800, 600);
}

SampleDraggerPluginAudioProcessorEditor::~SampleDraggerPluginAudioProcessorEditor()
{
	specialBufferThumbnail = nullptr;

	removeAllChildren();
	samples.clear(true);
}

//==============================================================================
void SampleDraggerPluginAudioProcessorEditor::paint (Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(Colour(30, 30, 30));

	g.setColour(Colour(120, 120, 120));
	auto area(getLocalBounds().removeFromRight(200));
	//for (auto b : sampleBuffers) {
	//g.drawText(b->fileName, area.removeFromTop(20), Justification::centredLeft);
	//}

	if (specialBufferThumbnail != nullptr) {
		double width = (processor.getNumSamples() / processor.getSampleRate()) * pixelsToSeconds.getValue();
		drawWaveform(g, getLocalBounds().removeFromBottom(100).withTrimmedBottom(20).withWidth(roundToInt(width)));
	}
}

void SampleDraggerPluginAudioProcessorEditor::resized()
{
	auto row(getLocalBounds().removeFromBottom(20));
	addSample->setBounds(row.removeFromLeft(100));
	generateWaveform->setBounds(row.removeFromRight(100));
	pixelsToSeconds.setBounds(row);

	saveGenerated->setBounds(getLocalBounds().removeFromTop(20).removeFromRight(100).translated(0, 20));
	playButton->setBounds(getLocalBounds().removeFromBottom(20).removeFromRight(100).translated(0, -20));

	scaleComponent.setBounds(getLocalBounds().removeFromTop(20));
}


void SampleDraggerPluginAudioProcessorEditor::drawWaveform(Graphics& g, const Rectangle<int>& thumbnailBounds)
{
	g.setColour(Colour(50, 50, 50));
	g.fillRect(thumbnailBounds);
	double startTime = 0.0f;
	double endTime = processor.getNumSamples() / processor.getSampleRate();
	float verticalZoom = 1.0f;
	g.setColour(Colours::red);
	specialBufferThumbnail->drawChannels(g, thumbnailBounds, startTime, endTime, verticalZoom);
}
void SampleDraggerPluginAudioProcessorEditor::sliderValueChanged(Slider* slider) 
{
	for (auto s : samples) {
		s->setPixelScale(slider->getValue());
	}
	scaleComponent.setPixelToSeconds(slider->getValue());
};

void SampleDraggerPluginAudioProcessorEditor::buttonClicked(Button* btn) 
{
	if (btn == addSample) {
		openButtonClicked();
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
}

void SampleDraggerPluginAudioProcessorEditor::generateFinalBuffer() {
	int min = INT32_MAX;
	int max = 0;
	for (auto s : samples) {
		auto start = s->getSampleStartPosition();
		auto len = s->getSampleLength();
		if (start < min) { min = start; }
		if ((start + len) > max) { max = (start + len); }
	}
	int length = max - min;

	if (specialBufferThumbnail == nullptr) specialBufferThumbnail = loader.createThumbnail();
	specialBufferThumbnail->reset(2, processor.getSampleRate());

	ScopedPointer<AudioSampleBuffer> workbuffer = new AudioSampleBuffer(2, length);
	ScopedPointer<AudioSampleBuffer> xtrabuffer = new AudioSampleBuffer(2, length);
	workbuffer->clear();
	xtrabuffer->clear();
	for (auto s : samples) {
		auto startPos = static_cast<int>(s->getSampleStartPosition() - min);
		for (int i = 0; i < s->getNumChannels(); ++i) {
			workbuffer->addFrom(i, startPos,
				s->getSource(), i, 0,
				s->getSampleLength());
			xtrabuffer->addFrom(i, startPos,
				s->getSource(), i, 0,
				s->getSampleLength());
		}

	}
	specialBufferThumbnail->addBlock(0, *xtrabuffer, 0, workbuffer->getNumSamples());
	processor.swapBuffer(workbuffer.release());
}

void SampleDraggerPluginAudioProcessorEditor::openButtonClicked()
{
	ScopedPointer<MySample> mySample = loader.loadAudioFile();
	if (mySample == nullptr) return;
	addAndMakeVisible(samples.add(new Sample(mySample.release())));
	samples.getLast()->setPixelScale(pixelsToSeconds.getValue());
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