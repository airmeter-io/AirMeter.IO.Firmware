#include "MB85FRAMManager.h"

MB85FRAMManager::MB85FRAMManager(I2C& pI2C) : _i2c(pI2C) {
	auto count = _i2c.GetNumDevices();
	for(auto i = 0; i<count; i++) {
		auto device = _i2c.GetDevice(i);
	// 	if(device>=0x50 && device < 0x58) {
	// 		for (uint16_t memSize = 8192; memSize != 0; memSize = memSize * 2)  // Check each memory size
    //   {
    //     Wire.beginTransmission(i);  // Start transmission
    //     Wire.write((uint8_t)0);
    //     Wire.write((uint8_t)0);                        // Start at address 0
    //     _TransmissionStatus = Wire.endTransmission();  // Close transmission
    //     Wire.requestFrom(i, (uint8_t)1);               // Request 1 byte of data
    //     uint8_t minimumByte = Wire.read();             // Store value of byte 0

    //     Wire.beginTransmission(i);  // Start transmission
    //     Wire.write((uint8_t)0);
    //     Wire.write((uint8_t)0);                        // Start at address 0
    //     Wire.write(0xFF);                              // write high value to address 0
    //     _TransmissionStatus = Wire.endTransmission();  // Close transmission

    //     Wire.beginTransmission(i);                     // Start transmission
    //     Wire.write((uint8_t)memSize >> 8);             // Write MSB of address
    //     Wire.write((uint8_t)memSize);                  // Write LsB of address
    //     _TransmissionStatus = Wire.endTransmission();  // Close transmission
    //     Wire.requestFrom(i, (uint8_t)1);               // Request 1 byte of data
    //     uint8_t maximumByte = Wire.read();             // Store value of high byte for chip
    //     Wire.beginTransmission(i);                     // Start transmission
    //     Wire.write((uint8_t)memSize >> 8);             // Write MSB of address
    //     Wire.write((uint8_t)memSize);                  // Write LsB of address
    //     Wire.write(0x00);                              // write low value to max address
    //     _TransmissionStatus = Wire.endTransmission();  // Close transmission

    //     Wire.beginTransmission(i);  // Start transmission
    //     Wire.write((uint8_t)0);
    //     Wire.write((uint8_t)0);                        // Start at address 0
    //     _TransmissionStatus = Wire.endTransmission();  // Close transmission
    //     Wire.requestFrom(i, (uint8_t)1);               // Request 1 byte of data
    //     uint8_t newMinimumByte = Wire.read();          // Store value of byte 0
    //     if (newMinimumByte != 0x00)                    // If the value has changed
    //     {
    //       _I2C[i - MB85_MIN_ADDRESS] = memSize / 1024;  // Store memory size in kB
    //       _TotalMemory += memSize;                      // Add value to total
    //       Wire.beginTransmission(i);                    // Start transmission
    //       Wire.write((uint8_t)0);
    //       Wire.write((uint8_t)0);                        // Position to address 0
    //       Wire.write(minimumByte);                       // restore original value
    //       _TransmissionStatus = Wire.endTransmission();  // Close transmission
    //       break;                                         // Exit the loop
    //     } else {
    //       Wire.beginTransmission(i);                     // Start transmission
    //       Wire.write((uint8_t)memSize >> 8);             // Write MSB of address
    //       Wire.write((uint8_t)memSize);                  // Write LsB of address
    //       Wire.write(maximumByte);                       // restore original value
    //       _TransmissionStatus = Wire.endTransmission();  // Close transmission
    //     }                                                // of if-then-else we've got a wraparound
    //     if (!_I2C[i - MB85_MIN_ADDRESS])                 // If none of the above, then 32kB
    //     {
    //       _I2C[i - MB85_MIN_ADDRESS] = 32;  // Set array size
    //       _TotalMemory += 32768;            // Add value to total
    //     }                                   // of if-then max memory
    //   }                                     // of for-next loop for each memory size
	//	}
	}
}
