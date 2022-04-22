#include "ScreenManager.h"
#include "ScreenDefinition.h"
#include "EpdSpi.h"
#include "SSD1680.h"
#include "DEPG0213BN.h"

ScreenManager::ScreenManager(StringValueSource& pValueSource) : _valueSource(pValueSource) {
    auto f = fopen("/dev/ui.json", "rb");
    if (f != NULL) {        
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET); 

        auto* rawJson = (char *)calloc(fsize + 1,1);
        fread(rawJson, 1, fsize, f);
        fclose(f);

        Json json(rawJson);
        LoadScreens(json);
        free(rawJson);
    }
}

void ScreenManager::LoadScreens(Json& pJson) {
    auto defaultScreen = pJson.GetStringProperty("DefaultScreen");
   // printf("Defualt screen is %s\n", defaultScreen.c_str());
    std::vector<Json*> elements;
    auto screensProp = pJson.GetObjectProperty("Screens");
  //  printf("Got screens property\n");
    screensProp->GetAsArrayElements(elements);
  //  printf("Got elements\n");
    delete screensProp;
    for(auto json : elements) {
        printf("Got a screen %X\n", (uint)json);
        if(json->HasProperty("Name")) {
        //    printf("getting name\n");
            auto name = json->GetStringProperty("Name");
            printf("Name = %sn\n",name.c_str());
            auto screen = new ScreenDefinition(name);
            if(name == defaultScreen) {
                printf("Setting default screen\n");                
                _default = screen;
            }
          //  printf("checking draw\n");
            if(json->HasArrayProperty("Draw")) {
                auto drawProp = json->GetObjectProperty("Draw");
              //  printf("Got draw prop %X\n", (uint)drawProp);
                std::vector<Json*> drawElements;
                drawProp->GetAsArrayElements(drawElements);
               // printf("Got draw elements\n");
                delete drawProp;
                for(auto drawItem : drawElements) {
                 //   printf("Got draw element\n");
                    auto type = drawItem->GetStringProperty("Type");
                  //  printf("Got draw element type\n");
                    if(type == "Clear") 
                        screen->AddDraw(new DrawClearAction(*drawItem));
                    else if (type == "Text") 
                        screen->AddDraw(new DrawTextAction(*drawItem, _fontManager));
                    delete drawItem;
                }
            //    printf("end draw prop\n");
            }
        //    printf("checking actions\n");
            if(json->HasArrayProperty("Actions")) {
                auto actionProp = json->GetObjectProperty("Actions");
                std::vector<Json*> actionElements;
                actionProp->GetAsArrayElements(actionElements);
                delete actionProp;

                for(auto actionItem : actionElements) {
                    delete actionItem;
                }
            }
            _screens.push_back(screen);
        }
       // printf("Finished screen\n");
        delete json;        
    }
   // printf("Finished Loading screens\n");
}


void ScreenManager::Run() {
     std::vector<gpio_num_t> items = {(gpio_num_t)GPIO_NUM_39};
    _buttons = new ButtonManager(items);
 
    EpdSpi io;
    SSD1680 *ssd1680;
    DEPG0213BN *display;
    
    io.init(4,false);
   
    ssd1680 = new SSD1680(io);
    display = new DEPG0213BN(*ssd1680);
    
    auto backBuffer = display->GetBackBuffer();
    auto gfx = new EPDDrawTarget(backBuffer);
    
    auto screen = GetDefault();
    printf("screen = %x\n", (uint)screen);
    gfx->setRotation(1);
    if(screen) {
        DrawContext ctx = {
            .Target = *gfx,
            .ValueSource = _valueSource
        };
        screen->ExecuteDraw(ctx);
        display->UpdateFull();
        
        while(true) {
        // vTaskDelay(5000 / portTICK_RATE_MS);
            _buttons->WaitForEvents();
            while(_buttons->HasQueued()) {
                auto event = _buttons->GetQueued();
                if(event.Gpio==0) {
                    printf("Got Refresh request");
                    screen->ExecuteDraw(ctx);
                    display->UpdatePartial();
                } else {
                    switch(event.Code) {
                        case ButtonEventCode::Pressed:
                            printf("Presssed %d\n", (int)event.Gpio);
                            break;
                        case ButtonEventCode::LongReleased:
                            printf("Long Press %d\n", (int)event.Gpio);
                            break;
                        case ButtonEventCode::VeryLongReleased:
                            printf("Very Long Press %d\n", (int)event.Gpio);
                            break;                
                        case ButtonEventCode::Released:
                            printf("Released %d\n", (int)event.Gpio);
                        // display->Test2(message[index]);
                        
                            break;
                    }
                }
            }
            
            printf("looped\n");
        }
    }
}


void ScreenManager::TriggerUpdate() {
    
    auto now = esp_timer_get_time();  
    GpioEvent event = { .Gpio = (gpio_num_t)0, .Level = 1, .When = now };
    xQueueSend(_buttons->GetGpioGroup()->GetQueueHandle(), &event, NULL);
}