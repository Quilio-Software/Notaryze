/*
  ==============================================================================

    ProfileScreen.h
    Created: 4 May 2023 4:41:38pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once
#include "Components.h"

class ProfileScreen : public Screen
{
    std::unique_ptr<juce::DrawableButton> quilioLogoButton;
    juce::ImageButton profilePictureButton;
    juce::TextButton backButton {"Back"}, signOutButton {"Sign out"};
    
    juce::String name {"Abhishek Shivakumar"};
    juce::String email {"abhishek.shivakumar@gmail.com"};
    juce::String devID {"sdlkfjhsdlkfj"};
    
    
    juce::Label emailLabel, devIDLabel;
    
public:
    
    ProfileScreen()
    {
//        addAndMakeVisible (*quilioLogoButton);
        addAndMakeVisible (profilePictureButton);
        addAndMakeVisible (backButton);
        addAndMakeVisible (signOutButton);
        
        addAndMakeVisible (emailLabel);
        addAndMakeVisible (devIDLabel);
        
        emailLabel.setText ("Email:", juce::dontSendNotification);
        devIDLabel.setText ("Developer ID:", juce::dontSendNotification);
        emailLabel.setColour (juce::Label::textColourId, juce::Colour::fromString ("#ffBFBFBF"));
        devIDLabel.setColour (juce::Label::textColourId, juce::Colour::fromString ("#ffBFBFBF"));

        
        backButton.onClick = [&] { onBack(); };
        signOutButton.onClick = [&] {onSignOut(); };
    }
    
    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour::fromString ("#ff000A1A"));
        
        g.setFont (28);
        g.setColour (juce::Colour::fromString ("#ffEFEFEF"));
        g.drawText (name, 190, 232, 301, 42, juce::Justification::centred);
        
        g.setFont (16);
        g.setColour (juce::Colour::fromString ("#ff595959"));
        g.drawText (email, 307.5, 306, 206, 24, juce::Justification::left);
        g.drawText (devID, 307.5, 346, 206, 24, juce::Justification::left);
    }
    
    void setName (juce::String newName)
    {
        name = newName;
    }
    
    void setEmail (juce::String newEmail)
    {
        email = newEmail;
    }
    
    void setDevID (juce::String newDevID)
    {
        devID = newDevID;
    }
    
    void resized() override
    {
        profilePictureButton.setBounds (290, 100, 100, 100);
        backButton.setBounds (32, 482 + 32, 113, 54);
        signOutButton.setBounds (502, 432 + 32, 146, 54);
//        quilioLogoButton->setBounds (32, 30, 80, 40);
        
        emailLabel.setBounds (174.5, 304, 52, 24);
        devIDLabel.setBounds (174.5, 348, 109, 24);
    }
    
    
    std::function<void()> onBack = []{};
    std::function<void()> onSignOut = []{};
};
