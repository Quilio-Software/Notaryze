/*
  ==============================================================================

    ScreenManager.h
    Created: 3 May 2023 1:32:51pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once
#include "Screens/SignInScreen.h"
#include "Screens/UtilityScreen.h"
#include "Screens/ProfileScreen.h"

enum ScreenID
{
    SIGN_IN,
    UTILITY,
    PROFILE
};

class ScreenManager : public juce::Component
{
public:
    
    //Declare profile data first
    std::shared_ptr<ProfileData> profileData;
    
    
    SignInScreen signInScreen;
    UtilityScreen utilityScreen;
    ProfileScreen profileScreen;
    
    juce::String quilioWebsiteURL = "www.quilio.dev";
    
    void launchWebpage (juce::String url)
    {
        juce::URL webpage (url);
        webpage.launchInDefaultBrowser();
    }
    
    void storeDetails()
    {
    }
    
    ScreenManager()
    {
        addChildComponent (signInScreen);
        addChildComponent (utilityScreen);
        addChildComponent (profileScreen);
        
        setCurrentScreen (SIGN_IN);
        
        //When we get past the login screen, we arrive at the utility screen.
        signInScreen.onSubmit = [&](bool value)
        {
            if (signInScreen.isDataComplete()) //If all the data has been filled out as necessary
            {
                profileData = std::make_shared<ProfileData> (signInScreen.getProfileData());
                profileScreen.setProfileData (profileData);
                utilityScreen.setProfileData (profileData);
                profileScreen.updateProfilePicture();
                utilityScreen.updateProfilePicture();
                
                profileData->saveToKeychain();
                
                utilityScreen.setDevName (profileData->getName());
                utilityScreen.setDevID (profileData->getDevID());
                
                setCurrentScreen (UTILITY);
            }
        };
        
        profileScreen.onBack    = [&] { setCurrentScreen (UTILITY); };
        profileScreen.onSignOut = [&]
        {
            setCurrentScreen (SIGN_IN);
            bool deleteStatus = profileData->removeFromKeychain ("Notaryze");
            signInScreen.clearTextEditors();
        };
        
        utilityScreen.onLogo    = [&] { launchWebpage (quilioWebsiteURL); };
        utilityScreen.onProfile = [&] { setCurrentScreen (PROFILE); };
        
        
        profileData = std::make_shared<ProfileData>();
        if (profileData->loadFromKeychain ("Notaryze"))
        {
            profileScreen.setProfileData (profileData);
            utilityScreen.setProfileData (profileData);
            profileScreen.updateProfilePicture();
            utilityScreen.updateProfilePicture();
            
//            profileData->saveToKeychain();
            
            utilityScreen.setDevName (profileData->getName());
            utilityScreen.setDevID (profileData->getDevID());
            
            setCurrentScreen (UTILITY);
        }
        
        profileScreen.chooseProfilePicture = [&]
        {
            juce::FileChooser chooser{ "Please load a file" };
            if (chooser.browseForFileToOpen ())
            {
                auto file = chooser.getResult(); //We have chosen an image
                //Ensure that it is an image
                if (FormatLibrary::isImage (file))
                {
                    profileData->setProfilePicture (file);
                    //Then load image file into profilePictureImage
                    profileScreen.updateProfilePicture();
                    utilityScreen.updateProfilePicture();
                }
                else
                {
                    DBG ("File is not an image");
                }
            }
        };
    }
    
    ~ScreenManager() {}
    
    //This function sets the visibility of all the screens in the application. This function could be useful for when the application is launched and all the screens need to be hidden until the user interacts with the application.
        void setAllScreenVisibilities (bool visibility)
        {
            signInScreen.setVisible (visibility);
            utilityScreen.setVisible (visibility);
            profileScreen.setVisible (visibility);
        }
    
    ScreenID lastScreenID = ScreenID::UTILITY;
    
    void setCurrentScreen (ScreenID screen)
    {
        setAllScreenVisibilities (false);
        
        if (screen == SIGN_IN) signInScreen.setVisible (true);
        else if (screen == UTILITY) utilityScreen.setVisible (true);
        else if (screen == PROFILE) profileScreen.setVisible (true);
        
        repaint();
    }
    
    void resized() override
    {
        signInScreen.setBounds (getBounds());
        utilityScreen.setBounds (getBounds());
        profileScreen.setBounds (getBounds());
    }
};
