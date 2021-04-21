/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "Audio.h"
#include "UI.h"

//==============================================================================
MangleAudioProcessorEditor::MangleAudioProcessorEditor(MangleAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
}

MangleAudioProcessorEditor::~MangleAudioProcessorEditor()
{
}

//==============================================================================
void MangleAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(juce::Colours::white);
	g.setColour(juce::Colours::black);
	g.setFont(18.0f);
	g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void MangleAudioProcessorEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
}
