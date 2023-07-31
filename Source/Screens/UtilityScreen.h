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
    juce::Image profilePicCircleImageDefault =  juce::ImageFileFormat::loadFrom (BinaryData::profilePicCircle_png, BinaryData::profilePicCircle_pngSize);
    juce::Image profilePicCircleImageHover =  juce::ImageFileFormat::loadFrom (BinaryData::profilePicCircleHover_png, BinaryData::profilePicCircleHover_pngSize);
    
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
        profileButton.setImages (true, true, true, profilePicCircleImageDefault, 1.0f, {}, profilePicCircleImageHover, 1.0f, {}, profilePicCircleImageHover, 1.0f, {});
        
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
        
        uploadButton.onStateChange = [&]
        {
            if (uploadButton.isDown()) {}
            else if (uploadButton.isOver())
            {
                uploadButtonSnapshot = getDropShadowSnapshotFromComponent (&uploadButton);
            }
            else {}
            
            repaint();
        };
        startButton.onStateChange = [&]
        {
            if (uploadButton.isDown()){}
            else if (startButton.isOver())
            {
                startButtonSnapshot = getDropShadowSnapshotFromComponent (&startButton);
            }
            else{}
            
            repaint();
        };
    }
    
    juce::Image uploadButtonSnapshot;
    juce::Image startButtonSnapshot;
    
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
    
    juce::GlowEffect glowEffect;
    
    juce::Image getGlowSnapshotFromComponent (juce::Component* component)
    {
        juce::Rectangle<int> area (-12, -12, component->getWidth() + 24, component->getHeight() + 24);
        juce::Image snapshot = component->createComponentSnapshot (area, false);
        juce::Graphics snapshotGraphics (snapshot);
    
        glowEffect.setGlowProperties (24.0f, juce::Colour (217, 217, 217).withAlpha (0.1f));
        glowEffect.applyEffect (snapshot, snapshotGraphics, 0.2f, 1.0f);

        return snapshot;
    }
    
    juce::Image getDropShadowSnapshotFromComponent (juce::Component* component)
    {
        juce::Rectangle<int> area (-12, -12, component->getWidth() + 24, component->getHeight() + 24);
        juce::Image snapshot = component->createComponentSnapshot (area, false);
        juce::Graphics snapshotGraphics (snapshot);
        
        juce::DropShadow dropShadow (juce::Colour (140, 140, 140).withAlpha (1.0f), 10.0f, {0, 0});
        juce::DropShadowEffect dropShadowEffect;
        dropShadowEffect.setShadowProperties (dropShadow);
        dropShadowEffect.applyEffect (snapshot, snapshotGraphics, 1.0f, 1.0f);
        
        return snapshot;
    }
    
    juce::Image backgroundImage = juce::ImageFileFormat::loadFrom (BinaryData::UtilityScreenBackground_png, BinaryData::UtilityScreenBackground_pngSize);
    void paint (juce::Graphics& g) override
    {
        juce::Rectangle<float> area = getBounds().toFloat();
        g.drawImage (backgroundImage, area);
        
        juce::AffineTransform moveUploadButton;
        moveUploadButton = juce::AffineTransform::translation (uploadButton.getX() - 12, uploadButton.getY() - 12);

        if (uploadButton.isDown()){}
        else if (uploadButton.isOver())
            g.drawImageTransformed (uploadButtonSnapshot, moveUploadButton);
        
        juce::AffineTransform moveStartButton;
        moveStartButton = juce::AffineTransform::translation (startButton.getX() - 12, startButton.getY() - 12);
        
        if (startButton.isDown()){}
        else if (startButton.isOver())
        {
            g.drawImageTransformed (startButtonSnapshot, moveStartButton);
        }
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
