/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>
#include <iostream>
#include "MainComponent.h"
#include "Notarization/Notarization.h"

namespace NotaryzeCLI
{
juce::String usage()
{
    return "Notaryze\n"
           "\n"
           "GUI:\n"
           "  Notaryze\n"
           "\n"
           "CLI:\n"
           "  Notaryze --help\n"
           "  Notaryze --list-identities\n"
           "  Notaryze --status <path>\n"
           "  Notaryze --verify <path>\n"
           "  Notaryze --adhoc-sign <path>\n"
           "  Notaryze --remove-signature <path>\n"
           "  Notaryze --sign <path> --name <developer-name> --team-id <team-id>\n";
}

void printResult (const CommandResult& result)
{
    if (result.output.isNotEmpty())
        std::cout << result.output;
}

juce::String getOptionValue (const juce::StringArray& args, const juce::String& option)
{
    const auto index = args.indexOf (option);

    if (index < 0 || index + 1 >= args.size())
        return {};

    return args[index + 1];
}

bool hasOption (const juce::StringArray& args, const juce::String& option)
{
    return args.contains (option);
}

int run (const juce::String& commandLine)
{
    auto args = juce::StringArray::fromTokens (commandLine, true);

    if (args.isEmpty() || hasOption (args, "--help") || hasOption (args, "-h"))
    {
        std::cout << usage();
        return 0;
    }

    if (hasOption (args, "--list-identities"))
    {
        const auto result = listCodeSigningIdentities();
        printResult (result);
        return result.exitCode;
    }

    const auto statusPath = getOptionValue (args, "--status");
    if (statusPath.isNotEmpty())
    {
        std::cout << getCodeSignatureStatus (statusPath) << "\n";
        return 0;
    }

    const auto verifyPath = getOptionValue (args, "--verify");
    if (verifyPath.isNotEmpty())
    {
        const auto result = verifyCodeSignature (verifyPath);
        printResult (result);
        return result.exitCode;
    }

    const auto adHocPath = getOptionValue (args, "--adhoc-sign");
    if (adHocPath.isNotEmpty())
    {
        const auto result = adHocSign (adHocPath);
        printResult (result);
        return result.exitCode;
    }

    const auto removePath = getOptionValue (args, "--remove-signature");
    if (removePath.isNotEmpty())
    {
        const auto result = removeCodeSignature (removePath);
        printResult (result);
        return result.exitCode;
    }

    const auto signPath = getOptionValue (args, "--sign");
    if (signPath.isNotEmpty())
    {
        const auto name = getOptionValue (args, "--name");
        const auto teamID = getOptionValue (args, "--team-id");

        if (name.isEmpty() || teamID.isEmpty())
        {
            std::cerr << "--sign requires --name and --team-id\n";
            return 2;
        }

        const auto result = runCommandVerboseResult ("codesign --force --sign "
                                                     + shellQuote (developerIdApplicationIdentity (name, teamID))
                                                     + " --verbose --deep --strict --options=runtime --timestamp "
                                                     + shellQuote (signPath));
        printResult (result);
        return result.exitCode;
    }

    std::cerr << "Unknown arguments.\n\n" << usage();
    return 2;
}

bool isCliInvocation (const juce::String& commandLine)
{
    return commandLine.trim().startsWithChar ('-');
}
}

//==============================================================================
class NotaryzeApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    NotaryzeApplication() {}

    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        if (NotaryzeCLI::isCliInvocation (commandLine))
        {
            cliExitCode = NotaryzeCLI::run (commandLine);
            setApplicationReturnValue (cliExitCode);
            quit();
            return;
        }

        mainWindow.reset (new MainWindow (getApplicationName()));
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..

        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        MainWindow (juce::String name)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (juce::ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent(), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            setResizable (true, true);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
    int cliExitCode = 0;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (NotaryzeApplication)
