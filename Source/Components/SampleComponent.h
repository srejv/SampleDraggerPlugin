/*
  ==============================================================================

    SampleComponent.h
    Created: 4 Nov 2017 6:38:45am
    Author:  srejv

  ==============================================================================
*/

#pragma once

	class SampleComponent : public Component
	{
	public:
		SampleComponent() {}

		void setThumbnail(AudioThumbnail* newThumbnail) {
			thumbnail = newThumbnail;
		}

		int getIndex() { return samplePoolIndex; }
		void setIndex(int newIndex) { samplePoolIndex = newIndex; }

		void setSampleRate(double newSampleRate) {
			sampleRate = newSampleRate;
		}

		void setNumSamples(int numSamples) {
			nsamples = numSamples;
		}

		
		void setPixelScale(double pts) {
			pixelToSeconds = pts;
			
			double width = (nsamples / sampleRate) * pixelToSeconds;
			setSize(roundToInt(width), 80);

			setTopLeftPosition(roundToInt(position * pixelToSeconds), getY());
			repaint();
		}

		void paint(Graphics& g) override {
			g.fillAll(Colour(50, 50, 50));
			g.setColour(Colour(200, 200, 200));
			g.drawText(getName(), getLocalBounds(), Justification::centred);

			if (thumbnail != nullptr) {
				const double startTime = 0.0f;
				const double endTime = thumbnail->getTotalLength();
				float verticalZoom = 1.0f;
				g.setColour(Colours::red);
				thumbnail->drawChannels(g, getLocalBounds(), 
					startTime, endTime, verticalZoom);
			}
		}

		int getSampleStartPosition() const {
			return roundToInt(position * sampleRate);
		}

		int getSampleLength() const {
			return nsamples;
		}

		int getNumChannels() {
			return thumbnail->getNumChannels();
		}

	private:
		double sampleRate = 48000.0;
		ComponentDragger myDragger;

		void mouseDown(const MouseEvent& e) override
		{
			myDragger.startDraggingComponent(this, e);
		}

		void mouseDrag(const MouseEvent& e) override
		{
			myDragger.dragComponent(this, e, nullptr);
			position = getX() / pixelToSeconds;
		}

		ScopedPointer<AudioThumbnail> thumbnail;
		int samplePoolIndex;

		double position = 0;
		int nsamples = 0;

		double pixelToSeconds = 100.0f;
	};