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
    };
    
    static StringVector getFormats (Types type)
    {
        StringVector formats;

        if (type == PLUGIN)
            formats = { ".vst3", ".au" };
        else if (type == AUDIO)
            formats = { ".mp3", ".wav", ".aiff", ".ogg" };
        else if (type == INSTALLER)
            formats = { ".pkg", ".dmg", ".zip" };
        else if (type == EXECUTABLE)
#ifdef JUCE_OSX
            formats = { ".app" };
#elif JUCE_WINDOWS
            formats = { ".exe" };
#endif
        else
            DBG("Format doesn't exist");

        return formats;
    }
};
