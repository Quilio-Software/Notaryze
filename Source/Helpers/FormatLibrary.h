/*
  ==============================================================================

    FormatLibrary.h
    Created: 18 May 2023 2:15:10pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once

using StringVector = std::vector<juce::String>;

class FormatLibrary
{
public:
    
    enum Types
    {
        PLUGIN,
        AUDIO,
        INSTALLER,
        EXECUTABLE,
        IMAGE,
        TEXT,
    };
    
    static StringVector getFormats (Types type)
    {
        StringVector formats;

        if (type == PLUGIN)
            formats = { ".vst3", ".au" };
        else if (type == AUDIO)
            formats = { ".mp3", ".wav", ".aiff", ".ogg" };
        else if (type == INSTALLER)
#ifdef JUCE_MAC
            formats = { ".pkg", ".dmg", ".zip" };
#elif JUCE_WINDOWS
            formats = {".exe"};
#endif
        else if (type == IMAGE)
            formats = { ".jpeg", ".png", ".jpg"};
        else if (type == EXECUTABLE)
#ifdef JUCE_MAC
            formats = { ".app" };
#elif JUCE_WINDOWS
            formats = { ".exe" };
#else
            formats = {};
#endif
        else if (type == TEXT)
        {
            formats = {".txt", ".rtf", ".pdf", ".otf"};
        }
        else
            DBG ("Format doesn't exist");

        return formats;
    }
    
    Types getFormatType (juce::String fileExtension)
    {
        for (int formatTypeID = 0; formatTypeID < 5; ++formatTypeID)
        {
            auto formatType = static_cast<Types> (formatTypeID);
            auto formatExtensions = getFormats (formatType);
            
            for (auto formatExtension : formatExtensions)
            {
                if (fileExtension == formatExtension)
                {
                    return formatType;
                }
            }
        }
    }
};
