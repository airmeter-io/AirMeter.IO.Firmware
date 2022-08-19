#include "ScreenManager.h"
#include "ScreenDefinition.h"



ScreenManager::ScreenManager(DrawControl* pDrawControl, StringValueSource& pValueSource, SensorManager& pSensorManager, ScreenManagerNotifier& pNotifier) : 
    _notifier(pNotifier),
    _valueSource(pValueSource), 
    _sensorManager(pSensorManager),
    _drawControl(pDrawControl) {
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


void ScreenManager::Run(TickType_t pNotifyPeriod) {
    
    

    std::vector<gpio_num_t> items = {(gpio_num_t)GPIO_NUM_39};
    _buttons = new ButtonManager(items);
   
    
    auto screen = GetCurrent();
    printf("screen = %x\n", (uint)screen);

    auto gfx = _drawControl->GetDrawTarget();
    gfx->SetRotation(1);
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
        _drawControl->RenderToDisplay(false);

       

        while(true) {
           
            
        // vTaskDelay(5000 / portTICK_RATE_MS);
    
            _buttons->WaitForEvents(pNotifyPeriod);

            bool requiresRedraw = _notifier.ProcessOnUIThread();
            screen = GetCurrent();
            bool hadButtons = false;
            while(_buttons->HasQueued()) {
                
                auto event = _buttons->GetQueued();
                if(event.Gpio==0) {
                    printf("Got Refresh request");
                    screen->ExecuteDraw(ctx);
                    _drawControl->RenderToDisplay(true);
                    requiresRedraw = false;
                } else {
                    hadButtons = true;
                     printf("Executing button %d\n", event.Code);
                    screen->ExecuteButton(ctx, event.Code);
                    _drawControl->RenderToDisplay(true);
                    requiresRedraw = false;
                }
            }

            if(requiresRedraw) {
                screen->ExecuteDraw(ctx);
                _drawControl->RenderToDisplay(true);
            }
            // if(hadButtons) {
            //     _buttons->WaitForEvents(10);
            //     while(_buttons->HasQueued()) {
            //         auto event = _buttons->GetQueued();
            //     }
            // }
        }
    }
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
    for(auto screen : _screens) {
        if(screen->GetName() == pScreen) {
            _current = screen;
           
            printf("Switched to %s screen \n", pScreen.c_str());
            return;
        }
    }
}

void ScreenManager::TriggerUpdate(DrawContext& pContext) {
   _drawControl->RenderToDisplay(true);
}