#include "yum_CredentialManager.h"

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


using namespace QuilioCredentials;

bool AppCredentials::removeEntry (const SigningDetails& concatenatedDetails)
{
    juce::String concatenatedDataString = std::get<0>(concatenatedDetails) + "|" + std::get<1>(concatenatedDetails) + "|" + std::get<2>(concatenatedDetails) + "|" + std::get<3>(concatenatedDetails);

    if (concatenatedDataString.isEmpty()) return false;
    
    juce::String applicationName = "Notaryze";
    
    auto deleteEntry = [&, applicationName, concatenatedDataString]() -> bool
    {
        JUCE_AUTORELEASEPOOL
        {
            const int numOptions = 3;

            CFStringRef keys [numOptions];
            keys[0] = kSecClass;
            keys[1] = kSecAttrAccount;
            keys[2] = kSecAttrService;

            CFTypeRef values [numOptions];
            values[0] = kSecClassGenericPassword;
            values[1] = applicationName.toCFString();
            values[2] = juce::String (ProjectInfo::projectName).toCFString();

            CFDictionaryRef query;
            query = CFDictionaryCreate (kCFAllocatorDefault,
                                        (const void**) keys,
                                        (const void**) values,
                                        numOptions, NULL, NULL);

            return SecItemDelete (query) == 0;
        }
    };

    auto credentialsExist = userCredentialsExist ("Notaryze");
    if (credentialsExist)
    {
        return deleteEntry();
    }
    return false;
}

bool AppCredentials::updateEntry (const SigningDetails& creds)
{
    const auto name = std::get<0> (creds);
    const auto email = std::get<1> (creds);
    const auto developerID = std::get<2> (creds);
    const auto password = std::get<3> (creds);

    juce::String applicationName = "Notaryze";

    if (password.isEmpty() || name.isEmpty() || developerID.isEmpty() || email.isEmpty()) return false;

    juce::String concatenatedData = name + "|" + email + "|" + developerID + "|" + password;

    auto createEntry = [&, applicationName, concatenatedData]() -> bool
    {
        JUCE_AUTORELEASEPOOL
        {
            const int numOptions = 4;

            CFStringRef keys[numOptions];
            keys[0] = kSecClass;
            keys[1] = kSecAttrAccount;
            keys[2] = kSecValueData;
            keys[3] = kSecAttrService;

            CFTypeRef values[numOptions];
            values[0] = kSecClassGenericPassword;
            values[1] = applicationName.toCFString();
            values[2] = concatenatedData.toCFString();
            values[3] = juce::String (ProjectInfo::projectName).toCFString();

            CFDictionaryRef query;
            query = CFDictionaryCreate (kCFAllocatorDefault,
                                       (const void**) keys,
                                       (const void**) values,
                                       numOptions, NULL, NULL);

            return SecItemAdd (query, NULL) == 0;
        }
    };

    auto credentialsExist = userCredentialsExist (name);
    if (!credentialsExist)
    {
        return createEntry();
    }
    else
    {
        const auto currentlyStoredDetails = getDetailsForName(name);
        auto storedConcatenatedData = std::get<0>(currentlyStoredDetails);
        juce::String applicationName("Notaryze");

        if (storedConcatenatedData != concatenatedData)
        {
            auto updateDetails = [&, applicationName, concatenatedData, createEntry]() -> bool
            {
                JUCE_AUTORELEASEPOOL
                {
                    const auto serviceName = [[NSString alloc] initWithUTF8String: ProjectInfo::projectName];
                    CFMutableDictionaryRef query = CFDictionaryCreateMutable(NULL, 0,
                                                                            &kCFTypeDictionaryKeyCallBacks,
                                                                            &kCFTypeDictionaryValueCallBacks);

                    CFDictionarySetValue(query, kSecClass, kSecClassGenericPassword);
                    CFDictionarySetValue(query, kSecAttrService, serviceName);
                    CFDictionarySetValue(query, kSecAttrAccount, [[NSString alloc] initWithUTF8String:applicationName.toRawUTF8()]);
                    CFDictionarySetValue(query, kSecReturnAttributes, kCFBooleanTrue);
                    CFDictionarySetValue(query, kSecReturnData, kCFBooleanTrue);

                    OSStatus status = SecItemDelete((__bridge CFDictionaryRef)query);

                    if (status == errSecSuccess)
                    {
                        return createEntry();
                    }
                    else
                    {
                        jassertfalse;
                        return false;
                    }
                }
            };

            return updateDetails();
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


SigningDetails AppCredentials::getDetailsForName(const Name& applicationName)
{
    JUCE_AUTORELEASEPOOL
    {
        const auto serviceName = [[NSString alloc] initWithUTF8String:ProjectInfo::projectName];
        
        CFMutableDictionaryRef query = CFDictionaryCreateMutable(NULL, 0,
            &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
            
        CFDictionarySetValue(query, kSecClass, kSecClassGenericPassword);
        CFDictionarySetValue(query, kSecAttrService, serviceName);
        CFDictionarySetValue(query, kSecAttrAccount, [[NSString alloc] initWithUTF8String:applicationName.toRawUTF8()]);
        CFDictionarySetValue(query, kSecReturnAttributes, kCFBooleanTrue);
        CFDictionarySetValue(query, kSecReturnData, kCFBooleanTrue);
        
        CFDictionaryRef result = nil;
        OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, (CFTypeRef*)&result);
        
        if (status == errSecSuccess)
        {
            const auto resultDict = (NSDictionary*)result;
            const auto concatenatedData = static_cast<NSData*>(resultDict[(__bridge id)kSecValueData]);
            const auto concatenatedString = [[NSString alloc] initWithData:concatenatedData encoding:NSUTF8StringEncoding];
            
            std::istringstream iss([concatenatedString UTF8String]);
            std::string token;
            std::vector<std::string> components;

            while (std::getline(iss, token, '|'))
            {
                components.push_back(token);
            }

            if (components.size() == 4)
            {
                return std::make_tuple(components[0], components[1], components[2], components[3]);
            }
            else
            {
                // Handle error: The split data does not have the expected number of components.
                return std::make_tuple("", "", "", "");
            }
        }
        else
        {
            // Handle errors
            return std::make_tuple("", "", "", "");
        }
    }
}

juce::Array<SigningDetails> AppCredentials::getAllStoredSigningDetails(std::function<bool()> onNoneFound)
{
    juce::Array<SigningDetails> creds;
    const auto entries = getAllAvailableEntries();

    if (entries.isEmpty() && onNoneFound != nullptr)
    {
        auto tryAgain = onNoneFound();
        if (tryAgain)
            return getAllStoredSigningDetails(onNoneFound);
    }

    for (auto& e : entries)
    {
        const auto details = getDetailsForName(e);
        juce::String concatenatedData = std::get<0>(details) + "|" + std::get<1>(details) + "|" + std::get<2>(details) + "|" + std::get<3>(details);
        std::istringstream iss (concatenatedData.toStdString());

        std::string token;
        std::vector<juce::String> components;

        while (std::getline(iss, token, '|'))
        {
            components.push_back(juce::String(token));
        }

        if (components.size() == 4)
        {
            auto name = components[0];
            auto email = components[1];
            auto developerID = components[2];
            auto password = components[3];
            creds.add(std::make_tuple(name, email, developerID, password));
        }
        else
        {
            // Handle error: The split data does not have the expected number of components.
        }
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
