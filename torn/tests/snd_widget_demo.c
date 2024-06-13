// Requires an audio file
#include <torn.h>
#include <assert.h>
#include <ui/widgets.h>


i32 main(void)
{
    TORN_Init();
    
    /////////////////////////////////////////////////////////////////
    OS_App* app = OS_ConstructApp("SND | Audio Example", 
                                  OS_AppDefault,
                                  OS_AppDefault,
                                  OS_AppStyleDefault);
    assert(app && "Failed to create window");
    /////////////////////////////////////////////////////////////////
    
    GFX_Renderer* renderer = GFX_CreateRenderer(app);
    assert(renderer && " Failed to create renderer");
    
    const u8* arial_font_location = "C:\\Windows\\Fonts\\Arial.ttf";
    GFX_Font arial = GFX_CreateFont(arial_font_location, 20);
    
    
    const u8* cambriab_font_path  = "C:\\Windows\\Fonts\\cambriab.ttf";
    
    GFX_Font cambriab = GFX_CreateFont(cambriab_font_path, 30);
    
    UI_Button button = UI_CreateButton(cambriab, "PLAY", v2(400,100), v2(500,500-100));
    UI_Button button2 = UI_CreateButton(cambriab, "PAUSE", v2(400,100), v2(500,500));
    UI_Button button3 = UI_CreateButton(cambriab, "Lower Volume", v2(400,100), v2(500,500-200));
    UI_Button button4 = UI_CreateButton(cambriab, "Higher Volume", v2(400,100), v2(500,500-300));
    
    GFX_Text text = 
        GFX_CreateText("...", v2(arial.size,arial.size), arial);
    
    SND_Sound* sound = 
        SND_CreateSound("eastward-legacy.mp3");
    SND_SetSoundVolume(sound, 10);
    while (app->running)
    {
        OS_PollEvents(app);
        GFX_Clear(renderer);
        
        if (UI_GetButtonSignal(&button, app).clicked )
        {
            
            if ((SND_GetSoundState(sound) != SND_AudioPlaying)) { 
                
                SND_PlaySound(sound);
                
            } else if ((SND_GetSoundState(sound) != SND_AudioPaused))
            {
                SND_ResumeSound(sound);
            }
        }
        
        if (UI_GetButtonSignal(&button2, app).clicked )
        {
            
            if ((SND_GetSoundState(sound) == SND_AudioPlaying)) { 
                
                SND_PauseSound(sound);
                
            }
        }
        
        if (UI_GetButtonSignal(&button3, app).clicked )
        {
            if (SND_GetSoundVolume(sound) - 10 >= 0){
                SND_SetSoundVolume(sound,
                                   SND_GetSoundVolume(sound) - 10);
            }
            
        }
        if (UI_GetButtonSignal(&button4, app).clicked )
        {
            if (SND_GetSoundVolume(sound) <= 100){
                SND_SetSoundVolume(sound,
                                   SND_GetSoundVolume(sound) + 10);
            }
            
        }
        
        
        sprintf(text.text, "Paused at: %i\nVolume: %f\nElapsed Time: %f/%f", sound->sound_paused_at, 
                SND_GetSoundVolume(sound),
                SND_GetElapsedSoundTime(sound), 
                sound->duration);
        
        UI_UpdateButton(&button, app);
        UI_UpdateButton(&button2, app);
        UI_UpdateButton(&button3, app);
        UI_UpdateButton(&button4, app);
        
        UI_DrawButton(&button, renderer);
        UI_DrawButton(&button2, renderer);
        UI_DrawButton(&button3, renderer);
        UI_DrawButton(&button4, renderer);
        GFX_DrawText(&text, renderer);
        GFX_Present(renderer);
    }
    
    TORN_Destroy();
    return 0;
}
