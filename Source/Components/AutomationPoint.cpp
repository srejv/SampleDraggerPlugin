/*
  ==============================================================================

    AutomationPoint.cpp
    Created: 12 Nov 2017 3:05:12pm
    Author:  srejv

  ==============================================================================
*/

#include "AutomationPoint.h"


AutomationPoint::AutomationPoint(double x, double y) : x(x), y(y) {
	constrainer.setMinimumOnscreenAmounts(4, 4, 4, 4);
	setSize(8, 8);
}

void AutomationPoint::paint(Graphics& g) {
	g.setColour(Colours::yellowgreen);
	g.fillRect(getLocalBounds());
}
void AutomationPoint::resized() { }

void AutomationPoint::mouseDown(const MouseEvent& e) {
	if (e.mods.isRightButtonDown()) {
		listeners.call(&Listener::automationPointRemoved, this);
		return;
	}
	myDragger.startDraggingComponent(this, e);
}
void AutomationPoint::mouseDrag(const MouseEvent& e) {
	myDragger.dragComponent(this, e, &constrainer);
	x = jlimit(0.0, 1.0, static_cast<double>(getX() + 4) / static_cast<double>(getParentComponent()->getWidth()));
	y = jlimit(0.0, 1.0, 1.0 - (static_cast<double>(getY() + 4) / static_cast<double>(getParentComponent()->getHeight())));
}

void AutomationPoint::checkContraints() {
	constrainer.checkComponentBounds(this);
}