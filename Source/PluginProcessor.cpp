#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
JC120AudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "volume", 1 },      "Volume",      0.0f, 1.0f, 0.8f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "bass", 1 },        "Bass",        0.0f, 1.0f, 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "mid", 1 },         "Mid",         0.0f, 1.0f, 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "treble", 1 },      "Treble",      0.0f, 1.0f, 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "bright", 1 },      "Bright",      0.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "drive", 1 },       "Drive",       0.0f, 1.0f, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "chorusRate", 1 },  "Chorus Rate", 0.1f, 5.0f, 0.8f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "chorusDepth", 1 }, "Chorus Depth",0.0f, 1.0f, 0.6f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "chorusMix", 1 },   "Chorus Mix",  0.0f, 1.0f, 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "reverbMix", 1 },   "Reverb Mix",  0.0f, 1.0f, 0.15f));

    return { params.begin(), params.end() };
}

//==============================================================================
JC120AudioProcessor::JC120AudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
    const char* ids[] = { "volume","bass","mid","treble","bright","drive",
                          "chorusRate","chorusDepth","chorusMix","reverbMix" };
    for (auto* id : ids)
        apvts.addParameterListener (id, this);
}

JC120AudioProcessor::~JC120AudioProcessor()
{
    const char* ids[] = { "volume","bass","mid","treble","bright","drive",
                          "chorusRate","chorusDepth","chorusMix","reverbMix" };
    for (auto* id : ids)
        apvts.removeParameterListener (id, this);
}

//==============================================================================
void JC120AudioProcessor::parameterChanged (const juce::String& id, float v)
{
    if      (id == "volume")      volume      = v;
    else if (id == "bass")        bass        = v;
    else if (id == "mid")         mid         = v;
    else if (id == "treble")      treble      = v;
    else if (id == "bright")      bright      = v;
    else if (id == "drive")       drive       = v;
    else if (id == "chorusRate")  chorusRate  = v;
    else if (id == "chorusDepth") chorusDepth = v;
    else if (id == "chorusMix")   chorusMix   = v;
    else if (id == "reverbMix")   reverbMix   = v;
}

//==============================================================================
void JC120AudioProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    currentSampleRate = sampleRate;

    std::fill (std::begin (delayBufL), std::end (delayBufL), 0.f);
    std::fill (std::begin (delayBufR), std::end (delayBufR), 0.f);
    writePos = 0;
    lfoPhase = 0.f;

    for (int ch = 0; ch < 2; ++ch)
        bassLP_z1[ch] = trebleHP_z1[ch] = brightHP_z1[ch] = 0.f;

    std::fill (std::begin(comb1L), std::end(comb1L), 0.f);
    std::fill (std::begin(comb1R), std::end(comb1R), 0.f);
    std::fill (std::begin(comb2L), std::end(comb2L), 0.f);
    std::fill (std::begin(comb2R), std::end(comb2R), 0.f);
    std::fill (std::begin(comb3L), std::end(comb3L), 0.f);
    std::fill (std::begin(comb3R), std::end(comb3R), 0.f);
    std::fill (std::begin(comb4L), std::end(comb4L), 0.f);
    std::fill (std::begin(comb4R), std::end(comb4R), 0.f);
    std::fill (std::begin(ap1L), std::end(ap1L), 0.f);
    std::fill (std::begin(ap1R), std::end(ap1R), 0.f);
    std::fill (std::begin(ap2L), std::end(ap2L), 0.f);
    std::fill (std::begin(ap2R), std::end(ap2R), 0.f);
    c1=c2=c3=c4=a1=a2=0;
}

void JC120AudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JC120AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainInputChannelSet()  != juce::AudioChannelSet::stereo())
        return false;
    return true;
}
#endif

//==============================================================================
// DSP helpers
//==============================================================================
float JC120AudioProcessor::softClip (float x, float amt)
{
    const float gain = 1.0f + amt * 8.0f;
    float y = x * gain;
    y = juce::jlimit (-1.0f, 1.0f, y);
    return y - (y * y * y) / 3.0f;
}

float JC120AudioProcessor::onePoleLP (float x, float& z1, float cutoff)
{
    const float a = 1.0f - cutoff;
    z1 = z1 * a + x * (1.0f - a);
    return z1;
}

float JC120AudioProcessor::onePoleHP (float x, float& z1, float cutoff)
{
    return x - onePoleLP (x, z1, cutoff);
}

float JC120AudioProcessor::toneStack (float x, int ch)
{
    const float bv = bass.load();
    const float mv = mid.load();
    const float tv = treble.load();
    const float br = bright.load();

    const float bassLPout = onePoleLP (x, bassLP_z1[ch], 0.05f);
    const float bassOut   = x + bassLPout * (bv * 2.0f - 1.0f) * 0.8f;

    const float trebleHPout = onePoleHP (bassOut, trebleHP_z1[ch], 0.4f);
    const float trebleOut   = bassOut + trebleHPout * (tv * 2.0f - 1.0f) * 0.8f;

    const float midBoost = (mv - 0.5f) * 1.2f;
    const float midOut   = trebleOut * (1.0f + midBoost);

    if (br > 0.5f)
    {
        const float brightHPout = onePoleHP (midOut, brightHP_z1[ch], 0.7f);
        return midOut + brightHPout * 0.4f;
    }
    return midOut;
}

float JC120AudioProcessor::readDelayInterp (float* buf, float delaySamples)
{
    const int d    = (int) delaySamples;
    const float fr = delaySamples - (float) d;
    const int i0   = (writePos - d + MAX_DELAY) % MAX_DELAY;
    const int i1   = (i0 - 1 + MAX_DELAY) % MAX_DELAY;
    return buf[i0] * (1.0f - fr) + buf[i1] * fr;
}

float JC120AudioProcessor::combFilter (float x, float* buf, int& pos, int size, float fb)
{
    const float out = buf[pos];
    buf[pos] = x + out * fb;
    pos = (pos + 1) % size;
    return out;
}

float JC120AudioProcessor::allpass (float x, float* buf, int& pos, int size, float coeff)
{
    const float bufOut = buf[pos];
    const float v = x + bufOut * coeff;
    buf[pos] = v;
    pos = (pos + 1) % size;
    return bufOut - v * coeff;
}

void JC120AudioProcessor::processReverb (float inL, float inR,
                                          float& outL, float& outR)
{
    const float fb   = 0.82f;
    const float mono = (inL + inR) * 0.5f;

    const float c1l = combFilter (mono, comb1L, c1, COMB1, fb);
    const float c2l = combFilter (mono, comb2L, c2, COMB2, fb);
    const float c3l = combFilter (mono, comb3L, c3, COMB3, fb);
    const float c4l = combFilter (mono, comb4L, c4, COMB4, fb);

    const float c1r = combFilter (mono, comb1R, c1, COMB1, fb);
    const float c2r = combFilter (mono, comb2R, c2, COMB2, fb);
    const float c3r = combFilter (mono, comb3R, c3, COMB3, fb);
    const float c4r = combFilter (mono, comb4R, c4, COMB4, fb);

    const float sumL = (c1l + c2l + c3l + c4l) * 0.25f;
    const float sumR = (c1r + c2r + c3r + c4r) * 0.25f;

    outL = allpass (sumL, ap1L, a1, AP1, 0.5f);
    outR = allpass (sumR, ap2R, a2, AP2, 0.5f);
}

//==============================================================================
void JC120AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                         juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int   numSamples = buffer.getNumSamples();
    const float sr         = (float) currentSampleRate;

    float* dataL = buffer.getWritePointer (0);
    float* dataR = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : dataL;

    const float vol   = volume.load();
    const float drv   = drive.load();
    const float rate  = chorusRate.load();
    const float depth = chorusDepth.load();
    const float cMix  = chorusMix.load();
    const float rMix  = reverbMix.load();

    const float centreDelay = sr * 0.007f;
    const float modRange    = sr * 0.003f;
    const float lfoInc      = rate / sr;

    for (int i = 0; i < numSamples; ++i)
    {
        const float inMono  = (dataL[i] + dataR[i]) * 0.5f;
        const float toned   = toneStack (inMono, 0);
        const float clipped = softClip (toned, drv);

        delayBufL[writePos] = clipped;
        delayBufR[writePos] = clipped;

        lfoPhase += lfoInc;
        if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
        const float lfoSin = std::sin (lfoPhase * juce::MathConstants<float>::twoPi);

        const float delayL = centreDelay + lfoSin * modRange * depth;
        const float delayR = centreDelay - lfoSin * modRange * depth;

        const float wetL = readDelayInterp (delayBufL, delayL);
        const float wetR = readDelayInterp (delayBufR, delayR);

        const float chorusL = clipped * (1.0f - cMix) + wetL * cMix;
        const float chorusR = clipped * (1.0f - cMix) + wetR * cMix;

        float revL = 0.f, revR = 0.f;
        processReverb (chorusL, chorusR, revL, revR);

        dataL[i] = (chorusL * (1.0f - rMix) + revL * rMix) * vol;
        dataR[i] = (chorusR * (1.0f - rMix) + revR * rMix) * vol;

        writePos = (writePos + 1) % MAX_DELAY;
    }
}

//==============================================================================
void JC120AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void JC120AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
juce::AudioProcessorEditor* JC120AudioProcessor::createEditor()
{
    return new JC120AudioProcessorEditor (*this);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JC120AudioProcessor();
}
