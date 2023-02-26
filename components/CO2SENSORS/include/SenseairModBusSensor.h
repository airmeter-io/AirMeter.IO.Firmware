#include "Common.h"
#include "PinSerial.h"
#include "SenseairBase.h"

class SenseairModBusCommand;

class SenseairModBusSensor : public SenseairBase {
    PinSerial* _serial;
    bool _firstCommand =  true;

    const std::string SOURCE_NAME = "SenseairModbus";;

    bool SendCommand(SenseairModBusCommand& pCommand);
    
    void ReadInputRegistersInfo();
    void ReadHoldingRegistersInfo();
protected: 
    const std::string& GetValuesSourceName() const override;    
public:
    SenseairModBusSensor(PinSerial *pSerial);
    ~SenseairModBusSensor();

    bool RefreshValues() override;
    
    void DisableABC() override;
    void ManualCalibration(int pBaseLinePPM) override;
    void EnableABC(int pBaseLinePPM, int pNumberOfHoursPerCycle) override;
};