/*
  ==============================================================================

    UtilityScreen.h
    Created: 4 May 2023 4:41:34pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once
#include "../Helpers/Components.h"
#include "../AdvancedTable/AdvancedTableComponent.h"
#include "../Helpers/FormatLibrary.h"



enum TableState
{
    CODE_SIGN, //This is purely dev ID based codesigning
    PRODUCT_SIGN, //This is notarization + stapling
};

class UtilityScreen : public Screen
{
    RoundedFillButtonStyling roundedFillStyling;
    RoundedOutlineButtonStyling roundedOutlineStyling;
    UnderlinedButtonStyling underlinedButtonStyling;
    
    juce::Image quilioLogoImage = juce::ImageFileFormat::loadFrom (BinaryData::quilioLogo_4x_png, BinaryData::quilioLogo_4x_pngSize);
    
    juce::ImageButton quilioLogoButton;
    juce::ImageButton profileButton;
    
    juce::TextButton codeSignButton {"CODE SIGN"}, productSignButton {"PRODUCT SIGN"};
    juce::TextButton uploadButton {"UPLOAD"};
    juce::TextButton startButton {"START"};
    
    std::unique_ptr<AdvancedTableComponent> codeSignTable, productSignTable;
    
    juce::Label nameLabel;
    
    juce::String nameText;
    
    juce::String devName = "";
    juce::String devID = "";
    
public:
    
    void setDevName (juce::String newDevName)
    {
        devName = newDevName;
        setNameText (devName);
    }
    
    void setDevID (juce::String newDevID) { devID = newDevID; }
    
    void setNameText (juce::String newNameText)
    {
        nameText = newNameText;
        nameLabel.setText (nameText, juce::dontSendNotification);
    }
    
    UtilityScreen()
    {
    //    quilioLogoButton = std::make_unique<juce::DrawableButton>();
    //    profileButton = std::make_unique<juce::DrawableButton>();
        
        codeSignButton.setToggleable (true);
        productSignButton.setToggleable (true);
        codeSignButton.setClickingTogglesState (true);
        productSignButton.setClickingTogglesState (true);
        
        startButton.setLookAndFeel (&roundedFillStyling);
        uploadButton.setLookAndFeel (&roundedOutlineStyling);
        codeSignButton.setLookAndFeel (&underlinedButtonStyling);
        productSignButton.setLookAndFeel (&underlinedButtonStyling);
        
        addAndMakeVisible (quilioLogoButton);
        addAndMakeVisible (profileButton);
        
        setImages (quilioLogoButton, quilioLogoImage, quilioLogoImage);
        setImages (profileButton, quilioLogoImage, quilioLogoImage);
        
        std::vector<AdvancedTableComponent::ColumnData> columns = {
            {"Item", 219},
            {"Type", 220},
            {"Status", 80},
            {"Clear", 79}
        };
        
        std::vector<AdvancedTableComponent::RowData> codesigndata = {};
        std::vector<AdvancedTableComponent::RowData> productsigndata = {};
        
        codeSignTable = std::make_unique<AdvancedTableComponent> (columns, codesigndata);
        productSignTable = std::make_unique<AdvancedTableComponent> (columns, productsigndata);
  
        codeSignTable->setAllowedFileTypes (FormatLibrary::Types::EXECUTABLE);
        productSignTable->setAllowedFileTypes (FormatLibrary::Types::INSTALLER);
        
        addAndMakeVisible (codeSignButton);
        addAndMakeVisible (productSignButton);
        addAndMakeVisible (uploadButton);
        addAndMakeVisible (startButton);
        
        addAndMakeVisible (*codeSignTable);
        addAndMakeVisible (*productSignTable);
        
        codeSignButton.setToggleState (true, juce::dontSendNotification);
        productSignButton.setToggleState (false, juce::dontSendNotification);
        
        codeSignButton.onClick = [&] { switchToTable (CODE_SIGN); productSignButton.setToggleState (false, juce::dontSendNotification); };
        productSignButton.onClick = [&] { switchToTable (PRODUCT_SIGN); codeSignButton.setToggleState (false, juce::dontSendNotification); };
        
        //Start on code sign table for the first time
        switchToTable (CODE_SIGN);
        
        startButton.onClick = [&]
        {
            if (currentTableState == CODE_SIGN)
            {
                codeSignTable->notarizeTable (devName, devID, true);
            }
            else if (currentTableState == PRODUCT_SIGN)
            {
                productSignTable->notarizeTable (devName, devID, false);
            }
        };
        
        uploadButton.onClick = [&]
        {
            getCurrentlyOpenTable()->browseForFileToUpload();
        };
        
        profileButton.onClick = [&] { onProfile(); };
        quilioLogoButton.onClick = [&] { onLogo(); };
        
        addAndMakeVisible (nameLabel);
        nameLabel.setFont (juce::Font (20.0));
        nameLabel.setColour (juce::Label::textColourId, juce::Colour::fromString ("#ffA6A6A6"));
    }
    
    juce::String removeDotAndCapitalize (juce::String inputString)
    {
        // Remove the first character (which is assumed to be a dot)
        juce::String withoutDot = inputString.substring(1);

        // Capitalize all letters
        withoutDot = withoutDot.toUpperCase();

        return withoutDot;
    }

    juce::String getStatus (juce::File file)
    {
        return "BLAH";
    }
    
    AdvancedTableComponent* getCurrentlyOpenTable()
    {
        if (currentTableState == CODE_SIGN) return codeSignTable.get();
        else if (currentTableState == PRODUCT_SIGN) return productSignTable.get();
        
        DBG ("Something is wrong. The program thinks no table is open.");
        return {};
    }
    
    TableState currentTableState;
    
    void switchToTable (TableState state)
    {
        if (state == CODE_SIGN)
        {
            codeSignTable->setVisible (true);
            productSignTable->setVisible (false);
        }
        else if (state == PRODUCT_SIGN)
        {
            codeSignTable->setVisible (false);
            productSignTable->setVisible (true);
        }
        
        currentTableState = state;
    }
    
    void resized() override
    {
        codeSignButton.setBounds (20, 100, 115, 40);
        productSignButton.setBounds (143, 100, 133, 40);
        
        uploadButton.setBounds (50, 512, 136, 54);
        startButton.setBounds (536, 512, 112, 54);
        
        codeSignTable->setBounds (32, 180, 616, 302);
        productSignTable->setBounds (32, 180, 616, 302);
        
        quilioLogoButton.setBounds (32, 30, 80, 40);
        profileButton.setBounds (598, 26, 48, 48);
        
        nameLabel.setBounds (412, 38, 172, 24);
    }
    
    std::function<void()> onProfile = []{};
    std::function<void()> onLogo = []{};
};
