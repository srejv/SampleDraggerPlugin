#pragma once

#include "JuceHeader.h"

class ScaleComponent : public Component {
public:
	ScaleComponent();

	void setPixelToSeconds(double ps);
	double getPixelToSeconds() const;
private:
	void paint(Graphics& g) override;
	double pixelToSeconds;
};
