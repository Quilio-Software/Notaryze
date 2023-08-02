/*
  ==============================================================================

    ProfileScreen.h
    Created: 4 May 2023 4:41:38pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once
#include "../Helpers/Components.h"

class ProfileScreen : public Screen
{
    RoundedOutlineButtonStyling roundedOutlineButtonStyle;
    SignOutStopClearButtonStyling signOutStopClearButtonStyling;

    std::unique_ptr<juce::DrawableButton> quilioLogoButton;
    juce::ImageButton profilePictureButton;
    juce::TextButton backButton {"Back"}, signOutButton {"Sign out"};
    
    juce::String name {"Abhishek Shivakumar"};
    juce::String email {"abhishek.shivakumar@gmail.com"};
    juce::String devID {"sdlkfjhsdlkfj"};
    
    juce::Label emailLabel, devIDLabel;
    
    juce::Image profilePicCircleImageDefault;
    

    
    juce::Image loadImage (const std::string& imageName)
    {
        std::string imageIdentifier = imageName + "_png";
        int imageSize;
        const char* imageData = reinterpret_cast<const char*>(BinaryData::getNamedResource (imageIdentifier.c_str(), imageSize));

        if (imageData != nullptr && imageSize > 0)
        {
            return juce::ImageFileFormat::loadFrom (imageData, imageSize);
        }
        else
        {
            // Handle error: Image not found or invalid image data
            return juce::Image(); // Return an empty image object
        }
    }
    
public:
    
    std::shared_ptr<ProfileData> profileData;
    void setProfileData (std::shared_ptr<ProfileData>& newProfileData) { profileData = newProfileData; }
    
    //TODO: change to updateProfileData()
    void updateProfilePicture()
    {
        setName (profileData->getName());
        setEmail (profileData->getEmail());
        setDevID (profileData->getDevID());
        
        profilePictureButton.setImages (true, true, true, profileData->getProfilePicture(), 1.0f, {}, profileData->getProfilePicture(), 1.0f, {}, profileData->getProfilePicture(), 1.0f, {});
        resized();
        repaint();
    }
    
    std::function<void()> chooseProfilePicture;
    
    ProfileScreen()
    {
//        addAndMakeVisible (*quilioLogoButton);
        addAndMakeVisible (profilePictureButton);
        profilePictureButton.onClick = [&]
        {
//            chooseProfilePicture();
        };
        
//        profilePictureButton.setImages (profilePictureButtonImageDefault, profilePictureButtonImageHover, profilePictureButtonImageDown);
        addAndMakeVisible (backButton);
        addAndMakeVisible (signOutButton);
        
        addAndMakeVisible (emailLabel);
        addAndMakeVisible (devIDLabel);
        
        emailLabel.setText ("Email:", juce::dontSendNotification);
        devIDLabel.setText ("Developer ID:", juce::dontSendNotification);
        emailLabel.setColour (juce::Label::textColourId, juce::Colour::fromString ("#ffBFBFBF"));
        devIDLabel.setColour (juce::Label::textColourId, juce::Colour::fromString ("#ffBFBFBF"));
        
        backButton.onClick = [&] { onBack(); };
        signOutButton.onClick = [&] { onSignOut(); };
        
        backButton.setLookAndFeel (&roundedOutlineButtonStyle);
        signOutButton.setLookAndFeel (&signOutStopClearButtonStyling);
        
        signOutButton.setColour (juce::ComboBox::outlineColourId, juce::Colour::fromString ("#ffF2571D"));
        signOutButton.setColour (juce::TextButton::textColourOffId, juce::Colour::fromString ("#ffF2571D"));
        signOutButton.setColour (juce::TextButton::textColourOnId, juce::Colour::fromString ("#ffF2571D"));

        profilePictureImage = loadImage ("profilePicCircle");
        profilePictureButton.setImages (true, true, true, profilePictureImage, 1.0f, {}, profilePicCircleImageHover, 1.0f, {}, profilePicCircleImageHover, 1.0f, {});
        
        backButton.onStateChange = [&]
        {
            if (backButton.isDown()){}
            else if (backButton.isOver())
            {
                backButtonSnapshot = getDropShadowSnapshotFromComponent (&backButton);
            }
            else{}
            
            repaint();
        };
        
    }
    juce::Image backButtonSnapshot;
    juce::GlowEffect glowEffect;
    
    juce::Image getGlowSnapshotFromComponent (juce::Component* component)
    {
        juce::Rectangle<int> area (-12, -12, component->getWidth() + 24, component->getHeight() + 24);
        juce::Image snapshot = component->createComponentSnapshot (area, false);
        juce::Graphics snapshotGraphics (snapshot);
    
        glowEffect.setGlowProperties (24.0f, juce::Colour (217, 217, 217).withAlpha (0.1f));
        glowEffect.applyEffect (snapshot, snapshotGraphics, 0.2f, 1.0f);

        return snapshot;
    }
    
    juce::Image getDropShadowSnapshotFromComponent (juce::Component* component)
    {
        juce::Rectangle<int> area (-12, -12, component->getWidth() + 24, component->getHeight() + 24);
        juce::Image snapshot = component->createComponentSnapshot (area, false);
        juce::Graphics snapshotGraphics (snapshot);
        
        juce::DropShadow dropShadow (juce::Colour (140, 140, 140).withAlpha (1.0f), 10.0f, {0, 0});
        juce::DropShadowEffect dropShadowEffect;
        dropShadowEffect.setShadowProperties (dropShadow);
        dropShadowEffect.applyEffect (snapshot, snapshotGraphics, 1.0f, 1.0f);
        
        return snapshot;
    }
    
    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour::fromString ("#ff000A1A"));
        
        g.setFont (28);
        g.setColour (juce::Colour::fromString ("#ffEFEFEF"));
        g.drawText (name, 190, 232, 301, 42, juce::Justification::centred);
        
        g.setFont (16);
        g.setColour (juce::Colour::fromString ("#ff595959"));
        g.drawText (email, 307.5, 306, 280, 24, juce::Justification::left);
        g.drawText (devID, 307.5, 346, 280, 24, juce::Justification::left);
        
        juce::AffineTransform moveBackButton;
        moveBackButton = juce::AffineTransform::translation (backButton.getX() - 12, backButton.getY() - 12);

        if (backButton.isDown()){}
        else if (backButton.isOver())
        {
            g.drawImageTransformed (backButtonSnapshot, moveBackButton);
        }
        else{}

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
        signOutButton.setBounds (502, 482 + 32, 146, 54);
//        quilioLogoButton->setBounds (32, 30, 80, 40);
        
        emailLabel.setBounds (174.5, 304, 109, 24);
        devIDLabel.setBounds (174.5, 348, 109, 24);
    }
    
    
    std::function<void()> onBack = []{};
    std::function<void()> onSignOut = []{};
};
