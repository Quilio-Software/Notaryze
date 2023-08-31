/*
  ==============================================================================

    Notarize.h
    Created: 17 Dec 2022 1:34:39pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once
#ifdef JUCE_WINDOWS
#include <io.h>
#elif JUCE_OSX
#include <unistd.h>
#endif
#include <filesystem>
#include <thread>
#include "ErrorHandling.h"

enum NotaryState
{
    CODESIGN,
    PRODUCTSIGN
};

inline juce::String checkIdentityFound()
{
    
}

inline int notarize (const juce::File file, const juce::String& email, const juce::String& password, const juce::String& teamID)
{
    juce::File parentDir = file.getParentDirectory();

    juce::String fileNameWithoutExtension = file.getFileNameWithoutExtension();
    juce::String fileExtension = file.getFileExtension();
    juce::String signedFileName = parentDir.getFullPathName() + "/" + fileNameWithoutExtension + " (signed)" + fileExtension;

    juce::String changeDirCommand = juce::String ("cd ") + parentDir.getFullPathName();
    juce::String command = "xcrun notarytool submit '" + file.getFullPathName() + "' --apple-id " + email + " --password " + password + " --team-id " + teamID;

    DBG (command);
    
    FILE* pipe;
    #ifdef JUCE_WINDOWS
        pipe = _popen (command.toUTF8(), "r");
    #elif JUCE_OSX
        pipe = popen (command.toUTF8(), "r");
    #endif

    if (!pipe)
    {
        return -1;
    }

    // Read the output of the command in real time
    
    char buffer[128];
    int progress = 0;
    while (fgets (buffer, 128, pipe) != NULL)
    {
        DBG (buffer);
        fflush (stdout);
    }
     
    // Close the pipe
    #ifdef JUCE_WINDOWS
        _pclose (pipe);
    #elif JUCE_OSX
        pclose (pipe);
    #endif
    
    // Return immediately
    return 0;
}

inline int runCommand (const juce::String& command)
{
    int result = std::system (command.toUTF8());
    return (result);
}





inline juce::String runCommandVerbose (const juce::String& command)
{
    std::ostringstream outputStream;

    // Modify the command to redirect stderr to stdout
    juce::String modifiedCommand = command + " 2>&1";

    FILE* pipe;
    
#ifdef JUCE_WINDOWS
    pipe = _popen(modifiedCommand.toRawUTF8(), "r");
#elif JUCE_MAC
    pipe = popen(modifiedCommand.toRawUTF8(), "r");
#endif

    if (!pipe)
    {
        std::cout << "Error executing command: " << command << std::endl;
        return {};
    }

    // Redirect the pipe output to a file stream
    FILE* output = fdopen (fileno(pipe), "r");
    if (!output)
    {
    #ifdef JUCE_WINDOWS
        _pclose (pipe);
    #elif JUCE_MAC
        pclose (pipe);
    #endif
        std::cout << "Error redirecting output for command: " << command << std::endl;
        return {};
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), output) != nullptr)
    {
        outputStream << buffer;
    }

    #ifdef JUCE_WINDOWS
        _pclose (pipe);
    #elif JUCE_MAC
        pclose (pipe);
    #endif

    return juce::String(outputStream.str());
}


inline int codesign (const juce::String& filename, const juce::String& name, const juce::String& teamID)
{
    // Keep in mind the " (" demarcation, necessary to maintain spacing.
    return runCommand ("codesign --force -s 'Developer ID Application: " + name + " (" + teamID + ")" + "' -v '" + filename + "' --deep --strict --options=runtime --timestamp");
}


//TODO: Make response an object with variables: 1. Message 2. SuccessState (Pass / Fail)
inline juce::String codesignVerbose (const juce::String& filename, const juce::String& name, const juce::String& teamID)
{
    // Keep in mind the " (" demarcation, necessary to maintain spacing.
    auto response =  runCommandVerbose ("codesign -dvvv --force -s 'Developer ID Application: " + name + " (" + teamID + ")" + "' -v '" + filename + "' --deep --strict --options=runtime --timestamp");
    DBG ("/n");
    DBG ("Response is: " + response);
    auto result = NotarizationErrorChecker::getResult (response);
    DBG ("/n");
    return result;
}

inline juce::String productsignVerbose (const juce::File& file, const juce::String& name, const juce::String& teamID)
{
    juce::File parentDir = file.getParentDirectory();
    juce::String fileExtension = file.getFileExtension();
    juce::String fileNameWithoutExtension = file.getFileNameWithoutExtension();
    juce::String unsignedFileName = parentDir.getFullPathName() + "/" + fileNameWithoutExtension + fileExtension;
    juce::String signedFileName = parentDir.getFullPathName() + "/" + fileNameWithoutExtension + " (signed)" + fileExtension;
    juce::String devName = name;
    juce::String devID = teamID;
    auto result = runCommandVerbose ("productsign --sign 'Developer ID Installer: " + devName + " (" + devID + ")" + "' '" + unsignedFileName + "' '" + signedFileName + "'");
    return result;
}

//PRT this adds the (signed) suffix
inline int staple (const juce::String& filename)
{
    juce::String input_filename = filename + " (signed).pkg";
    return runCommand ("xcrun stapler staple " + filename);
}

inline RESULT_CODE RunProcess (juce::File file, juce::String email, juce::String password, juce::String teamID, juce::String name, NotaryState notaryState)
{
    auto fileName = file.getFileNameWithoutExtension();
    auto formatName = file.getFileExtension();
    
//    juce::String filePath = juce::File::File::getSpecialLocation (juce::File::SpecialLocationType::currentExecutableFile).getParentDirectory().getParentDirectory().getParentDirectory().getParentDirectory().getFullPathName();
  
    juce::String quotedFileName = file.getFullPathName();

    int exitCode;
    if (notaryState == NotaryState::CODESIGN)
    {
        exitCode = codesign (quotedFileName, name, teamID);
        if (exitCode != 0)
            return INVALID_CREDENTIALS;
    }
    
    if (notaryState == NotaryState::PRODUCTSIGN)
    {
  //      exitCode = productsign (file, name, teamID);
  //      if (exitCode != 0) return PRODUCTSIGN_FAILED;
        exitCode = notarize (file, email, password, teamID);
        if (exitCode != 0) return NOTARIZATION_FAILED;
 //       exitCode = staple (quotedFileName);
 //       if (exitCode == 0) return STAPLE_FAILED;
    }
        
    return SUCCESS;
}

class NotarizationKernel
{
    juce::String name;
    juce::String teamID;
    juce::String email;
    juce::String password;
    
public:
    
    NotarizationKernel()
    {
        
    }
    
    RESULT_CODE RunProcess (juce::String filePath, NotaryState notaryState)
    {
        juce::File file (filePath);
        
        auto fileName = file.getFileNameWithoutExtension();
        auto formatName = file.getFileExtension();
        
    //    juce::String filePath = juce::File::File::getSpecialLocation (juce::File::SpecialLocationType::currentExecutableFile).getParentDirectory().getParentDirectory().getParentDirectory().getParentDirectory().getFullPathName();
      
        juce::String quotedFileName = file.getFullPathName();

        int exitCode;
        if (notaryState == NotaryState::CODESIGN)
        {
            exitCode = codesign (quotedFileName, name, teamID);
            if (exitCode != 0)
                return INVALID_CREDENTIALS;
        }
        
        if (notaryState == NotaryState::PRODUCTSIGN)
        {
      //      exitCode = productsign (file, name, teamID);
      //      if (exitCode != 0) return PRODUCTSIGN_FAILED;
            exitCode = notarize (file, email, password, teamID);
            if (exitCode != 0) return NOTARIZATION_FAILED;
     //       exitCode = staple (quotedFileName);
     //       if (exitCode == 0) return STAPLE_FAILED;
        }
            
        return SUCCESS;
    }
    
    
    int runCommand (const juce::String& command)
    {
        int result = std::system (command.toUTF8());
        return (result);
    }

    int codesign (const juce::String& filename, const juce::String& name, const juce::String& teamID)
    {
        // Keep in mind the " (" demarcation, necessary to maintain spacing.
        return runCommand ("codesign --force -s 'Developer ID Application: " + name + " (" + teamID + ")" + "' -v '" + filename + "' --deep --strict --options=runtime --timestamp");
    }

    int productsign (const juce::File& file, const juce::String& name, const juce::String& teamID)
    {
        juce::File parentDir = file.getParentDirectory();
        juce::String fileExtension = file.getFileExtension();
        juce::String fileNameWithoutExtension = file.getFileNameWithoutExtension();
        juce::String unsignedFileName = parentDir.getFullPathName() + "/" + fileNameWithoutExtension + fileExtension;
        juce::String signedFileName = parentDir.getFullPathName() + "/" + fileNameWithoutExtension + " (signed)" + fileExtension;
        
    //    return runCommand ("productsign --sign 'Developer ID Installer: " + name + "(" + teamID + ")" + "' '" + unsignedFileName + "' '" + signedFileName + "'");
        
        juce::String devName = name;
        juce::String devID = teamID;
        return runCommand ("productsign --sign 'Developer ID Installer: " + devName + " (" + devID + ")" + "' '" + unsignedFileName + "' '" + signedFileName + "'");
    }
};
