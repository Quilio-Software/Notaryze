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

//inline void setImages(juce::ImageButton& button,
//                      juce::Image& off,
//                      juce::Image& on,
//                      juce::Image& offHover,
//                      juce::Image& onHover,
//                      juce::Image& offActive,
//                      juce::Image& onActive)
//{
//    button.setImages(true,                  // shouldUseOnColImage
//                     true,                  // shouldUseOffColImage
//                     true,                  // shouldUseOnColForButtonDown
//                     off,                   // normalImage
//                     1.0f,                  // normalImageAlpha
//                     {},                     // normalImagePlacement
//                     offHover,              // overImage
//                     1.0f,                  // overImageAlpha
//                     {},                     // overImagePlacement
//                     on,                    // downImage
//                     1.0f,                  // downImageAlpha
//                     {},                     // downImagePlacement
//                     offActive,             // normalOnImage
//                     1.0f,                  // normalOnImageAlpha
//                     {},                     // normalOnImagePlacement
//                     onActive,              // overOnImage
//                     1.0f,                  // overOnImageAlpha
//                     {}                      // overOnImagePlacement
//                     );
//}


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

//TODO: Move this to profile data // alongside keychain stuff I think
class ProfileData
{
    juce::String name, email, devID, password;
    juce::Image profileImage;
    
public:
    ProfileData (juce::String newName, juce::String newEmail, juce::String newDevID, juce::String newPassword)
        : name (newName), email (newEmail), devID (newDevID), password (newPassword)
    {
        profileImage = juce::ImageFileFormat::loadFrom (BinaryData::profilePicCircle_png, BinaryData::profilePicCircle_pngSize);
    }
    
    ProfileData() : name (""), email (""), devID (""), password ("")
    {
        profileImage = juce::ImageFileFormat::loadFrom (BinaryData::profilePicCircle_png, BinaryData::profilePicCircle_pngSize);
    }
    
    void saveToKeychain()
    {
        
    }
    
    void setProfilePicture (juce::File& file)
    {
        profileImage = juce::ImageFileFormat::loadFrom (file);
    }
    
    juce::Image& getProfilePicture()
    {
        return profileImage;
    }
    
    juce::String getName() const { return name; }
    juce::String getEmail() const { return email; }
    juce::String getDevID() const { return devID; }
    juce::String getPassword() const { return password; }
};


