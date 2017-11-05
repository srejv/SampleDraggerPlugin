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

class SampleComponent : public Component, public Button::Listener
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
		virtual void sampleMoved(SampleComponent*, Command* cmd) = 0;
		virtual void sampleRemoved(SampleComponent*) = 0;
	};

	void addListener(Listener* listener) { listeners.add(listener);  }
	void removeListner(Listener* listener) { listeners.remove(listener); }

	void resized() override;
	void paint(Graphics& g) override;
	void buttonClicked(Button* btn) override;

	void setThumbnail(AudioThumbnail* newThumbnail);
	void setIndex(int newIndex);
	void setSampleRate(double newSampleRate);
	void setNumSamples(int numSamples);
	void setPixelScale(double pts);

	int getIndex();
	int getSampleStartPosition() const;
	int getSampleLength() const;
	int getNumChannels();

	double getPixelScale() const;

	struct MovePositionCommand : public Command
	{
		MovePositionCommand(SampleComponent* c, Point<double> from, Point<double> to) : c(c), from(from), to(to) { }

		void execute() override {
			if (!c.wasObjectDeleted()) {
				c->setPosition(to);
			}

		}
		void undo() override {
			if (!c.wasObjectDeleted()) {
				c->setPosition(from);
			}
		}

	private:
		WeakReference<SampleComponent> c;
		Point<double> from;
		Point<double> to;
	};

	void setPosition(const Point<double>& newPosition);

private:

	double sampleRate = 48000.0;
	ComponentDragger myDragger;

	void mouseDown(const MouseEvent& e) override;
	void mouseDrag(const MouseEvent& e) override;
	void mouseUp(const MouseEvent& e) override;

	Point<float> moveFrom;
	double positionMoveFrom;

	ScopedPointer<AudioThumbnail> thumbnail;
	int samplePoolIndex;

	double position = 0;
	int nsamples = 0;

	double pixelToSeconds = 100.0f;

	ListenerList<Listener> listeners;

	ScopedPointer<TextButton> remove;

	JUCE_DECLARE_WEAK_REFERENCEABLE(SampleComponent)
};
