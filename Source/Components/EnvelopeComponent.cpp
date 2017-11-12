/*
  ==============================================================================

    EnvelopeComponent.cpp
    Created: 12 Nov 2017 3:11:40pm
    Author:  srejv

  ==============================================================================
*/

#include "EnvelopeComponent.h"

EnvelopeComponent::EnvelopeComponent() {
	addAndMakeVisible(startPoint = new AutomationPoint(0.0, 0.0));
	startPoint->setInterceptsMouseClicks(false, false);

	addAndMakeVisible(endPoint = new AutomationPoint(1.0, 0.0));
	endPoint->setInterceptsMouseClicks(false, false);

	addAndMakeVisible(points.add(new AutomationPoint(0.0, 1.0)));
	addAndMakeVisible(points.add(new AutomationPoint(1.0, 1.0)));

	for (auto p : points) {
		p->addListener(this);
	}

	resized();
}

void EnvelopeComponent::paint(Graphics &g) {
	if (points.size() == 0) return;
	points.sort(comparer);
	const int n = points.size() - 1;

	g.setColour(Colours::yellowgreen);
	drawLine(g, startPoint, points.getFirst());
	for (int i = 0; i < n; ++i) {
		drawLine(g, points[i], points[i + 1]);
	}
	drawLine(g, points.getLast(), endPoint.get());
}

void EnvelopeComponent::drawLine(Graphics& g, const AutomationPoint* p1, const AutomationPoint* p2) {
	const float lineThickness = 2.0f;
	g.drawLine(p1->getX() + 4.0f, p1->getY() + 4.0f
		, p2->getX() + 4.0f, p2->getY() + 4.0f
		, lineThickness);
}

void EnvelopeComponent::resized()  {
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

void EnvelopeComponent::mouseDown(const MouseEvent& e)  { getParentComponent()->mouseDown(e); }
void EnvelopeComponent::mouseDrag(const MouseEvent& e)  { getParentComponent()->mouseDrag(e); }

void EnvelopeComponent::mouseDoubleClick(const MouseEvent& e)  {
	auto ev(e.getEventRelativeTo(this));
	addPoint(ev.getMouseDownX(), ev.getMouseDownY());
}

void EnvelopeComponent::addPoint(double clickComponentX, double clickComponentY) {
	double w = static_cast<double>(getWidth()), h = static_cast<double>(getHeight());
	addAndMakeVisible(points.add(new AutomationPoint(clickComponentX / w, 1.0 - (clickComponentY / h))));
	points.getLast()->addListener(this);
	resized();
	repaint();
}

void EnvelopeComponent::automationPointRemoved(AutomationPoint* caller)  {
	removePoint(caller);
}

void EnvelopeComponent::removePoint(AutomationPoint* point) {
	removeChildComponent(point);
	points.removeObject(point, true);
	resized();
}

std::vector<Ramp>* EnvelopeComponent::getRamps(int nsamples) {
	points.sort(comparer);

	ScopedPointer<std::vector<Ramp>> ramps = new std::vector<Ramp>();

	if (points.size() == 0) {
		ramps->push_back({
			nsamples, 1.0, 1.0
		});
		return ramps.release();
	}

	const int n = points.size() - 1;

	ramps->push_back({ roundToInt(std::floor((points.getFirst()->x - startPoint->x) * nsamples)), startPoint->y, points.getFirst()->y });

	for (int i = 0; i < n; ++i) {
		ramps->push_back({ roundToInt(std::floor((points[i + 1]->x - points[i]->x) * nsamples)), points[i]->y, points[i + 1]->y });
	}

	ramps->push_back({ roundToInt(std::floor((endPoint->x - points.getLast()->x) * nsamples)), points.getLast()->y, endPoint->y });

	return ramps.release();
}