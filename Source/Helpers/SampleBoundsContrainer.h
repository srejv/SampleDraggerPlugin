/*
  ==============================================================================

    SampleBoundsContrainer.h
    Created: 8 Nov 2017 2:52:54pm
    Author:  Oscar Dragén

  ==============================================================================
*/

#pragma once


class SampleBoundsContrainer : public ComponentBoundsConstrainer
{
public:
	class Listener {
	public:
		virtual ~Listener() {}
		virtual void resizeStarted() = 0;
		virtual void resizeEnded() = 0;
	};

	void resizeStart() override {
		listeners.call(&Listener::resizeStarted);
	}
	void resizeEnd() override {
		listeners.call(&Listener::resizeEnded);
	}
	
	void addListener(Listener* listener) {
		listeners.add(listener);
	}
	void removeListener(Listener* listener) {
		listeners.remove(listener);
	}

	ListenerList<Listener> listeners;
	enum class Direction : int {Left, Right};
	Direction current = Direction::Left;
};