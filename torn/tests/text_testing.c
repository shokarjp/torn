
#include <torn.h>
#include <windows.h>
#include <math.h>


i32 main(void)
{
    
    if (TORN_Init())
    {
        MessageBoxA(0, "TORN_Init(): Failed", "TORN_Init(): Failure", MB_ICONERROR|MB_OK);
        return 0;
    }
    
    OS_App* app = OS_ConstructApp("WINDOW", 
                                  OS_AppDefault,
                                  OS_AppDefault,
                                  OS_AppStyleDefault);
    
    OS_SetWindowBorderedFullscreen(app);
    
    
    if (app == 0) { MessageBoxA(0, "Window failed to create", "torn: app", MB_OK); }
    
    
    GFX_Renderer* renderer = GFX_CreateRenderer(app);
    GFX_Font font = GFX_CreateFont("JetBrainsMono-Regular.ttf", 30);
    char* loctext = "You are running a debug build of the engine. Please check if this is intended behavour";
    i32 text_length = 0;
    while(loctext[text_length++] != '\0');
    GFX_Text text = GFX_CreateText(loctext, v2(100,(app->size.h / 2)-30), font);
    
    GFX_Text text2 = GFX_CreateText("/// CONSTRUCTION /// DEBUG BUILD ///", v2(0,font.size), font);
    
    GFX_Rect rectangle = GFX_CreateRect(v2(1024,1024), v2(0,0));
    rectangle.texture = font.glyphs;
    
    GFX_SetClearColor(v4(11,11,11,1));
    while (app->running)
    {
        OS_PollEvents(app);
        GFX_Clear(renderer);
        text2.color = v4(200+ abs(cos(OS_GetTime(app)) * 255),
                         200+ abs(cos(OS_GetTime(app)*2 ) * 255),
                         0,
                         2551);
        
        text.pos.x = abs(cos(OS_GetTime(app)*2) * 20);
        GFX_DrawText(&text, renderer);
        GFX_DrawText(&text2, renderer);
        GFX_Present(renderer);
    }
    
    TORN_Destroy();
    return 0;
}

