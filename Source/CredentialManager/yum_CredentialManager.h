#pragma once


#define Point Potato
#include <JuceHeader.h>
#undef Point
#ifdef JUCE_MAC

typedef juce::String Name;
typedef juce::String Password;
typedef juce::String DeveloperID;
typedef juce::String Email;
typedef std::tuple<Name, Password, DeveloperID, Email> SigningDetails;

namespace QuilioCredentials
{
//using namespace juce;

//========================================================================
//========================================================================
struct AppCredentials
{
    /// returns, if item was added to system credential manager successfully
    static bool updateEntry (const SigningDetails& creds);
    
    /// returns if at least one item for the given app exists in system credential manager
    static bool anyExist ();
    
    /// Removes a particular entry from the keychain
    static bool removeEntry (const SigningDetails& creds);

    /// returns if a given username already has credentials stored in the keychain
    static bool userCredentialsExist (const Name& name);
    
    /// returns a list of all Username/Password items in Keychain/Credential Manager, but only the Usernames.
    /// Use a username to call getPasswordForUsername (const String& username)
    static juce::StringArray getAllAvailableEntries (const juce::String& filter = {});

    /// returns a password to a username that was found via getAllAvailableEntries ()
    static juce::String getPasswordForName (const Name& name);
    
    static SigningDetails getDetailsForName (const Name& name);
    
    /// returns an arry containing all credentials that could be found for the app
    /// 1st argument is callback on "no item found", if not defined a standard error will popup if no item can be found for the app
    /// callback return defines if password should be read again after callback finished
    /// !!! Depending on the keychain settings, this might require MULTIPLE password entries on Mac. It might be safer to just use getAllAvailableEntries () and call getPasswordForUsername () with a specific entry only
    static juce::Array<SigningDetails> getAllStoredSigningDetails (std::function<bool ()> onNoneFound = nullptr);
    
};

//========================================================================
//========================================================================
struct Certificates
{
    /// on Mac this will return the 10-letter Apple TeamID associated with a certificate (e.g. "SFXXXXXXXT")
    /// on Windows this will return the human readable name of the signer (e.g. "Yum Audio GmbH & Co. KG")
    static juce::String getSignerIdentity (const juce::File& f);

#if JUCE_MAC
    /// returns the app id imprinted on a certificate (e.g. com.YumAudio.Spread)
    static juce::String getAppIdFromSignature (const juce::File& f);
#endif
    
};

}


#endif // JUCE_MAC
