#include "renderer.h"

#include "../platform/platform.h"
#include "../external/stb_truetype.h"
#include "../external/khronos/glcorearb.h"

#include <stdlib.h>

// misc
static PFNGLCLEARCOLORPROC glClearColor;
static PFNGLCLEARPROC glClear;
static PFNGLVIEWPORTPROC glViewport;
static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
static PFNGLENABLEPROC glEnable;
static PFNGLBLENDFUNCPROC glBlendFunc;
// vertex array
static PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
static PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
// buffers
static PFNGLGENBUFFERSPROC glGenBuffers;
static PFNGLBINDBUFFERPROC glBindBuffer;
static PFNGLBUFFERDATAPROC glBufferData;
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

static void get_gl_procs()
{
    HGLRC glrc = wglGetCurrentContext();
    glClear = (PFNGLCLEARPROC)platform_get_gl_proc("glClear");
    glClearColor = (PFNGLCLEARCOLORPROC)platform_get_gl_proc("glClearColor");
    glViewport = (PFNGLVIEWPORTPROC)platform_get_gl_proc("glViewport");

    // vertex array
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)platform_get_gl_proc("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)platform_get_gl_proc("glBindVertexArray");

    // buffers
    glGenBuffers = (PFNGLGENBUFFERSPROC)platform_get_gl_proc("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)platform_get_gl_proc("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)platform_get_gl_proc("glBufferData");
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

    // misc
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)platform_get_gl_proc("glGetShaderInfoLog");
    glEnable = (PFNGLENABLEPROC)platform_get_gl_proc("glEnable");
    glBlendFunc = (PFNGLBLENDFUNCPROC)platform_get_gl_proc("glBlendFunc");
}

#if 0
struct Shader* renderer_create_shader(const char *vertex_source, const char *fragment_source)
{
    int  success;
    char info_log[512];

    u32 vertex_shader   = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        printf("error vertex shader compilation failed\n%s\n", info_log);

        glDeleteShader(vertex_shader);
        return 0;
    }

    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        printf("error fragment shader compilation failed\n%s\n", info_log);
        
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }

    u32 shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("error linking shader program\n%s\n", info_log);
        return 0;
    }

    struct Shader *shader = malloc(sizeof(struct Shader));
    if (!shader)
    {
        printf("error: out of memory\n");
        return 0
    }

    shader->id = shader_id;
    return shader;
}

void renderer_use_shader(struct Shader *shader)
{
    glUseProgram(shader->id);
}

struct Texture* renderer_create_texture(f32 *vertices, u32 cnt_vertices, u32 *indices, u32 cnt_indices)
{
    struct Texture *texture = malloc(sizeof(struct Texture));
    if (!texture)
    {
        return 0;
    }

    glGenVertexArrays(1, &texture->vao);
    glBindVertexArray(texture->vao);

    glGenBuffers(1, &texture->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, texture->vbo);
    glBufferData(GL_ARRAY_BUFFER, 2*cnt_vertices*sizeof(f32), vertices, GL_DYNAMIC_DRAW);
    // 0 for location of vertex attribute in shader
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(f32), (void*)0);
    // 0 activates attribute in location 0
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &texture->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texture->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cnt_indices*sizeof(u32), indices, GL_STATIC_DRAW);

    glGenTextures();
    glBindTexture();

    // required settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // glTexImage2D sends texture data to gpu
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    // can free buffer now

    // when using
    glActiveTexture(GL_TEXTURE0);

    // todo: shader set unifrom GL_TEXTURE_0

    // blending need too
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return texture;
}

void renderer_draw_texture(struct Texture *texture)
{
    glBindVertexArray(texture->vao);
    glDrawElements(GL_TRIANGLES, texture->cnt_vertices, GL_UNSIGNED_INT, 0);
}

struct Font* renderer_create_font(void *font_buff, u32 size)
{
    u8 tmp_bitmap[512*512];
    stbtt_BakeFontBitmap(font_buff, 0, 32.0, tmp_bitmap, 512, 512, 32, 96, g_glyphs); // no guarantee this fits!

    glGenTextures(1, &g_font_id);
    glBindTexture(GL_TEXTURE_2D, g_font_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512,512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, tmp_bitmap);
    // can free tmp_bitmap at this point
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void renderer_draw_text(f32 x, f32 y, const char *text)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ftex);
    glBegin(GL_QUADS);
    while (*text) {
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(cdata, 512,512, *text-32, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9
            glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y0);
            glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y0);
            glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y1);
            glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y1);
        }
        ++text;
    }
    glEnd();
}

struct Texture* renderer_create_texture()
{
    return 0;
    struct Texture *texture = malloc(sizeof(struct Texture));
    if (!texture)
    {
        return false;
    }

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512,512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
    // can free temp_bitmap at this point
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // shader
    // ...
    glUseProgram();

    // vertex array
    glGenVertexArrays(1, &texture->vertex_array.id);
    glBindVertexArray(texture->vertex_array.id);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);

    // vertex buffer
    glGenBuffers(1, &texture->vertex_buffer.id);
    glBindBuffer(GL_ARRAY_BUFFER, texture->vertex_buffer.id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    // index buffer
    glGenBuffers(1, &texture->index_buffer.id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texture->index_buffer.id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    return texture;
}

void renderer_bind_texture(struct Texture *texture)
{
    glBindVertexArray(texture->vertex_array.id);
}

void renderer_unbind_texture(struct Texture *texture)
{
    glBindVertexArray(0);
}
#endif

void renderer_draw_color(f32 r, f32 g, f32 b)
{
    glClearColor(r, g, b, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_viewport(s32 x, s32 y, s32 width, s32 height)
{
    glViewport(x, y, width, height);
}

bool renderer_init()
{
    get_gl_procs();
    return true;
}

#define STB_TRUETYPE_IMPLEMENTATION
#include "../external/stb_truetype.h"
