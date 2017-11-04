/*
  ==============================================================================

    Sample.h
    Created: 3 Nov 2017 3:19:12pm
    Author:  srejv

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "../Models/MySample.h"

class FadeOverlayComponent : public Component {

	double rampInTime = 0.0;
	double rampOutTime = 0.0;
};

class Sample : public Component {
public:
	Sample(MySample* sample);

	void setPixelScale(double newPixelScale);

	void paint(Graphics& g) override;

	int getSampleStartPosition() const ;
	int getSampleLength() const;
	int getNumChannels() const ;

	AudioBuffer<float>& getSource() ;

private:
	void paintIfFileLoaded(Graphics& g, const Rectangle<int>& thumbnailBounds);

	ComponentDragger myDragger;
	ComponentBoundsConstrainer constrainer;

	void mouseDown(const MouseEvent& e) override;
	void mouseDrag(const MouseEvent& e) override;
	int sampleIndex = 0;
	double pixelsToSeconds = 10.0f;

	ScopedPointer<MySample> mySample;
};
