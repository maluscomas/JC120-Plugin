#include "PluginEditor.h"

//==============================================================================
// Chicken-head style black knob — matches real JC-120
//==============================================================================
class JC120LookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override
    {
        const float radius  = (float) juce::jmin (width / 2, height / 2) - 4.0f;
        const float cx      = (float) x + width  * 0.5f;
        const float cy      = (float) y + height * 0.5f;
        const float angle   = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // ── Outer silver ring (like real JC-120 knob collar) ──────────────────
        juce::ColourGradient ringGrad (
            juce::Colour (0xffe0e0e0), cx - radius, cy - radius,
            juce::Colour (0xff888888), cx + radius, cy + radius, false);
        g.setGradientFill (ringGrad);
        g.fillEllipse (cx - radius - 5, cy - radius - 5, (radius+5)*2, (radius+5)*2);

        // ── Tick marks on silver collar ───────────────────────────────────────
        g.setColour (juce::Colour (0xff333333));
        const int numTicks = 11;
        for (int i = 0; i < numTicks; ++i)
        {
            const float ta  = rotaryStartAngle + (float)i/(float)(numTicks-1)
                              * (rotaryEndAngle - rotaryStartAngle);
            const float ti  = radius + 1.0f;
            const float to  = radius + 5.0f;
            const float lw  = (i == 0 || i == numTicks-1 || i == (numTicks/2)) ? 2.0f : 1.0f;
            g.drawLine (cx + std::sin(ta)*ti, cy - std::cos(ta)*ti,
                        cx + std::sin(ta)*to, cy - std::cos(ta)*to, lw);
        }

        // ── Black knob body ───────────────────────────────────────────────────
        juce::ColourGradient bodyGrad (
            juce::Colour (0xff3a3a3a), cx - radius*0.4f, cy - radius*0.4f,
            juce::Colour (0xff111111), cx + radius*0.5f, cy + radius*0.5f, false);
        g.setGradientFill (bodyGrad);
        g.fillEllipse (cx - radius, cy - radius, radius*2, radius*2);

        // ── Knob edge gloss ───────────────────────────────────────────────────
        g.setColour (juce::Colour (0xff555555));
        g.drawEllipse (cx - radius, cy - radius, radius*2, radius*2, 1.0f);

        // ── Chicken-head pointer (white teardrop shape) ───────────────────────
        juce::Path ptr;
        ptr.addRectangle (-2.0f, -radius + 3.0f, 4.0f, radius * 0.6f);
        // Triangular tip at bottom of pointer
        ptr.addTriangle (-4.0f, -radius + 3.0f,
                          4.0f, -radius + 3.0f,
                          0.0f, -radius - 2.0f);
        ptr.applyTransform (juce::AffineTransform::rotation (angle).translated (cx, cy));
        g.setColour (juce::Colour (0xfff0f0f0));
        g.fillPath (ptr);

        // ── Orange indicator dot (like real JC-120 panel markings) ───────────
        const float dotDist = radius - 6.0f;
        const float dotX = cx + std::sin(angle) * dotDist;
        const float dotY = cy - std::cos(angle) * dotDist;
        g.setColour (juce::Colour (0xffff8800));
        g.fillEllipse (dotX - 2.5f, dotY - 2.5f, 5.0f, 5.0f);
    }
};

//==============================================================================
JC120AudioProcessorEditor::JC120AudioProcessorEditor (JC120AudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
      jcLookAndFeel (std::make_unique<JC120LookAndFeel>())
{
    juce::LookAndFeel::setDefaultLookAndFeel (jcLookAndFeel.get());

    setupKnob (volumeSlider,      volumeLabel,      "VOLUME");
    setupKnob (driveSlider,       driveLabel,       "DRIVE");
    setupKnob (brightSlider,      brightLabel,      "BRIGHT");
    setupKnob (bassSlider,        bassLabel,        "BASS");
    setupKnob (midSlider,         midLabel,         "MID");
    setupKnob (trebleSlider,      trebleLabel,      "TREBLE");
    setupKnob (chorusRateSlider,  chorusRateLabel,  "RATE");
    setupKnob (chorusDepthSlider, chorusDepthLabel, "DEPTH");
    setupKnob (chorusMixSlider,   chorusMixLabel,   "MIX");
    setupKnob (reverbMixSlider,   reverbMixLabel,   "REVERB");

    auto& apvts = processorRef.apvts;
    volumeAtt      = std::make_unique<Attachment> (apvts, "volume",      volumeSlider);
    bassAtt        = std::make_unique<Attachment> (apvts, "bass",        bassSlider);
    midAtt         = std::make_unique<Attachment> (apvts, "mid",         midSlider);
    trebleAtt      = std::make_unique<Attachment> (apvts, "treble",      trebleSlider);
    brightAtt      = std::make_unique<Attachment> (apvts, "bright",      brightSlider);
    driveAtt       = std::make_unique<Attachment> (apvts, "drive",       driveSlider);
    chorusRateAtt  = std::make_unique<Attachment> (apvts, "chorusRate",  chorusRateSlider);
    chorusDepthAtt = std::make_unique<Attachment> (apvts, "chorusDepth", chorusDepthSlider);
    chorusMixAtt   = std::make_unique<Attachment> (apvts, "chorusMix",   chorusMixSlider);
    reverbMixAtt   = std::make_unique<Attachment> (apvts, "reverbMix",   reverbMixSlider);

    setSize (860, 240);
}

JC120AudioProcessorEditor::~JC120AudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel (nullptr);
}

//==============================================================================
void JC120AudioProcessorEditor::setupKnob (juce::Slider& s, juce::Label& l,
                                            const juce::String& name)
{
    s.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    s.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (s);

    l.setText (name, juce::dontSendNotification);
    l.setJustificationType (juce::Justification::centred);
    l.setFont (juce::Font (juce::FontOptions().withHeight (8.0f).withStyle ("Bold")));
    l.setColour (juce::Label::textColourId, juce::Colour (0xff222222));
    addAndMakeVisible (l);
}

//==============================================================================
void JC120AudioProcessorEditor::drawSectionPanel (juce::Graphics& g,
    int x, int y, int w, int h, const juce::String& title)
{
    // Recessed panel background
    juce::ColourGradient panelGrad (
        juce::Colour (0xffb8bec8), (float)x, (float)y,
        juce::Colour (0xffa0a8b8), (float)x, (float)(y+h), false);
    g.setGradientFill (panelGrad);
    g.fillRoundedRectangle ((float)x, (float)y, (float)w, (float)h, 4.0f);

    // Panel border
    g.setColour (juce::Colour (0xff707888));
    g.drawRoundedRectangle ((float)x, (float)y, (float)w, (float)h, 4.0f, 1.0f);

    // Section title
    if (title.isNotEmpty())
    {
        g.setColour (juce::Colour (0xff111111));
        g.setFont (juce::Font (juce::FontOptions().withHeight (8.5f).withStyle ("Bold")));
        g.drawText (title, x, y + 4, w, 12, juce::Justification::centred);

        // Underline
        g.setColour (juce::Colour (0xff444444));
        g.fillRect (x + 8, y + 17, w - 16, 1);
    }
}

void JC120AudioProcessorEditor::drawInputJack (juce::Graphics& g, float cx, float cy)
{
    // Jack socket
    g.setColour (juce::Colour (0xff111111));
    g.fillEllipse (cx - 7, cy - 7, 14, 14);
    g.setColour (juce::Colour (0xff555555));
    g.drawEllipse (cx - 7, cy - 7, 14, 14, 1.5f);
    // Centre pin
    g.setColour (juce::Colour (0xffaaaaaa));
    g.fillEllipse (cx - 2.5f, cy - 2.5f, 5, 5);
    // Label
    g.setColour (juce::Colour (0xff222222));
    g.setFont (juce::Font (juce::FontOptions().withHeight (7.0f)));
    g.drawText ("INPUT", (int)(cx - 14), (int)(cy + 9), 28, 10, juce::Justification::centred);
}

void JC120AudioProcessorEditor::drawIndicatorLED (juce::Graphics& g, float cx, float cy, bool on)
{
    // LED glow
    if (on)
    {
        g.setColour (juce::Colour (0x44ff6600));
        g.fillEllipse (cx - 6, cy - 6, 12, 12);
    }
    juce::ColourGradient ledGrad (
        on ? juce::Colour (0xffff9900) : juce::Colour (0xff553300),
        cx - 3, cy - 3,
        on ? juce::Colour (0xffcc5500) : juce::Colour (0xff221100),
        cx + 3, cy + 3, false);
    g.setGradientFill (ledGrad);
    g.fillEllipse (cx - 4, cy - 4, 8, 8);
    g.setColour (juce::Colours::black.withAlpha (0.5f));
    g.drawEllipse (cx - 4, cy - 4, 8, 8, 1.0f);
}

//==============================================================================
void JC120AudioProcessorEditor::paint (juce::Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight();

    // ── Main panel — silver/grey aluminium like real JC-120 ───────────────────
    juce::ColourGradient mainGrad (
        juce::Colour (0xffd8dce4), 0.0f, 0.0f,
        juce::Colour (0xffc0c4cc), 0.0f, (float)h, false);
    g.setGradientFill (mainGrad);
    g.fillRect (0, 0, w, h);

    // ── Brushed metal texture lines ───────────────────────────────────────────
    g.setColour (juce::Colours::white.withAlpha (0.08f));
    for (int i = 0; i < h; i += 3)
        g.drawHorizontalLine (i, 0, (float)w);

    // ── Top black rubber strip ────────────────────────────────────────────────
    juce::ColourGradient topStrip (
        juce::Colour (0xff2a2a2a), 0.0f, 0.0f,
        juce::Colour (0xff111111), 0.0f, 20.0f, false);
    g.setGradientFill (topStrip);
    g.fillRect (0, 0, w, 20);

    // ── Bottom black rubber strip ─────────────────────────────────────────────
    g.setGradientFill (topStrip);
    g.fillRect (0, h - 20, w, 20);

    // ── ROLAND logo block (black panel left) ──────────────────────────────────
    g.setColour (juce::Colour (0xff111111));
    g.fillRoundedRectangle (8.0f, 24.0f, 118.0f, h - 48.0f, 3.0f);

    // Roland red wordmark
    g.setColour (juce::Colour (0xffee1111));
    g.setFont (juce::Font (juce::FontOptions().withHeight (26.0f).withStyle ("Bold")));
    g.drawText ("ROLAND", 10, 32, 114, 32, juce::Justification::centred);

    // Model name
    g.setColour (juce::Colours::white);
    g.setFont (juce::Font (juce::FontOptions().withHeight (16.0f).withStyle ("Bold")));
    g.drawText ("JC-120", 10, 66, 114, 22, juce::Justification::centred);

    // Jazz Chorus
    g.setColour (juce::Colour (0xff999999));
    g.setFont (juce::Font (juce::FontOptions().withHeight (8.5f)));
    g.drawText ("JAZZ CHORUS", 10, 90, 114, 14, juce::Justification::centred);

    // LED indicators on logo panel
    drawIndicatorLED (g, 38, 130, true);   // power LED
    drawIndicatorLED (g, 96, 130, false);  // standby
    g.setColour (juce::Colour (0xff777777));
    g.setFont (juce::Font (juce::FontOptions().withHeight (7.0f)));
    g.drawText ("POWER", 24, 139, 28, 10, juce::Justification::centred);
    g.drawText ("STDBY", 82, 139, 28, 10, juce::Justification::centred);

    // Input jacks on logo panel
    drawInputJack (g, 38, 168);
    drawInputJack (g, 88, 168);

    // ── Section panels (recessed) ─────────────────────────────────────────────
    drawSectionPanel (g, 132, 24, 182, h-48, "MASTER");
    drawSectionPanel (g, 318, 24, 230, h-48, "EQUALIZER");
    drawSectionPanel (g, 552, 24, 180, h-48, "CHORUS");
    drawSectionPanel (g, 736, 24, 112, h-48, "REVERB");

    // ── Corner screws ─────────────────────────────────────────────────────────
    auto drawScrew = [&](float sx, float sy)
    {
        juce::ColourGradient sg (
            juce::Colour (0xffcccccc), sx-4, sy-4,
            juce::Colour (0xff666666), sx+4, sy+4, false);
        g.setGradientFill (sg);
        g.fillEllipse (sx-5, sy-5, 10, 10);
        g.setColour (juce::Colours::black.withAlpha (0.7f));
        // Phillips head
        g.drawLine (sx-3, sy, sx+3, sy, 1.2f);
        g.drawLine (sx, sy-3, sx, sy+3, 1.2f);
    };
    drawScrew (5,   5);
    drawScrew (w-5, 5);
    drawScrew (5,   h-5);
    drawScrew (w-5, h-5);
    drawScrew (w/2, 5);
    drawScrew (w/2, h-5);
}

//==============================================================================
void JC120AudioProcessorEditor::resized()
{
    const int knobSize = 58;
    const int labelH   = 12;
    const int knobY    = 75;
    const int labelY   = knobY + knobSize + 2;

    auto place = [&](juce::Slider& s, juce::Label& l, int cx)
    {
        s.setBounds (cx - knobSize/2, knobY, knobSize, knobSize);
        l.setBounds (cx - 32, labelY, 64, labelH);
    };

    // MASTER section (132–314): volume, drive, bright
    place (volumeSlider, volumeLabel, 175);
    place (driveSlider,  driveLabel,  231);
    place (brightSlider, brightLabel, 287);

    // EQUALIZER section (318–548): bass, mid, treble
    place (bassSlider,   bassLabel,   362);
    place (midSlider,    midLabel,    433);
    place (trebleSlider, trebleLabel, 504);

    // CHORUS section (552–732): rate, depth, mix
    place (chorusRateSlider,  chorusRateLabel,  592);
    place (chorusDepthSlider, chorusDepthLabel, 642);
    place (chorusMixSlider,   chorusMixLabel,   702);

    // REVERB section (736–848)
    place (reverbMixSlider, reverbMixLabel, 782);
}
