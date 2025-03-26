#include <wx/wx.h>
#include <wx/filedlg.h>
#include <wx/choice.h>
#include <wx/socket.h>
#include <wx/dynlib.h>
#include <vector>
#include <string>
#include <ifaddrs.h>
#include <net/if.h>
#include <arpa/inet.h>

class MyFrame : public wxFrame {
public:
    MyFrame() : wxFrame(nullptr, wxID_ANY, "wxWidgets GUI", wxDefaultPosition, wxSize(800, 700)) {
        wxPanel* panel = new wxPanel(this);
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        // File selection
        wxBoxSizer* fileSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* fileLabel = new wxStaticText(panel, wxID_ANY, "Select File:");
        fileTextCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(250, -1));
        wxButton* browseButton = new wxButton(panel, wxID_ANY, "Browse");
        fileSizer->Add(fileLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        fileSizer->Add(fileTextCtrl, 1, wxEXPAND | wxALL, 5);
        fileSizer->Add(browseButton, 0, wxALL, 5);
        sizer->Add(fileSizer, 0, wxEXPAND | wxALL, 5);

        // Ethernet interface selection
        wxBoxSizer* netSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* label = new wxStaticText(panel, wxID_ANY, "Select Interface:");
        interfaceChoice = new wxChoice(panel, wxID_ANY);
        netSizer->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        netSizer->Add(interfaceChoice, 1, wxEXPAND | wxALL, 5);
        sizer->Add(netSizer, 0, wxEXPAND | wxALL, 5);

        // Device ID and Version fields with button
        wxBoxSizer* deviceSizer = new wxBoxSizer(wxHORIZONTAL);
        wxButton* fetchDeviceInfoButton = new wxButton(panel, wxID_ANY, "Fetch Info");
        wxStaticText* idLabel = new wxStaticText(panel, wxID_ANY, "Device ID:");
        deviceIDText = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(100, -1), wxTE_READONLY);
        wxStaticText* versionLabel = new wxStaticText(panel, wxID_ANY, "Version:");
        deviceVersionText = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(100, -1), wxTE_READONLY);
        deviceSizer->Add(fetchDeviceInfoButton, 0, wxALL, 5);
        deviceSizer->Add(idLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        deviceSizer->Add(deviceIDText, 1, wxEXPAND | wxALL, 5);
        deviceSizer->Add(versionLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        deviceSizer->Add(deviceVersionText, 1, wxEXPAND | wxALL, 5);
        sizer->Add(deviceSizer, 0, wxEXPAND | wxALL, 5);

        // Operation checkboxes
        wxStaticBoxSizer* opSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Operations");
        eraseCheckBox = new wxCheckBox(panel, wxID_ANY, "Erase");
        blankCheckBox = new wxCheckBox(panel, wxID_ANY, "Blank Check");
        writeCheckBox = new wxCheckBox(panel, wxID_ANY, "Write");
        verifyCheckBox = new wxCheckBox(panel, wxID_ANY, "Verify");
        rebootCheckBox = new wxCheckBox(panel, wxID_ANY, "Reboot");

        eraseCheckBox->SetValue(true);
        blankCheckBox->SetValue(true);
        writeCheckBox->SetValue(true);
        verifyCheckBox->SetValue(true);
        rebootCheckBox->SetValue(true);

        opSizer->Add(eraseCheckBox, 0, wxALL, 5);
        opSizer->Add(blankCheckBox, 0, wxALL, 5);
        opSizer->Add(writeCheckBox, 0, wxALL, 5);
        opSizer->Add(verifyCheckBox, 0, wxALL, 5);
        opSizer->Add(rebootCheckBox, 0, wxALL, 5);

        sizer->Add(opSizer, 0, wxEXPAND | wxALL, 5);

        // Start button
        startButton = new wxButton(panel, wxID_ANY, "Start");
        sizer->Add(startButton, 0, wxALIGN_CENTER | wxALL, 10);

        panel->SetSizer(sizer);
        LoadNetworkInterfaces();

        // Bind events
        browseButton->Bind(wxEVT_BUTTON, &MyFrame::OnBrowse, this);
        fetchDeviceInfoButton->Bind(wxEVT_BUTTON, &MyFrame::OnFetchDeviceInfo, this);
        startButton->Bind(wxEVT_BUTTON, &MyFrame::OnStart, this);
    }

private:
    wxTextCtrl* fileTextCtrl;
    wxChoice* interfaceChoice;
    wxTextCtrl* deviceIDText;
    wxTextCtrl* deviceVersionText;
    wxCheckBox* eraseCheckBox;
    wxCheckBox* blankCheckBox;
    wxCheckBox* writeCheckBox;
    wxCheckBox* verifyCheckBox;
    wxCheckBox* rebootCheckBox;
    wxButton* startButton;

    void OnBrowse(wxCommandEvent&) {
        wxFileDialog openFileDialog(this, "Choose a file", "", "", "Bitstream and Binary files (*.bit;*.bin)|*.bit;*.bin", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_OK) {
            fileTextCtrl->SetValue(openFileDialog.GetPath());
        }
    }

    void OnFetchDeviceInfo(wxCommandEvent&) {
        // Example code to set values
        deviceIDText->SetValue("12345");
        deviceVersionText->SetValue("1.0.0");
    }

    void OnStart(wxCommandEvent&) {
        if (eraseCheckBox->IsChecked()) {
            // Add code to erase the device
        }
        if (blankCheckBox->IsChecked()) {
            // Add code to perform a blank check
        }
        if (writeCheckBox->IsChecked()) {
            // Add code to write to the device
        }
        if (verifyCheckBox->IsChecked()) {
            // Add code to verify the written data
        }
        if (rebootCheckBox->IsChecked()) {
            // Add code to reboot the device
        }
    }

    void LoadNetworkInterfaces() {
        struct ifaddrs* ifaddr;
        if (getifaddrs(&ifaddr) == -1) {
            wxMessageBox("Error fetching interfaces", "Error", wxICON_ERROR);
            return;
        }

        for (struct ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && (ifa->ifa_flags & IFF_UP)) {
                char ip[INET_ADDRSTRLEN] = {0};
                struct sockaddr_in* sa = (struct sockaddr_in*)ifa->ifa_addr;
                inet_ntop(AF_INET, &sa->sin_addr, ip, INET_ADDRSTRLEN);
                wxString ifaceEntry = wxString::Format("%s (%s)", ifa->ifa_name, ip);
                interfaceChoice->Append(ifaceEntry);
            }
        }
        freeifaddrs(ifaddr);
    }
};

// Declare the app class before using wxIMPLEMENT_APP
class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        MyFrame* frame = new MyFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);


