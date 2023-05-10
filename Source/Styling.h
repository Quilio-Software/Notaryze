/*
  ==============================================================================

    Styling.h
    Created: 4 May 2023 4:44:11pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class QuilioLoginLookAndFeel : public juce::LookAndFeel_V4
{
public:
    
    QuilioLoginLookAndFeel()
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
            g.fillRoundedRectangle (bounds, cornerSize);

            g.setColour (button.findColour (juce::ComboBox::outlineColourId));
            g.drawRoundedRectangle (bounds, cornerSize, 1.0f);
        }
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
