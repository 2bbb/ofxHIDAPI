//
//  ofxHIDAPI.h
//
//  Created by 2bit on 2023/06/30.
//

#ifndef ofxHIDAPI_h
#define ofxHIDAPI_h

#include "ofLog.h"
#include "ofUtils.h"

#include <hidapi.h>

#include <codecvt>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

namespace ofx {
    namespace HIDAPI {
        struct Device {
            enum class BusType {
                Unknown = 0x00,
                USB = 0x01,
                BlueTooth = 0x02,
                I2C = 0x03,
                SPI = 0x04
            };
            static std::string busTypeString(BusType busType) {
                switch(busType) {
                    case BusType::Unknown: return "Unknown";
                    case BusType::USB: return "USB";
                    case BusType::BlueTooth: return "BlueTooth";
                    case BusType::I2C: return "I2C";
                    case BusType::SPI: return "SPI";
                    default: return "Unexepct Value: " + ofToString((int)busType);
                }
            }
            struct Info {
                std::string path;
                uint32_t vendorID;
                uint32_t productID;
                std::string serialNumber;
                uint32_t releaseNumber;
                std::string manufacturer;
                std::string product;
                uint32_t usagePage;
                uint32_t usage;
                int32_t interfaceNumber;
                BusType busType;
                
                inline friend
                std::ostream &operator<<(std::ostream &os, const Info &info) {
                    return os   << "path: " << info.path << "\n"
                                << "    busType: " << busTypeString(info.busType) << "\n"
                                << "    vendorID: 0x" << std::hex << info.vendorID
                                << ", productID: 0x" << std::hex << info.productID
                                << ", serialNumber: \"" << info.serialNumber << "\""
                                << ", releaseNumber: 0x" << info.releaseNumber << "\n"
                                << "    manufacturer: \"" << info.manufacturer << "\""
                                << ", product: \"" << info.product << "\"\n"
                                << "    usagePage: 0x" << std::hex << info.usagePage << " = " << std::dec << info.usagePage
                                << ", usage: 0x" << std::hex << info.usage << " = " << std::dec << info.usage << "\n";
                }
            };
            
            static std::vector<Info> getDeviceInfos(uint32_t vendorID = 0,
                                                    uint32_t productID = 0)
            {
                auto deviceInfos = hid_enumerate(vendorID, productID);
                std::vector<Info> infos;
                auto it = deviceInfos;
                while(it != nullptr) {
                    infos.emplace_back();
                    auto &info = infos.back();
                    info.path = it->path;
                    info.vendorID = it->vendor_id;
                    info.productID = it->product_id;
                    info.serialNumber = fromWstring(it->serial_number);
                    info.releaseNumber = it->release_number;
                    info.manufacturer = fromWstring(it->manufacturer_string);
                    info.product = fromWstring(it->product_string);
                    info.usagePage = it->usage_page;
                    info.usage = it->usage;
                    info.interfaceNumber = it->interface_number;
                    info.busType = (BusType)it->bus_type;
                    
                    it = it->next;
                }
                hid_free_enumeration(deviceInfos);
                return infos;
            }
            
            inline static std::vector<Info> getAllDeviceInfos()
            { return getDeviceInfos(0, 0); }

            virtual ~Device() {
                close();
            }
            virtual void setup() {
                
            }
            virtual bool connect(uint32_t vendorID,
                                 uint32_t productID,
                                 std::string serial = "")
            {
                device = hid_open(vendorID, productID, serial != "" ? toWstring(serial).c_str() : nullptr);
                if(device == nullptr) {
                    auto err = hid_error(nullptr);
                    ofLogError("ofxHIDAPI::connect") << fromWstring(err);
                    return false;
                }
                return true;
            }
            virtual bool connectWithPath(const std::string &path) {
                device = hid_open_path(path.c_str());
                if(device == nullptr) {
                    auto err = hid_error(nullptr);
                    ofLogError("ofxHIDAPI::connectWithPath") << fromWstring(err);
                    return false;
                }
                return true;
            }

            virtual void close() {
                if(device) hid_close(device);
                device = nullptr;
            }
            
            void setNonBlocking(bool nonBlocking) {
                hid_set_nonblocking(device, nonBlocking ? 1 : 0);
            }
            
            std::string getManufacturer() const {
                wchar_t wstr[256];
                auto res = hid_get_manufacturer_string(device, wstr, 256);
                if(res != 0) {
                    ofLogError("ofxHIDAPI::getProduct") << fromWstring(hid_error(device));
                    return "";
                }
                return fromWstring(wstr);
            }
            
            std::string getProduct() const {
                wchar_t wstr[256];
                auto res = hid_get_product_string(device, wstr, 256);
                if(res != 0) {
                    ofLogError("ofxHIDAPI::getProduct") << fromWstring(hid_error(device));
                    return "";
                }
                return fromWstring(wstr);
            }
            
            std::string getSerialNumber() const {
                wchar_t wstr[256];
                auto res = hid_get_serial_number_string(device, wstr, 256);
                if(res != 0) {
                    ofLogError("ofxHIDAPI::getSerialNumber") << fromWstring(hid_error(device));
                    return "";
                }
                return fromWstring(wstr);
            }
            
            bool isConnected() const
            { return device != nullptr; };
            
#pragma mark - string utility
            static std::string fromWstring(const std::wstring &str) {
                static std::wstring_convert<std::codecvt_utf8<wchar_t>> string_converter;
                return string_converter.to_bytes(str);
            }
            static std::wstring toWstring(const std::string &str) {
                static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wstring_converter;
                return wstring_converter.from_bytes(str);
            }
            
        protected:
            hid_device *device{nullptr};
        }; // struct Device
    }; // namespace HIDAPI
}; // namespace ofx

namespace ofxHIDAPI = ofx::HIDAPI;
using ofxHIDAPIDevice = ofxHIDAPI::Device;

#endif /* ofxHIDAPI_h */
