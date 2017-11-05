/*
  ==============================================================================

    SampleComponent.h
    Created: 4 Nov 2017 1:09:23pm
    Author:  srejv

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "../Commands/Command.h"

class SampleComponent : public Component
{
public:
	SampleComponent();
	~SampleComponent() {
		// This will zero all the references - you need to call this in your destructor.
		masterReference.clear();
	}

	class Listener {
	public:
		virtual ~Listener() {}
		virtual void sampleMoved(SampleComponent*, const Point<float>& from, const Point<float>& to) = 0;
	};

	void addListener(Listener* listener) { listeners.add(listener);  }
	void removeListner(Listener* listener) { listeners.remove(listener); }

	void paint(Graphics& g) override;

	void setThumbnail(AudioThumbnail* newThumbnail);
	void setIndex(int newIndex);
	void setSampleRate(double newSampleRate);
	void setNumSamples(int numSamples);
	void setPixelScale(double pts);

	int getIndex();
	int getSampleStartPosition() const;
	int getSampleLength() const;
	int getNumChannels();

private:
	double sampleRate = 48000.0;
	ComponentDragger myDragger;

	void mouseDown(const MouseEvent& e) override;
	void mouseDrag(const MouseEvent& e) override;
	void mouseUp(const MouseEvent& e) override;

	Point<float> moveFrom;

	ScopedPointer<AudioThumbnail> thumbnail;
	int samplePoolIndex;

	double position = 0;
	int nsamples = 0;

	double pixelToSeconds = 100.0f;

	ListenerList<Listener> listeners;

	JUCE_DECLARE_WEAK_REFERENCEABLE(SampleComponent)
};
