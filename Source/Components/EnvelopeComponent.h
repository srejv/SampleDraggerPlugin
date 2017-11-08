/*
  ==============================================================================

    EnvelopeComponent.h
    Created: 8 Nov 2017 2:52:15pm
    Author:  Oscar Dragén

  ==============================================================================
*/

#pragma once


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
	void paint(Graphics &g) override {
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
    
    struct Ramp {
        int numSamples;
        double startGain;
        double endGain;
    };
    
    std::vector<Ramp>* getRamps(int nsamples) {
        points.sort(comparer);
        
        ScopedPointer<std::vector<Ramp>> ramps = new std::vector<Ramp>();
        
        if(points.size() == 0) {
            ramps->push_back({
                nsamples, 1.0, 1.0
            });
            return ramps.release();
        }
        
        const int n = points.size() - 1;
        
        ramps->push_back({ roundToInt(std::floor((points.getFirst()->x - startPoint->x) * nsamples)), startPoint->y, points.getFirst()->y });
        
        for (int i = 0; i < n; ++i) {
            ramps->push_back({ roundToInt(std::floor((points[i+1]->x - points[i]->x) * nsamples)), points[i]->y, points[i+1]->y });
        }
        
        ramps->push_back({roundToInt(std::floor((endPoint->x - points.getLast()->x) * nsamples)), points.getLast()->y, endPoint->y });
        
        return ramps.release();
    }
};