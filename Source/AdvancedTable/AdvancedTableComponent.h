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
#include "../Helpers/Components.h"

#ifdef JUCE_MAC
    #include <Security/SecStaticCode.h>
#endif

class TrashButton : public juce::Component
{
    //load in all trash icon states
    std::unique_ptr<juce::XmlElement> svgDefaultDocument;
    std::unique_ptr<juce::XmlElement> svgHoverDocument;
    std::unique_ptr<juce::XmlElement> svgDisabledDocument;

    bool isHovering = false;
    bool isDisabled = false;
    
    juce::Rectangle<float> trashRect;
    
public:
    TrashButton()
    {
        juce::MemoryBlock svgDataDefault (BinaryData::trashIcon_Default_svg, BinaryData::trashIcon_Default_svgSize);
        juce::MemoryBlock svgDataHover (BinaryData::trashIcon_Hover_svg, BinaryData::trashIcon_Hover_svgSize);
        juce::MemoryBlock svgDataDisabled (BinaryData::trashIcon_Disabled_svg, BinaryData::trashIcon_Disabled_svgSize);
        
        svgDefaultDocument = juce::parseXML (juce::String (reinterpret_cast<const char*> (svgDataDefault.getData()), static_cast<size_t> (svgDataDefault.getSize())));
        svgHoverDocument = juce::parseXML (juce::String (reinterpret_cast<const char*> (svgDataHover.getData()), static_cast<size_t> (svgDataHover.getSize())));
        svgDisabledDocument = juce::parseXML (juce::String (reinterpret_cast<const char*> (svgDataDisabled.getData()), static_cast<size_t> (svgDataDisabled.getSize())));
        
        trashRect = juce::Rectangle<float> (25.0f, 10.0f, 9.82f, 12.0f);
    }
    
    void paint (juce::Graphics& g) override
    {
        drawTrashButton (g);
    }
    
    bool hitTest (int x, int y) override
    {
        if ((x < 40 && x < 49.82) && (y > 10.0f && y < 22.0f))
        {
            isHovering = true;
            repaint();
            return true;
        }
        
        isHovering = false;
        repaint();
        return false;
    }
    
    void mouseDown (const juce::MouseEvent& event) override
    {
        onClick();
    }
    
    void setDisabled()
    {
        isDisabled = true;
        repaint();
    }
    
    void drawTrashButton (juce::Graphics& g)
    {
        if (isDisabled)
        {
            auto svgDisabled = juce::Drawable::createFromSVG (*svgDisabledDocument);
            svgDisabled->drawWithin (g, trashRect, juce::Justification::centred, 1.0f);
        }
        else if (isHovering)
        {
            auto svgHover = juce::Drawable::createFromSVG (*svgHoverDocument);
            svgHover->drawWithin (g, trashRect, juce::Justification::centred, 1.0f);
        }
        else
        {
            auto svgDefault = juce::Drawable::createFromSVG (*svgDefaultDocument);
            svgDefault->drawWithin (g, trashRect, juce::Justification::centred, 1.0f);
        }
    }
    
    std::function<void()> onClick;
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
#ifdef JUCE_MAC
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
#endif
        return "WINDOWS";
    }
    
    void clearAllRows();

    AdvancedTableComponent (std::vector<ColumnData> columns, std::vector<RowData> data)
    {
        setLookAndFeel (&tableStyle);
        
        //When clicking the "Clear" button on the top right, we want to clear the table data.
        tableStyle.clearAllData = [&]()
        {
            clearAllRows();
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
        
        //tooltipWindow settings
        addAndMakeVisible (tooltipWindow);
        tooltipWindow.setMillisecondsBeforeTipAppears (0);
    }
    
    juce::TooltipWindow tooltipWindow;
    
    
    //We maintain a list of status pills, and add to this list when a new dataItem is added.
    //The index of the statuspill links to the index of the row.
    std::vector<std::unique_ptr<StatusPill>> statusPills;
    std::vector<std::unique_ptr<TrashButton>> trashButtons;
    
    ~AdvancedTableComponent()
    {
        setLookAndFeel (nullptr);
    }
    
    bool checkItemExists (juce::String newItem)
    {
        for (auto* rowXml : dataList->getChildIterator())
        {
            if (newItem == rowXml->getStringAttribute ("Item"))
            {
                //Item already exists
                return true;
            }
        }
        return false;
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
    }
    
    void removeRow (int rowIndex);
    
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
   //             statusPill.drawStatusPill (g, statusText, 162, 179, width, height);
            }
        }
    }
    
    //This function has to do with cell drawing
    void paintCell (juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override
    {
        if (columnId == STATUS) //columnID 3 is status
        {
            drawStatusCell (g, rowNumber, columnId, width, height, rowIsSelected);
        }
        else if (columnId == CLEAR) //If we're on the Clear Column
        {
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
        
        int cellWidth = getColumnAutoSizeWidth (2); int cellHeight = 50;
        
        //We distribute the status pills vertically, as per the row index
        for (int rowIndex = 0; rowIndex < numRows; ++rowIndex)
        {
            juce::Rectangle<int> statusPillCellRect = table.getCellPosition (3, rowIndex, false);
            statusPillCellRect.translate (8, 8);
            statusPills[rowIndex]->setBounds (statusPillCellRect);
            
            juce::Rectangle<int> trashButtonCellRect = table.getCellPosition (4, rowIndex, false);
            trashButtonCellRect.translate (8, 8);
            trashButtons[rowIndex]->setBounds (trashButtonCellRect);
        }
//        drawableComposite.setBoundingBox (getBounds().toFloat());
//        drawableComposite.setContentArea (getBounds().toFloat());
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
