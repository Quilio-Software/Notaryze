/*
  ==============================================================================

    ErrorHandling.h
    Created: 18 May 2023 10:05:01pm
    Author:  Abhishek Shivakumar

  ==============================================================================
*/

#pragma once

//To Be Completed......
//Most of these error codes are unimplemented

enum RESULT_CODE
{
    INVALID_CREDENTIALS,
    PRODUCTSIGN_FAILED,
    NOTARIZATION_FAILED,
    STAPLE_FAILED,
    CODESIGN_FAILED,
    CONNECTION_ERROR,
    TIMEOUT,
    SUCCESS
};

//Error Parsing goes here
class NotarizationErrorChecker 
{
public:
    static bool containsNoIdentityError (const juce::String& text)
    {
        std::string lowerText = text.toStdString();
        std::transform (lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
        std::string keyword = "no identity found";
        size_t found = lowerText.find(keyword);
        return (found != std::string::npos);
    }
    
    static bool containsTimeoutError (const juce::String& text)
    {
        juce::String lowerText = text.toLowerCase();
        juce::String keyword = "timeout error";
        return (lowerText.containsIgnoreCase (keyword));
    }
    
    static RESULT_CODE getResultCode (const juce::String responseToParse)
    {
        if (containsNoIdentityError (responseToParse)) { return INVALID_CREDENTIALS; }
        else if (containsTimeoutError (responseToParse)) { return TIMEOUT; }
        else { return SUCCESS; }
    }
    
    static juce::String getResult (const juce::String responseToParse)
    {
        auto resultCode = getResultCode (responseToParse);
        if (resultCode == INVALID_CREDENTIALS)
        {
            return "The credentials are invalid";
        }
        else
        {
            return "Success";
        }
    }
};
