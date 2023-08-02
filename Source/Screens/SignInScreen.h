/*
  ==============================================================================

    SignInScreen.h
    Created: 4 May 2023 4:41:25pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once
#include "../Helpers/Components.h"
#include "../CredentialManager/yum_CredentialManager.h"

class PaddedTextEditor : public juce::TextEditor
{
    juce::Font textEditorFont {20.0f};
    juce::Colour emptyTextColour {juce::Colour::fromString ("#ff8C8C8C")};
    juce::Colour textBoxColourWhenActive {juce::Colour::fromString ("ffC69BF1")};

public:
    PaddedTextEditor (juce::String textToShowWhenEmpty) : juce::TextEditor (textToShowWhenEmpty)
    {
        setTextToShowWhenEmpty (textToShowWhenEmpty, emptyTextColour);
        setJustification (juce::Justification::centredLeft);
        setIndents (16, 4);
        setFont (textEditorFont);
        
        setWantsKeyboardFocus (true);
        
        setColour (TextEditor::focusedOutlineColourId, textBoxColourWhenActive);
    }
    
    ~PaddedTextEditor()
    {
    }
    
    bool keyPressed (const juce::KeyPress& key) override
    {
        if (key.getKeyCode() == juce::KeyPress::downKey || key.getKeyCode() == juce::KeyPress::returnKey)
        {
            onDownKey();
        }
        else if (key.getKeyCode() == juce::KeyPress::upKey)
        {
            onUpKey();
        }
        else if (key.getKeyCode() == juce::KeyPress::backspaceKey)
        {
    //        deleteBackwards (false);
        }
        
        juce::TextEditor::keyPressed (key);

        return false;
    }
    
    std::function<void()> onDownKey;
    std::function<void()> onUpKey;
};



class DisappearingMessage : public juce::Component
{
    
public:
    DisappearingMessage(){}
    
};

class SignInScreen : public Screen, public juce::TextEditor::Listener
{
    QuilioLoginLookAndFeel signInStyling;
    KeepMeSignedInButtonStyling keepMeSignedInButtonStyling;
    
    juce::TextButton submitButton {"Sign in"};

    //    std::unique_ptr<juce::Drawable> quilioLogoSVG;
    //    std::unique_ptr<juce::DrawableButton> quilioLogoButton;
    
    juce::Image quilioLogoFullFormImage = juce::ImageFileFormat::loadFrom (BinaryData::QuilioLogoLongForm_png, BinaryData::QuilioLogoLongForm_pngSize);
    juce::Image quilioLogoImage = juce::ImageFileFormat::loadFrom (BinaryData::quilioLogo_4x_png, BinaryData::quilioLogo_4x_pngSize);
    
    juce::ImageButton quilioLogoButton;
    juce::ToggleButton keepMeSignedInButton;
    
    PaddedTextEditor passEditor {"Pass"}, emailEditor {"Email"}, teamIdEditor {"Team ID"}, nameEditor {"Name"};
    juce::OwnedArray<PaddedTextEditor> textEditors {{ &nameEditor, &emailEditor, &teamIdEditor, &passEditor }};
    
    juce::GlowEffect glow;
    DisappearingMessage disappearingMessage;
public:
    
    std::function<void(bool)> onSubmit = [](bool value){};
    
    SignInScreen()
    {
        setLookAndFeel (&signInStyling);
        
        for (auto editor : textEditors)
        {
            addAndMakeVisible (editor);
            editor->addListener (this);
        }
        
        addAndMakeVisible (submitButton);
        
        setImages (quilioLogoButton, quilioLogoFullFormImage, quilioLogoFullFormImage);
        
        keepMeSignedInButton.setLookAndFeel (&keepMeSignedInButtonStyling);
        keepMeSignedInButton.setToggleable (true);
        keepMeSignedInButton.setClickingTogglesState (true);
        keepMeSignedInButton.setButtonText ("Keep me signed in");
        
        submitButton.setHasFocusOutline (false);
        submitButton.onClick = [&]
        {
            onSubmit (keepMeSignedInButton.getToggleState());
        };
        
        addAndMakeVisible (quilioLogoButton);
        addAndMakeVisible (keepMeSignedInButton);
        
        nameEditor.onDownKey = [&] {emailEditor.grabKeyboardFocus(); };
        emailEditor.onDownKey = [&] {teamIdEditor.grabKeyboardFocus(); };
        teamIdEditor.onDownKey = [&] {passEditor.grabKeyboardFocus(); };
        passEditor.onDownKey = [&] {submitButton.grabKeyboardFocus(); };
        
        passEditor.setPasswordCharacter ('*');
        
        nameEditor.onUpKey = [&] {};
        passEditor.onUpKey = [&] {teamIdEditor.grabKeyboardFocus(); };
        teamIdEditor.onUpKey = [&] {emailEditor.grabKeyboardFocus(); };
        emailEditor.onUpKey = [&] {nameEditor.grabKeyboardFocus(); };
        
//        std::unique_ptr<juce::XmlElement> svg (juce::XmlDocument::parse (BinaryData::logo_svg));
//
//        if (svg != nullptr)
//            quilioLogoSVG = juce::Drawable::createFromSVG (*svg);
//
//        quilioLogoButton = std::make_unique<juce::DrawableButton> ("myButton", juce::DrawableButton::ButtonStyle::ImageStretched);
//        quilioLogoButton->setImages (quilioLogoSVG.get(), quilioLogoSVG.get(), quilioLogoSVG.get(), quilioLogoSVG.get(), quilioLogoSVG.get(), quilioLogoSVG.get(), quilioLogoSVG.get(), quilioLogoSVG.get());
//        addAndMakeVisible (*quilioLogoButton);
        
        submitButton.onStateChange = [&]
        {
            if (submitButton.isDown()) {}
            else if (submitButton.isOver())
            {
                submitButtonSnapshot = getDropShadowSnapshotFromComponent (&submitButton);
            }
            else{}
            
            repaint();
        };
        
        addAndMakeVisible (disappearingMessage);
    }

    juce::Image submitButtonSnapshot;
    
    ~SignInScreen()
    {
        textEditors.clear (false);
        setLookAndFeel (nullptr);
    }
    
    ProfileData getProfileData()
    {
        return ProfileData (nameEditor.getText(),
                            emailEditor.getText(),
                            teamIdEditor.getText(),
                            passEditor.getText());
    }
    
    bool isDataComplete()
    {
        for (auto editor : textEditors)
        {
            if (editor->isEmpty())
            {
                DBG ("You haven't entered all of the necessary profile data.");
                return false;
            }
        }
        
        return true;
    }
    
    PaddedTextEditor* getCurrentlyFocusedTextEditor()
    {
        for (auto editor : textEditors)
        {
            if (editor->hasKeyboardFocus (true))
                return editor;
        }
        
        return {};
    }
    
//    bool keyPressed (const juce::KeyPress& key) override
//    {
//        auto currentlyFocusedTextEditor = getCurrentlyFocusedTextEditor();
//
//        if (key.getKeyCode() == juce::KeyPress::downKey)
//        {
//            for (int editorIndex = 0; editorIndex < textEditors.size() - 1; editorIndex++)
//            {
//                auto editor = textEditors[editorIndex];
//                if (editor->hasKeyboardFocus (true))
//                {
//                    textEditors[editorIndex + 1]->grabKeyboardFocus();
//                }
//            }
//
//            return true;
//        }
//        else if (key.getKeyCode() == juce::KeyPress::upKey)
//        {
//            for (int editorIndex = 0; editorIndex < textEditors.size() - 1; editorIndex++)
//            {
//                auto editor = textEditors[editorIndex];
//                if (editor->hasKeyboardFocus (true))
//                {
//                    textEditors[editorIndex + 1]->grabKeyboardFocus();
//                }
//            }
//
//            return true;
//        }
//
//        return false;
//    }
    

    
//    void textEditorReturnKeyPressed (juce::TextEditor& textEditor) override
//    {
//        if (&textEditor == &nameEditor)
//        {
//            emailEditor.grabKeyboardFocus();
//        }
//        else if (&textEditor == &emailEditor)
//        {
//            teamIdEditor.grabKeyboardFocus();
//        }
//        else if (&textEditor == &teamIdEditor)
//        {
//            passEditor.grabKeyboardFocus();
//        }
//        else if (&textEditor == &passEditor)
//        {
//            onSubmit();
//        }
//    }
    
    void textEditorEscapeKeyPressed (juce::TextEditor& textEditor) override
    {
        nameEditor.giveAwayKeyboardFocus();
        emailEditor.giveAwayKeyboardFocus();
        teamIdEditor.giveAwayKeyboardFocus();
        passEditor.giveAwayKeyboardFocus();
        submitButton.giveAwayKeyboardFocus();
    }
    
    void paintOverChildren (juce::Graphics& g) override
    {
        g.setFont (28.0f);
        g.setColour (juce::Colours::white);
        g.drawFittedText ("Sign in with your \n Apple Developer Account", 162, 79, 335, 84, juce::Justification::centred, 2);
        
//        g.setFont (24.0f);
//        g.setColour (juce::Colours::white);
//        g.drawFittedText ("Keep me signed in", 279.5, 497, 148, 24, juce::Justification::centred, 1);
    }
    
    void printBoundsDBG (juce::Button* component, juce::Image* snapshot)
    {
        juce::Rectangle<int> bounds = component->getBounds();
        bounds = snapshot->getBounds();
        DBG("Bounds of the component: x=" << bounds.getX() << ", y=" << bounds.getY() << ", width=" << bounds.getWidth() << ", height=" << bounds.getHeight());
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
        
    void paint (juce::Graphics& g) override
    {
        Screen::paint (g);

        juce::AffineTransform moveButton;
        moveButton = juce::AffineTransform::translation (submitButton.getX() - 12, submitButton.getY() - 12);

        if (submitButton.isDown()){}
        else if (submitButton.isOver())
        {
            g.drawImageTransformed (submitButtonSnapshot, moveButton);
        }
        else{}
    }

    
    void resized() override
    {
        nameEditor.setBounds (162, 179, 355, 46);
        emailEditor.setBounds (162, 241, 355, 46);
        teamIdEditor.setBounds (162, 303, 355, 46);
        passEditor.setBounds (162, 365, 355, 46);

        submitButton.setBounds (162, 423, 355, 54);
        
        quilioLogoButton.setBounds (32, 30, 80, 40);
        
        keepMeSignedInButton.setBounds (245.5, 493, 188, 32);
    }
};
