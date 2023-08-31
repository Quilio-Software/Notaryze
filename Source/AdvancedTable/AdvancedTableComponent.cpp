/*
  ==============================================================================

    AdvancedTableComponent.cpp
    Created: 10 May 2023 2:53:38pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/


#include "AdvancedTableComponent.h"
#include "../Helpers/SpecialStringProcessing.h"

/*  ROW OPERATIONS  */
void AdvancedTableComponent::removeRow (int rowIndex)
{
    auto childXMLElement = dataList->getChildElement (rowIndex);
    if (childXMLElement != nullptr)
        DBG ("Removing element " + juce::String (childXMLElement->getAttributeValue (2)));
    
    dataList->removeItemByIndex (rowIndex);
    statusPills.erase (statusPills.begin() + rowIndex);
    trashButtons.erase (trashButtons.begin() + rowIndex);
    updateTable();

    if (dataList->getNumChildElements() > 0)
        setTableState (HAS_ITEMS);
    else
        setTableState (NO_ITEMS);
}

void AdvancedTableComponent::addRow (juce::String newPropertyName, juce::String newItem, juce::String newType, juce::String newStatus, juce::String newClear)
{
    if (checkItemExists (newItem)) return; //To avoid adding duplicates, we check if the item already exists.
    
    //We need to add a status pill to the vector
    statusPills.push_back (std::make_unique<StatusPill>());
    trashButtons.push_back (std::make_unique<TrashButton>());
    addAndMakeVisible (*trashButtons.back());
    addAndMakeVisible (*statusPills.back());

    dataList->addProperty (newPropertyName, newItem, newType, newStatus, newClear);
    updateTable();
    
    trashButtons.back()->onClick = [&, this]()
    {
        auto it = std::find(trashButtons.begin(), trashButtons.end(), trashButtons.back());

        if (it != trashButtons.end())
        {
            int currentRowIndex = (int) std::distance (trashButtons.begin(), it);
            removeRow(currentRowIndex);
            updateTable();
        }
    };
}

void AdvancedTableComponent::clearAllRows()
{
    dataList->clear();
    updateTable();
    
    if (dataList->getNumChildElements() > 0)
        setTableState (HAS_ITEMS);
    else
        setTableState (NO_ITEMS);
    
    statusPills.clear();
    trashButtons.clear();
}

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
        //TODO: Handle different behavior if code / product
        
        if (notaryState == CODESIGN)
        {
            if (FormatLibrary::isExecutable (file) ||

                FormatLibrary::isPlugin (file))
                addRow ("lskj", file.getFileName(), removeDotAndCapitalize (file.getFileExtension()), getStatus (file), "CLEAR");
        }
        else if (notaryState == PRODUCTSIGN)
        {
            if (FormatLibrary::isExecutable (file) ||
                FormatLibrary::isInstaller (file) ||
                FormatLibrary::isPlugin (file))
                addRow ("lskj", file.getFileName(), removeDotAndCapitalize (file.getFileExtension()), getStatus (file), "CLEAR");
        }
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






/* BATCH OPERATIONS */
//Signing happens here
void AdvancedTableComponent::notarizeTable (juce::String devName, juce::String devID, bool isCodeSigning)
{
#ifdef JUCE_MAC
    int rowCount = 0;
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
                
                //Set status pill status to row attribute
                statusPills[rowCount]->setStatus ("Error");
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
        rowCount++;
    }
#endif
    updateTable();
}

class SigningJob : public juce::ThreadPoolJob
{
public:
    SigningJob(juce::XmlElement* rowXml, const juce::String& filename,
               const juce::String& devName, const juce::String& devID, bool isCodeSigning)
    : juce::ThreadPoolJob(filename),
      rowXml(rowXml), filename(filename), devName(devName), devID(devID), isCodeSigning(isCodeSigning)
    {
    }

    JobStatus runJob() override
    {
        try
        {
            if (isCodeSigning)
            {
                DBG("Attempting codesign for file: " + filename);
                auto response = codesignVerbose(filename, devName, devID);
                
                if (response != "Success")
                {
                    DBG("Error during codesign for file: " + filename + " Response: " + response);
                    rowXml->setAttribute("Status", "Error");
                }
                else
                {
                    DBG("Successfully signed file: " + filename);
                    rowXml->setAttribute("Status", "Signed");
                    //TODO: Add codesign check stage here
                }
            }
            else
            {
                DBG("Attempting productsign for file: " + filename);
                auto response = productsignVerbose(filename, devName, devID);
                
                if (response != "Success")
                {
                    DBG("Error during productsign for file: " + filename + " Response: " + response);
                    rowXml->setAttribute("Status", "Error");
                }
                else
                {
                    DBG("Successfully signed file: " + filename);
                    rowXml->setAttribute("Status", "Signed");
                    //TODO: Add productsign check stage here
                }
            }
        }
        catch (const std::exception& e)
        {
            DBG("Exception caught during signing process for file: " + filename + " - " + e.what());
            rowXml->setAttribute("Status", "Exception");
        }
        
        return jobHasFinished;
    }

private:
    juce::XmlElement* rowXml;
    juce::String filename, devName, devID;
    bool isCodeSigning;
};

void AdvancedTableComponent::notarizeTableAsynchronously (juce::String devName, juce::String devID, bool isCodeSigning)
{
#ifdef JUCE_MAC
    juce::ThreadPool threadPool;  // Create a thread pool. You might want to set some thread limits here.

    // Queue up all the signing tasks in the thread pool
    for (auto* rowXml : dataList->getChildIterator())
    {
        auto filename = rowXml->getStringAttribute("Item");
        DBG("Queuing signing job for file: " + filename);
        threadPool.addJob(new SigningJob(rowXml, filename, devName, devID, isCodeSigning), true);
    }

    // Wait for all tasks to complete
    DBG("Waiting for all signing jobs to complete...");
    while (threadPool.getNumJobs() > 0)
    {
        juce::Thread::sleep(100);  // Sleep for a bit so we're not constantly polling.
    }
    DBG ("Thread Pool: All signing jobs completed.");
    
    updateTable();
#endif
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
