#include "ScreenManager.h"
#include "ScreenDefinition.h"
#include "EpdSpi.h"
#include "SSD1680.h"
#include "DEPG0213BN.h"


ScreenManager::ScreenManager(StringValueSource& pValueSource, SensorManager& pSensorManager) : 
    _valueSource(pValueSource), 
    _sensorManager(pSensorManager) {
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
        _current = _default;
        free(rawJson);
    }
     std::vector<gpio_num_t> items = {(gpio_num_t)GPIO_NUM_39};
    _buttons = new ButtonManager(items);
}


std::vector<DrawAction *> ScreenManager::LoadActions(Json* pParentElement, std::string pElementName) {
    std::vector<DrawAction *> actions;
    if(pParentElement->HasArrayProperty(pElementName)) {
        auto drawProp = pParentElement->GetObjectProperty(pElementName);
        std::vector<Json*> drawElements;
        drawProp->GetAsArrayElements(drawElements);
        delete drawProp;
        for(auto drawItem : drawElements) {
            auto type = drawItem->GetStringProperty("Type");

            if(type == "Clear") 
                actions.push_back(new DrawClearAction(*drawItem));
            else if (type == "Text") 
                actions.push_back(new DrawTextAction(*drawItem, _fontManager));           
            else if (type == "QRCode") 
                actions.push_back(new DrawQRCodeAction(*drawItem));                
            else if (type == "ChangeScreen") 
                actions.push_back(new ChangeScreenAction(*drawItem));
            else if (type == "Calibrate") 
                actions.push_back(new CalibrateAction(*drawItem));            
            else if (type == "TriggerUpdate") 
                actions.push_back(new TriggerUpdateAction(*drawItem));   
            else if (type == "TimeSeries") 
                actions.push_back(new DrawTimeSeriesAction(*drawItem, _fontManager));                    
            delete drawItem;
        }
    }
    return actions;
}

void ScreenManager::LoadScreens(Json& pJson) {
    auto defaultScreen = pJson.GetStringProperty("DefaultScreen");
    std::vector<Json*> elements;
    auto screensProp = pJson.GetObjectProperty("Screens");
 
    screensProp->GetAsArrayElements(elements);
    delete screensProp;
    for(auto json : elements) {
        printf("Got a screen %X\n", (uint)json);
        if(json->HasProperty("Name")) {
            auto name = json->GetStringProperty("Name");
            printf("Name = %s\n",name.c_str());
            auto screen = new ScreenDefinition(name);
            if(name == defaultScreen) {
                printf("Setting default screen\n");                
                _default = screen;
            }

            auto items = LoadActions(json,"Draw");
            for(auto action : items)
                screen->AddDraw(action);
           
            auto pressedItems = LoadActions(json, "ButtonPressed");
            auto longReleaseItems = LoadActions(json, "ButtonLongReleased");
            auto veryLongReleaseItems = LoadActions(json, "ButtonVeryLongReleased");
            auto releaseItems = LoadActions(json, "ButtonReleased");
            
            if(pressedItems.size())
                screen->AddButton(ButtonEventCode::Pressed, pressedItems);
            if(longReleaseItems.size())
                screen->AddButton(ButtonEventCode::LongReleased, longReleaseItems);
            if(veryLongReleaseItems.size())
                screen->AddButton(ButtonEventCode::VeryLongReleased, veryLongReleaseItems);
            if(releaseItems.size())
                screen->AddButton(ButtonEventCode::Released, releaseItems);

            _screens.push_back(screen);
        }
        delete json;        
    }
}


void ScreenManager::Run() {
    
 
    EpdSpi io;
    SSD1680 *ssd1680;
    DEPG0213BN *display;
    
    io.init(4,false);
   
    ssd1680 = new SSD1680(io);
    _display = new DEPG0213BN(*ssd1680);
    
    auto backBuffer = _display->GetBackBuffer();
    auto gfx = new EPDDrawTarget(backBuffer);
    
    auto screen = GetCurrent();
    printf("screen = %x\n", (uint)screen);
    gfx->setRotation(1);
    if(screen) {
        DrawContext ctx = {
            .Target = *gfx,
            .ValueSource = _valueSource,
            .Sensors = _sensorManager,
            .Screens = *this
        };
        printf("Executing first draw\n");
        screen->ExecuteDraw(ctx);
        printf("Executing first update");
        _display->UpdateFull();

        printf("Done first update");
        if(_needScreenChange) {
            printf("Changing screen");
            _needScreenChange = false;
            ChangeScreen(ctx, _nextScreen);
            _display->UpdatePartial();
        }

        while(true) {
            screen = GetCurrent();
            
        // vTaskDelay(5000 / portTICK_RATE_MS);
            printf("Waiting for events\n");
            _buttons->WaitForEvents();
            printf("Got events\n");
            while(_buttons->HasQueued()) {
                auto event = _buttons->GetQueued();
                if(event.Gpio==0) {
                    printf("Got Refresh request");
                    if(_needScreenChange) {
                        printf("Changing screen");
                        _needScreenChange = false;
                        ChangeScreen(ctx, _nextScreen);
                    } else
                        screen->ExecuteDraw(ctx);
                    _display->UpdatePartial();
                } else {
                    screen->ExecuteButton(ctx, event.Code);
                    _display->UpdatePartial();
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


void ScreenManager::ChangeScreen(DrawContext& pContext, std::string pScreen) {
    for(auto screen : _screens) {
        if(screen->GetName() == pScreen) {
            _current = screen;
            screen->ExecuteDraw(pContext);
            printf("Switched to %s screen and redrawed\n", pScreen.c_str());
            return;
        }
    }

    printf("Tried to switch to %s screen and failed\n", pScreen.c_str());
}

void ScreenManager::ChangeScreen(std::string pScreen) {
    printf("changing screen\n");
    _needScreenChange = true;
    _nextScreen = pScreen;
    printf("Triggering update\n");
    TriggerUpdate();
    printf("Update triggered\n");
}

void ScreenManager::TriggerUpdate(DrawContext& pContext) {
    _display->UpdatePartial();
}