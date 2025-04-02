#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

void PrintNetworkInterfaces() {
    ULONG bufferSize = 0;
    GetAdaptersAddresses(AF_INET, 0, NULL, NULL, &bufferSize);

    PIP_ADAPTER_ADDRESSES adapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(bufferSize);
    if (GetAdaptersAddresses(AF_INET, 0, NULL, adapterAddresses, &bufferSize) == NO_ERROR) {
        for (PIP_ADAPTER_ADDRESSES adapter = adapterAddresses; adapter; adapter = adapter->Next) {
            std::wcout << adapter->FriendlyName << ",  " << adapter->Description << std::endl;
        }
    }
    free(adapterAddresses);
}

int main() {
    PrintNetworkInterfaces();

    std::getchar();

    return 0;
}