#pragma once

#include "JuceHeader.h"

struct MySample {
	double position = 0.0f;
	int numChannels = 0;
	String fileName;
	AudioSampleBuffer fileBuffer;
	ScopedPointer<AudioThumbnail> thumbnail;
};

struct Ramp {
    int numSamples;
    double startGain;
    double endGain;
};
