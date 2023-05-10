/*
  ==============================================================================

    SignInScreen.h
    Created: 4 May 2023 4:41:25pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once
#include "Components.h"

class PaddedTextEditor : public juce::TextEditor
{
    juce::Font textEditorFont {20.0f};
    juce::Colour emptyTextColour {juce::Colour::fromString ("#ff8C8C8C")};

public:
    PaddedTextEditor (juce::String textToShowWhenEmpty)
    {
        setTextToShowWhenEmpty (textToShowWhenEmpty, emptyTextColour);
        setJustification (juce::Justification::centredLeft);
        setIndents (16, 4);
        setFont (textEditorFont);
        
        setWantsKeyboardFocus (true);
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
    }
    
    std::function<void()> onDownKey;
    std::function<void()> onUpKey;
};



class SignInScreen : public Screen, public juce::TextEditor::Listener
{
    PaddedTextEditor passEditor {"Pass"}, emailEditor {"Email"}, teamIdEditor {"Team ID"}, nameEditor {"Name"};
    
    juce::TextButton submitButton {"Sign in"};
    QuilioLoginLookAndFeel signInStyling;
    
    
    //    std::unique_ptr<juce::Drawable> quilioLogoSVG;
    //    std::unique_ptr<juce::DrawableButton> quilioLogoButton;
    
    juce::Image quilioLogoImage = juce::ImageFileFormat::loadFrom (BinaryData::quilioLogo_4x_png, BinaryData::quilioLogo_4x_pngSize);
    juce::ImageButton quilioLogoButton;
    
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

        setImages (quilioLogoButton, quilioLogoImage, quilioLogoImage);
        
        submitButton.setHasFocusOutline (false);
        submitButton.onClick = [&] { onSubmit(); };
        
        addAndMakeVisible (quilioLogoButton);
        
        nameEditor.onDownKey = [&] {emailEditor.grabKeyboardFocus(); };
        emailEditor.onDownKey = [&] {teamIdEditor.grabKeyboardFocus(); };
        teamIdEditor.onDownKey = [&] {passEditor.grabKeyboardFocus(); };
        passEditor.onDownKey = [&] {submitButton.grabKeyboardFocus(); };
        
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
    
    PaddedTextEditor* getCurrentlyFocusedTextEditor()
    {
        for (auto editor : textEditors)
        {
            if (editor->hasKeyboardFocus (true))
                return editor;
        }
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
        
        quilioLogoButton.setBounds (32, 30, 40, 40);
    }
};
