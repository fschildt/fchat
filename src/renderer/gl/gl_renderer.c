#include "../../platform/platform.h"
#include "../../external/stb_truetype.h"
#include "../../external/khronos/glcorearb.h"

#include "gl_renderer.h"

#include <stdlib.h>

static struct Renderer g_renderer;
static f32 projection[4][4];

// misc
static PFNGLCLEARCOLORPROC glClearColor;
static PFNGLCLEARPROC glClear;
static PFNGLVIEWPORTPROC glViewport;
static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
static PFNGLENABLEPROC glEnable;
static PFNGLBLENDFUNCPROC glBlendFunc;
static PFNGLDRAWELEMENTSPROC glDrawElements;
static PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
static PFNGLGETERRORPROC glGetError;
static PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
static PFNGLACTIVETEXTUREPROC glActiveTexture;
static PFNGLUNIFORM1IPROC glUniform1i;
static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
static PFNGLDRAWARRAYSPROC glDrawArrays;
static PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
// vertex array
static PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
static PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
// buffers
static PFNGLGENBUFFERSPROC glGenBuffers;
static PFNGLBINDBUFFERPROC glBindBuffer;
static PFNGLBUFFERDATAPROC glBufferData;
static PFNGLBUFFERSUBDATAPROC glBufferSubData;
// textures
static PFNGLGENTEXTURESPROC glGenTextures;
static PFNGLBINDTEXTUREPROC glBindTexture;
static PFNGLTEXIMAGE2DPROC glTexImage2D;
static PFNGLTEXPARAMETERIPROC glTexParameteri;
// shader
static PFNGLCREATESHADERPROC glCreateShader;
static PFNGLSHADERSOURCEPROC glShaderSource;
static PFNGLCOMPILESHADERPROC glCompileShader;
static PFNGLGETSHADERIVPROC glGetShaderiv;
static PFNGLATTACHSHADERPROC glAttachShader;
static PFNGLDELETESHADERPROC glDeleteShader;
static PFNGLCREATEPROGRAMPROC glCreateProgram;
static PFNGLUSEPROGRAMPROC glUseProgram;
static PFNGLLINKPROGRAMPROC glLinkProgram;
static PFNGLVALIDATEPROGRAMPROC glValidateProgram;
static PFNGLGETPROGRAMIVPROC glGetProgramiv;

void gl_debug_message_callback(GLenum src, GLenum type, GLuint id,
                               GLenum severity, GLsizei length,
                               const GLchar *message, const void *user_param)
{
    printf("====================\n"
           "gl error detected\n"
           "--------------------\n"
           "src      = %d\n"
           "type     = %d\n"
           "id       = %d\n"
           "severity = %d\n"
           "length   = %d\n"
           "message  = %s\n"
           "====================\n",
           src, type, id, severity, length, message);
}

static void get_gl_procs()
{

    // only 4.3+ thing
    glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)platform_get_gl_proc("glDebugMessageCallback");

    // misc
    glClear = (PFNGLCLEARPROC)platform_get_gl_proc("glClear");
    glClearColor = (PFNGLCLEARCOLORPROC)platform_get_gl_proc("glClearColor");
    glViewport = (PFNGLVIEWPORTPROC)platform_get_gl_proc("glViewport");
    glDrawElements = (PFNGLDRAWELEMENTSPROC)platform_get_gl_proc("glDrawElements");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)platform_get_gl_proc("glGenerateMipmap");
    glGetError = (PFNGLGETERRORPROC)platform_get_gl_proc("glGetError");
    glActiveTexture = (PFNGLACTIVETEXTUREPROC)platform_get_gl_proc("glActiveTexture");
    glUniform1i = (PFNGLUNIFORM1IPROC)platform_get_gl_proc("glUniform1i");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)platform_get_gl_proc("glGetUniformLocation");
    glDrawArrays = (PFNGLDRAWARRAYSPROC)platform_get_gl_proc("glDrawArrays");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)platform_get_gl_proc("glUniformMatrix4fv");

    // vertex array
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)platform_get_gl_proc("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)platform_get_gl_proc("glBindVertexArray");

    // buffers
    glGenBuffers = (PFNGLGENBUFFERSPROC)platform_get_gl_proc("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)platform_get_gl_proc("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)platform_get_gl_proc("glBufferData");
    glBufferSubData = (PFNGLBUFFERSUBDATAPROC)platform_get_gl_proc("glBufferSubData");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)platform_get_gl_proc("glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)platform_get_gl_proc("glVertexAttribPointer");

    // textures
    glGenTextures = (PFNGLGENTEXTURESPROC)platform_get_gl_proc("glGenTextures");
    glBindTexture = (PFNGLBINDTEXTUREPROC)platform_get_gl_proc("glBindTexture");
    glTexImage2D = (PFNGLTEXIMAGE2DPROC)platform_get_gl_proc("glTexImage2D");
    glTexParameteri = (PFNGLTEXPARAMETERIPROC)platform_get_gl_proc("glTexParameteri");

    // shader
    glCreateShader = (PFNGLCREATESHADERPROC)platform_get_gl_proc("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)platform_get_gl_proc("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)platform_get_gl_proc("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)platform_get_gl_proc("glGetShaderiv");
    glAttachShader = (PFNGLATTACHSHADERPROC)platform_get_gl_proc("glAttachShader");
    glDeleteShader = (PFNGLDELETESHADERPROC)platform_get_gl_proc("glDeleteShader");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)platform_get_gl_proc("glCreateProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC)platform_get_gl_proc("glUseProgram");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)platform_get_gl_proc("glLinkProgram");
    glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)platform_get_gl_proc("glValidateProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)platform_get_gl_proc("glGetProgramiv");

    // misc
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)platform_get_gl_proc("glGetShaderInfoLog");
    glEnable = (PFNGLENABLEPROC)platform_get_gl_proc("glEnable");
    glBlendFunc = (PFNGLBLENDFUNCPROC)platform_get_gl_proc("glBlendFunc");
}

static bool compile_shader(u32 *id, const char *src, GLenum type)
{
    u32 shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    int  success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        printf("error %s shader compilation failed\n%s\n", type == GL_VERTEX_SHADER ? "vertex" : "fragment", info_log);

        glDeleteShader(shader);
        return false;
    }

    *id = shader;
    return true;
}

static bool create_shader(u32 *shader, const char *vs_src, const char *fs_src)
{
    u32 vs_id;
    u32 fs_id;
    if (!compile_shader(&vs_id, vs_src, GL_VERTEX_SHADER) ||
        !compile_shader(&fs_id, fs_src, GL_FRAGMENT_SHADER))
    {
        return false;
    }

    u32 program_id = glCreateProgram();
    glAttachShader(program_id, vs_id);
    glAttachShader(program_id, fs_id);
    glLinkProgram(program_id);

    glDeleteShader(vs_id);
    glDeleteShader(fs_id);

    int success;
    char info_log[512];
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if(!success)
    {
        printf("error linking shader program\n%s\n", info_log);
        return false;
    }

    *shader = program_id;
    return true;
}

// set the OpenGL orthographic projection matrix in vertex shader
void ortho(f32 mat44[4][4], f32 x0, f32 x1, f32 y0, f32 y1, f32 z0, f32 z1)
{
    mat44[0][0] = 2 / (x1 - x0); 
    mat44[0][1] = 0; 
    mat44[0][2] = 0; 
    mat44[0][3] = 0; 
 
    mat44[1][0] = 0; 
    mat44[1][1] = 2 / (y1 - y0); 
    mat44[1][2] = 0; 
    mat44[1][3] = 0; 
 
    mat44[2][0] = 0; 
    mat44[2][1] = 0; 
    mat44[2][2] = 2 / (z1 - z0); 
    mat44[2][3] = 0; 
 
    mat44[3][0] = -(x1 + x0) / (x1 - x0); 
    mat44[3][1] = -(y1 + y0) / (y1 - y0); 
    mat44[3][2] = -(z1 + z0) / (z1 - z0); 
    mat44[3][3] = 1; 
}
 

void renderer_draw_text(const char *text, f32 x, f32 y)
{
    struct Render_Font *font = &g_renderer.font;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(font->shader);

    GLint loc = glGetUniformLocation(font->shader, "u_projection");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &projection[0][0]);

    glBindVertexArray(font->vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, font->vertex_buffer);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font->texture);
    glUniform1i(glGetUniformLocation(font->shader, "u_texture"), 0);

    while (*text)
    {
        if (*text >= 32 && *text <= 127)
        {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font->baked_chars, 512, 512, *text-32, &x, &y, &q, 1);
            float vertices[] = {
                q.x0, q.y0, q.s0, q.t0,
                q.x1, q.y0, q.s1, q.t0,
                q.x1, q.y1, q.s1, q.t1,
                q.x0, q.y1, q.s0, q.t1,
                q.x0, q.y0, q.s0, q.t0,
                q.x1, q.y1, q.s1, q.t1,
            };
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glDrawArrays(GL_TRIANGLES, 0, 6); 
        }
        text++;
    }
}

// NOTE: ttf_buff must keep laying around
bool renderer_setup_text_drawing(u8 *ttf_buff, const char *vs_src, const char *fs_src)
{
    struct Render_Font *font = &g_renderer.font;

    if (!create_shader(&font->shader, vs_src, fs_src))
        return false;

    // vao
    glGenVertexArrays(1, &font->vertex_array);
    glBindVertexArray(font->vertex_array);

    // vbo
    u32 cnt_vertices = 6;
    u32 bytes_per_vertex = 4 * sizeof(f32);
    glGenBuffers(1, &font->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, font->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, bytes_per_vertex*cnt_vertices, 0, GL_DYNAMIC_DRAW);

    // 2 position, 2 texture
    u32 stride = 4 *sizeof(f32);
    void *offset_texture = (void*)(2*sizeof(f32));
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, offset_texture);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // font texture
    glActiveTexture(GL_TEXTURE0);
    u8 tmp_bitmap[512*512];
    stbtt_BakeFontBitmap(ttf_buff, 0, 32.0, tmp_bitmap, 512, 512, 32, 96, font->baked_chars);
    glGenTextures(1, &font->texture);
    glBindTexture(GL_TEXTURE_2D, font->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, tmp_bitmap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return true;
}

void renderer_draw_color(f32 r, f32 g, f32 b)
{
    glClearColor(r, g, b, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_viewport(s32 x, s32 y, s32 width, s32 height)
{
    ortho(projection, 0, width, height, 0, 0, 1);
    glViewport(x, y, width, height);
}

stbtt_bakedchar* renderer_get_stbtt_baked_chars()
{
    return g_renderer.font.baked_chars;
}

bool renderer_init()
{
    get_gl_procs();

    glDebugMessageCallback(gl_debug_message_callback, 0);

    return true;
}

#define STB_TRUETYPE_IMPLEMENTATION
#include "../../external/stb_truetype.h"
