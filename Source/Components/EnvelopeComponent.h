/*
  ==============================================================================

    EnvelopeComponent.h
    Created: 8 Nov 2017 2:52:15pm
    Author:  Oscar Dragén

  ==============================================================================
*/

#pragma once

#include "../Models/MySample.h"
#include "AutomationPoint.h"

class EnvelopeComponent : public Component, public AutomationPoint::Listener {
public:
	EnvelopeComponent();

	void paint(Graphics &g) override;
	void drawLine(Graphics& g, const AutomationPoint* p1, const AutomationPoint* p2);

	void resized() override;

	void mouseDown(const MouseEvent& e) override;
	void mouseDrag(const MouseEvent& e) override;
	void mouseDoubleClick(const MouseEvent& e) override;

	void automationPointRemoved(AutomationPoint* caller) override;

	void addPoint(double clickComponentX, double clickComponentY);
	void removePoint(AutomationPoint* point);

	std::vector<Ramp>* getRamps(int nsamples);

private:
	OwnedArray<AutomationPoint> points;
	ScopedPointer<AutomationPoint> startPoint;
	ScopedPointer<AutomationPoint> endPoint;

	AutomationPoint::Comparer comparer;
};
