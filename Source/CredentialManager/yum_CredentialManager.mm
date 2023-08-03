#include "yum_CredentialManager.h"
/// For Keychain examples see:
/// https://es1015.tistory.com/243
/// https://cpp.hotexamples.com/examples/-/-/SecItemCopyMatching/cpp-secitemcopymatching-function-examples.html
///
#if JUCE_MAC || JUCE_IOS

#define Point CarbonDummyPoint
#define Rectangle CarbonDummyRect
#define Component CarbonDummyComp
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFUtilities.h>
#include <Foundation/Foundation.h>
#include <Security/Security.h>
#include <Security/SecCode.h>
#undef Point
#undef Rectangle
#undef Component

#include <JuceHeader.h>

using namespace QuilioCredentials;

bool AppCredentials::removeEntry (const SigningDetails& details)
{
    const auto name = std::get<0> (details);
    const auto email = std::get<1> (details);
    const auto developerID = std::get<2> (details);
    const auto password = std::get<3> (details);
    
    juce::String applicationName = "Notaryze";
    
    if (password.isEmpty () || name.isEmpty () || developerID.isEmpty() || email.isEmpty()) return false;
    
    auto createEntry = [&, applicationName, name, email, developerID, password]() -> bool
    {
        JUCE_AUTORELEASEPOOL
        {
            const int numOptions = 8;
            
            CFStringRef keys [numOptions];
            keys[0] = kSecClass;
            keys[1] = kSecAttrAccount;
            keys[2] = kSecValueData;
            keys[3] = kSecValueData;
            keys[4] = kSecValueData;
            keys[5] = kSecValueData;
            keys[6] = kSecAttrService;
            keys[7] = kSecAttrAccessible;
            
            CFTypeRef values [numOptions];
            values[0] = kSecClassGenericPassword;
            values[1] = applicationName.toCFString();
            values[2] = name.toCFString();
            values[3] = email.toCFString();
            values[4] = developerID.toCFString ();
            values[5] = password.toCFString ();
            values[6] = juce::String (ProjectInfo::projectName).toCFString();
            values[7] = kSecAttrAccessibleAfterFirstUnlock;
            
            CFDictionaryRef query;
            query = CFDictionaryCreate (kCFAllocatorDefault,
                                        (const void**) keys,
                                        (const void**) values,
                                        numOptions, NULL, NULL);
            
            return SecItemDelete (query) == 0;
        }
    };
    
    auto credentialsExist = userCredentialsExist (name);
    if ( ! credentialsExist )
    {
#if RunHeadless
        return createEntry ();
#else
        auto o = juce::MessageBoxOptions ().withTitle("Save login data in Keychain?")
            .withMessage ("Do you want to store your login data in Keychain?")
            .withButton ("Yes").withButton ("No");
        
        juce::AlertWindow::showAsync (o, [&, createEntry](int result)
                                      {
            if (result == 1)
                createEntry ();
        });
        
        return true;
#endif
        
    }
}

bool AppCredentials::updateEntry (const SigningDetails& creds)
{
    const auto name = std::get<0> (creds);
    const auto email = std::get<1> (creds);
    const auto developerID = std::get<2> (creds);
    const auto password = std::get<3> (creds);
    
    juce::String applicationName = "Notaryze";
    
    if (password.isEmpty () || name.isEmpty () || developerID.isEmpty() || email.isEmpty()) return false;
    
    auto createEntry = [&, applicationName, name, email, developerID, password]() -> bool
    {
        JUCE_AUTORELEASEPOOL
        {
            const int numOptions = 8;
            
            CFStringRef keys [numOptions];
            keys[0] = kSecClass;
            keys[1] = kSecAttrAccount;
            keys[2] = kSecValueData;
            keys[3] = kSecValueData;
            keys[4] = kSecValueData;
            keys[5] = kSecValueData;
            keys[6] = kSecAttrService;
            keys[7] = kSecAttrAccessible;

            CFTypeRef values [numOptions];
            values[0] = kSecClassGenericPassword;
            values[1] = applicationName.toCFString();
            values[2] = name.toCFString();
            values[3] = email.toCFString();
            values[4] = developerID.toCFString ();
            values[5] = password.toCFString ();
            values[6] = juce::String (ProjectInfo::projectName).toCFString();
            values[7] = kSecAttrAccessibleAfterFirstUnlock;
            
            CFDictionaryRef query;
            query = CFDictionaryCreate (kCFAllocatorDefault,
                                        (const void**) keys,
                                        (const void**) values,
                                        numOptions, NULL, NULL);

            return SecItemAdd (query, NULL) == 0;
        }
    };
    
    auto credentialsExist = userCredentialsExist (name);
    if ( ! credentialsExist )
    {
        return createEntry ();
    }
    else //credentials exist, check if we have to update
    {
        const auto currentlyStoredDetails = getDetailsForName (name);
        auto name = std::get<0> (currentlyStoredDetails);
        auto email = std::get<1> (currentlyStoredDetails);
        auto developerID = std::get<2> (currentlyStoredDetails);
        auto password = std::get<3> (currentlyStoredDetails);
        juce::String applicationName ("Notaryze");
 //       if (currentlyStoredPassword != password)
        {
            //update
            auto updateDetails = [&, applicationName, name, email, developerID, password, createEntry]() -> bool
            {
                JUCE_AUTORELEASEPOOL
                {
                    const auto serviceName = [[NSString alloc] initWithUTF8String: ProjectInfo::projectName];
                    
                    CFMutableDictionaryRef query = CFDictionaryCreateMutable (NULL, 0,
                                                                             &kCFTypeDictionaryKeyCallBacks,
                                                                             &kCFTypeDictionaryValueCallBacks);
                    
                    CFDictionarySetValue (query, kSecClass, kSecClassGenericPassword);
                    CFDictionarySetValue (query, kSecAttrService, serviceName);
                    CFDictionarySetValue (query, kSecAttrAccount, [[NSString alloc] initWithUTF8String:applicationName.toRawUTF8()]);
                    CFDictionarySetValue (query, kSecReturnAttributes, kCFBooleanTrue);
                    CFDictionarySetValue (query, kSecReturnData, kCFBooleanTrue);
                    
                    /// I can't figure out for the life of me how to use SecItemUpdate (),
                    /// so we do it the rough way, deleting the item and recreating it
                    /// i have tried every single example I was able to find, but if you think you can manage
                    /// be my guest and submit a PR using SecItemUpdate () here instead
                    OSStatus status = SecItemDelete((__bridge CFDictionaryRef)query);
                    
                    if (status == errSecSuccess)
                    {
                        DBG("Item deleted... recreating now");
                        return createEntry ();
                    }
                    else
                    {
                        jassertfalse; // error handling?
                        return false;
                    }
                }
            };
            
            return updateDetails ();
        }
        
        return false;
    }
}

bool AppCredentials::anyExist ()
{
    JUCE_AUTORELEASEPOOL
    {
       //Let's create an empty mutable dictionary:
       NSMutableDictionary *keychainItem = [NSMutableDictionary dictionary];
        
       auto serviceName = [[NSString alloc] initWithUTF8String: ProjectInfo::projectName];
     
       //Populate it with the data and the attributes we want to use.
        
       keychainItem[(__bridge id)kSecClass] = (__bridge id)kSecClassGenericPassword; // We specify what kind of keychain item this is.
       keychainItem[(__bridge id)kSecAttrAccessible] = (__bridge id)kSecAttrAccessibleAfterFirstUnlock; // This item can only be accessed when the user unlocks the device.
       keychainItem[(__bridge id)kSecAttrService] = serviceName;
        
       //Check if this keychain item exists.
       return SecItemCopyMatching((__bridge CFDictionaryRef)keychainItem, NULL) == noErr;
    }
}

bool AppCredentials::userCredentialsExist (const Name& name)
{
    const auto allEntriesForUser = getAllAvailableEntries (name);
    return ! allEntriesForUser.isEmpty ();
}

juce::StringArray AppCredentials::getAllAvailableEntries (const juce::String& filter)
{
    JUCE_AUTORELEASEPOOL
    {
        juce::StringArray entries;
        const auto serviceName = [[NSString alloc] initWithUTF8String: ProjectInfo::projectName];
    
        CFMutableDictionaryRef query = CFDictionaryCreateMutable(NULL, 0,
                                                                &kCFTypeDictionaryKeyCallBacks,
                                                                &kCFTypeDictionaryValueCallBacks);
     
        CFDictionarySetValue (query, kSecReturnRef, kCFBooleanTrue);
        CFDictionarySetValue (query, kSecReturnAttributes, kCFBooleanTrue);
        CFDictionarySetValue (query, kSecMatchLimit, kSecMatchLimitAll);
        CFDictionarySetValue (query, kSecAttrService, (__bridge id)serviceName);
        CFDictionarySetValue (query, kSecClass, kSecClassGenericPassword);
        
        CFArrayRef items = NULL;
        
        OSStatus status = SecItemCopyMatching(query, (CFTypeRef *)&items);

        if (status == errSecSuccess)
        {
            const auto count = CFArrayGetCount (items);
            for (int j = 0; j < count; j++)
            {
                CFDictionaryRef properties = (CFDictionaryRef) CFArrayGetValueAtIndex (items, j);
                const juce::String accountName = [(NSString*)CFDictionaryGetValue(properties, kSecAttrAccount) UTF8String];
              
                if (filter.isEmpty () || accountName.contains (filter))
                    entries.add (accountName);
            }
            
            entries.sort (true);
            return entries;
        }
        else if (status == errSecItemNotFound)
        {
            //No password found for service (/app)
            return {};
        }
        else
        {
            jassertfalse; //do we need to handle any errors?
        }
    }
    
    jassertfalse;
    return {};
}

juce::String AppCredentials::getPasswordForName (const Name& name)
{
    JUCE_AUTORELEASEPOOL
    {
        const auto serviceName = [[NSString alloc] initWithUTF8String: ProjectInfo::projectName];
        
        CFMutableDictionaryRef query = CFDictionaryCreateMutable(NULL, 0,
                                                                &kCFTypeDictionaryKeyCallBacks,
                                                                &kCFTypeDictionaryValueCallBacks);
        
        CFDictionarySetValue (query, kSecClass, kSecClassGenericPassword);
        CFDictionarySetValue (query, kSecAttrService, serviceName);
        CFDictionarySetValue (query, kSecAttrAccount, [[NSString alloc] initWithUTF8String:name.toRawUTF8()]);
        CFDictionarySetValue (query, kSecReturnAttributes, kCFBooleanTrue);
        CFDictionarySetValue (query, kSecReturnData, kCFBooleanTrue);
        
        CFDictionaryRef result = nil;
        OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, (CFTypeRef *)&result);
        
        if (status == errSecSuccess)
        {
            const auto resultDict = (NSDictionary *)result;
            const auto password = [[NSString alloc] initWithData:resultDict[(__bridge id)kSecValueData]
                                                        encoding:NSUTF8StringEncoding];
            return [password UTF8String];
        }
        else
        {
//            jassertfalse;
            DBG ("User denied access or other error");
            return {};
        }
    }
    
    jassertfalse;
    return {};
}
SigningDetails AppCredentials::getDetailsForName (const Name& applicationName)
{
    JUCE_AUTORELEASEPOOL
    {
        const auto serviceName = [[NSString alloc] initWithUTF8String:ProjectInfo::projectName];

        CFMutableDictionaryRef query = CFDictionaryCreateMutable(NULL, 0,
            &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

        CFDictionarySetValue (query, kSecClass, kSecClassGenericPassword);
        CFDictionarySetValue (query, kSecAttrService, serviceName);
        CFDictionarySetValue (query, kSecAttrAccount, [[NSString alloc] initWithUTF8String:applicationName.toRawUTF8()]);
        CFDictionarySetValue (query, kSecReturnAttributes, kCFBooleanTrue);
        CFDictionarySetValue (query, kSecReturnData, kCFBooleanTrue);

        CFDictionaryRef result = nil;
        OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, (CFTypeRef*)&result);

        //Prompts for keychain access here
        
        if (status == errSecSuccess)
        {
            const auto resultDict = (NSDictionary*)result;
            
//            const auto applicationNameData = static_cast<NSData*>(resultDict[(__bridge id)kSecAttrAccount]);
//            const auto applicationName = [[NSString alloc] initWithData:applicationNameData encoding:NSUTF8StringEncoding];
//
            const auto nameData = static_cast<NSData*>(resultDict[(__bridge id)kSecValueData]);
            const auto name = [[NSString alloc] initWithData:nameData encoding:NSUTF8StringEncoding];
            
            const auto emailData = static_cast<NSData*>(resultDict[(__bridge id)kSecValueData]);
            const auto email = [[NSString alloc] initWithData:emailData encoding:NSUTF8StringEncoding];

            const auto developerIDData = static_cast<NSData*>(resultDict[(__bridge id)kSecValueData]);
            const auto developerID = [[NSString alloc] initWithData:developerIDData encoding:NSUTF8StringEncoding];

            const auto passwordData = static_cast<NSData*>(resultDict[(__bridge id)kSecValueData]);
            const auto password = [[NSString alloc] initWithData:passwordData encoding:NSUTF8StringEncoding];

            // Extract other details from the keychain result or provide appropriate default values
            const Name _name = [name UTF8String];
            const Password _password = [password UTF8String];
            const Email _email = [email UTF8String];
            const DeveloperID _developerID = [developerID UTF8String];

            // Create and return the SigningDetails tuple
            return std::make_tuple (_name, _password, _developerID, _email);
        }
        else
        {
            // jassertfalse;
            DBG("User denied access or other error");
            // Return an empty SigningDetails tuple or appropriate default values
            return std::make_tuple("", "", "", "");
        }
    }

    // jassertfalse;
    // Return an empty SigningDetails tuple or appropriate default values
    return std::make_tuple("", "", "", "");
}


//juce::Array<UsernameAndPassword> AppCredentials::getAllStoredUsernamesAndPasswords (std::function<bool ()> onNoneFound)
//{
//    juce::Array<UsernameAndPassword> creds;
//    const auto entries = getAllAvailableEntries();
//
//    if (entries.isEmpty () && onNoneFound != nullptr)
//    {
//        auto tryAgain = onNoneFound ();
//        if (tryAgain)
//            return getAllStoredUsernamesAndPasswords (onNoneFound);
//    }
//
//    for (auto& e : entries)
//    {
//        const auto pass = getPasswordForUsername (e);
//        creds.add ({e, pass});
//    }
//
//    return creds;
//}







juce::Array<SigningDetails> AppCredentials::getAllStoredSigningDetails (std::function<bool ()> onNoneFound)
{
    juce::Array<SigningDetails> creds;
    const auto entries = getAllAvailableEntries();
    
    if (entries.isEmpty () && onNoneFound != nullptr)
    {
        auto tryAgain = onNoneFound ();
        if (tryAgain)
            return getAllStoredSigningDetails (onNoneFound);
    }
    
    for (auto& e : entries)
    {
        const auto details = getDetailsForName (e);
        auto name = std::get<0> (details);
        auto email = std::get<1> (details);
        auto developerID = std::get<2> (details);
        auto password = std::get<3> (details);
        
        creds.add (std::make_tuple (name, email, developerID, password));
    }


    return creds;
}

//========================================================================
//========================================================================


juce::String Certificates::getAppIdFromSignature (const juce::File& f)
{
    juce::String appId;
    JUCE_AUTORELEASEPOOL
    {
        SecStaticCodeRef code;
        CFURLRef url = (CFURLRef)[NSURL fileURLWithPath:[[NSString alloc] initWithUTF8String:f.getFullPathName().toUTF8()]];
        auto result = SecStaticCodeCreateWithPath (url, kSecCSDefaultFlags, &code);
        
        if (result == noErr)
        {
            CFDictionaryRef info;
            auto infoResult = SecCodeCopySigningInformation (code, kSecCSRequirementInformation, &info);
            
            if (infoResult == noErr)
            {
                appId = [[(id)info objectForKey:@"identifier"] UTF8String];
            }
            else
            {
                appId = "Error getting certificate info";
            }
        }
        else
        {
            appId = "Error getting static code reference";
        }
    }

    return appId;
}


juce::String Certificates::getSignerIdentity (const juce::File& f)
{
    juce::String signerIdentity;
    JUCE_AUTORELEASEPOOL
    {
        SecStaticCodeRef code;
        CFURLRef url = (CFURLRef)[NSURL fileURLWithPath:[[NSString alloc] initWithUTF8String:f.getFullPathName().toUTF8()]];
        auto result = SecStaticCodeCreateWithPath (url, kSecCSDefaultFlags, &code);
        
        if (result == noErr)
        {
            CFDictionaryRef info;
            auto infoResult = SecCodeCopySigningInformation (code, kSecCSSigningInformation, &info);
            
            if (infoResult == noErr)
            {
                signerIdentity = [[(id)info objectForKey:@"teamid"] UTF8String];
            }
            else
            {
                signerIdentity = "Error getting certificate info";
            }
        }
        else
        {
            signerIdentity = "Error getting static code reference";
        }
    }

    return signerIdentity;
}

#endif //end JUCE_MAC
