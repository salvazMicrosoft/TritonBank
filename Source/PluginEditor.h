/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::Listener ListenerAttachment;

//==============================================================================
/**
*/
class TritonBankAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TritonBankAudioProcessorEditor (TritonBankAudioProcessor&);
    TritonBankAudioProcessorEditor(TritonBankAudioProcessor& p, AudioProcessorValueTreeState& vts);
    ~TritonBankAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TritonBankAudioProcessor& audioProcessor;
    AudioProcessorValueTreeState& valueTreeState;

    Slider sliderBankMSB;
    Slider sliderBankLSB;
    std::unique_ptr<SliderAttachment> MSBAttachment;
    std::unique_ptr<SliderAttachment> LSBAttachment;
//    void sliderValueChanged(Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TritonBankAudioProcessorEditor)
};
