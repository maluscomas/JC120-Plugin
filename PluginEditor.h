#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class JC120LookAndFeel;

//==============================================================================
class JC120AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    explicit JC120AudioProcessorEditor (JC120AudioProcessor&);
    ~JC120AudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JC120AudioProcessor& processorRef;
    std::unique_ptr<JC120LookAndFeel> jcLookAndFeel;

    juce::Slider volumeSlider, bassSlider, midSlider, trebleSlider;
    juce::Slider brightSlider, driveSlider;
    juce::Slider chorusRateSlider, chorusDepthSlider, chorusMixSlider, reverbMixSlider;

    juce::Label volumeLabel, bassLabel, midLabel, trebleLabel;
    juce::Label brightLabel, driveLabel;
    juce::Label chorusRateLabel, chorusDepthLabel, chorusMixLabel, reverbMixLabel;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> volumeAtt, bassAtt, midAtt, trebleAtt;
    std::unique_ptr<Attachment> brightAtt, driveAtt;
    std::unique_ptr<Attachment> chorusRateAtt, chorusDepthAtt, chorusMixAtt, reverbMixAtt;

    void setupKnob (juce::Slider& s, juce::Label& l, const juce::String& name);
    void drawSectionPanel (juce::Graphics& g, int x, int y, int w, int h, const juce::String& title);
    void drawInputJack (juce::Graphics& g, float cx, float cy);
    void drawIndicatorLED (juce::Graphics& g, float cx, float cy, bool on);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JC120AudioProcessorEditor)
};
