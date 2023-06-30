/*
  ==============================================================================

    AdvancedTableComponent.cpp
    Created: 10 May 2023 2:53:38pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/


#include "AdvancedTableComponent.h"
#include "../Helpers/SpecialStringProcessing.h"


void AdvancedTableComponent::filesDropped (const juce::StringArray& files, int x, int y)
{
    for (auto filePath : files)
    {
        if (isInterestedInFileDrag (files))
        {
            auto file = juce::File (filePath);
            addRow ("lskj", file.getFileName(), removeDotAndCapitalize (file.getFileExtension()), getStatus (file), "CLEAR");
            setTableState (HAS_ITEMS);
        }
    }
}

void AdvancedTableComponent::browseForFileToUpload()
{
    juce::FileChooser chooser ("Select a file to open...",
                                    {},
                                    ""); // No wildcard pattern

    if (chooser.browseForFileToOpen())
    {
        juce::File file (chooser.getResult());
        addRow ("lskj", file.getFileName(), removeDotAndCapitalize (file.getFileExtension()), getStatus (file), "CLEAR");
    }
}

bool AdvancedTableComponent::checkFileTypeIsValid (juce::File file, StringVector allowedFileTypes)
{
    for (juce::String fileType : allowedFileTypes)
    {
        if (file.getFileExtension().contains (fileType))
        {
            return true;
        }
    }
    return false;
}

bool AdvancedTableComponent::isInterestedInFileDrag (const juce::StringArray& files)
{
    for (auto file : files)
    {
        if (checkFileTypeIsValid (juce::File (file), getAllowedFileTypes()))
        {
            setTableState (DRAGGING);
            return true;
        }
    }
    return false;
}
