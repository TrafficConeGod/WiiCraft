#pragma once
#include "chunk.hpp"
#include "camera.hpp"
#include "gfx.hpp"

namespace game {
    void init_chunk_drawing();

    void draw_chunks_standard(const math::matrix view, const camera& cam, chunk::map& chunks);
    void draw_chunks_foliage(chunk::map& chunks);
    void draw_chunks_water(chunk::map& chunks);
}