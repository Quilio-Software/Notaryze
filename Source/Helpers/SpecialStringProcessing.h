/*
  ==============================================================================

    SpecialStringProcessing.h
    Created: 10 May 2023 2:49:38pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once

juce::String removeDotAndCapitalize (juce::String inputString)
{
    // Remove the first character (which is assumed to be a dot)
    juce::String withoutDot = inputString.substring(1);

    // Capitalize all letters
    withoutDot = withoutDot.toUpperCase();

    return withoutDot;
}
