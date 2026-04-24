#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
class JC120AudioProcessor  : public juce::AudioProcessor,
                              public juce::AudioProcessorValueTreeState::Listener
{
public:
    JC120AudioProcessor();
    ~JC120AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi()  const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int  getNumPrograms()    override { return 1; }
    int  getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void parameterChanged (const juce::String& paramID, float newValue) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    //==============================================================================
    std::atomic<float> volume      { 0.8f };
    std::atomic<float> bass        { 0.5f };
    std::atomic<float> mid         { 0.5f };
    std::atomic<float> treble      { 0.5f };
    std::atomic<float> bright      { 0.0f };
    std::atomic<float> drive       { 0.1f };
    std::atomic<float> chorusRate  { 0.8f };
    std::atomic<float> chorusDepth { 0.6f };
    std::atomic<float> chorusMix   { 0.5f };
    std::atomic<float> reverbMix   { 0.15f };

    // Tone stack filter states
    float bassLP_z1[2]   = {};
    float trebleHP_z1[2] = {};
    float brightHP_z1[2] = {};

    // BBD chorus delay buffer
    static constexpr int MAX_DELAY = 8192;
    float delayBufL[MAX_DELAY] = {};
    float delayBufR[MAX_DELAY] = {};
    int   writePos = 0;

    // LFO
    float lfoPhase = 0.0f;

    // Schroeder reverb
    static constexpr int COMB1=1557, COMB2=1617, COMB3=1491, COMB4=1422;
    static constexpr int AP1=225, AP2=341;

    float comb1L[COMB1]={}, comb1R[COMB1]={};
    float comb2L[COMB2]={}, comb2R[COMB2]={};
    float comb3L[COMB3]={}, comb3R[COMB3]={};
    float comb4L[COMB4]={}, comb4R[COMB4]={};
    float ap1L[AP1]={}, ap1R[AP1]={};
    float ap2L[AP2]={}, ap2R[AP2]={};
    int c1=0, c2=0, c3=0, c4=0, a1=0, a2=0;

    double currentSampleRate = 44100.0;

    // DSP helpers
    float softClip      (float x, float amt);
    float onePoleLP     (float x, float& z1, float cutoff);
    float onePoleHP     (float x, float& z1, float cutoff);
    float toneStack     (float x, int ch);
    float readDelayInterp (float* buf, float delaySamples);
    float combFilter    (float x, float* buf, int& pos, int size, float fb);
    float allpass       (float x, float* buf, int& pos, int size, float coeff);
    void  processReverb (float inL, float inR, float& outL, float& outR);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JC120AudioProcessor)
};
