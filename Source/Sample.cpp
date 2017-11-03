#include "Sample.h"

Sample::Sample(MySample* sample) : mySample(sample) {
  setName(mySample->fileName);

  auto nsamples = sample->fileBuffer.getNumSamples();
  double sampleRate = 44100.0;
  double pixelsPerSecond = 10.0f;

  double width = (nsamples / sampleRate) * pixelsPerSecond;
  setSize(roundToInt(width), 80);
}

void Sample::setPixelScale(double newPixelScale) {
  pixelsToSeconds = newPixelScale;
  auto nsamples = mySample->fileBuffer.getNumSamples();
  double sampleRate = 44100.0;

  double width = (nsamples / sampleRate) * pixelsToSeconds;
  setSize(roundToInt(width), 80);

  setTopLeftPosition(roundToInt(mySample->position * pixelsToSeconds), getY());
  repaint();
}

void Sample::paint(Graphics& g)  {
  g.setColour(Colour(240, 240, 240));
  g.fillRect(getLocalBounds());


  g.setColour(Colour(30, 30, 30));
  g.drawText(getName(), getLocalBounds(), Justification::centred);


  if (mySample != nullptr) {
    paintIfFileLoaded(g, getLocalBounds());

    g.setColour(Colour(30, 30, 30));
    auto nsamples = mySample->fileBuffer.getNumSamples();
    g.drawText(String(), getLocalBounds().removeFromBottom(20), Justification::centred);
  }
}

void Sample::paintIfFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds)
{
  const double startTime = 0.0f;
  const double endTime = mySample->thumbnail->getTotalLength();
  float verticalZoom = 1.0f;
  g.setColour(Colours::red);
  mySample->thumbnail->drawChannels(g,
    thumbnailBounds, startTime, endTime, verticalZoom);
}

int Sample::getSampleStartPosition() const {
  const double sampleRate = 48000.0;
  return roundToInt(mySample->position * sampleRate);
}

int Sample::getSampleLength() const {
  return mySample->fileBuffer.getNumSamples();
}

AudioBuffer<float>& Sample::getSource() {
  return mySample->fileBuffer;
}

int Sample::getNumChannels() const {
  return mySample->numChannels;
 }

void Sample::mouseDown(const MouseEvent& e) 
{
  myDragger.startDraggingComponent(this, e);
}

void Sample::mouseDrag(const MouseEvent& e) 
{
  myDragger.dragComponent(this, e, &constrainer);
  mySample->position = getX() / pixelsToSeconds;
}
