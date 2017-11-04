/*
  ==============================================================================

    MyLookAndFeel.h
    Created: 4 Nov 2017 4:50:13am
    Author:  srejv

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class MyLookAndFeel : public LookAndFeel_V4
{
public:
	MyLookAndFeel() { setColourScheme(getMyColourScheme()); }
	
	static ColourScheme getMyColourScheme()
	{
		return { 
			0xff1e1e1e, // windowBackground 
			0xff303030, // widgetBackground,
			0xff303030, // menuBackground,
			0xfff0f0f0, // outline,
			0xfff0f0f0, // defaultText,
			0xff808080, // defaultFill,
			0xff1e1e1e, // highlightedText,
			0xfff0f0f0, // highlightedFill
			0xfff0f0f0  // menuText
		};
	}
};