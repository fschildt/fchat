struct Render_Font {
    u32 vertex_array;
    u32 vertex_buffer;
    u32 index_buffer;
    u32 texture;
    u32 shader;
    stbtt_bakedchar baked_chars[96];
};

struct Renderer {
    struct Render_Font font;
};

