/*
  ==============================================================================

    Components.h
    Created: 4 May 2023 4:42:51pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once
#include "../Styles/Styling.h"

//Inline Helpers
inline void setImages (juce::ImageButton &button, juce::Image& off, juce::Image& on)
{
    button.setImages (true, true, true, off, 1.0f, {}, off, 1.0f, {}, on, 1.0f, {});
}

inline void setImages (juce::ImageButton &button, juce::Image& singleImage)
{
    button.setImages (true, true, true, singleImage, 1.0f, {}, singleImage, 1.0f, {}, singleImage, 1.0f, {});
}

//Use this to implement any behavior that seems to be shared across screens
class Screen : public juce::Component
{
public:
    
    Screen()
    {
        
    }
    
    ~Screen()
    {
        setLookAndFeel (nullptr);
    }
    
    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour::fromString ("#ff000A1A"));
    }
};


