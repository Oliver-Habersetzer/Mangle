/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "Audio.h"
#include "Utils.h"
#include "DistortionAlgorithms.h"
using namespace std;

//==============================================================================
MangleAudioProcessor::MangleAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
{
	addParameter(inputGain = new AudioParameterFloat("input gain", "Input gain", 0, 24, 1));
	addParameter(algorithm = new AudioParameterChoice("algorithm", "Algorithm", { "Brickwall", "Mod", "Infinite", "Mirror" }, 0));
	addParameter(threshold = new AudioParameterFloat("threshold", "Threshold", 0.001, 1, .8));
	addParameter(outputGain = new AudioParameterFloat("output gain", "Output gain", 0, 6, 1));
}

MangleAudioProcessor::~MangleAudioProcessor()
{
}

//==============================================================================
const juce::String MangleAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool MangleAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool MangleAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool MangleAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double MangleAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int MangleAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int MangleAudioProcessor::getCurrentProgram()
{
	return 0;
}

void MangleAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String MangleAudioProcessor::getProgramName(int index)
{
	return {};
}

void MangleAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void MangleAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
}

void MangleAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MangleAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif
/*
void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override
{
	auto mainInputOutput = getBusBuffer(buffer, true, 0);
	auto sideChainInput = getBusBuffer(buffer, true, 1);

	auto alphaCopy = alpha->get();
	auto thresholdCopy = threshold->get();

	for (int j = 0; j < buffer.getNumSamples(); ++j)
	{
		auto mixedSamples = 0.0f;

		for (int i = 0; i < sideChainInput.getNumChannels(); ++i)
			mixedSamples += sideChainInput.getReadPointer(i)[j];

		mixedSamples /= static_cast<float> (sideChainInput.getNumChannels());
		lowPassCoeff = (alphaCopy * lowPassCoeff) + ((1.0f - alphaCopy) * mixedSamples);

		if (lowPassCoeff >= thresholdCopy)
			sampleCountDown = (int)getSampleRate();

		// very in-effective way of doing this
		for (int i = 0; i < mainInputOutput.getNumChannels(); ++i)
			*mainInputOutput.getWritePointer(i, j) = sampleCountDown > 0 ? *mainInputOutput.getReadPointer(i, j) : 0.0f;

		if (sampleCountDown > 0)
			--sampleCountDown;
	}
}
*/
void MangleAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;

	const float gi = inputGain->get();
	const float go = outputGain->get();
	float t = threshold->get();
	auto t2 = t * 2.0;
	const auto oneOverT = 1.0 / t;

	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	// clear output buffers if necessary
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	buffer.applyGain(*inputGain);
	for (int channel = 0; channel < totalNumInputChannels; ++channel)
	{
		auto* output = buffer.getWritePointer(channel);
		for (int i = 0; i < buffer.getNumSamples(); i++) {
			output[i] = buffer.getReadPointer(channel)[i] * gi;
			switch (*algorithm)
			{
			case 0: // Brickwall
			default:
				output[i] = DistortionAlgorithms::Brickwall(output[i], t);
				break;
			case 1: // Mod
				output[i] = DistortionAlgorithms::Mod(output[i], t);
				break;
			case 2: // Infinite
				output[i] = DistortionAlgorithms::Infinite(output[i], t);
				break;
			case 3: // Mirror
				output[i] = DistortionAlgorithms::Mirror(output[i], t);
				break;
			}
			output[i] = output[i] * go;
		}
	}
	buffer.applyGain(*outputGain);
}

//==============================================================================
bool MangleAudioProcessor::hasEditor() const {
	return true;
}
juce::AudioProcessorEditor* MangleAudioProcessor::createEditor() {
	return new GenericAudioProcessorEditor(*this);
}

//==============================================================================
void MangleAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void MangleAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new MangleAudioProcessor();
}
