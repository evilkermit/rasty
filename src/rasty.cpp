#include "rasty.h"

#include <ospray/ospray.h>

#include <chrono>
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include "gdal/gdal_priv.h"
#include "gdal/cpl_conv.h"
namespace rasty
{

/**
 * rastyInit, rastyDestroy
 * initialize and shutdown ospray and gdal
*/
void rastyInit(int argc, const char **argv)
{
    // initialize ospray
    ospInit(&argc, argv);
    // register gdal drivers
    GDALAllRegister();
}

void rastyDestroy()
{
    ospShutdown();
    GDALDestroy();
}

/**
 * createID
 * creates a unique ID for an object
 * uses the MAC address of the first interface
 * and the current time
*/
std::string createID()
{
    // IDs are composed of a MAC address followed by a time
    FILE *arp = fopen("/proc/net/arp", "r");
    if(arp == NULL) {
        std::cerr << "WARNING: Could not open /proc/net/arp. ";
        std::cerr << "Object IDs won't work." << std::endl;
        return "";
    }

    char buffer[1024];
    // read the column headers
    if(fgets(buffer, 1024, arp) == NULL) {
        std::cerr << "WARNING: Unexpected format in /proc/net/arp. ";
        std::cerr << "Object IDs won't work." << std::endl;
        return "";
    }

    if(fgets(buffer, 1024, arp) == NULL) {
        std::cerr << "WARNING: Unexpected error reading /proc/net/arp. ";
        std::cerr << "Object IDs won't work." << std::endl;
        return "";
    }

    fclose(arp);

    char *macaddr;
    macaddr = strtok(buffer, " ");
    macaddr = strtok(NULL, " ");
    macaddr = strtok(NULL, " ");
    macaddr = strtok(NULL, " ");

    // current time in milliseconds
    unsigned long int ms = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    std::string ID(macaddr);
    
    return ID + ":" + std::to_string(ms);
}
}
