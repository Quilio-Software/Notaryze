/*
  ==============================================================================

    SignInScreen.h
    Created: 4 May 2023 4:41:25pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once
#include "../Helpers/Components.h"

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

class ProfileData
{
    juce::String name, email, devID, password;
    
public:
    ProfileData (juce::String newName, juce::String newEmail, juce::String newDevID, juce::String newPassword)
        : name (newName), email (newEmail), devID (newDevID), password (newPassword)
    {
    }
    
    juce::String getName() const { return name; }
    juce::String getEmail() const { return email; }
    juce::String getDevID() const { return devID; }
    juce::String getPassword() const { return password; }
};


class SignInScreen : public Screen, public juce::TextEditor::Listener
{
    QuilioLoginLookAndFeel signInStyling;
    
    juce::TextButton submitButton {"Sign in"};

    //    std::unique_ptr<juce::Drawable> quilioLogoSVG;
    //    std::unique_ptr<juce::DrawableButton> quilioLogoButton;
    
    juce::Image quilioLogoFullFormImage = juce::ImageFileFormat::loadFrom (BinaryData::QuilioLogoLongForm_png, BinaryData::QuilioLogoLongForm_pngSize);
    juce::Image quilioLogoImage = juce::ImageFileFormat::loadFrom (BinaryData::quilioLogo_4x_png, BinaryData::quilioLogo_4x_pngSize);
    
    juce::Image keepMeSignedInSquareDefaultImage = juce::ImageFileFormat::loadFrom (BinaryData::checkbox_Default_png, BinaryData::checkbox_Default_pngSize);
    juce::Image keepMeSignedInSquareHoverImage = juce::ImageFileFormat::loadFrom (BinaryData::checkbox_InactiveHover_png, BinaryData::checkbox_InactiveHover_pngSize);
    juce::Image keepMeSignedInSquareOnImage = juce::ImageFileFormat::loadFrom (BinaryData::checkbox_ON_png, BinaryData::checkbox_ON_pngSize);
    
    juce::ImageButton quilioLogoButton;
    juce::ImageButton keepMeSignedInButton;
    
    PaddedTextEditor passEditor {"Pass"}, emailEditor {"Email"}, teamIdEditor {"Team ID"}, nameEditor {"Name"};
    juce::OwnedArray<PaddedTextEditor> textEditors {{ &nameEditor, &emailEditor, &teamIdEditor, &passEditor }};
    
public:
    
    std::function<void()> onSubmit = []{};
    
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
        
        keepMeSignedInButton.setToggleable (true);
        keepMeSignedInButton.setClickingTogglesState (true);
        setImages (keepMeSignedInButton, keepMeSignedInSquareDefaultImage, keepMeSignedInSquareOnImage);
        
        submitButton.setHasFocusOutline (false);
        submitButton.onClick = [&] { onSubmit(); };
        
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
    }
    
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
    }
        
    void paint (juce::Graphics& g) override
    {
        Screen::paint (g);
    }
    
    void resized() override
    {
        nameEditor.setBounds (162, 179, 355, 46);
        emailEditor.setBounds (162, 241, 355, 46);
        teamIdEditor.setBounds (162, 303, 355, 46);
        passEditor.setBounds (162, 365, 355, 46);

        submitButton.setBounds (162, 427, 355, 54);
        
        quilioLogoButton.setBounds (32, 30, 80, 40);
        
        keepMeSignedInButton.setBounds (251, 499, 20, 20);
    }
};
