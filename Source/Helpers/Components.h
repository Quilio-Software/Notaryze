/*
  ==============================================================================

    Components.h
    Created: 4 May 2023 4:42:51pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once
#include "../CredentialManager/yum_CredentialManager.h"
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
//Purpose: This class stores the profile data of a user, such as name, email, developer ID, and password.
    //Functionality: The class provides methods to save the data to the keychain, set the profile picture, and get the profile picture, name, email, developer ID, and password.
    //Input: The constructor takes 4 strings (name, email, developer ID, and password) as inputs. The setProfilePicture method takes a file
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
        
    juce::String toBase64 (const juce::String& input)
    {
        juce::MemoryBlock memBlock;
        memBlock.append(input.toRawUTF8(), input.getNumBytesAsUTF8());
        
        return memBlock.toBase64Encoding();
    }
    
    juce::String decryptData(const juce::String& data, const juce::String& key)
    {
        return encryptData(data, key);  // XOR decryption is the same as encryption
    }

//    juce::String fromBase64 (const juce::String& input)
//    {
//        juce::MemoryBlock memBlock;
//
//        if (memBlock.fromBase64Encoding(input))
//        {
//            return juce::String::fromUTF8((const uint8_t*) memBlock.getData(), (int) memBlock.getSize());
//        }
//
//        return juce::String();
//    }
    
    juce::Image fromBase64(const juce::String& input)
    {
        juce::MemoryBlock memBlock;
        
        if (memBlock.fromBase64Encoding(input))
        {
            juce::MemoryInputStream memoryInputStream(memBlock, false);
            juce::Image image = juce::ImageFileFormat::loadFrom(memoryInputStream);
            return image;
        }
        
        return juce::Image();
    }

    YumAudio::AppCredentials credentialsManager;
    bool loadFromKeychain (juce::String applicationName)
    {
        try
        {
            // Load and decode the Base64 image
   //         juce::String base64Image = credentialsManager.getPasswordForUsername("profileImage");
   //         juce::Image profileImage = fromBase64 (base64Image);
            
            SigningDetails details = credentialsManager.getDetailsForName (applicationName);
            name = std::get<0> (details);
            email = std::get<1> (details);
            devID = std::get<2> (details);
            password = std::get<3> (details);
            
            DBG ("email is: " + juce::String (email));

            if (name.isNotEmpty() && email.isNotEmpty() && devID.isNotEmpty() && password.isNotEmpty())
            {
                juce::Logger::writeToLog ("Success: Loaded all details from keychain");
                return true;
            }
            else return false;

//            if (profileImage.isNull())
//            {
//                juce::Logger::writeToLog ("Error: Unable to decode image from stream");
//                return false;
//            }

            // Use or store the loaded and decrypted data and image as needed...
        }
        catch (const std::exception& e)
        {
            juce::Logger::writeToLog ("Exception caught in loadFromKeychain: " + juce::String(e.what()));
        }
        catch (...)
        {
            juce::Logger::writeToLog ("Unknown exception caught in loadFromKeychain");
        }
    }

    void saveToKeychain()
    {
        try
        {
            // Store the data as a single entry
            credentialsManager.updateEntry (std::make_tuple (name, email, devID, password));

            // Save the image as a Base64 string
            juce::MemoryOutputStream outputStream;
            juce::JPEGImageFormat jpegFormat;
            if (!jpegFormat.writeImageToStream(profileImage, outputStream))
            {
                juce::Logger::writeToLog ("Error: Unable to write image to stream");
                return;
            }

            juce::String base64Image = toBase64 (outputStream.toString());
     //       credentialsManager.updateEntry (std::make_pair ("profileImage", base64Image));

            juce::Logger::writeToLog ("Success: Saved user details to the keychain");
        }
        catch (const std::exception& e)
        {
            juce::Logger::writeToLog ("Exception caught in saveToKeychain: " + juce::String(e.what()));
        }
        catch (...)
        {
            juce::Logger::writeToLog ("Unknown exception caught in saveToKeychain");
        }
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
    
    
    void saveToValueTree (juce::ValueTree& valueTree)
    {
        auto jsonData = getDataAsJSON();
        juce::DynamicObject* dynamicObject = jsonData.getDynamicObject();

        if (dynamicObject == nullptr)
        {
            std::cout << "The juce::var does not represent a JSON object." << std::endl;
            return;
        }

        for (const auto& property : dynamicObject->getProperties())
        {
            valueTree.setProperty(property.name, property.value, nullptr);
        }
    }

    juce::var createJsonVarFromPairs (const std::vector<std::pair<std::string, std::string>>& pairs)
    {
        juce::DynamicObject::Ptr jsonObject = new juce::DynamicObject();

        for (const auto& pair : pairs)
        {
            jsonObject->setProperty(juce::String(pair.first), juce::var(pair.second));
        }

        return juce::var (jsonObject.getObject());
    }

    juce::var getDataAsJSON()
    {
        // Sample JSON data as a string
        juce::String jsonString = R"(
            {
                "name": ")" + name +  R"(",
                "email": ")" + email +  R"(",
                "devID": ")" + devID +  R"(",
                "password": ")" + password +  R"(",
                "profileImage": ")" + profileImageToString() + R"("
            }
        )";

        // Parse the JSON data into a juce::var object
        juce::var profileDataJson = juce::JSON::parse (jsonString);

        return profileDataJson;
    }

private:
    juce::String profileImageToString()
    {
        juce::MemoryOutputStream outputStream;
        juce::PNGImageFormat pngFormat;

        if (!pngFormat.writeImageToStream(profileImage, outputStream)) {
            // handle error
            juce::Logger::writeToLog("Failed to write image to stream.");
            return "";
        }

        auto imageData = outputStream.getMemoryBlock();
        return "blah blah blah";
    }
    
    // TODO: Fix this function
    juce::String encryptData (const juce::String& data, const juce::String& key)
    {
        juce::String result;
        int keyIndex = 0;

        for (auto it = data.getCharPointer(); !it.isEmpty(); ++it)
        {
            result += (*it) ^ key[keyIndex];
            keyIndex = (keyIndex + 1) % key.length();
        }

        return data;
    }
};


