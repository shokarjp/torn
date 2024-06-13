#include <torn.h>
#include <windows.h>
#define HANDMADE_MATH_IMPLEMENTATION
#include <third-party/handmade/handmademath.h>
int main(int argc, char** argv)
{
    
    if (TORN_Init())
    {
        MessageBoxA(0, "TORN_Init(): Failed", "TORN_Init(): Failure", MB_ICONERROR|MB_OK);
        return 0;
    }
    
    
    OS_App* window = OS_ConstructApp("app", 
                                     OS_AppDefault,
                                     OS_AppDefault,
                                     OS_AppStyleDefault);
    
    if (window == 0)
    {
        MessageBoxA(0, "Window failed to create", "torn: app", MB_OK);
    }
    
    GFX_Renderer* renderer = GFX_CreateRenderer(window);
    glViewport(0,0,window->size.w, window->size.h);
    glClearColor(0.001f, 0.001f, 0.001f, 1.f);
    
    const char* vs = 
        "#version 330\n"
        "layout (location = 0) in vec3 v_pos;\n"
        "layout (location = 1) in vec4 c_color;\n"
        "layout (location = 2) in vec3 n_normals;\n"
        "out vec4 v_color;\n"
        "out vec3 normals;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "out vec3 current_pos;\n"
        "void main() {\n"
        "current_pos = vec3(model * vec4(v_pos, 1.0f));\n"
        "v_color = c_color;\n"
        "normals = n_normals;\n"
        "gl_Position = projection * model * view * vec4(v_pos, 1.0f);\n"
        "}\n\0";
    
    const char* fs =
        "#version 330\n"
        "out vec4 Color;\n"
        "uniform vec3 light_pos;\n"
        "in vec4 v_color;\n"
        "in vec3 current_pos;\n"
        "in vec3 normals;\n"
        "void main() {\n"
        "vec3 normal = normalize(normals);\n"
        "vec3 light_dir =  normalize(light_pos - current_pos);\n"
        "float diffuse = max(dot(normal, light_dir), 0);"
        "float as = 0.5f;\n"
        "vec3 ambient = as * vec3(1,1,1);\n"
        
        
        "vec3 final = vec3(ambient * v_color.xyz);\n"
        "Color =  vec4(final, 1.0) * diffuse*3;\n"
        "}\n\0";
    
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1,  &vs, 0);
    glCompileShader(vertex_shader);
    
    b32 success =0;
    i8 info_log[512];
    
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader,  512, 0, info_log);
        MessageBox(0, info_log, "Vertex Shader: Compilation Error", MB_OK | MB_ICONERROR);
    }
    
    
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1,  &fs, 0);
    glCompileShader(fragment_shader);
    
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader,  512, 0, info_log);
        MessageBox(0, info_log, "Fragment Shader: Compilation Error", MB_OK | MB_ICONERROR);
    }
    
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    
    
    if (!success)
    {
        glGetProgramInfoLog(program, 512, 0, info_log);
        MessageBox(0, info_log, "Program: Linking Error", MB_OK | MB_ICONERROR);
    }
    
    
    
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    
    
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    
    GLfloat vertices[] =
    { //     COORDINATES     /        COLORS          /    TexCoord   /        NORMALS       //
        -1.0f, 0.0f,  1.0f,     0.83f, 0.70f, 0.44f,  1,      0.0f, -1.0f, 0.0f, // Bottom side
        -1.0f, 0.0f, -1.0f,     0.83f, 0.70f, 0.44f,  1,      0.0f, -1.0f, 0.0f, // Bottom side
        1.0f, 0.0f, -1.0f,     0.83f, 0.70f, 0.44f,   1,      0.0f, -1.0f, 0.0f, // Bottom side
        1.0f, 0.0f,  1.0f,     0.83f, 0.70f, 0.44f,   1,      0.0f, -1.0f, 0.0f, // Bottom side
        
        -1.0f, 0.0f,  1.0f,     0.83f, 0.70f, 0.44f,  1,     -1.f, 1.0f,  0.0f, // Left Side
        -1.0f, 0.0f, -1.0f,     0.83f, 0.70f, 0.44f,  1,     -1.f, 1.0f,  0.0f, // Left Side
        0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,   1,     -1.f, 1.0f,  0.0f, // Left Side
        
        -1.0f, 0.0f, -1.0f,     0.83f, 0.70f, 0.44f,  1,      0.1f, 1.0f, -1.f, // Non-facing side
        1.0f, 0.0f, -1.0f,     0.83f, 0.70f, 0.44f,   1,      0.1f, 1.0f, -1.f, // Non-facing side
        0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,   1,      0.1f, 1.0f, -1.f, // Non-facing side
        
        1.0f, 0.0f, -1.0f,     0.83f, 0.70f, 0.44f,   1,      1.f, 1.0f,  0.0f, // Right side
        1.0f, 0.0f,  1.0f,     0.83f, 0.70f, 0.44f,   1,      1.f, 1.0f,  0.0f, // Right side
        0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,   1,      1.f, 1.0f,  0.0f, // Right side
        
        1.0f, 0.0f,  1.0f,     0.83f, 0.70f, 0.44f,   1,      0.0f, 1.0f,  1.f, // Facing side
        -1.0f, 0.0f,  1.0f,     0.83f, 0.70f, 0.44f, 1,      0.0f,  1.0f,  1.f, // Facing side
        0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,1,      0.0f,    1.0f,  1.f  // Facing side
    };
    
    // Indices for vertices order
    GLuint indices[] =
    {
        0, 1, 2, // Bottom side
        0, 2, 3, // Bottom side
        4, 6, 5, // Left side
        7, 9, 8, // Non-facing side
        10, 12, 11, // Right side
        13, 15, 14 // Facing side
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, 0);
    glEnableVertexAttribArray(0);
    
    
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (void*)(3*sizeof(r32)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (void*)(7*sizeof(r32)));
    glEnableVertexAttribArray(2);
    
    
    
    hmm_mat4 proj = HMM_Mat4d(1);
    proj = HMM_Perspective(HMM_ToRadians(45.0f), (float)(window->size.w /window->size.h), 100.f, window->size.w);
    
    glUseProgram(program);
    hmm_mat4 model = HMM_Mat4d(1);
    
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model.Elements[0][0]);
    
    
    
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE,&proj.Elements[0][0]);
    r32 time = 0;
    
    glEnable(GL_DEPTH_TEST);
    while (window->running)
    {
        OS_PollEvents(window);
        
        
        if (time >= 300) time = 0;
        V2F position = OS_GetMousePosition(window);
        glUniform3f(glGetUniformLocation(program, "light_pos"),  
                    cos( OS_GetTime(window)) * 500,
                    sin( OS_GetTime(window)) * 500,
                    sin( OS_GetTime(window)) * 40);
        
        hmm_mat4 view = HMM_Mat4d(1); 
        view = HMM_LookAt(HMM_Vec3((cos( OS_GetTime(window)) * 800),
                                   (sin( OS_GetTime(window)) * 800),40),
                          HMM_Vec3(0,0,0),
                          HMM_Vec3(0,2,0));
        
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view.Elements[0][0]);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);
        glDrawElements(GL_TRIANGLES, sizeof(indices)/ sizeof(int), GL_UNSIGNED_INT, 0);
        
        GFX_Present(renderer);
        
    }
    
    
    TORN_Destroy();
    return 0;
}
