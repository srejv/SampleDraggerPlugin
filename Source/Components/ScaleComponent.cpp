#include "ScaleComponent.h"

ScaleComponent::ScaleComponent() {}

void ScaleComponent::setPixelToSeconds(double ps) {
  pixelToSeconds = ps;
  repaint();
}

double ScaleComponent::getPixelToSeconds() const {
  return pixelToSeconds;
}

void ScaleComponent::paint(Graphics& g) {
	const auto colourScheme(static_cast<LookAndFeel_V4&>(getLookAndFeel()).getCurrentColourScheme());
	colourScheme.getUIColour(LookAndFeel_V4::ColourScheme::menuBackground);

  // Mark every second?
  auto area(getLocalBounds());

  double w = static_cast<double>(area.getWidth());

  float h = static_cast<float>(getHeight());
    
  auto offset = roundToInt(viewPosition * pixelToSeconds);

  g.setColour(Colours::red);

  g.setFont(Font(10.0f));
  // Seconds!
  double c = 0.0f;
  int i = std::floor(viewPosition);
  while ((c - offset) < w) {
	auto x = roundToInt(c) - offset;
    g.drawVerticalLine(x, 0, h);
	if (x > 0) {
		g.drawText(String(++i) + "s", roundToInt(c) - offset, 0, 40, getHeight(), Justification::centredLeft);
	}
	c += pixelToSeconds;
  }

  // Half a second!
  g.setColour(Colours::green);
  c = 0.0;
  while ((c - offset) < w) {
    g.drawVerticalLine(roundToInt(c) - offset, 0, h - 8);
    c += pixelToSeconds * 0.5f;
  }

  // Quarter of a second!
  g.setColour(Colours::blue);
  c = 0.0;
  while ((c - offset) < w) {
    g.drawVerticalLine(roundToInt(c) - offset, 0, h - 16);
    c += pixelToSeconds * 0.25f;
  }
}

void ScaleComponent::setViewPosition(double nviewPosition) {
    viewPosition = nviewPosition;
    repaint();
}

void ScaleComponent::mouseDown(const MouseEvent& e) {
	auto event = e.getEventRelativeTo(this);
	isDragging = true;
	startDragPosition = event.getMouseDownPosition().toFloat();
	startPixelToSeconds = pixelToSeconds;
	startViewPosition = viewPosition;
}

void ScaleComponent::mouseDrag(const MouseEvent& e) {
	auto distanceX = -e.getDistanceFromDragStartX() / startPixelToSeconds;
	auto distanceY = e.getDistanceFromDragStartY() * 0.5;

	auto newPixelToSeconds = startPixelToSeconds + distanceY;
	auto newViewPosition = startViewPosition + distanceX;
	newPixelToSeconds = jlimit(2.0, 2000.0, newPixelToSeconds);
	newViewPosition = jlimit(0.0, 300.0, newViewPosition);

	setPixelToSeconds(newPixelToSeconds);
	setViewPosition(newViewPosition);

	listeners.call(&Listener::scaleChanged, this, newPixelToSeconds, newViewPosition);
	repaint();
}

void ScaleComponent::mouseUp(const MouseEvent&) {
	isDragging = false;
}