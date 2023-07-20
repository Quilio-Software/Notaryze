/*
  ==============================================================================

    AdvancedTableComponent.cpp
    Created: 10 May 2023 2:53:38pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/


#include "AdvancedTableComponent.h"
#include "../Helpers/SpecialStringProcessing.h"

/* FILE RELATED STUFF */
void AdvancedTableComponent::filesDropped (const juce::StringArray& files, int x, int y)
{
    for (auto filePath : files)
    {
        if (isInterestedInFileDrag (files))
        {
            auto file = juce::File (filePath);
            addRow ("lskj", file.getFileName(), removeDotAndCapitalize (file.getFileExtension()), getStatus (file), "CLEAR");
            
            if (numRows > 0)
                setTableState (HAS_ITEMS);
            else
                setTableState (NO_ITEMS);
        }
    }
}

void AdvancedTableComponent::browseForFileToUpload()
{
    juce::FileChooser chooser ("Select a file to open...", {}, ""); // No wildcard pattern

    if (chooser.browseForFileToOpen()) //If file is selected
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
/*        */



/*  ROW OPERATIONS  */
void AdvancedTableComponent::addRow (juce::String newPropertyName, juce::String newItem, juce::String newType, juce::String newStatus, juce::String newClear)
{
    //check if file already exists, to avoid adding duplicates
    for (auto* rowXml : dataList->getChildIterator())
    {
        if (newItem == rowXml->getStringAttribute ("Item"))
        {
            //Item already exists
            return;
        }
    }

    dataList->addProperty (newPropertyName, newItem, newType, newStatus, newClear);
    updateTable();
}


/* BATCH OPERATIONS */
//Signing happens here
void AdvancedTableComponent::notarizeTable (juce::String devName, juce::String devID, bool isCodeSigning)
{
#ifdef JUCE_MAC
    for (auto* rowXml : dataList->getChildIterator())
    {
        auto filename = rowXml->getStringAttribute ("Item");
        if (isCodeSigning)
        {
            DBG ("ATTEMPTING CODESIGN");
            auto response = codesignVerbose (filename, devName, devID);
            DBG ("Response: " + response);
            
            if (response != "Success")
            {
                rowXml->setAttribute ("Status", "Error");
            }
            else
            {
                //TODO: Add codesign check stage here
                rowXml->setAttribute ("Status", "Signed");
            }
        }
        else
        {
            DBG ("ATTEMPTING PRODUCTSIGN");
            auto response = productsignVerbose (filename, devName, devID);
            DBG ("Response: " + response);
            
            if (response != "Success")
            {
                rowXml->setAttribute ("Status", "Error");
            }
            else
            {
                //TODO: Add codesign check stage here
                rowXml->setAttribute ("Status", "Signed");
            }
        }
    }
#endif
    updateTable();
}







/* STATE Related  */
//Animation related behavior
void AdvancedTableComponent::update()
{
    if (currentStatusIconRotationInRadians >= juce::MathConstants<float>::pi * 2.0f)
    {
        currentStatusIconRotationInRadians = 0.0f;
    }
    else
    {
        currentStatusIconRotationInRadians += statusIconRotationIncrementInRadians;
    }
    
    repaint();
    
    updateRowStatuses();
}
