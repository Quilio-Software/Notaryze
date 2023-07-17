/*
  ==============================================================================

    DataModel.h
    Created: 17 Jul 2023 6:05:55pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once

// A table requires a column model and a data model
class ColumnModel : public juce::XmlElement
{
    std::vector<juce::XmlElement*> columnElement;
    
public:
    
    ColumnModel (juce::String columnModelName) : juce::XmlElement (columnModelName)
    {
    }
    
    void addColumn (juce::String columnName, int width)
    {
        columnElement.push_back (createNewChildElement (columnName));
        columnElement.back()->setAttribute ("ColumnId", (int) columnElement.size());
        columnElement.back()->setAttribute ("Name", columnName);
        columnElement.back()->setAttribute ("Width", width);
    }
};


//TODO: Fix issues with lack of plurality... dataElement should be something like xmlVector
//Also, what in the world is the distinction between vector and array, from a high level perspective?
//They are storage elements at the end of the day, regardless of what operations they allow.
class DataModel : public juce::XmlElement
{
    std::vector<juce::XmlElement*> dataElement;
    
public:
    
    enum Columns { ID, ITEM, TYPE, STATUS, CLEAR };
    
    DataModel (juce::String columnModelName) : juce::XmlElement (columnModelName)
    {
    }
    
    void addProperty (juce::String propertyName, juce::String newItem, juce::String newType, juce::String newStatus, juce::String newClear)
    {
        dataElement.push_back (createNewChildElement (propertyName));
        dataElement.back()->setAttribute ("ColumnId", (int) dataElement.size());
        dataElement.back()->setAttribute ("Item", newItem);
        dataElement.back()->setAttribute ("Type", newType);
        dataElement.back()->setAttribute ("Status", newStatus);
        dataElement.back()->setAttribute ("Clear", newClear);
    }
    
    void clear()
    {
        dataElement.clear();
        deleteAllChildElements();
    }
    
    void removeItemByName (juce::String itemToRemove)
    {
        for (auto it = dataElement.begin(); it != dataElement.end(); ++it)
        {
            if ((juce::String) ((*it)->getAttributeValue (ITEM)) == itemToRemove)
            {
                dataElement.erase (it);
                break; // Optional, if you only want to remove the first matching element
            }
        }
    }
    
    void removeItemByIndex (const int indexToRemove)
    {
        //TODO: Fix weird double removal thing here
        removeChildElement (getChildElement(indexToRemove), true);
        dataElement.erase (dataElement.begin() + indexToRemove);
        DBG ("Data Element List now has " + juce::String (dataElement.size()) + " elements");
    }
};
