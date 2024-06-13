// This source code uses everything avaliable in TORN, as of now.
// This is used to check if behavour matches up correctly through all refactoring and 
// introductions to new rendering APIs.
#include <torn.h>
#include <assert.h>
#include <math.h>

i32 main(void)
{
    TORN_Init();
    
    /////////////////////////////////////////////////////////////////
    OS_App* app = OS_ConstructApp("Hello, Torn!", 
                                  OS_AppDefault,
                                  OS_AppDefault,
                                  OS_AppStyleDefault);
    assert(app && "Failed to create window");
    /////////////////////////////////////////////////////////////////
    
    GFX_Renderer* renderer = GFX_CreateRenderer(app);
    assert(renderer && "Failed to create renderer");
    
    
    // Remark: Graphics2D High-level pipeline
    /////////////////////////////////////////////////////////////////
    V2F gradient_rectangle_pos = v2(100,100);
    GFX_Rect gradient_rectangle = GFX_CreateRect(v2(100,100), gradient_rectangle_pos);
    {
        gradient_rectangle.color.top_left  =    v4(78,78,78,255);
        gradient_rectangle.color.top_right =    v4(78,78,78,255);
        gradient_rectangle.color.bottom_left  = v4(10,10,10,255);
        gradient_rectangle.color.bottom_right = v4(10,10,10,255);
    }
    
    
    /////////////////////////////////////////////////////////////////
    V2F rgb_gradient_pos = v2(gradient_rectangle.size.w+gradient_rectangle.pos.x+10,gradient_rectangle.pos.y);
    GFX_Rect rgb_gradient = GFX_CreateRect(v2(100,100),rgb_gradient_pos);
    {
        rgb_gradient.color.top_left  =    v4(255,0,0,255);
        rgb_gradient.color.top_right =    v4(0,255,0,255);
        rgb_gradient.color.bottom_left  = v4(0,0,255,255);
        rgb_gradient.color.bottom_right = v4(255,255,255,255);
    }
    /////////////////////////////////////////////////////////////////
    GFX_Texture2D texture = GFX_LoadTexture2D("C:\\Windows\\System32\\@WLOGO_48x48.png");  // This is valid in Windows 10 21H2 KB5016616.
    /////////////////////////////////////////////////////////////////
    V2F textured_rectangle_pos = v2(rgb_gradient.size.w+rgb_gradient.pos.x+10,rgb_gradient.pos.y);
    GFX_Rect textured_rectangle = GFX_CreateRect(v2(100,100),textured_rectangle_pos);
    {
        textured_rectangle.color.top_left  =    v4(255,0,0,255);
        textured_rectangle.color.top_right =    v4(0,255,0,255);
        textured_rectangle.color.bottom_left  = v4(0,0,255,255);
        textured_rectangle.color.bottom_right = v4(255,255,255,255);
        textured_rectangle.texture = texture;
    }
    /////////////////////////////////////////////////////////////////
    const u8* arial_font_location = "C:\\Windows\\Fonts\\Arial.ttf";
    GFX_Font arial = GFX_CreateFont(arial_font_location, 20);
    /////////////////////////////////////////////////////////////////
    V2F text_position = v2(gradient_rectangle.pos.x, gradient_rectangle.pos.y+gradient_rectangle.size.h+arial.size+20);
    GFX_Text text = GFX_CreateText("the quick brown fox jumps over the lazy dog.\nThe Quick Brown Fox Jumps Over The Lazy Dog.\nTHE QUICK BROWN FOX JUMPS OVER THE LAZY DOG", text_position , arial);
    text.v_spacing = 20;
    text.h_spacing = 1;
    /////////////////////////////////////////////////////////////////
    V2F moveable_text_position = v2(app->size.w / 2 + 100, app->size.h/2+100);
    GFX_Text moveable_text = GFX_CreateText("Circling through space time!", moveable_text_position , arial);
    /////////////////////////////////////////////////////////////////
    // Remark: Graphics2D High-level pipeline end.
    
#ifdef DRAW_ON_SEPERATE_FRAMEBUFFER
    GFX_Framebuffer framebuffer = GFX_CreateFramebuffer(renderer, GFX_Format_RGBA);
    
    /////////////////////////////////////////////////
    GFX_Rect        framebuffer_rectangle = GFX_CreateRect(v2(app->size.w, app->size.h), v2(0,0));
    framebuffer_rectangle.texture = framebuffer.texture;
    GFX_FlipRectUV(&framebuffer_rectangle);
    /////////////////////////////////////////////////
#endif
    
    
    while (app->running)
    {
        OS_PollEvents(app);
        
        // OS
        /////////////////////////////////////////////////
        r32 time = OS_GetTime(app);
        /////////////////////////////////////////////////
        // OS End
        
#ifdef DRAW_ON_SEPERATE_FRAMEBUFFER
        GFX_PushFramebuffer(framebuffer); {
#endif
            GFX_Clear(renderer);
            // Remark: Graphics2D High-level pipeline
            /////////////////////////////////////////////////////////////////
            moveable_text.pos.x = moveable_text_position.x +  cos(time*2) * 100;
            moveable_text.pos.y = moveable_text_position.y +  sin(time*2) * 100;
            
            gradient_rectangle.pos.y = gradient_rectangle_pos.y   +  (sin(time*3) * 10 );
            rgb_gradient.pos.y       = rgb_gradient_pos.y         +  (sin(time*2) * 10 );
            textured_rectangle.pos.y = textured_rectangle_pos.y   +  (sin(time*1) * 10 );
            GFX_DrawRect(&gradient_rectangle, renderer);
            GFX_DrawRect(&rgb_gradient, renderer);
            GFX_DrawRect(&textured_rectangle, renderer);
            GFX_DrawText(&text, renderer);
            GFX_DrawText(&moveable_text, renderer);
            /////////////////////////////////////////////////////////////////
            // Remark: Graphics2D High-level pipeline end.
#ifdef DRAW_ON_SEPERATE_FRAMEBUFFER
        } GFX_PopFramebuffer();
#endif
        
#ifdef DRAW_ON_SEPERATE_FRAMEBUFFER
        /////////////////////////////////////////////////
        GFX_Clear(renderer);
        GFX_DrawRect(&framebuffer_rectangle, renderer);
        /////////////////////////////////////////////////
#endif
        GFX_Present(renderer);
    }
    
    TORN_Destroy();
    return 0;
}

