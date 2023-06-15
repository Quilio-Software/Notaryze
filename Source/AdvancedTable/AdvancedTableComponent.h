/*
  ==============================================================================

    NotaryTable.h
    Created: 4 May 2023 4:41:06pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#ifndef MYHEADER_H_INCLUDED
#define MYHEADER_H_INCLUDED

#pragma once
#include <JuceHeader.h>
#include "../Notarization/Notarization.h"
#include "../Helpers/FormatLibrary.h"

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

class TableComponentStyling : public juce::LookAndFeel_V4
{
public:
    TableComponentStyling()
    {
        setColour (juce::ListBox::backgroundColourId, juce::Colour::fromString ("#ff000A1A"));
        setColour (juce::TableHeaderComponent::textColourId, juce::Colours::white);
    }
    
    void drawTableHeaderBackground (juce::Graphics& g, juce::TableHeaderComponent& header) override
    {
        auto r = header.getLocalBounds();
        auto outlineColour = header.findColour (juce::TableHeaderComponent::outlineColourId);

        
        outlineColour = juce::Colours::red;
        
        g.setColour (outlineColour);
//        g.fillRect (r.removeFromBottom (1));

        g.setColour (header.findColour (juce::TableHeaderComponent::backgroundColourId));
//        g.fillRect (r);

        g.setColour (outlineColour);

//        for (int i = header.getNumColumns (true); --i >= 0;)
//            g.fillRect (header.getColumnPosition (i).removeFromRight (1));
        
        
    }
    
    
    void drawTableHeaderColumn (juce::Graphics& g, juce::TableHeaderComponent& header,
                                                const juce::String& columnName, int /*columnId*/,
                                                int width, int height, bool isMouseOver, bool isMouseDown,
                                                int columnFlags) override
    {
        juce::Rectangle<int> area (width, height);
        
        if (columnName != "Clear")
        {
            auto highlightColour = header.findColour (juce::TableHeaderComponent::highlightColourId);
            
            if (isMouseDown)
                g.fillAll (highlightColour);
            else if (isMouseOver)
                g.fillAll (highlightColour.withMultipliedAlpha (0.625f));
            
            
            //Set table header bounds
            area.reduce (4, 0);
            
            if ((columnFlags & (juce::TableHeaderComponent::sortedForwards | juce::TableHeaderComponent::sortedBackwards)) != 0)
            {
                juce::Path sortArrow;
                sortArrow.addTriangle (0.0f, 0.0f,
                                       0.5f, (columnFlags & juce::TableHeaderComponent::sortedForwards) != 0 ? -0.8f : 0.8f,
                                       1.0f, 0.0f);
                
                g.setColour (header.findColour (juce::TableHeaderComponent::backgroundColourId));
                g.fillPath (sortArrow, sortArrow.getTransformToScaleToFit (area.removeFromRight (height / 2).reduced (2).toFloat(), true));
            }
            
            
            //Draw table header text
            g.setColour (header.findColour (juce::TableHeaderComponent::textColourId));
            g.setFont (juce::Font ((float) height * 0.5f, juce::Font::bold));
            g.drawFittedText (columnName, area, juce::Justification::centredLeft, 1);
        }
        else
        {

            juce::Rectangle<float> roundedRectArea (width * 0.25f * 0.5f, height * 0.25f, width * 0.75f, height * 0.5f);
            
            //Handle different clear button colors here
            if (isMouseOver)
            {
                
            }
            else if (isMouseDown)
            {
                clearAllData();
            }
            
            g.drawFittedText ("CLEAR", area, juce::Justification::centred, 1);
            g.drawRoundedRectangle (roundedRectArea.toFloat(), 4, 1.0f);
        }
    }
    
    void clearAllData()
    {
        
    }
};

class AdvancedTableComponent : public juce::AnimatedAppComponent,
                               public juce::TableListBoxModel,
                               public juce::FileDragAndDropTarget,
                               public juce::DragAndDropContainer
                               
{
    TableComponentStyling tableStyle;
    
    juce::Image statusLoadingIconImage = juce::ImageFileFormat::loadFrom (BinaryData::statusLoadingIcon_png, BinaryData::statusLoadingIcon_pngSize);
    juce::Image trashIconImage = juce::ImageFileFormat::loadFrom (BinaryData::trashIcon_png, BinaryData::trashIcon_pngSize);
    
    enum ColumnNames
    {
        NONE,
        ITEM,
        TYPE,
        STATUS,
        CLEAR,
    };
    
public:
    
    struct ColumnData
    {
        juce::String name;
        int width;
    };
    
    struct RowData
    {
    public:
        juce::String item;
        juce::String type;
        juce::String status;
        juce::String clear;
    };
    
    DataModel* getDataModel()
    {
        return dataList.get();
    }
    
    AdvancedTableComponent (std::vector<ColumnData> columns)
    {
        AdvancedTableComponent (columns, std::vector<RowData> ());
    }
    
    //File operations
    void filesDropped (const juce::StringArray& files, int x, int y) override;
    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    bool checkFileTypeIsValid (juce::File file, StringVector allowedFileTypes);
    void browseForFileToUpload();
    
    StringVector allowedFileTypes;
    StringVector getAllowedFileTypes() { return allowedFileTypes; }
    void setAllowedFileTypes (StringVector fileTypesToAllow) { allowedFileTypes = fileTypesToAllow; }
    void setAllowedFileTypes (FormatLibrary::Types fileType) { allowedFileTypes = FormatLibrary::getFormats (fileType); }
    
    juce::String getStatus (juce::File file)
    {
        return "BLAH";
    }

    AdvancedTableComponent (std::vector<ColumnData> columns, std::vector<RowData> data)
    {
        setLookAndFeel (&tableStyle);
        
        // Create data model
        
        columnList = std::make_unique<ColumnModel> ("COLUMN");

        // Add columns based on input
        for (const auto& col : columns)
        {
            columnList->addColumn (col.name, col.width);
        }

        table.updateContent();
        table.setOutlineThickness (1);
        table.getHeader().setSortColumnId (1, true);
        table.setMultipleSelectionEnabled (true);
        table.setRowHeight (36.0f);
        table.setHeaderHeight (36.0f);
        addAndMakeVisible (table);
        
        resized();

        jassert (columnList != nullptr);
        
        // Create data model
        dataList = std::make_unique<DataModel> ("ITEM");
        for (const auto& row : data)
        {
            auto* childElement = new juce::XmlElement("ITEM");
            childElement->setAttribute ("Item", row.item);
            childElement->setAttribute ("Type", row.type);
            childElement->setAttribute ("Status", row.status);
            childElement->setAttribute ("Clear", row.clear);
            dataList->addChildElement (childElement);
        }
        numRows = dataList->getNumChildElements();
        
        int columnIndex = 1;
        for (auto* columnXml : columnList->getChildIterator())
        {
            table.getHeader().addColumn (columnXml->getStringAttribute ("Name"),
                                         columnIndex,
                                         columnXml->getIntAttribute ("Width"),
                                         50,
                                         400,
                                         juce::TableHeaderComponent::defaultFlags);
            columnIndex++;
        }
        
        setFramesPerSecond (60);
    }
    
    ~AdvancedTableComponent()
    {
        setLookAndFeel (nullptr);
    }
    
    //Notarization functions
    void notarizeRow (juce::String rowID, juce::String devName, juce::String devID)
    {
        DBG ("Row " + rowID + " has been set to notarize.");
        auto row = getRow (rowID);
        
        auto filename = row->getStringAttribute ("Item");
        
        auto response = codesignVerbose (filename, devName, devID);
        
        DBG ("Response: " + response);
        
        row->setAttribute ("Status", "PROCESSING");
        updateTable();
    }
    
    void signRow (juce::String rowIdentifier)
    {
        auto* rowToSign = getRow (rowIdentifier);
        
        rowToSign->setAttribute ("Status", "PROCESSING");
        updateTable();
    }
    
    void notarizeTable (juce::String devName, juce::String devID, bool isCodeSigning)
    {
        for (auto* rowXml : dataList->getChildIterator())
        {
            auto filename = rowXml->getStringAttribute ("Item");
            
            if (isCodeSigning)
            {
                auto response = codesignVerbose (filename, devName, devID);
                DBG ("Response: " + response);
            }
            else
            {
                auto response = productsignVerbose (filename, devName, devID);
                DBG ("Response: " + response);
            }
            
            rowXml->setAttribute ("Status", "PROCESSING");
        }
        updateTable();
    }
    
    float currentStatusIconRotationInRadians = 0.0f;
    float statusIconRotationIncrementInRadians = 0.1f;
    
    //Animation related behavior
    void update() override
    {
        if (currentStatusIconRotationInRadians >= juce::MathConstants<float>::pi * 2.0f)
        {
            currentStatusIconRotationInRadians = 0.0f;
        }
        else
        {
            currentStatusIconRotationInRadians += statusIconRotationIncrementInRadians;
        }
    }
    
    void paint (juce::Graphics& g) override
    {
        g.setOpacity (1.0f);

        // Fill the entire component with a solid color
        g.fillAll (juce::Colour::fromString ("#ff000A1A"));
    }
    
    
    void updateTable()
    {
        numRows = dataList->getNumChildElements();
        table.updateContent();
        resized();
    }
    
    ///==================================================================================
    
    juce::XmlElement* getRow (juce::String rowIdentifier)
    {
        for (auto* rowXml : dataList->getChildIterator())
        {
            if (rowXml->getTagName() == rowIdentifier)
            {
                return rowXml;
            }
        }
        
        return {};
    }
    
    //Table Population
    void addRow (juce::String newPropertyName, juce::String newItem, juce::String newType, juce::String newStatus, juce::String newClear)
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
    
    int getNumRows() override
    {
        return numRows;
    }

    void paintRowBackground (juce::Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
    {
        auto alternateColour = getLookAndFeel().findColour (juce::ListBox::backgroundColourId)
                                               .interpolatedWith (getLookAndFeel().findColour (juce::ListBox::textColourId), 0.03f);
        if (rowIsSelected)
            g.fillAll (juce::Colours::lightblue);
        else if (rowNumber % 2)
            g.fillAll (alternateColour);
    }
    
    
    
    //Paint Methods
    void drawStatusPill (juce::Graphics& g, const juce::String& text, const int& x, const int& y, const int& width, const int& height, const juce::Colour& colour = juce::Colours::green, const float& cornerSize = 10.0f) {
        // Set up the rectangle parameters with padding
        juce::Rectangle<float> textBounds (2 + 5, 7.5, width - 4 - 10, 20);

        // Draw the rounded rectangle with padding
        g.setColour (colour);
        g.drawRoundedRectangle (textBounds.reduced (1), cornerSize, 1.0f);

        // Draw the text with padding
        juce::Font font (10.0f);
        g.setFont (font);

        g.drawText (text, textBounds.reduced (5), juce::Justification::centred, true);

        // Center the rounded rectangle vertically within the row
        float yOffset = (height - textBounds.getHeight()) / 2.0f;
        textBounds.setY (y + yOffset);

        // Center the rounded rectangle horizontally within the cell
        float xOffset = (width - textBounds.getWidth()) / 2.0f;
        textBounds.setX (x + xOffset);

        // Draw the background
        g.setColour (juce::LookAndFeel::getDefaultLookAndFeel().findColour (juce::ListBox::backgroundColourId));
        g.fillRect (x + width - 1, y, 1, height);
    }
    
    void drawCenteredFilledSquare (juce::Graphics& g, const int& componentX, const int& componentY, const int& componentWidth, const int& componentHeight, const int& paddingTop, const int& paddingBottom, const juce::Colour& colour = juce::Colours::green)
    {
        // Calculate the center position of the component
        int centerX = componentX + (componentWidth / 2);
        int centerY = componentY + (componentHeight / 2);

        // Calculate the size of the square (available height minus padding)
        int availableHeight = componentHeight - paddingTop - paddingBottom;
        int size = std::min(componentWidth, availableHeight);

        // Calculate the position of the square to be centered within the component (with padding)
        int squareX = centerX - (size / 2);
        int squareY = componentY + paddingTop + ((availableHeight - size) / 2);

        // Draw the filled square
        g.setColour(colour);
        g.fillRect(squareX, squareY, size, size);

        // Draw the background
        g.setColour(juce::LookAndFeel::getDefaultLookAndFeel().findColour(juce::ListBox::backgroundColourId));
        g.fillRect(componentX + componentWidth - 1, componentY, 1, componentHeight);
    }
    
    void cellClicked (int rowNumber, int columnId, const juce::MouseEvent&) override
    {
        //TODO: Add Mouse position check to ensure it's within the bounds of the trash icon
        if (columnId == CLEAR) //Then we've hit the trash icon
        {
            auto childXMLElement = dataList->getChildElement (rowNumber);
            DBG ("Just removed element " + juce::String (childXMLElement->getAttributeValue (2)));
            dataList->removeItemByIndex (rowNumber);
            updateTable();
        }
    }
    
    //This function has to do with cell drawing
    void paintCell (juce::Graphics& g, int rowNumber, int columnId,
                    int width, int height, bool rowIsSelected) override
    {
        if (columnId == STATUS) //columnID 3 is status
        {
            if (auto* rowElement = dataList->getChildElement (rowNumber))
            {
                auto text = rowElement->getStringAttribute (getAttributeNameForColumnId (columnId));
                if (text == "PROCESSING")
                {
                    float iconWidth = statusLoadingIconImage.getWidth();
                    float iconHeight = statusLoadingIconImage.getHeight();
                    
                    auto rotationalTransform = juce::AffineTransform::rotation (currentStatusIconRotationInRadians, iconWidth / 2.0f, iconHeight / 2.0f)
                    .translated (0, height / 2.0f - iconHeight / 2.0f);
                    
                    g.drawImageTransformed (statusLoadingIconImage, rotationalTransform, false);
                }
                else
                {
                    drawStatusPill (g, text, 162, 179, width, height);
                }
            }
        }
        else if (columnId == CLEAR) //If we're on the Clear Column
        {
            //draw the trash can image... or position a button?
            float iconWidth = trashIconImage.getWidth();
            float iconHeight = trashIconImage.getHeight();
            
            g.drawImageWithin (trashIconImage, width / 2.0f - 9.8 * 0.5, height / 2.0f - 12 * 0.5, 9.8, 12, juce::Justification::centred);
        }
        else
        {
            g.setColour (rowIsSelected ? juce::Colours::darkblue : getLookAndFeel().findColour (juce::ListBox::textColourId));
            g.setFont (font);
            
            if (auto* rowElement = dataList->getChildElement (rowNumber))
            {
                auto text = rowElement->getStringAttribute (getAttributeNameForColumnId (columnId));
                g.setColour (getLookAndFeel().findColour (juce::TableHeaderComponent::textColourId));
                g.drawText (text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
            }
            
            g.setColour (getLookAndFeel().findColour (juce::ListBox::backgroundColourId));
        }
    }

    void sortOrderChanged (int newSortColumnId, bool isForwards) override
    {
        if (newSortColumnId != 0)
        {
            TutorialDataSorter sorter (getAttributeNameForColumnId (newSortColumnId), isForwards);
            dataList->sortChildElements (sorter);

            table.updateContent();
        }
    }

    Component* refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/,
                                        Component* existingComponentToUpdate) override
    {
        if (columnId == 9)  // [8]
        {
            auto* selectionBox = static_cast<SelectionColumnCustomComponent*> (existingComponentToUpdate);

            if (selectionBox == nullptr)
                selectionBox = new SelectionColumnCustomComponent (*this);

            selectionBox->setRowAndColumn (rowNumber, columnId);
            return selectionBox;
        }

        if (columnId == 8)  // [9]
        {
            auto* textLabel = static_cast<EditableTextCustomComponent*> (existingComponentToUpdate);

            if (textLabel == nullptr)
                textLabel = new EditableTextCustomComponent (*this);

            textLabel->setRowAndColumn (rowNumber, columnId);
            return textLabel;
        }

        jassert (existingComponentToUpdate == nullptr);
        return nullptr;     // [10]
    }

    int getColumnAutoSizeWidth (int columnId) override
    {
        if (columnId == 9)
            return 50;

        int widest = 32;

        for (auto i = getNumRows(); --i >= 0;)
        {
            if (auto* rowElement = dataList->getChildElement (i))
            {
                auto text = rowElement->getStringAttribute (getAttributeNameForColumnId (columnId));

                widest = juce::jmax (widest, font.getStringWidth (text));
            }
        }

        return widest + 8;
    }

    int getSelection (const int rowNumber) const
    {
        return dataList->getChildElement (rowNumber)->getIntAttribute ("Select");
    }

    void setSelection (const int rowNumber, const int newSelection)
    {
        dataList->getChildElement (rowNumber)->setAttribute ("Select", newSelection);
    }

    juce::String getText (const int columnNumber, const int rowNumber) const
    {
        return dataList->getChildElement (rowNumber)->getStringAttribute (getAttributeNameForColumnId (columnNumber));
    }

    void setText (const int columnNumber, const int rowNumber, const juce::String& newText)
    {
        const auto& columnName = table.getHeader().getColumnName (columnNumber);
        dataList->getChildElement (rowNumber)->setAttribute (columnName, newText);
    }

    //==============================================================================
    void resized() override
    {
        table.setBoundsInset (juce::BorderSize<int> (8));
    }

private:
    
    juce::TableListBox table  { {}, this };
    
    juce::Font font           { 14.0f };
    juce::Font statusPillFont { 12.0f };

    std::unique_ptr<ColumnModel> columnList;
    std::unique_ptr<DataModel> dataList;
    
    int numRows = 0;

    //==============================================================================
    class EditableTextCustomComponent  : public juce::Label
    {
    public:
        EditableTextCustomComponent (AdvancedTableComponent& td)
            : owner (td)
        {
            setEditable (false, true, false);
        }

        void mouseDown (const juce::MouseEvent& event) override
        {
            owner.table.selectRowsBasedOnModifierKeys (row, event.mods, false);

            Label::mouseDown (event);
        }

        void textWasEdited() override
        {
            owner.setText (columnId, row, getText());
        }

        void setRowAndColumn (const int newRow, const int newColumn)
        {
            row = newRow;
            columnId = newColumn;
            setText (owner.getText(columnId, row), juce::dontSendNotification);
        }

    private:
        AdvancedTableComponent& owner;
        int row, columnId;
        juce::Colour textColour;
    };

    //==============================================================================
    class SelectionColumnCustomComponent    : public Component
    {
    public:
        SelectionColumnCustomComponent (AdvancedTableComponent& td)
            : owner (td)
        {
            addAndMakeVisible (toggleButton);

            toggleButton.onClick = [this] { owner.setSelection (row, (int) toggleButton.getToggleState()); };
        }

        void resized() override
        {
            toggleButton.setBoundsInset (juce::BorderSize<int> (2));
        }

        void setRowAndColumn (int newRow, int newColumn)
        {
            row = newRow;
            columnId = newColumn;
            toggleButton.setToggleState ((bool) owner.getSelection (row), juce::dontSendNotification);
        }

    private:
        AdvancedTableComponent& owner;
        juce::ToggleButton toggleButton;
        int row, columnId;
    };

    //==============================================================================
    class TutorialDataSorter
    {
    public:
        TutorialDataSorter (const juce::String& attributeToSortBy, bool forwards)
            : attributeToSort (attributeToSortBy),
              direction (forwards ? 1 : -1)
        {}

        int compareElements (juce::XmlElement* first, juce::XmlElement* second) const
        {
            auto result = first->getStringAttribute (attributeToSort)
                                .compareNatural (second->getStringAttribute (attributeToSort)); // [1]

            if (result == 0)
                result = first->getStringAttribute ("ID")
                               .compareNatural (second->getStringAttribute ("ID"));             // [2]

            return direction * result;                                                          // [3]
        }

    private:
        juce::String attributeToSort;
        int direction;
    };

    //==============================================================================


    juce::String getAttributeNameForColumnId (const int columnId) const
    {
        if (dataList == nullptr) return {};
        
        for (auto* columnXml : columnList->getChildIterator())
        {
            if (columnXml->getIntAttribute ("ColumnId") == columnId)
                return columnXml->getStringAttribute ("Name");
        }

        return {};
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdvancedTableComponent)
};


#endif //AdvancedTableComponent_H
