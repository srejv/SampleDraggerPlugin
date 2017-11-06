#pragma once

#include "JuceHeader.h"

class ScaleComponent : public Component {
public:
	ScaleComponent();

	void setPixelToSeconds(double ps);
	double getPixelToSeconds() const;
    
    void setViewPosition(double viewPosition);
private:
	void paint(Graphics& g) override;
	double pixelToSeconds = 100.0f;
    double viewPosition = 0.0f;
};
