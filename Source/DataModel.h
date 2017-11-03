#pragma once

struct MySample {
	double position = 0.0f;
	int numChannels = 0;
	String fileName;
	AudioSampleBuffer fileBuffer;
	ScopedPointer<AudioThumbnail> thumbnail;
};
