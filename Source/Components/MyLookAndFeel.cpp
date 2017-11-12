/*
  ==============================================================================

    MyLookAndFeel.cpp
    Created: 12 Nov 2017 2:51:48pm
    Author:  srejv

  ==============================================================================
*/

#include "MyLookAndFeel.h"

void MyLookAndFeel::drawComboBox(Graphics& g, int width, int height, bool,
  int, int, int, int, ComboBox& box) 
{
  const auto cornerSize = 0.0f; //box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
  const juce::Rectangle<int> boxBounds(0, 0, width, height);

  g.setColour(box.findColour(ComboBox::backgroundColourId));
  g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

  g.setColour(box.findColour(ComboBox::outlineColourId));
  g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

  Rectangle<int> arrowZone(width - 30, 0, 20, height);
  Path path;
  path.startNewSubPath(arrowZone.getX() + 3.0f, arrowZone.getCentreY() - 2.0f);
  path.lineTo(static_cast<float> (arrowZone.getCentreX()), arrowZone.getCentreY() + 3.0f);
  path.lineTo(arrowZone.getRight() - 3.0f, arrowZone.getCentreY() - 2.0f);

  g.setColour(box.findColour(ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
  g.strokePath(path, PathStrokeType(2.0f));
}
