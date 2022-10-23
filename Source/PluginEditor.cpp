/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TritonBankAudioProcessorEditor::TritonBankAudioProcessorEditor (TritonBankAudioProcessor& p,AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);

    sliderBankMSB.setSliderStyle(Slider::IncDecButtons);
    sliderBankMSB.setTextBoxStyle(Slider::TextBoxAbove, false, 90, 20);
    sliderBankMSB.setPopupDisplayEnabled(true, false, this);
//    sliderBankMSB.setTextValueSuffix(" Banco");
    sliderBankMSB.setValue(1.0);

    // this function adds the slider to the editor
    addAndMakeVisible(&sliderBankMSB);

    MSBAttachment.reset(new SliderAttachment(valueTreeState, "bankMSB", sliderBankMSB));
    sliderBankMSB.setRange(0.0, 127.0, 1.0);
    sliderBankMSB.setNumDecimalPlacesToDisplay(0);


    sliderBankLSB.setSliderStyle(Slider::IncDecButtons);
    sliderBankLSB.setTextBoxStyle(Slider::TextBoxAbove, false, 120, 30);
    sliderBankLSB.setPopupDisplayEnabled(true, false, this);
//    sliderBankLSB.setTextValueSuffix(" Banco");
    sliderBankLSB.setValue(1.0);

    // this function adds the slider to the editor
    addAndMakeVisible(&sliderBankLSB);

    LSBAttachment.reset(new SliderAttachment(valueTreeState, "bankLSB", sliderBankLSB));
    sliderBankLSB.setRange(0.0, 127.0, 1.0);
    sliderBankLSB.setNumDecimalPlacesToDisplay(0);

    // add the listener to the slider
//    sliderBankLSB.addListener(this);

}

TritonBankAudioProcessorEditor::~TritonBankAudioProcessorEditor()
{
}

//==============================================================================
void TritonBankAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // set the current drawing colour to black
    g.setColour(juce::Colours::greenyellow);

    // set the font size and draw text to the screen
    g.setFont(15.0f);
    String miValor = String(audioProcessor.bankMSB);
    String miValorMenos = String(audioProcessor.bankLSB);
    g.drawFittedText("Triton Bank "+miValor+" "+miValorMenos, 0, 0, getWidth(), 30, Justification::centred, 1);
}

void TritonBankAudioProcessorEditor::resized()
{
    // quiero saber si lo modifica
    sliderBankMSB.setValue(audioProcessor.bankMSB);
    sliderBankLSB.setValue(audioProcessor.bankLSB);
    sliderBankMSB.setBounds(40, 30, 80, 160);
    sliderBankLSB.setBounds(160, 30, 80, 160);
}
/*
void TritonBankAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider==&sliderBankMSB)
        audioProcessor.bankMSB = sliderBankMSB.getValue();
    if (slider == &sliderBankLSB)
        audioProcessor.bankLSB = sliderBankLSB.getValue();
}
*/

