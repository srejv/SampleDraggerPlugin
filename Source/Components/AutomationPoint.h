/*
  ==============================================================================

    AutomationPoint.h
    Created: 12 Nov 2017 3:05:12pm
    Author:  srejv

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class AutomationPoint : public Component {
public:
	AutomationPoint(double x, double y);

	class Listener {
	public:
		virtual ~Listener() {}
		virtual void automationPointRemoved(AutomationPoint* caller) = 0;
	};

	void paint(Graphics& g) override;
	void resized() override;

	void mouseDown(const MouseEvent& e) override;
	void mouseDrag(const MouseEvent& e) override;

	void checkContraints();

	double x, y;
	ComponentDragger myDragger;
	ComponentBoundsConstrainer constrainer;

	void addListener(Listener* listener) { listeners.add(listener); }
	void removeListener(Listener* listener) { listeners.remove(listener); }

public:
	ListenerList<Listener> listeners;

	class Comparer {
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
};
