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


class SVGObject
{
    juce::MemoryBlock svgData;
    int numBytes = 0;
    
    juce::DrawableComposite drawable;
    juce::DrawableImage drawableImage;
    
    juce::AffineTransform transform;
    
    float opacity = 1.0f;
    
    juce::MemoryBlock getSvgData (const char* filename)
    {
        juce::Identifier dataName (filename);
        juce::Identifier sizeName (filename);
        juce::MemoryBlock svgData;
        const char* namedResourceName = BinaryData::getNamedResource (filename, numBytes);
        
        DBG ("Just loaded " + juce::String (numBytes) + " bytes of: " + juce::String (filename));
        
        svgData.append (namedResourceName, numBytes);;
        return svgData;
    }
    
public:
    SVGObject (const char* filename)
    {
        svgData = getSvgData (filename);
        juce::Image image = juce::ImageFileFormat::loadFrom (svgData.getData(), svgData.getSize());
        drawableImage.setImage (image);
        drawable.addChildComponent (drawableImage);
    }
    
    void draw (juce::Graphics& g, juce::Rectangle<int> bounds)
    {
        drawable.setBounds (bounds);
        drawable.draw (g, opacity, transform);
    }
};

enum TableState
{
    CODE_SIGN, //This is purely dev ID based codesigning
    PRODUCT_SIGN, //This is notarization + stapling
};

class UtilityScreen : public Screen
{
    RoundedFillButtonStyling roundedFillStyling;
    RoundedOutlineButtonStyling roundedOutlineStyling;
    SignOutStopClearButtonStyling signOutStopClearButtonStyling;
    UnderlinedButtonStyling underlinedButtonStyling;
    
    juce::Image quilioLogoFullFormImage = juce::ImageFileFormat::loadFrom (BinaryData::QuilioLogoLongForm_png, BinaryData::QuilioLogoLongForm_pngSize);
    juce::Image quilioLogoImage = juce::ImageFileFormat::loadFrom (BinaryData::quilioLogo_4x_png, BinaryData::quilioLogo_4x_pngSize);
    
    juce::ImageButton quilioLogoButton;
    juce::ImageButton profileButton;
    
    juce::Image profilePicCircleImageDefault;
    
    juce::TextButton codeSignButton {"Code Sign"}, productSignButton {"Product Sign"};
    juce::TextButton uploadButton {"Upload"};
    bool uploadButtonDisabled;
    juce::TextButton startButton {"Start"};
    bool startButtonDisabled;
    
    std::unique_ptr<AdvancedTableComponent> codeSignTable, productSignTable;
    
    juce::Label nameLabel;
    
    juce::String nameText;
    
    juce::String devName = "";
    juce::String devID = "";
    
public:
    
    std::shared_ptr<ProfileData> profileData;
    void setProfileData (std::shared_ptr<ProfileData>& newProfileData) { profileData = newProfileData; }
    
    void updateProfilePicture()
    {
        profileButton.setImages (true, true, true, profileData->getProfilePicture(), 1.0f, {}, profileData->getProfilePicture(), 1.0f, {}, profileData->getProfilePicture(), 1.0f, {});
        resized();
        repaint();
    }
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
        uploadButton.setToggleable (true);
        codeSignButton.setClickingTogglesState (true);
        productSignButton.setClickingTogglesState (true);
        
        startButton.setLookAndFeel (&roundedFillStyling);
        uploadButton.setLookAndFeel (&roundedOutlineStyling);
        codeSignButton.setLookAndFeel (&underlinedButtonStyling);
        productSignButton.setLookAndFeel (&underlinedButtonStyling);
        
        addAndMakeVisible (quilioLogoButton);
        addAndMakeVisible (profileButton);
        
        setImages (quilioLogoButton, quilioLogoFullFormImage, quilioLogoFullFormImage);
        profileButton.setImages (true, true, true, profilePicCircleImageDefault, 1.0f, {}, profilePicCircleImageDefault, 1.0f, {}, profilePicCircleImageDefault, 1.0f, {});
        
        std::vector<AdvancedTableComponent::ColumnData> columns = {
            {"Item", 271},
            {"Type", 134},
            {"Status", 155 - 40},
            {"Clear", 616 - 155 - 134 - 271}
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
        nameLabel.setJustificationType (juce::Justification::right);
    }
    
    void toggleSigningTableType()
    {
        //Switch to Table and update button state
        if (currentTableState == CODE_SIGN)
        {
            switchToTable (PRODUCT_SIGN);
            productSignButton.setToggleState (true, juce::dontSendNotification);
            codeSignButton.setToggleState (false, juce::dontSendNotification);
        }
        else if (currentTableState == PRODUCT_SIGN)
        {
            switchToTable (CODE_SIGN);
            productSignButton.setToggleState (false, juce::dontSendNotification);
            codeSignButton.setToggleState (true, juce::dontSendNotification);
        }
    }
    
    bool keyPressed (const juce::KeyPress& key) override
    {
        if (key.getKeyCode() == juce::KeyPress::tabKey)
        {
            toggleSigningTableType();
            return true;
        }

        return false;
    }
    
    
    juce::Image backgroundImage = juce::ImageFileFormat::loadFrom (BinaryData::UtilityScreenBackground_png, BinaryData::UtilityScreenBackground_pngSize);
    void paint (juce::Graphics& g) override
    {
        juce::Rectangle<float> area = getBounds().toFloat();
        g.drawImage (backgroundImage, area);
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
    
    void drawToolTip (juce::Graphics& g, const juce::String& text, float x, float y)
    {
        std::unordered_map<juce::String, juce::String> statusToToolTip
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
        
        juce::String toolTipMessage;
        
        auto toolTipIterator = statusToToolTip.find(text);
        if (toolTipIterator != statusToToolTip.end()) { toolTipMessage = toolTipIterator->second; }
        // calculate
        // - 2 - pill height - rectangle
        // 2 px above status pill
        // max size - (109, 32)
        // max textbox - (93, 24)

        // add text
        juce::Colour fontColour = juce::Colour (89, 89, 89);
        juce::Typeface::Ptr lightTypeFace = juce::Typeface::createSystemTypefaceFor (BinaryData::PoppinsLight_ttf, BinaryData::PoppinsLight_ttfSize);
        g.setFont (lightTypeFace);
        g.setColour (fontColour);
        juce::Font font = g.getCurrentFont();
        int stringWidth = font.getStringWidth (toolTipMessage);
        juce::Rectangle<int> fontRectangle (x, y, stringWidth, 12);
        
        //set up rectangle
        juce::Colour rectangleFillColour = juce::Colour (239, 239, 239);
        juce::Colour rectangleOutlineColour = juce::Colour (217, 217, 217);
        
        juce::Rectangle<float> toolTipRectangle (x - 8, y - 4.0f, stringWidth + 16.0f, 20.0f);
        g.setColour (rectangleFillColour);
        g.fillRoundedRectangle (toolTipRectangle, 4.0f);
        
        g.setColour (rectangleOutlineColour);
        g.drawRoundedRectangle (toolTipRectangle, 4.0f, 1.0f);
        
        //draw text
        g.drawFittedText (toolTipMessage, fontRectangle, juce::Justification::centred, 1);
    }
    
    void resized() override
    {
        codeSignButton.setBounds (20, 100, 115, 40);
        productSignButton.setBounds (143, 100, 133, 40);
        
        uploadButton.setBounds (32, 514, 136, 54);
        startButton.setBounds (536, 514, 112, 54);
        
        codeSignTable->setBounds (32, 180, 616, 302);
        productSignTable->setBounds (32, 180, 616, 302);
        
        quilioLogoButton.setBounds (32, 30, 73, 40);
        profileButton.setBounds (600, 26, 48, 48);
        
        nameLabel.setBounds (412, 38, 172, 24);
    }
    
    std::function<void()> onProfile = []{};
    std::function<void()> onLogo = []{};
};
