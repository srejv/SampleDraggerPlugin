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
#include "EnvelopeComponent.h"
#include "../Helpers/SampleBoundsContrainer.h"


class SampleComponent : public Component, public Button::Listener, public SampleBoundsContrainer::Listener
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
		virtual void sampleStartPointChanged(SampleComponent*, Command* cmd) = 0;
		virtual void sampleEndPointChanged(SampleComponent*, Command* cmd) = 0;
	};

	void addListener(Listener* listener) { listeners.add(listener);  }
	void removeListner(Listener* listener) { listeners.remove(listener); }

	void resizeStarted() override;
	void resizeEnded() override;

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
    
    int getInternalSampleStart();

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
	
	struct SetStartCommand : public Command
	{
		SetStartCommand(SampleComponent* c, double from, double to) : c(c), from(from), to(to) { }

		void execute() override {
			if (!c.wasObjectDeleted()) {
				c->setStart(to);
			}

		}
		void undo() override {
			if (!c.wasObjectDeleted()) {
				c->setStart(from);
			}
		}
		
	private:
		WeakReference<SampleComponent> c;
		double from;
		double to;
	};

	struct SetEndCommand : public Command
	{
		SetEndCommand(SampleComponent* c, double from, double to) : c(c), from(from), to(to) { }

		void execute() override {
			if (!c.wasObjectDeleted()) {
				c->setEnd(to);
			}

		}
		void undo() override {
			if (!c.wasObjectDeleted()) {
				c->setEnd(from);
			}
		}

	private:
		WeakReference<SampleComponent> c;
		double from;
		double to;
	};

	void setPosition(const Point<double>& newPosition);
    
    void setStart(double newStart) {
		double fullWidth = (nsamples / sampleRate) * pixelToSeconds;
		double newWidth = (nsamples / sampleRate) * pixelToSeconds * (endTime - newStart);
		auto startDiff = newStart - startTime;
		auto xdiff = (startDiff * fullWidth) / pixelToSeconds;
		position += xdiff;
		startTime = newStart;
		
		setTopLeftPosition(roundToInt(position * pixelToSeconds), getY());
		setSize(roundToInt(newWidth), getHeight());
    }
    void setEnd(double newEnd) {
		double newWidth = (nsamples / sampleRate) * pixelToSeconds * (newEnd - startTime);
        endTime = newEnd;
		setSize(roundToInt(newWidth), getHeight());
    }
    
    double getStartTime() {
        return startTime;
    }
    double getEndTime() {
        return endTime;
    }
    
    std::vector<EnvelopeComponent::Ramp>* getRamps() {
        return envelopeComponent->getRamps(getSampleLength());
    }
private:
	SampleBoundsContrainer resizeContrain;
    ResizableBorderComponent resizeableEnd;
   

    
    double viewPosition = 0.0f;
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
    
    double startTime = 0.0f;
    double endTime = 1.0f;

	double oldStart = 0.0;
	double oldEnd = 0.0;

	ListenerList<Listener> listeners;

	ScopedPointer<TextButton> remove;

	ScopedPointer<EnvelopeComponent> envelopeComponent;

	JUCE_DECLARE_WEAK_REFERENCEABLE(SampleComponent)
};
