/*
  ==============================================================================

    Styling.h
    Created: 4 May 2023 4:44:11pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class PoppinsLookAndFeel : public juce::LookAndFeel_V4
{
    juce::Typeface::Ptr regularTypeface = juce::Typeface::createSystemTypefaceFor (BinaryData::PoppinsRegular_ttf, BinaryData::PoppinsRegular_ttfSize);
    juce::Typeface::Ptr  boldTypeface = juce::Typeface::createSystemTypefaceFor (BinaryData::PoppinsBold_ttf, BinaryData::PoppinsBold_ttfSize);
    juce::Typeface::Ptr mediumTypeFace = juce::Typeface::createSystemTypefaceFor (BinaryData::PoppinsMedium_ttf, BinaryData::PoppinsMedium_ttfSize);
    
public:
    
    PoppinsLookAndFeel()
    {
        
    }
    
    juce::Typeface::Ptr getBoldTypeface()
    {
        return boldTypeface;
    }
    
    juce::Typeface::Ptr getRegularTypeface()
    {
        return regularTypeface;
    }
    
    juce::Typeface::Ptr getMediumTypeFace()
    {
        return mediumTypeFace;
    }
};

class QuilioLoginLookAndFeel : public PoppinsLookAndFeel
{
public:
    
    
    QuilioLoginLookAndFeel()
    {
        setColour (juce::TextEditor::backgroundColourId, juce::Colour::fromString ("#ff000A1A"));
        
        setColour (juce::TextEditor::outlineColourId, juce::Colour::fromString ("#ff9C49E7"));
        setColour (juce::TextEditor::focusedOutlineColourId, juce::Colour::fromString ("#ff9C49E7"));
        
        setColour (juce::TextEditor::textColourId, juce::Colour::fromString ("#ffEFEFEF"));

        setColour (juce::TextButton::buttonColourId, juce::Colour::fromString ("#ff9C49E7"));
        setColour (juce::TextButton::textColourOffId, juce::Colour (239, 239, 239));
        setColour (juce::TextButton::buttonOnColourId, juce::Colour::fromString ("#ffC69BF1"));
        setColour (juce::TextButton::textColourOnId, juce::Colour (239, 239, 239));
        
        setColour (juce::ComboBox::outlineColourId, juce::Colour::fromString ("#ff9C49E7"));
        
  //      juce::Font poppinsRegularFont (poppinsRegularTypeface);
        setDefaultSansSerifTypeface (getRegularTypeface());
    }
    
    void drawButtonBackground (juce::Graphics& g,
                               juce::Button& button,
                               const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override
    {
        auto cornerSize = 6.0f;
        auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

        auto baseColour = backgroundColour;//.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                          //.withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);
        
        
        // button states

        g.setColour (baseColour);

        auto flatOnLeft   = button.isConnectedOnLeft();
        auto flatOnRight  = button.isConnectedOnRight();
        auto flatOnTop    = button.isConnectedOnTop();
        auto flatOnBottom = button.isConnectedOnBottom();

        if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
        {
            juce::Path path;
//            path.addRoundedRectangle (bounds.getX(), bounds.getY(),
//                                      bounds.getWidth(), bounds.getHeight(),
//                                      cornerSize, cornerSize,
//                                      ! (flatOnLeft  || flatOnTop),
//                                      ! (flatOnRight || flatOnTop),
//                                      ! (flatOnLeft  || flatOnBottom),
//                                      ! (flatOnRight || flatOnBottom));

//            g.fillPath (path);

//            g.setColour (button.findColour (juce::ComboBox::outlineColourId));
//            g.strokePath (path, juce::PathStrokeType (1.0f));
            

        }
        else
        {
            g.fillRoundedRectangle (bounds, cornerSize);
        }
        
        juce::Colour hoverAndDownColour = juce::Colour (156, 73, 231);
        juce::Colour hoverOutlineColour = juce::Colour (239, 239, 239);
        
        if (shouldDrawButtonAsDown)
        {
            // add blur
        }
        else if (shouldDrawButtonAsHighlighted)
        {
            baseColour = hoverAndDownColour;
            g.setColour (hoverOutlineColour.withAlpha (0.35f));
            g.drawRoundedRectangle (bounds, cornerSize, 2);
        }
    }
    
    void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::Font font (getRegularTypeface());
        
        if (shouldDrawButtonAsDown)
        {
            font = getMediumTypeFace();
        }
        
        g.setFont (font);
        g.setFont (30.0f);
        
        g.setColour (button.findColour (shouldDrawButtonAsDown ? juce::TextButton::textColourOnId : juce::TextButton::textColourOffId));

        const int yIndent = fmin (4, button.proportionOfHeight (0.3f));
        const int cornerSize = fmin (button.getHeight(), button.getWidth()) / 2;
        const int fontHeight = juce::roundToInt (font.getHeight());
        const int leftIndent  = fmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        const int rightIndent = fmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        const int textWidth = button.getWidth() - leftIndent - rightIndent;

        if (textWidth > 0)
            g.drawFittedText (button.getButtonText(),
                              leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                              juce::Justification::centred, 2);
    }
    
    void fillTextEditorBackground (juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override
    {
        if (dynamic_cast<juce::AlertWindow*> (textEditor.getParentComponent()) != nullptr)
        {
            g.setColour (textEditor.findColour (juce::TextEditor::backgroundColourId));
            g.fillRect (0, 0, width, height);

            g.setColour (textEditor.findColour (juce::TextEditor::outlineColourId));
            g.drawHorizontalLine (height - 1, 0.0f, static_cast<float> (width));
        }
        else
        {
            juce::LookAndFeel_V2::fillTextEditorBackground (g, width, height, textEditor);
        }
    }
    
    void drawTextEditorOutline (juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override
    {
        if (dynamic_cast<juce::AlertWindow*> (textEditor.getParentComponent()) == nullptr)
        {
            if (textEditor.isEnabled())
            {
                if (textEditor.hasKeyboardFocus (true) && ! textEditor.isReadOnly())
                {
                    g.setColour (textEditor.findColour (juce::TextEditor::focusedOutlineColourId));
                    g.drawRoundedRectangle (0, 0, width, height, 4, 1);
                }
                else
                {
                    g.setColour (textEditor.findColour (juce::TextEditor::outlineColourId));
                    g.drawRoundedRectangle (0, 0, width, height, 4, 1);
                }
            }
        }
    }
};


class RoundedFillButtonStyling : public PoppinsLookAndFeel
{
public:
    
    RoundedFillButtonStyling()
    {
        setColour (juce::TextEditor::backgroundColourId, juce::Colour::fromString ("#ff000A1A"));
        setColour (juce::TextEditor::outlineColourId, juce::Colour::fromString ("#ff9C49E7"));
        setColour (juce::TextEditor::textColourId, juce::Colour (239, 239, 239));
        setColour (juce::TextButton::buttonColourId, juce::Colour::fromString ("#ff9C49E7"));
        setColour (juce::TextButton::textColourOffId, juce::Colour (239, 239, 239));
        setColour (juce::TextButton::buttonOnColourId, juce::Colour::fromString ("#ffC69BF1"));
        setColour (juce::TextButton::textColourOnId, juce::Colour (239, 239, 239));
        
        setColour (juce::ComboBox::outlineColourId, juce::Colour::fromString ("#ff9C49E7"));
    }
    
    void drawButtonBackground (juce::Graphics& g,
                               juce::Button& button,
                               const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override
    {
        auto cornerSize = 6.0f;
        auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

        auto baseColour = juce::Colour (156, 73, 231);//.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                          //.withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

        g.setColour (baseColour);

        auto flatOnLeft   = button.isConnectedOnLeft();
        auto flatOnRight  = button.isConnectedOnRight();
        auto flatOnTop    = button.isConnectedOnTop();
        auto flatOnBottom = button.isConnectedOnBottom();
        
        juce::Colour downStateColour = juce::Colour (191, 191, 191);

        g.fillRoundedRectangle (bounds, cornerSize);

        
        if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
        {
            juce::Path path;
            path.addRoundedRectangle (bounds.getX(), bounds.getY(),
                                      bounds.getWidth(), bounds.getHeight(),
                                      cornerSize, cornerSize,
                                      ! (flatOnLeft  || flatOnTop),
                                      ! (flatOnRight || flatOnTop),
                                      ! (flatOnLeft  || flatOnBottom),
                                      ! (flatOnRight || flatOnBottom));

            g.fillPath (path);

            g.setColour (button.findColour (juce::ComboBox::outlineColourId));
            g.strokePath (path, juce::PathStrokeType (1.0f));
        }
        else
        {
//            g.fillRoundedRectangle (bounds, cornerSize);
            juce::Colour hoverColour = juce::Colour (239, 239, 239);
            if (shouldDrawButtonAsDown)
            {
            // add shadows
            }
            else if (shouldDrawButtonAsHighlighted)
            {
                // downstate currently does not register
                g.setColour (hoverColour.withAlpha (0.35f));
                g.drawRoundedRectangle (bounds, cornerSize, 2.0f);
            }
// TODO:    Add a disabled state
//            g.setColour (juce::Colours::transparentBlack);
        }
    }
    
    void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::Font font (getRegularTypeface());
        
        if (shouldDrawButtonAsDown)
        {
            font = getMediumTypeFace();
        }
        
        g.setFont (font);
        g.setFont (30.0f);
        
        g.setColour (button.findColour (shouldDrawButtonAsDown ? juce::TextButton::textColourOnId : juce::TextButton::textColourOffId));

        const int yIndent = fmin (4, button.proportionOfHeight (0.3f));
        const int cornerSize = fmin (button.getHeight(), button.getWidth()) / 2;
        const int fontHeight = juce::roundToInt (font.getHeight());
        const int leftIndent  = fmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        const int rightIndent = fmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        const int textWidth = button.getWidth() - leftIndent - rightIndent;

        if (textWidth > 0)
            g.drawFittedText (button.getButtonText(),
                              leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                              juce::Justification::centred, 2);
    }

    
    void fillTextEditorBackground (juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override
    {
        if (dynamic_cast<juce::AlertWindow*> (textEditor.getParentComponent()) != nullptr)
        {
            g.setColour (textEditor.findColour (juce::TextEditor::backgroundColourId));
            g.fillRect (0, 0, width, height);

            g.setColour (textEditor.findColour (juce::TextEditor::outlineColourId));
            g.drawHorizontalLine (height - 1, 0.0f, static_cast<float> (width));
        }
        else
        {
            juce::LookAndFeel_V2::fillTextEditorBackground (g, width, height, textEditor);
        }
    }
    
    void drawTextEditorOutline (juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override
    {
        if (dynamic_cast<juce::AlertWindow*> (textEditor.getParentComponent()) == nullptr)
        {
            if (textEditor.isEnabled())
            {
                if (textEditor.hasKeyboardFocus (true) && ! textEditor.isReadOnly())
                {
                    g.setColour (textEditor.findColour (juce::TextEditor::focusedOutlineColourId));
                    g.drawRoundedRectangle (0, 0, width, height, 4, 1);
                }
                else
                {
                    g.setColour (textEditor.findColour (juce::TextEditor::outlineColourId));
                    g.drawRoundedRectangle (0, 0, width, height, 4, 1);
                }
            }
        }
    }
};

class RoundedOutlineButtonStyling : public PoppinsLookAndFeel
{
public:
    
    RoundedOutlineButtonStyling()
    {
        setColour (juce::TextEditor::backgroundColourId, juce::Colour::fromString ("#ff000A1A"));
        
        setColour (juce::TextEditor::outlineColourId, juce::Colour::fromString ("#ff9C49E7"));
        setColour (juce::TextEditor::focusedOutlineColourId, juce::Colour::fromString ("#ff9C49E7"));
        
        setColour (juce::TextEditor::textColourId, juce::Colour::fromString ("#ffEFEFEF"));

        
        setColour (juce::TextButton::buttonColourId, juce::Colour (156, 73, 231));
        setColour (juce::TextButton::textColourOffId, juce::Colour::fromString ("#ffEFEFEF"));
        setColour (juce::TextButton::buttonOnColourId, juce::Colour (156, 73, 231));
        setColour (juce::TextButton::textColourOnId, juce::Colour (191, 191, 191));

        setColour (juce::ComboBox::outlineColourId, juce::Colour::fromString ("#ff9C49E7"));
    }
    
    void drawButtonBackground (juce::Graphics& g,
                               juce::Button& button,
                               const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override
    {
        auto cornerSize = 6.0f;
        auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

        auto baseColour = backgroundColour;//.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                          //.withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

        

        auto flatOnLeft   = button.isConnectedOnLeft();
        auto flatOnRight  = button.isConnectedOnRight();
        auto flatOnTop    = button.isConnectedOnTop();
        auto flatOnBottom = button.isConnectedOnBottom();

        float lineThickness = 1.0f;
        juce::Colour fontColour = juce::Colour (239, 239, 239);
        
       // if button is disabled:
       
        
        if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
        {
            juce::Path path;
            path.addRoundedRectangle (bounds.getX(), bounds.getY(),
                                      bounds.getWidth(), bounds.getHeight(),
                                      cornerSize, cornerSize,
                                      ! (flatOnLeft  || flatOnTop),
                                      ! (flatOnRight || flatOnTop),
                                      ! (flatOnLeft  || flatOnBottom),
                                      ! (flatOnRight || flatOnBottom));

            g.fillPath (path);

            g.setColour (button.findColour (juce::ComboBox::outlineColourId));
            g.strokePath (path, juce::PathStrokeType (1.0f));
        }

        else
        {
            if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            {
                lineThickness = 2.0f;
            }
            setColour (juce::TextButton::textColourOffId, fontColour);
// TODO:    Add a disabled state

            g.setColour (button.findColour (juce::ComboBox::outlineColourId));
            // if disabled:
//            g.setOpacity (0.35f);
            g.drawRoundedRectangle (bounds, cornerSize, lineThickness);
            // reset opacity
            // g.setOpacity (1.0f);
        }
    }
    
    
    void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::Font font (getRegularTypeface());
        
        if (shouldDrawButtonAsDown)
        {
            font = getMediumTypeFace();
        }
        
        g.setFont (font);
        g.setFont (30.0f);
        
        g.setColour (button.findColour (shouldDrawButtonAsDown ? juce::TextButton::textColourOnId : juce::TextButton::textColourOffId));

        const int yIndent = fmin (4, button.proportionOfHeight (0.3f));
        const int cornerSize = fmin (button.getHeight(), button.getWidth()) / 2;
        const int fontHeight = juce::roundToInt (font.getHeight());
        const int leftIndent  = fmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        const int rightIndent = fmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        const int textWidth = button.getWidth() - leftIndent - rightIndent;

        if (textWidth > 0)
            g.drawFittedText (button.getButtonText(),
                              leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                              juce::Justification::centred, 2);
    }
};

class SignOutStopClearButtonStyling : public PoppinsLookAndFeel
{
public:
    
    SignOutStopClearButtonStyling()
    {
        setColour (juce::TextEditor::backgroundColourId, juce::Colour::fromString ("#ff000A1A"));
        
        setColour (juce::TextEditor::outlineColourId, juce::Colour::fromString ("#ff9C49E7"));
        setColour (juce::TextEditor::focusedOutlineColourId, juce::Colour::fromString ("#ff9C49E7"));
        
        setColour (juce::TextEditor::textColourId, juce::Colour::fromString ("#ffEFEFEF"));

        
        setColour (juce::TextButton::buttonColourId, juce::Colour::fromString ("#ff9C49E7"));
        setColour (juce::TextButton::textColourOffId, juce::Colour::fromString ("#ffEFEFEF"));
        setColour (juce::TextButton::buttonOnColourId, juce::Colour::fromString ("#ffC69BF1"));
        setColour (juce::TextButton::textColourOnId, juce::Colour::fromString ("#ffC69BF1"));
        
        setColour (juce::ComboBox::outlineColourId, juce::Colour::fromString ("#ff9C49E7"));
    }
    
    void drawButtonBackground (juce::Graphics& g,
                               juce::Button& button,
                               const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override
    {
        auto cornerSize = 6.0f;
        auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

        auto baseColour = backgroundColour;//.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                          //.withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

        

        auto flatOnLeft   = button.isConnectedOnLeft();
        auto flatOnRight  = button.isConnectedOnRight();
        auto flatOnTop    = button.isConnectedOnTop();
        auto flatOnBottom = button.isConnectedOnBottom();

        float lineThickness;
        juce::Colour fontColour = juce::Colour (239, 239, 239);
        
        if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
        {
            juce::Path path;
            path.addRoundedRectangle (bounds.getX(), bounds.getY(),
                                      bounds.getWidth(), bounds.getHeight(),
                                      cornerSize, cornerSize,
                                      ! (flatOnLeft  || flatOnTop),
                                      ! (flatOnRight || flatOnTop),
                                      ! (flatOnLeft  || flatOnBottom),
                                      ! (flatOnRight || flatOnBottom));

            g.fillPath (path);

            g.setColour (button.findColour (juce::ComboBox::outlineColourId));
            g.strokePath (path, juce::PathStrokeType (1.0f));
        }

        else
        {
            if (!shouldDrawButtonAsDown)
            {
                lineThickness = (shouldDrawButtonAsHighlighted ? 2.0f : 1.0f);
                // downstate currently does not register
            }
            
            if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            {
                juce::Colour hoverFillColour = juce::Colour (242, 87, 29).withAlpha (0.20f);
                g.setColour (hoverFillColour);
                // if disabled:
    //            g.setOpacity (0.35f);
                g.fillRoundedRectangle (bounds, cornerSize);
            }
            setColour (juce::TextButton::textColourOffId, fontColour);
// TODO:    Add a disabled state
        

            g.setColour (button.findColour (juce::ComboBox::outlineColourId));
            g.drawRoundedRectangle (bounds, cornerSize, lineThickness);
        }
    }
    
    void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::Font font (getRegularTypeface());
        
        if (shouldDrawButtonAsDown)
        {
            font = getMediumTypeFace();
        }
        
        g.setFont (font);
        g.setFont (30.0f);
        
        g.setColour (button.findColour (juce::TextButton::textColourOnId));

        const int yIndent = fmin (4, button.proportionOfHeight (0.3f));
        const int cornerSize = fmin (button.getHeight(), button.getWidth()) / 2;
        const int fontHeight = juce::roundToInt (font.getHeight());
        const int leftIndent  = fmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        const int rightIndent = fmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        const int textWidth = button.getWidth() - leftIndent - rightIndent;

        if (textWidth > 0)
            g.drawFittedText (button.getButtonText(),
                              leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                              juce::Justification::centred, 2);
    }
};


class UnderlinedButtonStyling : public PoppinsLookAndFeel
{
public:
    
    UnderlinedButtonStyling()
    {
        setColour (juce::TextEditor::backgroundColourId, juce::Colour::fromString ("#ff000A1A"));
        
        setColour (juce::TextEditor::outlineColourId, juce::Colour::fromString ("#ff9C49E7"));
        setColour (juce::TextEditor::focusedOutlineColourId, juce::Colour::fromString ("#ff9C49E7"));
        
        setColour (juce::TextEditor::textColourId, juce::Colour::fromString ("#ffEFEFEF"));

        
        setColour (juce::TextButton::buttonColourId, juce::Colour::fromString ("#ff9C49E7"));
        setColour (juce::TextButton::textColourOffId, juce::Colour::fromString ("#ffEFEFEF"));
        setColour (juce::TextButton::buttonOnColourId, juce::Colour::fromString ("#ffC69BF1"));
        setColour (juce::TextButton::textColourOnId, juce::Colour::fromString ("#ffC69BF1"));
        
        setColour (juce::ComboBox::outlineColourId, juce::Colour::fromString ("#ff9C49E7"));
        
        setDefaultSansSerifTypeface (getBoldTypeface());
    }
    
    void drawButtonBackground (juce::Graphics& g,
                               juce::Button& button,
                               const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override
    {
        auto cornerSize = 6.0f;
        auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

        auto baseColour = backgroundColour;//.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                          //.withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.contrasting (shouldDrawButtonAsDown ? 0.2f : 0.05f);

        g.setColour (baseColour);

        auto flatOnLeft   = button.isConnectedOnLeft();
        auto flatOnRight  = button.isConnectedOnRight();
        auto flatOnTop    = button.isConnectedOnTop();
        auto flatOnBottom = button.isConnectedOnBottom();

        if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
        {
            juce::Path path;
            path.addRoundedRectangle (bounds.getX(), bounds.getY(),
                                      bounds.getWidth(), bounds.getHeight(),
                                      cornerSize, cornerSize,
                                      ! (flatOnLeft  || flatOnTop),
                                      ! (flatOnRight || flatOnTop),
                                      ! (flatOnLeft  || flatOnBottom),
                                      ! (flatOnRight || flatOnBottom));

            g.fillPath (path);

            g.setColour (button.findColour (juce::ComboBox::outlineColourId));
            g.strokePath (path, juce::PathStrokeType (1.0f));
        }
        else
        {
            if (button.getToggleState())
                drawCenteredLine (g, 100, 3.0, juce::Colours::purple, button);
        }
    }
    
    void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                         bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/) override
    {
        juce::Font font;
        if (button.getToggleState ()) //if button is down
        {
            font = juce::Font (getBoldTypeface());
            g.setFont (font);
            g.setFont (24.0f);
            g.setColour (button.findColour (button.getToggleState() ? juce::TextButton::textColourOnId
                                                                    : juce::TextButton::textColourOffId)
                               .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
        }
        else
        {
            font = juce::Font (getRegularTypeface());
            g.setFont (font);
            g.setFont (24.0f);
            g.setColour (button.findColour (button.getToggleState() ? juce::TextButton::textColourOnId
                                                                    : juce::TextButton::textColourOffId)
                               .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
        }
        


        const int yIndent = juce::jmin (4, button.proportionOfHeight (0.3f));
        const int cornerSize = juce::jmin (button.getHeight(), button.getWidth()) / 2;

        const int fontHeight = juce::roundToInt (font.getHeight() * 0.6f);
        const int leftIndent  = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        const int rightIndent = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        const int textWidth = button.getWidth() - leftIndent - rightIndent;

        g.setColour (juce::Colours::white);
        if (textWidth > 0)
            g.drawFittedText (button.getButtonText(),
                              leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                              juce::Justification::centred, 2);
    }
    
    void drawCenteredLine (juce::Graphics& g, float length, float thickness, juce::Colour colour, juce::Component& component)
    {
        int y = component.getHeight() - thickness / 2;
        int x1 = (component.getWidth() - length) / 2;
        int x2 = x1 + length;

        g.setColour (colour);
        g.drawLine (x1, y, x2, y, thickness);
    }
};

class BackButton : public juce::ImageButton
{
    
public:
    BackButton() {}
    
};


