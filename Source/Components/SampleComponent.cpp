/*
  ==============================================================================

    SampleComponent.cpp
    Created: 4 Nov 2017 1:09:23pm
    Author:  srejv

  ==============================================================================
*/

#include "SampleComponent.h"


SampleComponent::SampleComponent() {
	addAndMakeVisible(remove = new TextButton("X"));
	remove->addListener(this);
}

void SampleComponent::resized() {
	remove->setBounds(getLocalBounds().removeFromTop(20).removeFromLeft(20));
}

void SampleComponent::setThumbnail(AudioThumbnail* newThumbnail) {
  thumbnail = newThumbnail;
}

int SampleComponent::getIndex() { return samplePoolIndex; }
void SampleComponent::setIndex(int newIndex) { samplePoolIndex = newIndex; }

void SampleComponent::setSampleRate(double newSampleRate) {
  sampleRate = newSampleRate;
}

void SampleComponent::setNumSamples(int numSamples) {
  nsamples = numSamples;
}

void SampleComponent::setPixelScale(double pts) {
  pixelToSeconds = pts;

  double width = (nsamples / sampleRate) * pixelToSeconds;
  setSize(roundToInt(width), 80);

  setTopLeftPosition(roundToInt(position * pixelToSeconds), getY());
  repaint();
}

void SampleComponent::paint(Graphics& g) {
  const auto colourScheme(static_cast<LookAndFeel_V4&>(getLookAndFeel()).getCurrentColourScheme());

  g.fillAll(colourScheme.getUIColour(LookAndFeel_V4::ColourScheme::widgetBackground));

  g.setColour(colourScheme.getUIColour(LookAndFeel_V4::ColourScheme::outline));
  g.drawRect(getLocalBounds());

  g.setColour(colourScheme.getUIColour(LookAndFeel_V4::ColourScheme::defaultText));
  g.drawText(getName(), getLocalBounds(), Justification::centred);

  if (thumbnail != nullptr) {
    const double startTime = 0.0f;
    const double endTime = thumbnail->getTotalLength();
    float verticalZoom = 1.0f;
    g.setColour(colourScheme.getUIColour(LookAndFeel_V4::ColourScheme::defaultFill));
    thumbnail->drawChannels(g, getLocalBounds(),
      startTime, endTime, verticalZoom);
  }
}

int SampleComponent::getSampleStartPosition() const {
  return roundToInt(position * sampleRate);
}

int SampleComponent::getSampleLength() const {
  return nsamples;
}

int SampleComponent::getNumChannels() {
  return thumbnail->getNumChannels();
}

void SampleComponent::mouseDown(const MouseEvent& e)
{
	moveFrom = getPosition().toFloat();
	myDragger.startDraggingComponent(this, e);
}

void SampleComponent::mouseDrag(const MouseEvent& e)
{
	myDragger.dragComponent(this, e, nullptr);
	position = getX() / pixelToSeconds;
}

void SampleComponent::mouseUp(const MouseEvent& e)
{
	ScopedPointer<Command> cmd = new MoveCommand(this, moveFrom, getPosition().toFloat());
	listeners.call(&Listener::sampleMoved, this, cmd.release());
}

void SampleComponent::buttonClicked(Button* btn)
{
	listeners.call(&Listener::sampleRemoved, this);
}

