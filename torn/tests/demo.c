// This demo requires a sound file. 
// --------------------------------------------------------------
// Keys:
// WASD to move.
// E to shoot.
// T to hide debugging UI
// Y to show debugging UI
// Z to enable post-processing.
// X to disable vignette
// C to disable post-processing
 


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <torn.h>
#include <assert.h>


struct Enemy
{
    GFX_Rect enemy;
    i32 speed;
    V2F velocity;
    i32 on_ground;
};


struct Enemy EnemyCreate(V2F position)
{
    struct Enemy enemy;
    enemy.enemy = GFX_CreateRect(v2(100,150), position);
    GFX_SetRectColor(&enemy.enemy, v4(0,255,0,255));
    enemy.speed = 3;
    enemy.velocity = v2(10,0);
    return enemy;
} 

void EnemyUpdate(struct Enemy* enemy, V2F player_position, V2F ground, i32 facing)
{
    
    enemy->velocity.y += 1.5;
    enemy->enemy.pos.y += enemy->velocity.y;
    enemy->enemy.pos.x += enemy->velocity.x;
    enemy->velocity.y *= 0.9;
    enemy->velocity.x *= 0.9;
    
    // Player-following code.
    
    if (enemy->on_ground){
        V2F direction = v2(player_position.x - enemy->enemy.pos.x,
                           player_position.y - enemy->enemy.pos.y);
        
        enemy->velocity.x = enemy->speed * -(
                                             
                                             ( direction.x / sqrt(direction.x * direction.x + direction.y * direction.y))
                                             ) ;
    }
    
    if (enemy->enemy.pos.y > ground.y - enemy->enemy.size.y)
    {
        enemy->on_ground = 1;
        enemy->enemy.pos.y = ground.y  - enemy->enemy.size.y;
        enemy->velocity.y = 0;
        
    }
}

void EnemyDraw(struct Enemy* enemy, GFX_Renderer* renderer)
{
    
    GFX_DrawRect(&enemy->enemy, renderer);
}
struct Bullet
{
    V2F velocity;
    GFX_Rect bullet;
    i32 speed;
    i32 facing;
};
i32 SizeOfStr(const char* size)
{
    i32 i = 0;
    while   (size[i++]!='\0');
    return i;
    
}
r32 Lerp(i32 s, i32 e, i32 amt)
{
    return amt * (s-e)+s;
}

struct Bullet BulletCreate(V2F position, i32 facing)
{
    struct Bullet bullet = {0};
    bullet.speed = 20;// default speed;
    bullet.facing = facing;
    
    
    bullet.velocity.x = bullet.speed; 
    bullet.bullet = GFX_CreateRect( v2(25,25), position);
    GFX_SetRectColor(&bullet.bullet, v4(255,0,0,255));
    return bullet;
}

void BulletShoot(struct Bullet* bullet)
{
    
    bullet->bullet.pos.x += bullet->velocity.x * bullet->facing;
    
    
    
    
    
}
 
void BulletDraw(struct Bullet* bullet, GFX_Renderer* renderer)
{
    GFX_DrawRect(&bullet->bullet, renderer);
}

i32 main(void)
{
    TORN_Init();
    
    /////////////////////////////////////////////////////////////////
    OS_App* app = OS_ConstructApp("Untitled Game Project", 
                                  OS_AppDefault,
                                  OS_AppDefault  ,
                                  OS_AppStyleDefault);
    assert(app && "Failed to create window");
    
    //OS_SetWindowBorderlessFullscreen(app);
    /////////////////////////////////////////////////////////////////            
    
    GFX_Renderer* renderer = GFX_CreateRenderer(app);
    assert(renderer && "Failed to create renderer");
    
    GFX_Rect player = GFX_CreateRect(v2(100,150), v2(0,0));
    GFX_SetRectColor(&player, v4(255,0,0,255));
    V2F velocity = v2(0,0);
    i32 speed = 10;
    
    //GFX_Texture2D player_texture = GFX_LoadTexture2D("player.png");
    //player.texture = player_texture;
    
    
    
    const u8* arial_font_location = "C:\\Windows\\Fonts\\Arial.ttf";
    GFX_Font arial = GFX_CreateFont(arial_font_location, 20);
    GFX_Text debug = GFX_CreateText(" ", v2(arial.size,arial.size+20), arial);
    
    b32 on_ground = 0;
    b32 facing_left = 1;
    
    
    struct Bullet* bullet = MEM_ArenaAlloc(MEM_GetDefaultArena(), 
                                           sizeof(struct Bullet) * 100);
    i32 bullet_memory_size = 100;
    i32 bullet_memory_used = 0;
    
    
    const u8* cambriab_font_path  = "C:\\Windows\\Fonts\\cambriab.ttf";
    
    GFX_Font cambriab = GFX_CreateFont(cambriab_font_path, 90);
    
    GFX_Text splash_text = GFX_CreateText("Act I", v2(   (app->size.w /2)-cambriab.size, app->size.h /2), cambriab);
    
    
    splash_text.color = v4(255,255,255,255);
    r32 time_elapsed = 1;
    r32 time = 0;
    
    GFX_Rect ground = GFX_CreateRect(v2(app->size.x, 100), v2(0, (app->size.h/2)+200));
    GFX_SetRectColor(&ground, v4(255,255,255,255));
    
    GFX_Rect skybox = GFX_CreateRect(v2(app->size.x, app->size.y), v2(0,0));
    
    V4F g1 = v4(40,40,40,255);
    V4F g2 = v4(0,0,0,255);
    skybox.color.top_left = g1;
    skybox.color.top_right = g1;
    
    skybox.color.bottom_left = g2;
    skybox.color.bottom_right = g2;
    
    GFX_Rect cover = GFX_CreateRect(v2(350,300), v2(0,0));
    GFX_SetRectColor(&cover, v4(0,0,0,200));
    
    
    
    i32 waves = 0;
    MEM_Arena enemy_arena = MEM_ArenaInit(1024);
    
    struct Enemy* enemies = MEM_ArenaAlloc(MEM_GetDefaultArena(), sizeof(struct Enemy) * 1024);
    i32 enemy_memory_used = 0;
    i32 enemy_memory_size = 1024;
    U_Clock clock; 
    
    for (i32 i = 0;i<1;i++)
    {
        enemies[enemy_memory_used]  = EnemyCreate(v2(300+(100 * i),i*10));
        enemy_memory_used++;
    }
    
    GFX_Framebuffer framebuffer = GFX_CreateFramebuffer(renderer,
                                                        GFX_Format_RGB);
    
    
    GFX_Rect display = GFX_CreateRect(v2(app->size.w, app->size.h), v2(0,0));
    display.texture = framebuffer.texture;
    GFX_FlipRectUV(&display);
    
    
    GFX_Framebuffer vignette = GFX_CreateFramebuffer(renderer,
                                                     GFX_Format_RGB);
    GFX_Rect vignette_rect = GFX_CreateRect(v2(app->size.w, app->size.h), v2(0,0));
    vignette_rect.texture = vignette.texture;
    GFX_FlipRectUV(&vignette_rect);
    
    u8* vertex_shader =
        "#version 330\n"
        "layout (location = 0) in vec3 i_pos;\n"
        "layout (location = 1) in vec4 i_color;\n"
        "layout (location = 2) in vec2 i_uv;\n"
        "layout (location = 3) in vec4 i_normals;\n"
        "uniform mat4 orthographic;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "out vec4 v_pos;\n"
        "out mat4 mvp;\n"
        "out vec4 v_color;\n"
        "out vec2 v_uv;\n"
        "void main()\n"
        "{\n"
        "\tv_uv = i_uv;\n"
        "\tv_color = i_color;\n"
        "\tmvp = orthographic * model * view;\n"
        "\tv_pos = vec4(i_pos, 1.0) * mvp;\n"
        "\tgl_Position = mvp *vec4(i_pos, 1);\n"
        "}\n\0";
    
    u8* fragment_shader= 
        "#version 330\n"
        "out vec4 f_color;\n"
        "in vec4 v_color;\n"
        "in vec2 v_uv;\n"
        "in vec4 v_pos;\n"
        "in mat4 mvp;\n"
        
        "uniform sampler2D t_texture2d;\n"
        
        
        
        "void main() {\n"
        
        "vec4 ambient = vec4(0.02, 0.02, 0.3, 1.0);\n"
        "vec2 light_pos = vec2(600,600);\n" // Perhaps make this a uniform
    
        "light_pos = ( mvp * vec4(light_pos, 0, 1) ).xy;\n"
        
        "vec2 light_frag = light_pos - v_pos.xy;\n"
        "light_frag.y /= 1.0;\n"
        
        "float vec_length = clamp(length(light_frag) /2.5, 0, 1);\n"
        
        "vec4 pixel = texture2D(t_texture2d, v_uv);\n"
        
        "\tf_color = pixel * v_color * ( clamp(ambient + vec4(1-vec_length, 1-vec_length, 1-vec_length, 1), 0, 1));\n\n"
        "}\n\0";
    
    
    u8* vignette_vs =
        "#version 330\n"
        "layout (location = 0) in vec3 i_pos;\n"
        "layout (location = 1) in vec4 i_color;\n"
        "layout (location = 2) in vec2 i_uv;\n"
        "layout (location = 3) in vec4 i_normals;\n"
        "uniform mat4 orthographic;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "out vec4 v_color;\n"
        "out vec2 v_uv;\n"
        "void main()\n"
        "{\n"
        "\tv_uv = i_uv;\n"
        "\tv_color = i_color;\n"
        "\tgl_Position = orthographic * model * view * vec4(i_pos, 1);\n"
        "}\n\0";
    
    u8* vignette_fs = 
        "#version 330\n"
        "out vec4 f_color;\n"
        "in vec4 v_color;\n"
        "in vec2 v_uv;\n"
        "uniform sampler2D t_texture2d;\n"
        "uniform vec2 res;\n"
        "void main() {\n"
        "vec4 color = texture2D(t_texture2d, v_uv);\n"
        "float dist = distance(v_uv, vec2(0.5,0.5));\n"
        "color.rgb *= smoothstep(0.8, 0.0799, dist * (.95));\n"
        "\tf_color = color;\n"
        "}\n\0";
    
    GFX_Shader shader = GFX_CreateShader(vertex_shader, fragment_shader);
    GFX_ConfigureShader(&shader, renderer);
    
    
    
    GFX_Shader vignette_shader = GFX_CreateShader(vignette_vs, vignette_fs);
    GFX_ConfigureShader(&vignette_shader, renderer);
    GFX_GLSLShader2f(vignette_shader, "res", app->size.w, app->size.y);
    GFX_PopShader();
    b32 disable_vignette  = 1;
    b32 hide_debug  = 0;
    b32 disable_post_processing = 1;
    
    U_Clock gun_cool_down;
    
    GFX_Rect black_bar_top = GFX_CreateRect(v2(app->size.w, 50), v2(0,0));
    GFX_Rect black_bar_bottom = GFX_CreateRect(v2(app->size.w, 50), v2(0,
                                                                       app->size.h - black_bar_top.size.h));
    GFX_SetRectColor(&black_bar_top, v4(0,0,0,255));
    GFX_SetRectColor(&black_bar_bottom, v4(0,0,0,255));
    
    
    SND_Sound* sound = SND_CreateSound("eastward-legacy.mp3");
    if (sound == 0)
    {
        TORN_Error("This demo requires a sound file.", "eastward-legacy.mp3 not found!");
        return 1;
    } 
    SND_PlaySound(sound);
    SND_SetSoundVolume(sound, 10);
    while (app->running)
    {
        
        
        OS_PollEvents(app);
        sprintf(debug.text, "Bullet Objects: %i\nFacing: %i\nElapsed: %f\nEnemy Count: %i\nWave: %i\nPosition: (X:%i, Y:%i)\nPost-Processing: %i\nisAudioPlaying: %i\n%f/%f\nVol: %i", bullet_memory_used, facing_left, time_elapsed, enemy_memory_used, waves, (i32)player.pos.x, (i32)player.pos.y, disable_post_processing,SND_GetSoundState(sound), SND_GetElapsedSoundTime(sound), sound->duration, (i32)SND_GetSoundVolume(sound));
        
        if (SND_GetSoundState(sound) == SND_AudioStopped)
        {
            SND_SetSoundTime(sound, 0);
        }
        
        splash_text.text = "DEBUG";
        if (OS_GetKeyState('E').pressed)
        { 
            if (U_ClockGetElapsedTime(&gun_cool_down, app) >= 0.2) {
                bullet[bullet_memory_used++] = BulletCreate(v2(player.pos.x, player.pos.y + (player.size.y/2)), facing_left);
                U_ClockRestart(&gun_cool_down, app);
            }
        }
        
        if (OS_GetKeyState('T').pressed)
            hide_debug = 1;
        if (OS_GetKeyState('Y').pressed)
            hide_debug = 0;
        if (OS_GetKeyState('Z').pressed) { 
            disable_vignette = 1;
            disable_post_processing = 1;
        }
        
        if (OS_GetKeyState('C').pressed) { 
            disable_post_processing = 0;
            disable_vignette = 0;
        }
        
        if (OS_GetKeyState('X').pressed) { 
            disable_vignette = 0;
        }
        
        
        if (OS_GetKeyState('C').pressed) { 
            disable_vignette = 1;
        }
        
        if (OS_GetKeyState('F').pressed)
        {
            enemies[enemy_memory_used]  = EnemyCreate(v2(player.pos.x, player.pos.y - 500));
            
            
            enemies[enemy_memory_used].speed = 1+(rand()%30);
            enemy_memory_used++;
        }
        
        velocity.y += 1.5;
        
        player.pos.y += velocity.y;
        player.pos.x += velocity.x;
        
        velocity.x *= 0.9;
        velocity.y *= 0.9;
        
        if (player.pos.y > ground.pos.y-player.size.y)
        {
            on_ground = 1;
            player.pos.y = ground.pos.y-player.size.y;
            velocity.y = 0.0;
        }
        
        
        
        if (OS_GetKeyState(' ').pressed && on_ground) {
            i32 displacement_y = (-speed) * 5;
            
            velocity.y = displacement_y ;
            on_ground = 0;
            
        }
        
        
        if (OS_GetKeyState('D').pressed) { 
            velocity.x = speed;
            facing_left = 1;
        }
        if (OS_GetKeyState('A').pressed)
        {
            velocity.x = -speed;
            facing_left = -1;
            
        }
        
        GFX_PushFramebuffer(framebuffer);
        {
            GFX_Clear(renderer);
            GFX_DrawRect(&skybox, renderer);
            
            for (i32 i=0;i<enemy_memory_used;i++)
            {
                EnemyUpdate(&enemies[i], v2(player.pos.x, player.pos.y), v2(ground.pos.x, ground.pos.y), facing_left);
                EnemyDraw(&enemies[i], renderer);
            }
            
            
            for (i32 i=0;i<bullet_memory_used; i++)
            {
                
                BulletShoot(&bullet[i]);
                BulletDraw(&bullet[i], renderer);
            }
            
            GFX_DrawRect(&player, renderer);
            GFX_DrawRect(&ground, renderer);
            
            
        } GFX_PopFramebuffer();
        
        
        
        
        if (disable_post_processing){
            
            GFX_PushFramebuffer(vignette);
            {
                GFX_PushShader(shader);
                GFX_Clear(renderer);
                GFX_DrawRect(&display, renderer);
                GFX_PopShader();
            } GFX_PopFramebuffer();
            
            if (disable_vignette)
            {
                GFX_PushShader(vignette_shader);
                GFX_Clear(renderer);
                GFX_DrawRect(&vignette_rect, renderer);
                GFX_PopShader();
            } else
            {
                GFX_DrawRect(&vignette_rect, renderer);
            }
            
        } else
        {
            GFX_DrawRect(&display, renderer);
        }
        
        GFX_DrawRect(&black_bar_top, renderer);
        GFX_DrawRect(&black_bar_bottom, renderer);
        
        if (!hide_debug){
            GFX_DrawRect(&cover, renderer);
            GFX_DrawText(&debug, renderer);
            if (U_ClockGetElapsedTime(&clock, app) >= 2) {
                if (splash_text.color.a >= 1 && time_elapsed == 1){ 
                    splash_text.color.a -= 2;
                }
            }
        }
        GFX_DrawText(&splash_text, renderer);
        
        GFX_Present(renderer);
    }
    
    
    TORN_Destroy();
    return 0;
}
