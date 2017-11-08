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

class EnvelopeComponent : public Component {
public:
	class AutomationPoint : public Component {
	public:
		AutomationPoint(EnvelopeComponent &owner, double x, double y) : x(x), y(y), owner(owner) {
			constrainer.setMinimumOnscreenAmounts(4, 4, 4, 4);
			setSize(8, 8);
		}

		void paint(Graphics& g) override {
			g.setColour(Colours::yellowgreen);
			g.fillRect(getLocalBounds());
		}
		void resized() override { }

		void mouseDown(const MouseEvent& e) override
		{
			if (e.mods.isRightButtonDown()) {
				owner.removePoint(this);
				return;
			}
			myDragger.startDraggingComponent(this, e);
		}
		void mouseDrag(const MouseEvent& e) override
		{
			myDragger.dragComponent(this, e, &constrainer);
			x = jlimit(0.0, 1.0, static_cast<double>(getX() + 4) / static_cast<double>(getParentComponent()->getWidth()));
			y = jlimit(0.0, 1.0, 1.0 - (static_cast<double>(getY() + 4) / static_cast<double>(getParentComponent()->getHeight())));
		}

		void checkContraints() {
			constrainer.checkComponentBounds(this);
		}

		double x, y;
		ComponentDragger myDragger;
		ComponentBoundsConstrainer constrainer;
		EnvelopeComponent &owner;
	};

	void removePoint(AutomationPoint* point) {
		removeChildComponent(point);
		points.removeObject(point, true);
		resized();
	}

	class CompareAutomationPoints {
	public:
		int compareElements(AutomationPoint* first, AutomationPoint* second) {
			int d = (first->getX() + 4) - (second->getX() + 4);
			if (d == 0) {
				if (first->getY() > second->getY()) {
					return 1;
				}
				if (first->getY() < second->getY()) {
					return -1;
				}
			}
			return d;
		}
	};

	ScopedPointer<AutomationPoint> startPoint;
	ScopedPointer<AutomationPoint> endPoint;

	CompareAutomationPoints comparer;
	EnvelopeComponent() {
		addAndMakeVisible(startPoint = new AutomationPoint(*this, 0.0, 0.0));
		startPoint->setInterceptsMouseClicks(false, false);

		addAndMakeVisible(endPoint = new AutomationPoint(*this, 1.0, 0.0));
		endPoint->setInterceptsMouseClicks(false, false);
		
		addAndMakeVisible(points.add(new AutomationPoint(*this, 0.0, 1.0)));
		addAndMakeVisible(points.add(new AutomationPoint(*this, 1.0, 1.0)));

		resized();
	}
	// A [0,1],[0,1] grid, sort by x, apply y.
	void paint(Graphics &g) { 
		g.setColour(Colours::yellowgreen);

		if (points.size() == 0) return;

		points.sort(comparer);

		const int n = points.size() - 1;
		
		g.drawLine(startPoint->getX() + 4, startPoint->getY() + 4
			, points.getFirst()->getX() + 4, points.getFirst()->getY() + 4
			, 1.0f);

		for (int i = 0; i < n; ++i) {
			g.drawLine(points[i]->getX() + 4, points[i]->getY() + 4
				, points[i + 1]->getX() + 4, points[i + 1]->getY() + 4
				, 1.0f);
		}

		g.drawLine(points.getLast()->getX() + 4, points.getLast()->getY() + 4
			, endPoint->getX() + 4, endPoint->getY() + 4
			, 1.0f);
	}

	double samplePosition(double p) {
	}

	void resized() override {
		double w = static_cast<double>(getWidth()), h = static_cast<double>(getHeight());
		for (auto* p : points) {
			auto x = roundToInt(p->x * w) - 4;
			auto y = roundToInt((1.0 - p->y) * h) - 4;
			p->setTopLeftPosition(x, y);
			p->checkContraints();
		}
		{
			auto x = roundToInt(startPoint->x * w) - 4;
			auto y = roundToInt((1.0 - startPoint->y) * h) - 4;
			startPoint->setTopLeftPosition(x, y);
			startPoint->checkContraints();
		}
		{
			auto x = roundToInt(endPoint->x * w) - 4;
			auto y = roundToInt((1.0 - endPoint->y) * h) - 4;
			endPoint->setTopLeftPosition(x, y);
			endPoint->checkContraints();
		}
		repaint();
	}

	void mouseDown(const MouseEvent& e) override { getParentComponent()->mouseDown(e); }
	void mouseDrag(const MouseEvent& e) override { getParentComponent()->mouseDrag(e); }

	void mouseDoubleClick(const MouseEvent& e) override
	{
		auto ev(e.getEventRelativeTo(this));
		addPoint(ev.getMouseDownX(), ev.getMouseDownY());
	}
	
	void addPoint(double clickComponentX, double clickComponentY) {
		double w = static_cast<double>(getWidth()), h = static_cast<double>(getHeight());
		addAndMakeVisible(points.add(new AutomationPoint(*this, clickComponentX / w, 1.0 - (clickComponentY / h))));
		resized();
		repaint();
	}
	OwnedArray<AutomationPoint> points;
};

class SampleBoundsContrainer : public ComponentBoundsConstrainer
{
public:
	class Listener {
	public:
		virtual ~Listener() {}
		virtual void resizeStarted() = 0;
		virtual void resizeEnded() = 0;
	};

	void resizeStart() override {
		listeners.call(&Listener::resizeStarted);
	}
	void resizeEnd() override {
		listeners.call(&Listener::resizeEnded);
	}
	
	void addListener(Listener* listener) {
		listeners.add(listener);
	}
	void removeListener(Listener* listener) {
		listeners.remove(listener);
	}

	ListenerList<Listener> listeners;
	enum class Direction : int {Left, Right};
	Direction current = Direction::Left;
};

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
