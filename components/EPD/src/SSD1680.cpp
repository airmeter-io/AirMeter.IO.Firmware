#include "SSD1680.h"

#define TAG "SSD1680"

extern "C" {
    #include "esp_timer.h"
}
SSD1680::SSD1680(EpdSpi& pSpi) : _spi(pSpi), _gpios({(gpio_num_t)CONFIG_EINK_BUSY}), _group(GpioGroup(_gpios, false, true, RaisingEdge)){
    
    GpioManager::RegisterGpioGroup(&_group);
    _group.Disable();
    gpio_set_pull_mode((gpio_num_t)CONFIG_EINK_BUSY, GPIO_PULLUP_ONLY);
    ResetAll();

}

SSD1680::~SSD1680() {

}

void SSD1680::ResetAll() {
   _group.Enable();
   _spi.reset(50);
   _spi.cmd(ResetAllCommandsAndParameter);
   WaitBusy("epd_wakeup_power:ON"); 
    _group.Disable();
}

void SSD1680::SetRamDataEntryMode(SSD1680RamDataEntryMode pMode, uint16_t pWidth, uint16_t pHeight, uint16_t pXOffset) {

    _spi.cmd(DefineDataEntrySequence);
    _spi.data(pMode);
    switch (pMode)
    {
        case XDecreaseYDescrease: // x decrease, y decrease
            SetRamArea(pWidth / 8, 0, pHeight, 0, pXOffset);  // X-source area,Y-gate area
            SetRamPointer(pWidth / 8, pHeight, pXOffset); // set ram
            break;
        case XIncreaseYDecrease: // x increase, y decrease : as in demo code
            SetRamArea(0, pWidth / 8, pHeight, 0, pXOffset);  // X-source area,Y-gate area
            SetRamPointer(0, pHeight, pXOffset); // set ram
            break;
        case XDecreaseYIncrease: // x decrease, y increase
            SetRamArea(pWidth / 8, 0,0, pHeight, pXOffset);  // X-source area,Y-gate area
            SetRamPointer(pWidth / 8, 0, pXOffset); // set ram
            break;
        case XIncreaseYIncrease: // x increase, y increase : normal mode
            SetRamArea(0, pWidth / 8, 0, pHeight, pXOffset);  // X-source area,Y-gate area
            SetRamPointer(0, 0, pXOffset); // set ram
            break;
    }
}


void SSD1680::SetRamArea(uint8_t pXStart, uint8_t pXEnd, uint16_t pYStart, uint16_t pYEnd, uint16_t pXOffset) {
    _spi.cmd(SetRamXAddressRange);
    _spi.data(pXStart+pXOffset);
    _spi.data(pXEnd+pXOffset);
    _spi.cmd(SetRamYAddressRange);
    _spi.data(pYStart % 255);
    _spi.data(pYStart / 255);
    _spi.data(pYEnd % 255);
    _spi.data(pYEnd / 255);
}


void SSD1680::SetRamPointer(uint8_t pX, uint16_t pY, uint16_t pXOffset) {
    _spi.cmd(SetRamXAddressCounter);
    _spi.data(pX+pXOffset);
    _spi.cmd(SetRamYAddressCounter);
    _spi.data(pY % (256-pXOffset));
    _spi.data(pY /(256-pXOffset));
}


void SSD1680::SetSleepMode(SSD1306SleepMode pMode) {
    _spi.cmd(EnterDeepSleep); // power off display
    _spi.data(pMode);
   // WaitBusy("power_off");
}


void SSD1680::WriteToBWRam(EPDBackBuffer& pBuffer) {
    _spi.cmd(SSD1680Cmd::WriteToBWRam);        // update current data
    auto dataLen = (pBuffer.GetWidth() / 8)*pBuffer.GetHeight();
    uint8_t *sendBuffer = (uint8_t *)malloc(dataLen);
    for (uint16_t y = 0; y < pBuffer.GetHeight(); y++)
    {
        for (uint16_t x = 0; x < pBuffer.GetWidth() / 8; x++)
        {
            uint16_t idx = y * (pBuffer.GetWidth() / 8) + x;
            uint8_t data = (idx < pBuffer.GetBufferLength()) ? pBuffer.GetBuffer()[idx] : 0x00;
           // _spi.data(~data);
            sendBuffer[idx] = ~data;
        }
    }
    _spi.data(sendBuffer,dataLen);  
    free(sendBuffer);
}

void SSD1680::WriteToRedRam(EPDBackBuffer& pBuffer) {
    _spi.cmd(SSD1680Cmd::WriteToRedRam);        // update current data
    auto dataLen = (pBuffer.GetWidth() / 8)*pBuffer.GetHeight();
    uint8_t *sendBuffer = (uint8_t *)malloc(dataLen);
    for (uint16_t y = 0; y < pBuffer.GetHeight(); y++)
    {
        for (uint16_t x = 0; x < pBuffer.GetWidth() / 8; x++)
        {
            uint16_t idx = y * (pBuffer.GetWidth() / 8) + x;
            uint8_t data = (idx < pBuffer.GetBufferLength()) ? pBuffer.GetBuffer()[idx] : 0x00;
           // _spi.data(~data);
            sendBuffer[idx] = ~data;
        }
    }
    _spi.data(sendBuffer,dataLen);  
    free(sendBuffer);
}


void SSD1680::ActivateDisplayUpdateSequence(int pWaitMs) {
    _group.Enable();
    _spi.cmd(SSD1680Cmd::ActivateDisplayUpdateSequence);
//    ets_delay_us(pWaitMs*1000);
    WaitBusy("_Update_Full", 1200);
    _group.Disable();
}


void SSD1680::ActivateDisplayUpdateSequence2(int pWaitMs) {
    _spi.cmd(SSD1680Cmd::ActivateDisplayUpdateSequence);
//    ets_delay_us(pWaitMs*1000);
    WaitBusy2("_Update_Full2",300);
}

void SSD1680::SetDisplayUpdateSequence(SSD1680DisplayUpdateSequence pSequence) {
    _spi.cmd(SSD1680Cmd::DisplayUpdateSequence);
    _spi.data(pSequence);
}

void SSD1680::SendLUT(const SSD1680Lut& pLut) {
  //  uint8_t buffer[sizeof(pLut.Data)+1];
  //  buffer[0] = SSD1680Cmd::SendLUT;
 //   memcpy(buffer+1, pLut.Data, sizeof(pLut.Data));
    _spi.cmd(SSD1680Cmd::SendLUT);
    _spi.data(pLut.Data,pLut.DataLength);
}

void SSD1680::SendEndLUT(SSD1680LutEndOption pOption) {
    _spi.cmd(SSD1680Cmd::EndOptionLUT);
    _spi.data(SSD1680LutEndOption::Normal);
}

void SSD1680::SetGateDrivingVoltage(SSD1680Lut& pLut) {
    if(pLut.GateDrivingVoltage<100 || pLut.GateDrivingVoltage>200 || pLut.GateDrivingVoltage%5!=0) {
        printf("Voltage must be between 10v and 20v and in 0.5v increments\n");
        return;
    }
    uint8_t voltage = 3 + (pLut.GateDrivingVoltage-100)/5;
    _spi.cmd(SSD1680Cmd::SetGateDrivingVoltage);
    _spi.data(voltage);
}

void SSD1680::SetSourceDrivingVoltage(SSD1680Lut& pLut) {
    if(pLut.SourceDrivingVoltage.Vsh1<24 || pLut.SourceDrivingVoltage.Vsh1>170 || (pLut.SourceDrivingVoltage.Vsh1 > 88 && pLut.SourceDrivingVoltage.Vsh1%2!=0)) {
        printf("VSH1 must be between 24 and 170 and after 8.8v be incremented in .2v intervals\n");
        return;
    }
    

    if( (pLut.SourceDrivingVoltage.Vsh2<24 && pLut.SourceDrivingVoltage.Vsh2!=0) || pLut.SourceDrivingVoltage.Vsh2>170 || (pLut.SourceDrivingVoltage.Vsh2 > 88 && pLut.SourceDrivingVoltage.Vsh2%2!=0)) {
        printf("VSH2 must be between 24 and 170 and after 8.8v be incremented in .2v intervals\n");
        return;
    }

    if(pLut.SourceDrivingVoltage.Vsl> -55 || pLut.SourceDrivingVoltage.Vsl< -170 || pLut.SourceDrivingVoltage.Vsl%5!=0) {
        printf("VSL must be between -5.5v and -17v\n");
        return;
    }

    uint8_t vsh1 = pLut.SourceDrivingVoltage.Vsh1 <= 88 ? 0x8e  + (pLut.SourceDrivingVoltage.Vsh1-24) : 0x23+(pLut.SourceDrivingVoltage.Vsh1-90)/2;
    uint8_t vsh2 = pLut.SourceDrivingVoltage.Vsh2==0 ? 0 : pLut.SourceDrivingVoltage.Vsh2 <= 88 ? 0x8e  + (pLut.SourceDrivingVoltage.Vsh2-24) : 0x23+(pLut.SourceDrivingVoltage.Vsh2-90)/2;
    uint8_t vsl = 0xC+((abs(pLut.SourceDrivingVoltage.Vsl)-55)/5)*2;
    _spi.cmd(SSD1680Cmd::SetSourceDrivingVoltage);
    _spi.data(vsh1);
    _spi.data(vsh2);
    _spi.data(vsl);
}

void SSD1680::SetVCOMRegister(SSD1680Lut& pLut) {
    _spi.cmd(SSD1680Cmd::WriteVCOMRegister);
    _spi.data(pLut.VCOM);
}

void SSD1680::EnablePingPong() {
    _spi.cmd(WriteRegisterForDisplayOption);
    _spi.data(0x00);
    _spi.data(0x00);
    _spi.data(0x00);
    _spi.data(0x00);
    _spi.data(0x00);
    _spi.data(0x40);
    _spi.data(0x00);
    _spi.data(0x00);
    _spi.data(0x00);
    _spi.data(0x00);
}

void SSD1680::SetBorderWaveForm(uint8_t pValue) {
    _spi.cmd(SSD1680Cmd::SetBorderWaveForm);
    _spi.data(pValue);
}

void SSD1680::WaitBusy(const char* message, uint16_t busy_time){
//     int64_t time_since_boot = esp_timer_get_time();
//     if (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 0){
//     printf("Is 0\n");
//        return;
//    } else {
//         printf("Is 1\n");
//         while(esp_timer_get_time()-time_since_boot<7000000) {
            
//              _group.WaitForEvents(1000/portTICK_PERIOD_MS);
//             while(_group.HasQueued()) {
//                 auto event = _group.GetQueued();
//                 printf("Wait Busy: %s %d - %d\n",message, (int)(event.When-time_since_boot), event.Level );
//                 if(event.Level == 0&& event.When - time_since_boot>0) {
//                     printf("Done BY gpio\n");
//                     return;
//                 }
//             }
//             vTaskDelay(250/portTICK_PERIOD_MS);
//         }
//         printf("Done BY TIMEOUT\n");
//    }
//    if (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 0){
//        printf("Is 0\n");
//        return;
//    } else {
//         printf("Is 1\n");
//         while(esp_timer_get_time()-time_since_boot<7000000) {
            
//             _group.WaitForEvents(1000/portTICK_PERIOD_MS);
//             while(_group.HasQueued()) {
//                 auto event = _group.GetQueued();
//                 printf("Wait Busy: %s %d - %d\n",message, (int)(event.When-time_since_boot), event.Level );
//                 if(event.Level == 0&& event.When - time_since_boot>0) {
//                     printf("Done BY gpio\n");
//                     return;
//                 }
//             }
//         }
//          printf("Done BY TIMEOUT\n");
//    }

  int64_t time_since_boot = esp_timer_get_time();
  _group.PollEvents();
 //  printf("WB\n");
  while(_group.HasQueued()) {
      _group.GetQueued();
  }
  // On high is busy
  if (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 1) {
    //   printf("Is 1\n");
  while (1){
    _group.PollEvents();
    while(_group.HasQueued()) {
        _group.GetQueued();
    }
    if (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 0) {
   //      printf("Is now 0\n");
        break;

    } 
    vTaskDelay(1);//portTICK_PERIOD_MS);

    if (esp_timer_get_time()-time_since_boot>7000000)
    {
      printf("timeout\n");
      break;
    }
  }
  } else {
  //  printf("Was 0\n");
    vTaskDelay(busy_time/portTICK_PERIOD_MS); 
    _group.PollEvents();
    while(_group.HasQueued()) {
       _group.GetQueued();
   
    }
  }
  _group.PollEvents();
  while(_group.HasQueued()) {
       _group.GetQueued();
   
  }
}

void SSD1680::WaitBusy2(const char* message, uint16_t busy_time){


  int64_t time_since_boot = esp_timer_get_time();
  _group.PollEvents();
 //  printf("WB\n");
  while(_group.HasQueued()) {
      auto event = _group.GetQueued();
    //  printf("Wait Busy: %d - %d\n", (int)(event.When-time_since_boot), event.Level );
  }
  // On high is busy
  if (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 1) {
    //   printf("Is 1\n");
  while (1){
    _group.PollEvents();
    auto shouldBreak = true;
    while(_group.HasQueued()) {
        auto event = _group.GetQueued();
   //     printf("Wait Busy: %d - %d\n", (int)(event.When-time_since_boot), event.Level );
        if(event.Level==0) {
         //   printf("Is2 now 0\n");
        break; 
        }
    }
    if (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 0) {
      //   printf("Is now 0\n");
        break;

    } 
    if(esp_timer_get_time()-time_since_boot>300000)
        vTaskDelay(1);//portTICK_PERIOD_MS);
    else    
        vTaskDelay(1);

    // if (esp_timer_get_time()-time_since_boot>7000000)
    // {
    //   printf("timeout\n");
    //   break;
    // }
  }
  } else {
//    printf("Was 0\n");
    vTaskDelay(busy_time/portTICK_PERIOD_MS); 
    _group.PollEvents();
    while(_group.HasQueued()) {
        auto event = _group.GetQueued();
     //   printf("Wait Busy: %d - %d\n", (int)(event.When-time_since_boot), event.Level );
    }
  }
  _group.PollEvents();
  while(_group.HasQueued()) {
      auto event = _group.GetQueued();
    //  printf("Wait Busy: %d - %d\n", (int)(event.When-time_since_boot), event.Level );
  }
}

void SSD1680::WaitBusy(const char* message){
  int64_t time_since_boot = esp_timer_get_time();
  while(true) {
      _group.WaitForEvents(250/portTICK_PERIOD_MS);
      if (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 0) break;
      while(_group.HasQueued()) {
         auto event = _group.GetQueued();
  //        printf("Wait BusyM: %d - %d\n", (int)(event.When-time_since_boot), event.Level );
         if(event.Level == 0 && event.When - time_since_boot>0) {
             vTaskDelay(1000/portTICK_PERIOD_MS); 
            return;
         }
      }
  }

//  int64_t time_since_boot = esp_timer_get_time();

//   while (1){
//     _group.PollEvents();
//     while(_group.HasQueued()) {
//         auto event = _group.GetQueued();
//         printf("Wait Busy: %d - %d\n", (int)(event.When-time_since_boot), event.Level );
//     }
//     // On low is not busy anymore
//     if (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 0) break;
//     vTaskDelay(1);///portTICK_PERIOD_MS);
//     if (esp_timer_get_time()-time_since_boot>7000000)
//     {
   
//       break;
//     }
//   }
//   _group.PollEvents();
//   while(_group.HasQueued()) {
//       auto event = _group.GetQueued();
//       printf("Wait Busy: %d - %d\n", (int)(event.When-time_since_boot), event.Level );
//   }
}
