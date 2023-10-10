[![General badge](https://img.shields.io/badge/Instagram-E4405F?style=for-the-badge&logo=instagram&logoColor=white)](https://www.instagram.com/quilio.dev/)
![Discord](https://img.shields.io/discord/1133624079262421032)
[![GitHub issues](https://img.shields.io/github/issues/Quilio-Software/Notaryze)](https://github.com/Quilio-Software/Notaryze/issues/)
[![GitHub pull-requests](https://img.shields.io/github/issues-pr/Quilio-Software/Notaryze)](https://github.com/Quilio-Software/Notaryze/pulls/)
# **Notaryze**
Notaryze is an open-source tool designed and developed by `Quilio Ltd` to simplify the notarization process for developers. It provides a convenient interface to code sign and product sign software, ensuring compatibility and security for macOS users.
## **Motivation**
As developers ourselves, we understand the frustrations and challenges that come with code and product signing. The security measures put in place by platforms like Ventura are crucial, but they can also create significant barriers for entry, especially for newer developers. We've experienced firsthand how the process can be both time-consuming and finicky, leading to unnecessary roadblocks in the development workflow.
Our motivation for creating Notaryze is to break down these barriers and streamline the signing process. We believe that developers should be able to focus on what they do best—creating amazing software—rather than getting tangled in the complexities of code signing. By offering a simple and intuitive tool, we aim to make the process more accessible to developers of all experience levels.
## **Support**
We're here to ensure ongoing support and updates for this project. If you find Notaryze helpful and want to contribute to its development, you can sponsor the repository. Your sponsorship would help us dedicate more resources to improve the tool and add new features. Click the Sponsor button on the top right to learn more.
## **Acknowledgments**
A big thank you to all the individuals and contributors who have generously offered their time to beta test, explore the tool's capabilities, and report issues. Your feedback has been invaluable in shaping Notaryze into what it is today. We deeply appreciate your dedication to making this project better.
### **Contributors**
Special thanks to the following contributors for their valuable input and assistance:
- Members of the Quilio Team:
    - Abhishek Shivakumar
    - Ethan Danish Joseph
    - Sagarnil Gupta
    - Matthew Baum
# Features
- Simple and intuitive user experience and interface
- Secure sign-in credentials
- Robust signing procedure
- Profile saving for faster signing process
# **Installation**
To use Notaryze, follow these steps:
1. Clone the repository to your local machine.
2. Build the project using a C++ compiler compatible with your system.
3. Run the compiled executable to launch Notaryze.
# How to use
After opening the software, sign in using valid credentials (Developer ID or Team ID and associated Password) to start using the tool. Based on whether you want to Code Sign or Product Sign, you can click and drag the file(s) to be signed into the interface. Alternatively, you could click on "Upload" to upload files using the Finder.
Once files have been uploaded to the tool, you will see a list that provides information about the uploaded files. This includes the name, file type, and status. As long as there is one file ready to be signed, the "Start" button will be enabled. During signing, you can stop the process before all of them time out if required.
You can view your profile information or sign out by clicking on the profile icon on the top right.
## Data and Privacy
“Name” will be stored in keychain on your system locally as a key under Notaryze
“Email ID”, “Developer / Team ID”, and “Password” are stored as secured keys under the dictionary reference.
When the user signs-out explicitly, the data is removed from the keychain.
When “Keep me signed in” is checked, the information gets stored in the keychain before you sign-in.
