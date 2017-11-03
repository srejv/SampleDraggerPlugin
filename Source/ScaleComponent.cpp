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
  // Mark every second?
  auto area(getLocalBounds());

  double w = static_cast<double>(area.getWidth());

  float h = static_cast<float>(getHeight());

  g.setColour(Colours::red);
  // Seconds!
  double c = 0.0;
  while (c < w) {
    g.drawVerticalLine(roundToInt(c), 0, h);
    c += pixelToSeconds;
  }

  // Half a second!
  g.setColour(Colours::green);
  c = 0.0;
  while (c < w) {
    g.drawVerticalLine(roundToInt(c), 0, h - 8);
    c += pixelToSeconds * 0.5f;
  }

  // Quarter of a second!
  g.setColour(Colours::blue);
  c = 0.0;
  while (c < w) {
    g.drawVerticalLine(roundToInt(c), 0, h - 16);
    c += pixelToSeconds * 0.25f;
  }
}
