// This is an example application demonstrating the use of wxWidgets for GUI generation and
// the Boost library for UDP sockets communication.
// All wxWidgets GUI code was generted by ChatGPT.
// Boost UDP networking is derived from example code generated by ChatGPT.
//
#include <wx/wx.h>
#include <wx/filedlg.h>
#include <wx/choice.h>
#include <wx/socket.h>
#include <wx/dynlib.h>
#include <vector>
#include <string>
#include <thread>

//#include <boost/asio.hpp>
#include <iostream>
#include <stdio.h>

#define BUF_LEN         2048
#define DATA_SIZE       1024
#define BUF_SIZE        (DATA_SIZE+4+4)

// some udp destination/source codes
#define UDP_STAT_CON     9
#define UDP_STAT_REQ     10
#define UDP_FLASH        11

// flash operations
#define FLASH_OP_WRITE  1
#define FLASH_OP_READ   2
#define FLASH_OP_ERASE  3
#define FLASH_OP_ECHO   4
#define FLASH_OP_REBOOT 5

//using boost::asio::ip::udp;

class MyFrame : public wxFrame {
public:

    // constructor for MyFrame
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
        wxButton* fetchDeviceInfoButton = new wxButton(panel, wxID_ANY, "FPGA Info");
        wxStaticText* idLabel = new wxStaticText(panel, wxID_ANY, "ID:");
        deviceIDText = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(100, -1), wxTE_READONLY);
        wxStaticText* versionLabel = new wxStaticText(panel, wxID_ANY, "Version:");
        deviceVersionText = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(100, -1), wxTE_READONLY);
        deviceSizer->Add(fetchDeviceInfoButton, 0, wxALL, 5);
        deviceSizer->Add(idLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        deviceSizer->Add(deviceIDText, 1, wxEXPAND | wxALL, 5);
        deviceSizer->Add(versionLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        deviceSizer->Add(deviceVersionText, 1, wxEXPAND | wxALL, 5);
        sizer->Add(deviceSizer, 0, wxEXPAND | wxALL, 5);

        // Operation checkboxes and progress bars
        wxStaticBoxSizer* opSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Operations");

        eraseCheckBox = new wxCheckBox(panel, wxID_ANY, "Erase");
        blankCheckBox = new wxCheckBox(panel, wxID_ANY, "Blank Check");
        writeCheckBox = new wxCheckBox(panel, wxID_ANY, "Write");
        verifyCheckBox = new wxCheckBox(panel, wxID_ANY, "Verify");
        rebootCheckBox = new wxCheckBox(panel, wxID_ANY, "Reboot");

        eraseProgress = new wxGauge(panel, wxID_ANY, 100, wxDefaultPosition, wxSize(200, 20));
        blankProgress = new wxGauge(panel, wxID_ANY, 100, wxDefaultPosition, wxSize(200, 20));
        writeProgress = new wxGauge(panel, wxID_ANY, 100, wxDefaultPosition, wxSize(200, 20));
        verifyProgress = new wxGauge(panel, wxID_ANY, 100, wxDefaultPosition, wxSize(200, 20));
        rebootProgress = new wxGauge(panel, wxID_ANY, 100, wxDefaultPosition, wxSize(200, 20));

        eraseCheckBox->SetValue(true);
        blankCheckBox->SetValue(true);
        writeCheckBox->SetValue(true);
        verifyCheckBox->SetValue(true);
        rebootCheckBox->SetValue(true);

        opSizer->Add(eraseCheckBox, 0, wxALL, 5);
        opSizer->Add(eraseProgress, 0, wxEXPAND | wxALL, 5);
        opSizer->Add(blankCheckBox, 0, wxALL, 5);
        opSizer->Add(blankProgress, 0, wxEXPAND | wxALL, 5);
        opSizer->Add(writeCheckBox, 0, wxALL, 5);
        opSizer->Add(writeProgress, 0, wxEXPAND | wxALL, 5);
        opSizer->Add(verifyCheckBox, 0, wxALL, 5);
        opSizer->Add(verifyProgress, 0, wxEXPAND | wxALL, 5);
        opSizer->Add(rebootCheckBox, 0, wxALL, 5);
        opSizer->Add(rebootProgress, 0, wxEXPAND | wxALL, 5);

        sizer->Add(opSizer, 0, wxEXPAND | wxALL, 5);

        // Start button
        startButton = new wxButton(panel, wxID_ANY, "Start");
        sizer->Add(startButton, 0, wxALIGN_CENTER | wxALL, 10);

        panel->SetSizer(sizer);

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

    // Progress bars for each operation
    wxGauge* eraseProgress;
    wxGauge* blankProgress;
    wxGauge* writeProgress;
    wxGauge* verifyProgress;
    wxGauge* rebootProgress;

    void OnBrowse(wxCommandEvent&) {

        const uint32_t sync_pat = 0xAA995566;
        const int preamble_length = 48;

        wxFileDialog openFileDialog(this, "Choose a file", "", "", "Bitstream and Binary files (*.bit;*.bin)|*.bit;*.bin", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_OK) {

            // get filename
            fileTextCtrl->SetValue(openFileDialog.GetPath());
            wxString infile = openFileDialog.GetPath();

        }

    }


    void OnFetchDeviceInfo(wxCommandEvent&) {

        // print values
        uint32_t fpga_id, fpga_version;
        fpga_version = 0x00001234;
        fpga_id = 0xdeadbeef;
        deviceIDText->SetValue(wxString::Format(wxT("0x%08x"),fpga_id));
        deviceVersionText->SetValue(wxString::Format(wxT("0x%08x"),fpga_version));

    }

    void OnStart(wxCommandEvent&) {
        // Reset all progress bars to zero at the start
        eraseProgress->SetValue(0);
        blankProgress->SetValue(0);
        writeProgress->SetValue(0);
        verifyProgress->SetValue(0);
        rebootProgress->SetValue(0);
        wxYield();

        if (eraseCheckBox->IsChecked()) {

            // ************ Erase Sectors
            printf("ERASE\n");
            for (int i=0; i<(100); i++){

                eraseProgress->SetValue(i);
                wxYield();
                std::this_thread::sleep_for(std::chrono::milliseconds(5));

            }

        }

        // **********  Blank Check
        if (blankCheckBox->IsChecked()) {

            printf("BLANK_CHECK\n");
            for (int i=0; i<(100); i++) {

                blankProgress->SetValue(i);
                wxYield();
                std::this_thread::sleep_for(std::chrono::milliseconds(5));

            }

        }

        // **** Write Flash
        if (writeCheckBox->IsChecked()) {

            printf("WRITE\n");
            for (int i=0; i<100; i++) {

                writeProgress->SetValue(i);
                wxYield();
                std::this_thread::sleep_for(std::chrono::milliseconds(5));

            }

        }

        // **** Verify Flash
        if (verifyCheckBox->IsChecked()) {

            printf("VERIFY\n");
            for (int i=0; i<100; i++) {

                verifyProgress->SetValue(i);
                wxYield();
                std::this_thread::sleep_for(std::chrono::milliseconds(5));

            }

        }


        // *** Reboot FPGA
        if (rebootCheckBox->IsChecked()) {

            printf("REBOOT\n");
            rebootProgress->SetValue(100);
            wxYield();
        }


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


