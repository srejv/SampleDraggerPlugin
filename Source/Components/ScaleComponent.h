#pragma once

#include "JuceHeader.h"

class ScaleComponent : public Component {
public:
	ScaleComponent();

	class Listener {
	public:
		virtual ~Listener() {}
		virtual void scaleChanged(ScaleComponent*, double newPixelToSeconds, double viewPosition) = 0;
	};

	void setPixelToSeconds(double ps);
	double getPixelToSeconds() const;

	void setViewPosition(double viewPosition);
	double getViewPosition() const {
		return viewPosition;
	}

	void addListener(Listener* listener) { listeners.add(listener); }
	void removeListener(Listener* listener) { listeners.remove(listener); }

	void mouseDown(const MouseEvent& e) override;
	void mouseDrag(const MouseEvent& e) override;
	void mouseUp(const MouseEvent& e) override;

private:
	void paint(Graphics& g) override;
	double pixelToSeconds = 100.0;
	double viewPosition = 0.0;

	Point<float> startDragPosition;
	bool isDragging = false;
	double startPixelToSeconds, startViewPosition;

	ListenerList<Listener> listeners;
};
