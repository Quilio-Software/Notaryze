/*
  ==============================================================================

    TableStyling.h
    Created: 17 Jul 2023 6:06:06pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once


class TableComponentStyling : public juce::LookAndFeel_V4
{
    juce::Colour columnHeaderColour = juce::Colours::white;
    juce::Typeface::Ptr poppinsSemiBoldTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::PoppinsSemiBold_ttf, BinaryData::PoppinsSemiBold_ttfSize);
    juce::Typeface::Ptr poppinsRegularTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::PoppinsRegular_ttf, BinaryData::PoppinsRegular_ttfSize);

    
    juce::Typeface::Ptr getSemiBoldTypeFace() { return poppinsSemiBoldTypeface; }
    juce::Typeface::Ptr getRegularTypeFace() { return poppinsRegularTypeface; }
    
public:
    TableComponentStyling()
    {
        //First we set the colour
        setColour (juce::ListBox::backgroundColourId, juce::Colour::fromString ("#ff000A1A"));
        setColour (juce::TableHeaderComponent::textColourId, juce::Colour::fromString ("#ffbfbfbf")); //This sets the table text color
        setColour (juce::TableHeaderComponent::outlineColourId, juce::Colours::transparentBlack); //Not yet sure what this does
        setColour (juce::ListBox::outlineColourId, juce::Colours::transparentBlack); //This has to do with the rounded rect outline of the table.
    
        //Then we set the font
        
    }
    
    bool canSeeHeader = false;
    void setHeaderVisibility (bool visibility) {canSeeHeader = visibility;}
    
    void drawTooltip (juce::Graphics& g, const juce::String& text, int width, int height) override
    {
        juce::String tooltipMessage = text;

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
        int stringWidth = font.getStringWidth (tooltipMessage);
        juce::Rectangle<int> fontRectangle (8, 4, stringWidth, 12);

        //set up rectangle
        juce::Colour rectangleFillColour = juce::Colour (239, 239, 239);
        juce::Colour rectangleOutlineColour = juce::Colour (217, 217, 217);

        juce::Rectangle<float> tooltipRectangle (0, 0, stringWidth + 16.0f, 20.0f);
        g.setColour (rectangleFillColour);
        g.fillRoundedRectangle (tooltipRectangle, 4.0f);

        g.setColour (rectangleOutlineColour);
        g.drawRoundedRectangle (tooltipRectangle, 4.0f, 1.0f);

        //draw text
        g.drawFittedText (tooltipMessage, fontRectangle, juce::Justification::centred, 1);
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
        bool isClearButtonDisabled = false;
        
        if (canSeeHeader)
        {
            if (columnName != "Clear" && columnName != "Status")
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
                g.setColour (columnHeaderColour);
                g.setFont (poppinsRegularTypeface);
                g.setFont (24.0f); //Bug: This needs to be 2x whatever it actually is WTFFFFF.
                //Todo: Find a way to MAKE IT NOT BE 2X WHAT IT ACTUALLY IS WTFFFFF.
                g.drawFittedText (columnName, area, juce::Justification::centredLeft, 1);
            }
            else if (columnName == "Status")
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
                g.setColour (columnHeaderColour);
                g.setFont (poppinsRegularTypeface);
                g.setFont (24.0f); //Bug: This needs to be 2x whatever it actually is WTFFFFF.
                //Todo: Find a way to MAKE IT NOT BE 2X WHAT IT ACTUALLY IS WTFFFFF.
                g.drawFittedText (columnName, area, juce::Justification::centred, 1);
            }
            else if (columnName == "Clear")
            {
                //TODO: Convert this manual postiioning to some sort of relative positioning
                juce::Rectangle<float> roundedRectArea (8, 9, 41, 18);//(width * 0.25f * 0.5f, height * 0.25f, width * 0.75f, height * 0.5f);
                
                float stroke; //px
                juce::Colour fillColour;
                juce::Colour backgroundFillColour;
                juce::Font typeface = poppinsRegularTypeface;
                float fontHeight = 18.0f; //same for all
                
                if (isClearButtonDisabled)
                {
                    stroke = 1.0f;
                    fillColour = juce::Colour::fromString ("#ffF2571D");
                    backgroundFillColour = fillColour.withAlpha (0.6f);
                }
                else
                {
                    //Handle different clear button colors here
                    if (isMouseDown) //Clear button in the Header has been clicked
                    {
                        stroke = 1.0f;
                        fillColour = juce::Colour::fromString ("#ffF2571D");
                        backgroundFillColour = fillColour.withAlpha (0.2f);
                        clearAllData();
                    }
                    else if (isMouseOver)
                    {
                        stroke = 2.0f; //px
                        fillColour = juce::Colour::fromString ("#ffF2571D");
                        backgroundFillColour = fillColour.withAlpha (0.2f); // 20%
                    }
                    else //if mouse is frickin somewhere else ;)
                    {
                        stroke = 1.0f;
                        fillColour = juce::Colour::fromString ("#ffF2571D");
                        backgroundFillColour = fillColour.withAlpha (0.0f);
                    }
                    
                    g.setFont (fontHeight);
                    g.setFont (typeface);
                    g.setColour (backgroundFillColour);
                    g.fillRoundedRectangle (roundedRectArea.toFloat(), 4);
                    g.setColour (fillColour);
                    g.drawText ("CLEAR", roundedRectArea.toFloat(), juce::Justification::centred, 1);
                    g.drawRoundedRectangle (roundedRectArea.toFloat(), 4, stroke);
                    
                }
            }
            bool isClearButtonDisabled;
        }
    }
    
    std::function<void()> clearAllData;
};
