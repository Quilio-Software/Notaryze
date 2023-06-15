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
    
    std::unique_ptr<ProfileData> profileData;

    ScreenManager()
    {
        addChildComponent (signInScreen);
        addChildComponent (utilityScreen);
        addChildComponent (profileScreen);
        
        //When we get past the login screen, we arrive at the utility screen.
        signInScreen.onSubmit   = [&]
        {
            if (signInScreen.isDataComplete())
            {
                profileData = std::make_unique<ProfileData> (signInScreen.getProfileData());
                
                utilityScreen.setDevName (profileData->getName());
                utilityScreen.setDevID (profileData->getDevID());
                
                setCurrentScreen (UTILITY);
            }
        };
        
        profileScreen.onBack    = [&] { setCurrentScreen (UTILITY); };
        profileScreen.onSignOut = [&] { setCurrentScreen (SIGN_IN); };
        
        utilityScreen.onLogo    = [&] { launchWebpage (quilioWebsiteURL); };
        utilityScreen.onProfile = [&] { setCurrentScreen (PROFILE); };
    }
    
    ~ScreenManager() {}
    
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
