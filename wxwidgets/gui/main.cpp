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

#include <boost/asio.hpp>
#include <iostream>
#include <stdio.h>

#define     UDP_STAT_CON        9
#define     UDP_STAT_REQ        10

#define     BUF_LEN             2048

using boost::asio::ip::udp;

class MyFrame : public wxFrame {
public:
    MyFrame() : wxFrame(nullptr, wxID_ANY, "wxWidgets GUI", wxDefaultPosition, wxSize(800, 700)) {

        // network settings
        network_interface = "enx94103eb7e201";
        pc_ip     = "16.0.0.200";  
        device_ip = "16.0.0.128"; 
        port = 1234;

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

    std::string network_interface;
    std::string pc_ip;
    std::string device_ip;
    int port;

    // Progress bars for each operation
    wxGauge* eraseProgress;
    wxGauge* blankProgress;
    wxGauge* writeProgress;
    wxGauge* verifyProgress;
    wxGauge* rebootProgress;


    void OnInterfaceSelected(wxCommandEvent&) {
        int selection = interfaceChoice->GetSelection();
        if (selection != wxNOT_FOUND) {
            network_interface = interfaceChoice->GetString(selection);
            wxLogMessage("User selected interface: %s", network_interface);
        }
    }

    void OnBrowse(wxCommandEvent&) {
        wxFileDialog openFileDialog(this, "Choose a file", "", "", "Bitstream and Binary files (*.bit;*.bin)|*.bit;*.bin", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_OK) {
            fileTextCtrl->SetValue(openFileDialog.GetPath());
        }
    }

    void OnFetchDeviceInfo(wxCommandEvent&) {
        // Set IP address on network interface 
        SetIPAddress(network_interface, pc_ip);

        // setup tx socket
        boost::asio::io_service tx_io_service;
        udp::socket tx_socket(tx_io_service);
        udp::endpoint device_endpoint(boost::asio::ip::address::from_string(device_ip), port);
        tx_socket.open(udp::v4());

        // setup rx socket
        boost::asio::io_service rx_io_service;
        udp::socket rx_socket(rx_io_service, udp::endpoint(udp::v4(), port));
        std::cout << "UDP server listening on port " << port << "..." << std::endl;

        // send status request packet
        char txbuf[BUF_LEN];
        txbuf[0] = 0xaa; txbuf[1] = 0xbb; txbuf[2] = 0xcc; txbuf[3] = UDP_STAT_REQ;
        tx_socket.send_to(boost::asio::buffer(std::string(txbuf,4)), device_endpoint);

        // receive packets
        char rxbuf[BUF_LEN];
        uint32_t* rxregbuf = (uint32_t *)rxbuf;
        udp::endpoint remote_endpoint;
        uint8_t fpga_source;
        do {
            size_t length = rx_socket.receive_from(boost::asio::buffer(rxbuf), remote_endpoint);
            fpga_source = rxbuf[3];
        } while (fpga_source != UDP_STAT_CON);

        // print received values
        uint32_t fpga_id, fpga_version;
        fpga_version = rxregbuf[1]; fpga_id = rxregbuf[2]; 
        printf("fpga_source = 0x%02x, fpga_id = 0x%08x, fpga_version = 0x%08x", fpga_source, fpga_id, fpga_version);
        std::cout << " from " << remote_endpoint << std::endl;

        // Example code to set values
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

        if (eraseCheckBox->IsChecked()) {
            // Simulate a long-running operation for Erase
            for (int i = 0; i <= 100; i++) {
                wxMilliSleep(5);  // Faster progress (5 ms delay)
                eraseProgress->SetValue(i);
            }
        }
        if (blankCheckBox->IsChecked()) {
            // Simulate a long-running operation for Blank Check
            for (int i = 0; i <= 100; i++) {
                wxMilliSleep(5);  // Faster progress (5 ms delay)
                blankProgress->SetValue(i);
            }
        }
        if (writeCheckBox->IsChecked()) {
            // Simulate a long-running operation for Write
            for (int i = 0; i <= 100; i++) {
                wxMilliSleep(5);  // Faster progress (5 ms delay)
                writeProgress->SetValue(i);
            }
        }
        if (verifyCheckBox->IsChecked()) {
            // Simulate a long-running operation for Verify
            for (int i = 0; i <= 100; i++) {
                wxMilliSleep(5);  // Faster progress (5 ms delay)
                verifyProgress->SetValue(i);
            }
        }
        if (rebootCheckBox->IsChecked()) {
            // Simulate a long-running operation for Reboot
            for (int i = 0; i <= 100; i++) {
                wxMilliSleep(5);  // Faster progress (5 ms delay)
                rebootProgress->SetValue(i);
            }
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


    void SetIPAddress(const std::string& interface, const std::string& ipAddress) {
        std::string command;
        #ifdef __linux__
            command = "sudo ifconfig " + interface + " " + ipAddress;
        #elif _WIN32
            command = "netsh interface ip set address name=\"" + interface + "\" static " + ipAddress + " 255.255.255.0";
        #else
            std::cerr << "Unsupported platform" << std::endl;
            return;
        #endif
        if (system(command.c_str()) != 0) {
            std::cerr << "Error setting IP address" << std::endl;
            exit(1);
        } else {
            std::cerr << "interface = " << interface << ", IP address = " << ipAddress << std::endl;
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


