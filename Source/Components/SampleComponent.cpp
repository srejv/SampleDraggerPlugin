/*
  ==============================================================================

    SampleComponent.cpp
    Created: 4 Nov 2017 1:09:23pm
    Author:  srejv

  ==============================================================================
*/

#include "SampleComponent.h"


SampleComponent::SampleComponent() : resizeableEnd(this, &resizeContrain)
{
	resizeContrain.addListener(this);
	addAndMakeVisible(remove = new TextButton("X"));
	remove->addListener(this);
    
    addAndMakeVisible(resizeableEnd);
}

void SampleComponent::resizeStarted() {
	if (resizeableEnd.isMouseOverOrDragging())
	{
		if (resizeableEnd.getCurrentZone().getZoneFlags() == 1) {
			oldStart = startTime;
		}
		if (resizeableEnd.getCurrentZone().getZoneFlags() == 4) {
			oldEnd = endTime;
		}
	}
}
void SampleComponent::resizeEnded() {
	if (resizeableEnd.isMouseOverOrDragging())
	{
		if (resizeableEnd.getCurrentZone().getZoneFlags() == 1) {
			if (oldEnd == endTime) {
				listeners.call(&SampleComponent::Listener::sampleEndPointChanged, this, new SetEndCommand(this, oldEnd, endTime));
			}
			listeners.call(&SampleComponent::Listener::sampleStartPointChanged, this, new  SetStartCommand(this, oldStart, startTime));
		}
		if (resizeableEnd.getCurrentZone().getZoneFlags() == 4) {
			if (oldStart == startTime) {
				listeners.call(&SampleComponent::Listener::sampleStartPointChanged, this, new  SetStartCommand(this, oldStart, startTime));
			}
			listeners.call(&SampleComponent::Listener::sampleEndPointChanged, this, new SetEndCommand(this, oldEnd, endTime));
		}
	}
}

void SampleComponent::resized() {
	remove->setBounds(getLocalBounds().removeFromTop(20).removeFromLeft(20));
    resizeableEnd.setBounds(getLocalBounds());
    
    double fullWidth = (nsamples / sampleRate) * pixelToSeconds;
    
    if(resizeableEnd.isMouseOverOrDragging())
    {
        if(resizeableEnd.getCurrentZone().getZoneFlags() == 1)
        {
			auto xpos = getX() / pixelToSeconds;
            auto diff = xpos - position;
        
            startTime += (diff * pixelToSeconds) / fullWidth;
            if(startTime > endTime) {
				startTime -= (diff * pixelToSeconds) / fullWidth;
			}
            
			position = xpos;
            
            if(startTime < 0.0) {
				endTime -= startTime;
                startTime = 0.0;
            }
        }
    
        if(resizeableEnd.getCurrentZone().getZoneFlags() == 4)
        {
            endTime = (static_cast<double>(getWidth()) / fullWidth) + startTime;
            
            if(endTime > 1.0) {
				startTime -= endTime - 1.0;
                endTime = 1.0;
            }
        }
    }
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

void SampleComponent::setPosition(const Point<double>& p) {
	position = p.getX();
	setTopLeftPosition(getX(), roundToInt(p.getY()));
	setPixelScale(getPixelScale());
}

void SampleComponent::setPixelScale(double pts) {
    pixelToSeconds = pts;

    auto mw = (nsamples / sampleRate) * pixelToSeconds;
    resizeContrain.setMaximumWidth(mw);
    resizeContrain.setMinimumWidth(1);
    resizeContrain.setMinimumHeight(getHeight());
    resizeContrain.setMaximumHeight(getHeight());
    
    double width = (nsamples / sampleRate) * pixelToSeconds * (endTime - startTime);
    
    setSize(jmax(1, roundToInt(width)), 80);
    
    setTopLeftPosition(roundToInt(position * pixelToSeconds), getY());
    repaint();
}

void SampleComponent::paint(Graphics& g) {
  const auto colourScheme(static_cast<LookAndFeel_V4&>(getLookAndFeel()).getCurrentColourScheme());

  g.fillAll(colourScheme.getUIColour(LookAndFeel_V4::ColourScheme::widgetBackground));


  if (thumbnail != nullptr) {
    float verticalZoom = 1.0f;
    g.setColour(colourScheme.getUIColour(LookAndFeel_V4::ColourScheme::defaultFill));
    thumbnail->drawChannels(g, getLocalBounds(),
      startTime*nsamples/sampleRate, endTime*nsamples/sampleRate, verticalZoom);
  }

    g.setColour(colourScheme.getUIColour(LookAndFeel_V4::ColourScheme::defaultText));
    g.drawText(getName(), getLocalBounds(), Justification::centred);

    
  g.setColour(colourScheme.getUIColour(LookAndFeel_V4::ColourScheme::outline));
  g.drawRect(getLocalBounds());
}

int SampleComponent::getSampleStartPosition() const {
	return roundToInt((getX() / pixelToSeconds) * sampleRate);
}

int SampleComponent::getInternalSampleStart() {
	return roundToInt(startTime * nsamples);
}

int SampleComponent::getSampleLength() const {
	return roundToInt(nsamples * (endTime - startTime));
}

int SampleComponent::getNumChannels() {
	return thumbnail->getNumChannels();
}

void SampleComponent::mouseDown(const MouseEvent& e) {
    if(e.mods.isRightButtonDown()) {
        PopupMenu menu;
        menu.addItem(1, String(startTime));
        menu.addItem(2, String(endTime));
        
        menu.show();
        
        return;
    }
	moveFrom = getPosition().toFloat();
	positionMoveFrom = position;
	myDragger.startDraggingComponent(this, e);
}

void SampleComponent::mouseDrag(const MouseEvent& e)
{
	myDragger.dragComponent(this, e, nullptr);
	position = getX() / pixelToSeconds;
}

void SampleComponent::mouseUp(const MouseEvent& e)
{
	ScopedPointer<Command> cmd = new MovePositionCommand(this, 
		{ positionMoveFrom, moveFrom.getY() }, 
		{ position, static_cast<double>(getY()) });
	listeners.call(&SampleComponent::Listener::sampleMoved, this, cmd.release());
}

void SampleComponent::buttonClicked(Button* btn)
{
	if (btn == remove) {
		listeners.call(&SampleComponent::Listener::sampleRemoved, this);
	}
}

double SampleComponent::getPixelScale() const {
	return pixelToSeconds;
}
