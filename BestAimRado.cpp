#include <iostream>
#include <windows.h>
#include <vector>

// Function to read a value from a memory address
float ReadMemoryValue(HANDLE processHandle, uintptr_t address) {
    float value = 0;
    ReadProcessMemory(processHandle, (LPCVOID)address, &value, sizeof(value), nullptr);
    return value;
}

// Function to write a value to a memory address
void WriteMemoryValue(HANDLE processHandle, uintptr_t address, float value) {
    WriteProcessMemory(processHandle, (LPVOID)address, &value, sizeof(value), nullptr);
}

// Function to calculate the aim angle to a target
void CalculateAimAngle(float playerX, float playerY, float targetX, float targetY, float& aimX, float& aimY) {
    float dx = targetX - playerX;
    float dy = targetY - playerY;
    float distance = CalculateDistance(playerX, playerY, targetX, targetY);
    
    // Add some prediction based on distance
    float predictionFactor = 0.1f;
    aimX = targetX + dx * predictionFactor;
    aimY = targetY + dy * predictionFactor;
    
    // Normalize the aim angle
    float magnitude = sqrt(aimX * aimX + aimY * aimY);
    aimX /= magnitude;
    aimY /= magnitude;
}

// Function to calculate distance between two points
float CalculateDistance(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int main() {
    // Replace 'FortniteClient-Win64-Shipping.exe' with the actual name of the Fortnite executable
    const char* targetGame = "FortniteClient-Win64-Shipping.exe";

    // Find the process ID of the target game
    DWORD processID = 0;
    HWND hwnd = FindWindowA(NULL, targetGame);
    if (hwnd == NULL) {
        std::cerr << "Could not find the game window." << std::endl;
        return 1;
    }
    GetWindowThreadProcessId(hwnd, &processID);
    if (processID == 0) {
        std::cerr << "Could not get the process ID." << std::endl;
        return 1;
    }

    // Open the process with necessary permissions
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (processHandle == NULL) {
        std::cerr << "Could not open the process." << std::endl;
        return 1;
    }

    // Example memory addresses for player coordinates and aim angles (placeholders)
    uintptr_t playerXAddress = 0x12345678;
    uintptr_t playerYAddress = 0x12345680;
    uintptr_t targetXAddress = 0x12345688;
    uintptr_t targetYAddress = 0x12345690;
    uintptr_t aimXAddress = 0x12345698;
    uintptr_t aimYAddress = 0x123456A0;

    const int UPDATE_INTERVAL_MS = 10; // Update every 10 milliseconds

    while (true) {
        // Read player and target coordinates
        float playerX = ReadMemoryValue(processHandle, playerXAddress);
        float playerY = ReadMemoryValue(processHandle, playerYAddress);
        float targetX = ReadMemoryValue(processHandle, targetXAddress);
        float targetY = ReadMemoryValue(processHandle, targetYAddress);

        float aimX, aimY;
        CalculateAimAngle(playerX, playerY, targetX, targetY, aimX, aimY);

        // Write calculated aim angles to memory
        WriteMemoryValue(processHandle, aimXAddress, aimX);
        WriteMemoryValue(processHandle, aimYAddress, aimY);

        // Add a small delay to reduce CPU usage
        Sleep(UPDATE_INTERVAL_MS);

        // Check if the program should exit (e.g., if the End key is pressed)
        if (GetAsyncKeyState(VK_END) & 0x8000) {
            break;
        }
    }

    // Clean up and close the process handle
    CloseHandle(processHandle);

    return 0;
}