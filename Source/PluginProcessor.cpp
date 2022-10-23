/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TritonBankAudioProcessor::TritonBankAudioProcessor()
     : AudioProcessor (BusesProperties()) ,
     parametros(*this, nullptr, juce::Identifier("TritonBanco"),
        {
            std::make_unique<AudioParameterFloat>("bankMSB",            // parameterID
                                                  "bankMSB",            // parameter name
                                                         0.0f,              // minimum value
                                                         127.0f,              // maximum value
                                                         121.0f),             // default value
            std::make_unique<AudioParameterFloat>("bankLSB",      // parameterID
                                                "bankLSB",     // parameter name
                                                         0.0f,              // minimum value
                                                         127.0f,              // maximum value
                                                         0.0f)             // default value
        }) 
{
    state.addChild({ "Banco", { { "MSB",  121 }, { "LSB", 0 } }, {} }, 0, nullptr);

    for (int kk = 0; kk < 20; kk++)
    {
        pcCanal[kk] = false;
    }
    bankMSBParameter = parametros.getRawParameterValue("bankMSB");
    bankLSBParameter = parametros.getRawParameterValue("bankLSB");
}

TritonBankAudioProcessor::~TritonBankAudioProcessor()
{
}

//==============================================================================
const juce::String TritonBankAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TritonBankAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TritonBankAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TritonBankAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TritonBankAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TritonBankAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TritonBankAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TritonBankAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TritonBankAudioProcessor::getProgramName (int index)
{
    return {};
}

void TritonBankAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TritonBankAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oldBankMSB = *bankMSBParameter;
    oldBankLSB = *bankLSBParameter;
}

void TritonBankAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TritonBankAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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

void TritonBankAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }

    MidiBuffer processedMidi;
    float newBankMSB = *bankMSBParameter;
    float newBankLSB = *bankLSBParameter;
    if (newBankMSB != oldBankMSB) oldBankMSB = newBankMSB;
    if (newBankLSB != oldBankMSB) oldBankMSB = newBankLSB;
    bankMSB = int(newBankMSB);
    bankLSB = int(newBankLSB);

    for (const auto metadata : midiMessages)
    {

        auto message = metadata.getMessage();
        auto messageBackup = message;
        const auto time = metadata.samplePosition;

        int canal = message.getChannel();
        if (message.isController())
        {
            int ccNum=message.getControllerNumber();
            if (ccNum == 0)
            {
                message = MidiMessage::controllerEvent(canal, message.getControllerNumber(), bankMSB);
                pcCanal[canal] = true;
            }
            else if (ccNum==32)
            {
                message = MidiMessage::controllerEvent(canal, message.getControllerNumber(), bankLSB);
                pcCanal[canal] = true;
            }
            else
            {
                pcCanal[canal] = false;
            }
        }
        else if (message.isProgramChange() && pcCanal[canal]==false)
        {
            messageBackup= MidiMessage::controllerEvent(canal, 0, bankMSB);
            processedMidi.addEvent(messageBackup, time);
            messageBackup = MidiMessage::controllerEvent(canal, 32, bankLSB);
            processedMidi.addEvent(messageBackup, time);
            pcCanal[canal] = false;
        }
        else
        {
            pcCanal[canal] = false;
        }

        processedMidi.addEvent(message, time);
    }

    midiMessages.swapWith(processedMidi);

}

//==============================================================================
bool TritonBankAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TritonBankAudioProcessor::createEditor()
{
    return new TritonBankAudioProcessorEditor (*this,parametros);
}

//==============================================================================
void TritonBankAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto estado = parametros.copyState();
    std::unique_ptr<XmlElement> xml(estado.createXml());
    copyXmlToBinary(*xml, destData);
}

void TritonBankAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parametros.state.getType()))
            parametros.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TritonBankAudioProcessor();
}
