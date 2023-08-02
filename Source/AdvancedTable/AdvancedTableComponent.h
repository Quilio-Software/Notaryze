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
#include "DataModel.h"
#include "TableStyling.h"

#include <Security/SecStaticCode.h>

class StatusPill : public juce::Component, public juce::StatusPillTooltip, public juce::SettableTooltipClient
{

public:
    StatusPill()
    {
        juce::StatusPillTooltip toolTipStyling;
    }
    
    void drawStatusPill (juce::Graphics& g, const juce::String& text, const int& x, const int& y, const int& width, const int& height, const float& cornerSize = 10.0f)
    {
        juce::Colour colour;

        std::unordered_map<juce::String, juce::Colour> statusToColour
        {
            {"Uploading", juce::Colour::fromString ("#ffDEE833")},
            {"Unsigned", juce::Colour::fromString ("#ff2D72E1")},
            {"Signed", juce::Colour::fromString ("#ff34A700")},
            {"Success", juce::Colour::fromString ("#ff34A700")},
            {"Error", juce::Colours::red }
        };

        auto it = statusToColour.find(text);
        if (it != statusToColour.end())
        {
            colour = it->second;
        }
        else
        {
            // handle the case when the text is not found in the map
            // for example, you might want to assign a default colour
            colour = juce::Colours::black;
        }

        // Draw the text with padding
        juce::Font font (10.0f);
        g.setFont (font);
        
        int padding = 8;

        // Set up the rectangle parameters with padding

        float textWidth = font.getStringWidth (text);

        juce::Rectangle<float> textBounds (7, 6, width - 14, 20);

        // Calculate the x origin (left position) of the text
        float xOrigin = (width - textWidth) / 2.0f - padding; // + (textBounds.getWidth() - textWidth) / 2.0f;
        
  //      g.fillAll (juce::Colours::blue);

        //TODO: Fix this god awful positioning
        // It looks like the offset amount changes based on the text there...
        juce::Rectangle<float> rectangleBounds (xOrigin, 6, textWidth + 2 * padding, height - 12);

        // Draw the rounded rectangle with padding
        g.setColour (colour);
        g.drawRoundedRectangle (rectangleBounds, cornerSize, 1.0f);
        
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
        
        setTooltipHelperFunction (text);
    }
    
    void setTooltipHelperFunction (juce::String status)
    {
        std::unordered_map<juce::String, juce::String> statusToTooltip
        {
            {"Unsigned", "Unsigned"},
            {"Uploading", "File upload in progress"},
            {"Signed", "Signed by <Dev name>"},
            {"Success", "Signed by <Dev name>"},
            {"Error state 1", "Not signed in"},
            {"Error state 2", "Product sign failed"},
            {"Error state 3", "Notarization failed"},
            {"Error state 4", "Staple failed"},
            {"Error state 5", "Staple failed"},
            {"Error state 6", "Code sign failed"},
            {"Error state 7", "Connection error"},
            {"Error state 8", "Timed out"},
            {"Signing in progress", "Signing in progress"}
        };
        
        juce::String tooltipMessage;
        
        auto tooltipIterator = statusToTooltip.find (status);
        if (tooltipIterator != statusToTooltip.end()) { tooltipMessage = tooltipIterator->second; }
        setTooltip (tooltipMessage);
    }
};

class ClearCell : public juce::Component
{
    
public:
    
    ClearCell(){}
    
    void drawClearCell (juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
    {
        //load in all trash icon states
        juce::MemoryBlock svgDataDefault (BinaryData::trashIcon_Default_svg, BinaryData::trashIcon_Default_svgSize);
        juce::MemoryBlock svgDataHover (BinaryData::trashIcon_Hover_svg, BinaryData::trashIcon_Hover_svgSize);
        juce::MemoryBlock svgDataDisabled (BinaryData::trashIcon_Disabled_svg, BinaryData::trashIcon_Disabled_svgSize);
        
        auto svgDocument = juce::parseXML (juce::String (reinterpret_cast<const char*> (svgDataDefault.getData()), static_cast<size_t> (svgDataDefault.getSize())));
        auto svg = juce::Drawable::createFromSVG (*svgDocument);
        juce::Rectangle<float> trashRect (40.0f, 10.0f, 9.82f, 12.0f);
        svg->drawWithin (g, trashRect, juce::Justification::centred, 1.0f);
    }
};

class ClearButton : public juce::Component
{
    
public:
    ClearButton(){}
    
};

class AdvancedTableComponent : public juce::AnimatedAppComponent,
                               public juce::TableListBoxModel,
                               public juce::FileDragAndDropTarget,
                               public juce::DragAndDropContainer
                               
{
    TableComponentStyling tableStyle;
    




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
    
    
    juce::Image statusLoadingIconImage = juce::ImageFileFormat::loadFrom (BinaryData::statusLoadingIcon_png, BinaryData::statusLoadingIcon_pngSize);

    AdvancedTableComponent (std::vector<ColumnData> columns)
    {
        AdvancedTableComponent (columns, std::vector<RowData> ());

    }
    
    void update() override;
    
    //File operations
    void filesDropped (const juce::StringArray& files, int x, int y) override;
    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    bool checkFileTypeIsValid (juce::File file, StringVector allowedFileTypes);
    void browseForFileToUpload();
    
    StringVector allowedFileTypes;
    StringVector getAllowedFileTypes() { return allowedFileTypes; }
    void setAllowedFileTypes (StringVector fileTypesToAllow) { allowedFileTypes = fileTypesToAllow; }
    void setAllowedFileTypes (FormatLibrary::Types fileType) { allowedFileTypes = FormatLibrary::getFormats (fileType); }
    
    /*
    juce::String getStatus (juce::File file)
    {
        juce::String command = "xcrun notary -v --info " + file.getFullPathName();
        juce::String status = "Not signed";
        std::string output;

        FILE* pipe = popen(command.toRawUTF8(), "r");
        if (pipe)
        {
            char buffer[128];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            {
                output += buffer;
            }

            pclose(pipe);
        }
        else
        {
            std::cout << "Failed to run command: " << command << std::endl;
            return "Error";
        }

        if (!output.empty())
        {
            status = "Signed";
        }

        return status;
    }
     */
    
    void updateRowStatuses()
    {
        int numRows = dataList->getNumChildElements();
        
        for (auto* rowXml : dataList->getChildIterator())
        {
            auto status = rowXml->getStringAttribute ("Status");
            rowXml->setAttribute ("Status", status);
        }
        updateTable();
    }
    
    juce::String getStatus (juce::File file)
    {
        juce::String command = "codesign -dv -- \"" + file.getFullPathName() + "\"";
        juce::String status = "Unsigned";
        std::string output;

        FILE* pipe = popen (command.toRawUTF8(), "r");
        if (pipe)
        {
            char buffer[128];
            while (fgets (buffer, sizeof(buffer), pipe) != nullptr)
            {
                output += buffer;
            }

            pclose (pipe);

            // Check the output for signing information
            if (output.find ("signed") != std::string::npos)
            {
                status = "Signed";
            }
        }
        else
        {
            std::cout << "Failed to run command: " << command << std::endl;
            status = "Error";
        }

        return status;
    }

    AdvancedTableComponent (std::vector<ColumnData> columns, std::vector<RowData> data)
    {
        setLookAndFeel (&tableStyle);
        
        //When clicking the "Clear" button on the top right, we want to clear the table data.
        tableStyle.clearAllData = [&]()
        {
            dataList->clear();
            updateTable();
            
            //TODO: Move the stuff below into updatetable
            if (dataList->getNumChildElements() > 0)
                setTableState (HAS_ITEMS);
            else
                setTableState (NO_ITEMS);
        };
        
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
        table.setRowHeight (32.0f);
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
            juce::TableHeaderComponent::ColumnPropertyFlags propertyFlags;

            if (columnIndex == 3)
            {
                propertyFlags = juce::TableHeaderComponent::notResizableOrSortable;
            }
            else propertyFlags = juce::TableHeaderComponent::notResizable;

            table.getHeader().addColumn (columnXml->getStringAttribute ("Name"),
                                         columnIndex,
                                         columnXml->getIntAttribute ("Width"),
                                         50,
                                         400,
                                         propertyFlags);
            columnIndex++;
        }

        table.setMultipleSelectionEnabled (false);
        table.setClickingTogglesRowSelection (false);
        
        setFramesPerSecond (60);
        
        addAndMakeVisible (statusPill);
        addAndMakeVisible (clearCell);
        juce::TooltipWindow tooltipWindow = juce::TooltipWindow ();
    }
    
    StatusPill statusPill;
    ClearCell clearCell;
    ClearButton clearButton;
    
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
    
    
    void notarizeTable (juce::String devName, juce::String devID, bool isCodeSigning);
    
    float currentStatusIconRotationInRadians = 0.0f;
    float statusIconRotationIncrementInRadians = 0.1f;
    

    
    enum TableState {NO_ITEMS, DRAGGING, HAS_ITEMS};
    TableState tableState = NO_ITEMS;
    
    juce::Image tableBackgroundNoItems = juce::ImageFileFormat::loadFrom (BinaryData::Table_NoFiles_png, BinaryData::Table_NoFiles_pngSize);
    juce::Image tableBackgroundDragging = juce::ImageFileFormat::loadFrom (BinaryData::Table_FileHover_png, BinaryData::Table_FileHover_pngSize);
    juce::Image tableBackgroundHasItems = juce::ImageFileFormat::loadFrom (BinaryData::Table_FilesUploaded_png, BinaryData::Table_FilesUploaded_pngSize);
    
    void paint (juce::Graphics& g) override
    {
        g.setOpacity (1.0f);

        // Fill the entire component with a solid color
        g.fillAll (juce::Colour::fromString ("#ff000A1A"));
    }
    
    juce::Typeface::Ptr poppinsRegularTypeface = juce::Typeface::createSystemTypefaceFor (BinaryData::PoppinsRegular_ttf, BinaryData::PoppinsRegular_ttfSize);

    TableState getTableState() { return tableState; }
    void setTableState (TableState newState)
    {
        tableState = newState;
        if (getTableFillState())
            tableStyle.setHeaderVisibility (true);
        else
            tableStyle.setHeaderVisibility (false);
        
        repaint();
    }
    
    bool getTableFillState()
    {
        return tableState == HAS_ITEMS || dataList->getNumChildElements() > 0;
    }
    
    bool getIsDraggingToEmptyTable()
    {
        return (tableState == DRAGGING && dataList->getNumChildElements() == 0);
    }
    
    void paintOverChildren (juce::Graphics& g) override
    {
        if (tableState == NO_ITEMS)
            g.drawImage (tableBackgroundNoItems, juce::Rectangle<float> (0, 0, getWidth(), getHeight()));
        else if (tableState == DRAGGING)
            g.drawImage (tableBackgroundDragging, juce::Rectangle<float> (0, 0, getWidth(), getHeight()));
        else if (tableState == HAS_ITEMS)
            g.drawImage (tableBackgroundHasItems, juce::Rectangle<float> (0, 0, getWidth(), getHeight()));
                
        
        DBG (tableState);
        
        if (tableState == NO_ITEMS || getIsDraggingToEmptyTable())
        {
            juce::String text ("drop files to upload");
            juce::Rectangle<int> textArea (0, 0, getWidth(), getHeight());
            g.setColour (juce::Colours::white);
            juce::Font font (poppinsRegularTypeface);
            font.setHeight (24.0f);
            g.setFont (font);
            g.drawFittedText (text, textArea, juce::Justification::centred, 1);
        }
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
    void addRow (juce::String newPropertyName, juce::String newItem, juce::String newType, juce::String newStatus, juce::String newClear);
    
    int getNumRows() override { return numRows; }

    void paintRowBackground (juce::Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
    {
        auto alternateColour = getLookAndFeel().findColour (juce::ListBox::backgroundColourId)
                                               .interpolatedWith (getLookAndFeel().findColour (juce::ListBox::textColourId), 0.03f);

        
        juce::Colour rowSelectionColour = juce::Colour::fromString ("#ff000A1A");
        juce::Colour oddRowColour = juce::Colour::fromString ("#ff291523");
        juce::Colour evenRowColour = juce::Colour::fromString ("#ff000A1A");

        if (rowNumber % 2)
            g.fillAll (evenRowColour);
        else
            g.fillAll (oddRowColour);
    }
    
    //Paint Methods
//    void drawStatusPill (juce::Graphics& g, const juce::String& text, const int& x, const int& y, const int& width, const int& height, const float& cornerSize = 10.0f)
//    {
//        juce::Colour colour;
//
//        std::unordered_map<juce::String, juce::Colour> statusToColour
//        {
//            {"Uploading", juce::Colour::fromString ("#ffDEE833")},
//            {"Unsigned", juce::Colour::fromString ("#ff2D72E1")},
//            {"Signed", juce::Colour::fromString ("#ff34A700")},
//            {"Success", juce::Colour::fromString ("#ff34A700")},
//            {"Error", juce::Colours::red }
//        };
//
//        auto it = statusToColour.find(text);
//        if (it != statusToColour.end())
//        {
//            colour = it->second;
//        }
//        else
//        {
//            // handle the case when the text is not found in the map
//            // for example, you might want to assign a default colour
//            colour = juce::Colours::black;
//        }
//
//        // Draw the text with padding
//        juce::Font font (10.0f);
//        g.setFont (font);
//
//        int padding = 8;
//
//        // Set up the rectangle parameters with padding
//
//        float textWidth = font.getStringWidth (text);
//
//        juce::Rectangle<float> textBounds (7, 6, width - 14, 20);
//
//        // Calculate the x origin (left position) of the text
//        float xOrigin = (width - textWidth) / 2.0f - padding; // + (textBounds.getWidth() - textWidth) / 2.0f;
//
//  //      g.fillAll (juce::Colours::blue);
//
//        //TODO: Fix this god awful positioning
//        // It looks like the offset amount changes based on the text there...
//        juce::Rectangle<float> rectangleBounds (xOrigin, 6, textWidth + 2 * padding, height - 12);
//
//        // Draw the rounded rectangle with padding
//        g.setColour (colour);
//        g.drawRoundedRectangle (rectangleBounds, cornerSize, 1.0f);
//
//        g.drawText (text, textBounds.reduced (5), juce::Justification::centred, true);
//
//        // Center the rounded rectangle vertically within the row
//        float yOffset = (height - textBounds.getHeight()) / 2.0f;
//        textBounds.setY (y + yOffset);
//
//        // Center the rounded rectangle horizontally within the cell
//        float xOffset = (width - textBounds.getWidth()) / 2.0f;
//        textBounds.setX (x + xOffset);
//
//        // Draw the background
//        g.setColour (juce::LookAndFeel::getDefaultLookAndFeel().findColour (juce::ListBox::backgroundColourId));
//        g.fillRect (x + width - 1, y, 1, height);
//    }
    
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
    
    void cellClicked (int rowIndex, int columnId, const juce::MouseEvent&) override
    {
        //TODO: Add Mouse position check to ensure it's within the bounds of the trash icon
        if (columnId == CLEAR) //Then we've hit the trash icon
        {
            auto childXMLElement = dataList->getChildElement (rowIndex);
            DBG ("Just removed element " + juce::String (childXMLElement->getAttributeValue (2)));
            removeRow (rowIndex);
            updateTable();
        }
    }
    
    void removeRow (int rowIndex)
    {
        dataList->removeItemByIndex (rowIndex);
    }
    
    void drawStatusCell (juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
    {
        if (auto* rowElement = dataList->getChildElement (rowNumber))
        {
            auto statusText = rowElement->getStringAttribute (getAttributeNameForColumnId (columnId));
            if (statusText == "PROCESSING")
            {
                float iconWidth = statusLoadingIconImage.getWidth();
                float iconHeight = statusLoadingIconImage.getHeight();
                
                auto rotationalTransform = juce::AffineTransform::rotation (currentStatusIconRotationInRadians, iconWidth / 2.0f, iconHeight / 2.0f)
                .translated (200, height / 2.0f - iconHeight / 2.0f);
                
                g.fillAll(juce::Colours::red);
                g.drawImageTransformed (statusLoadingIconImage, rotationalTransform, false);
            }
            else
            {
                StatusPill statusPill;
                juce::TooltipWindow tooltipWindow ();
                statusPill.drawStatusPill (g, statusText, 162, 179, width, height);
            }
        }
    }
    
    //This function has to do with cell drawing
    void paintCell (juce::Graphics& g, int rowNumber, int columnId,
                    int width, int height, bool rowIsSelected) override
    {
        if (columnId == STATUS) //columnID 3 is status
        {
            drawStatusCell (g, rowNumber, columnId, width, height, rowIsSelected);
        }
        else if (columnId == CLEAR) //If we're on the Clear Column
        {
            clearCell.drawClearCell (g, rowNumber, columnId, width, height, rowIsSelected);
        }
        else
        {
            g.setColour (getLookAndFeel().findColour (juce::TableHeaderComponent::textColourId));
            g.setFont (poppinsRegularTypeface);
            g.setFont (24.0f); //TODO: MAKE THIS ALSO NOT BE FUCKING 2X WTF
            
            if (auto* rowElement = dataList->getChildElement (rowNumber))
            {
                auto text = rowElement->getStringAttribute (getAttributeNameForColumnId (columnId));

                g.drawText (text, /*2*/ 5, 0, width - 4, height, juce::Justification::centredLeft, true);
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

    //Selection
    int getSelection (const int rowNumber) const { return dataList->getChildElement (rowNumber)->getIntAttribute ("Select"); }
    void setSelection (const int rowNumber, const int newSelection) { dataList->getChildElement (rowNumber)->setAttribute ("Select", newSelection); }

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
        SelectionColumnCustomComponent (AdvancedTableComponent& td) : owner (td)
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
