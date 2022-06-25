#pragma once
#include "block.hpp"
#include "block_core.hpp"
#include "face_mesh_generation_core.hpp"
#include "math/box.hpp"
#include "traits.hpp"
#include "util.hpp"
#include <array>

namespace game {
    #define BF_FUNC static constexpr

    template<block::type type>
    struct block_functionality {
        BF_FUNC block_traits get_block_traits(bl_st) { return {}; }

        template<typename Vf>
        BF_FUNC void add_general_vertices(Vf&, math::vector3u8, bl_st) {}

        template<block::face face>
        BF_FUNC face_traits get_face_traits(bl_st) { return {}; }

        template<block::face face>
        BF_FUNC bool is_face_visible_with_neighbor(bl_st, const block&) { return false; }

        template<block::face face, typename Vf>
        BF_FUNC void add_face_vertices(Vf&, math::vector3u8, bl_st) {}

        BF_FUNC std::array<math::box, 0> get_boxes(bl_st) { return {}; }
    };

    template<typename Bf>
    struct cube_block_functionality {
        using self = cube_block_functionality<Bf>;

        BF_FUNC block_traits get_block_traits(bl_st) { return {
            .visible = true,
            .general_vertex_count = 0
        }; }

        template<typename Vf>
        BF_FUNC void add_general_vertices(Vf&, math::vector3u8, bl_st) {}

        template<block::face face>
        BF_FUNC face_traits get_face_traits(bl_st) { return {
            .partially_transparent = false,
            .vertex_count = 4
        }; }

        template<block::face face>
        BF_FUNC bool is_face_visible_with_neighbor(bl_st, const block& block) { return get_neighbor_face_traits<face>(block).partially_transparent; }

        template<block::face face, typename Vf>
        BF_FUNC void add_face_vertices(Vf& vf, math::vector3u8 block_pos, bl_st st) {
            add_flat_face_vertices_from_block_position<face, self>(vf, block_pos, st);
        }

        BF_FUNC std::array<math::box, 1> get_boxes(bl_st) {
            return {
                math::box{
                    .lesser_corner = { 0.0f, 0.0f, 0.0f },
                    .greater_corner = { 1.0f, 1.0f, 1.0f }
                }
            };
        }

        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st st) {
            return Bf::template get_uv_position<face>(st);
        }

        BF_FUNC draw_positions get_draw_positions(const draw_positions& draw_positions, bl_st) { return draw_positions; }

        BF_FUNC draw_positions get_offset_draw_positions(const draw_positions& draw_positions, bl_st) {
            return {
                .block_draw_pos = draw_positions.block_draw_pos + math::vector3u8{ block_draw_size, block_draw_size, block_draw_size },
                .uv_draw_pos = draw_positions.uv_draw_pos + math::vector2u8{ block_draw_size, block_draw_size }
            };
        }
    };

    template<typename Bf>
    struct slab_block_functionality {
        using self = slab_block_functionality<Bf>;
        using state = block::slab_state;

        BF_FUNC block_traits get_block_traits(bl_st st) { return {
            .visible = true,
            .general_vertex_count = (st.slab != block::slab_state::BOTH ? 4u : 0u)
        }; }

        template<typename Vf>
        BF_FUNC void add_general_vertices(Vf& vf, math::vector3u8 block_pos, bl_st st) {
            switch (st.slab) {
                default: break;
                case state::BOTTOM: add_flat_face_vertices_from_block_position<block::face::TOP, self>(vf, block_pos, st);
                case state::TOP: add_flat_face_vertices_from_block_position<block::face::BOTTOM, self>(vf, block_pos, st);
            }
        }

        template<block::face face>
        BF_FUNC face_traits get_face_traits(bl_st st) { return {
            .partially_transparent = false,
            .vertex_count = [&st]() -> std::size_t {
                if constexpr (face == block::face::TOP || face == block::face::BOTTOM) {
                    return st.slab != state::BOTH ? 0 : 4;
                }
                return 4;
            }()
        }; }

        template<block::face face>
        BF_FUNC bool is_face_visible_with_neighbor(bl_st, const block& block) { return get_neighbor_face_traits<face>(block).partially_transparent; }

        template<block::face face, typename Vf>
        BF_FUNC void add_face_vertices(Vf& vf, math::vector3u8 block_pos, bl_st st) {
            add_flat_face_vertices_from_block_position<face, self>(vf, block_pos, st);
        }

        BF_FUNC std::array<math::box, 1> get_boxes(bl_st st) {
            return {
                math::box{
                    .lesser_corner = { 0.0f, (st.slab == state::TOP ? 0.5f : 0.0f), 0.0f },
                    .greater_corner = { 1.0f, (st.slab == state::BOTTOM ? 0.5f : 0.0f), 1.0f }
                }
            };
        }

        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st st) {
            return Bf::template get_uv_position<face>(st);
        }

        BF_FUNC draw_positions get_draw_positions(const draw_positions& d_positions, bl_st st) {
            return (st.slab == state::TOP ? draw_positions{
                .block_draw_pos = d_positions.block_draw_pos + math::vector3u8{ 0, half_block_draw_size, 0 },
                .uv_draw_pos = d_positions.uv_draw_pos
            } : d_positions);
        }

        BF_FUNC draw_positions get_offset_draw_positions(const draw_positions& d_positions, bl_st st) {
            return {
                .block_draw_pos = d_positions.block_draw_pos + math::vector3u8{ block_draw_size, (st.slab == state::BOTTOM ? half_block_draw_size : block_draw_size), block_draw_size },
                .uv_draw_pos = d_positions.uv_draw_pos + math::vector2u8{ block_draw_size, (st.slab != state::BOTH ? half_block_draw_size : block_draw_size) }
            };
        }
    };

    template<>
    struct block_functionality<block::type::DEBUG> : public cube_block_functionality<block_functionality<block::type::DEBUG>> {
        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st) {
            using v2u8 = math::vector2u8;
            return call_face_func_for<face, v2u8>(
                []() { return v2u8{0, 0}; },
                []() { return v2u8{1, 0}; },
                []() { return v2u8{2, 0}; },
                []() { return v2u8{3, 0}; },
                []() { return v2u8{4, 0}; },
                []() { return v2u8{5, 0}; }
            );
        }
    };

    template<>
    struct block_functionality<block::type::GRASS> : public cube_block_functionality<block_functionality<block::type::GRASS>> {
        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st) {
            using v2u8 = math::vector2u8;
            return call_face_func_for<face, v2u8>(
                []() { return v2u8{3, 0}; },
                []() { return v2u8{3, 0}; },
                []() { return v2u8{0, 0}; },
                []() { return v2u8{2, 0}; },
                []() { return v2u8{3, 0}; },
                []() { return v2u8{3, 0}; }
            );
        }
    };

    template<>
    struct block_functionality<block::type::DIRT> : public cube_block_functionality<block_functionality<block::type::DIRT>> {
        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st) {
            return { 2, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::STONE> : public cube_block_functionality<block_functionality<block::type::STONE>> {
        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st) {
            return { 1, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::STONE_SLAB> : public slab_block_functionality<block_functionality<block::type::STONE_SLAB>> {
        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st) {
            return { 1, 0 };
        }
    };
}