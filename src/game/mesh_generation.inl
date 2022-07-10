#include "mesh_generation.hpp"

template<typename Lf, typename Df>
void game::write_into_display_lists(const ext::data_array<chunk::quad>& building_quads, const standard_vertex_function& vf,
    gfx::display_list& standard_disp_list,
    gfx::display_list& foliage_disp_list,
    gfx::display_list& water_disp_list,
    Lf get_display_list_size, Df draw_vert
) {
    std::size_t standard_vert_count = vf.standard_quad_count * 4;
    std::size_t foliage_vert_count = vf.foliage_quad_count * 4;
    std::size_t water_vert_count = vf.water_quad_count * 4;

    standard_disp_list.resize(get_display_list_size(standard_vert_count));

    standard_disp_list.write_into([&building_quads, &vf, &draw_vert, standard_vert_count]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, standard_vert_count);

        std::size_t drawn_quads = 0;
        for (auto it = building_quads.begin(); drawn_quads < vf.standard_quad_count && it != vf.quad_it; ++it) {
            if (it->tp == chunk::quad::type::STANDARD) {
                auto& verts = it->verts;
                draw_vert(verts.vert0);
                draw_vert(verts.vert1);
                draw_vert(verts.vert2);
                draw_vert(verts.vert3);
                drawn_quads++;
            }
        }
        
        GX_End();
    });

    foliage_disp_list.resize(get_display_list_size(foliage_vert_count));

    foliage_disp_list.write_into([&building_quads, &vf, &draw_vert, foliage_vert_count]() {
        // TODO: Possibly optimize this by having the above iteration overwrite the previous vertices with foliage vertices so that we can iterate over less vertices.
        GX_Begin(GX_QUADS, GX_VTXFMT0, foliage_vert_count);

        std::size_t drawn_quads = 0;
        for (auto it = building_quads.begin(); drawn_quads < vf.foliage_quad_count && it != vf.quad_it; ++it) {
            if (it->tp == chunk::quad::type::FOLIAGE) {
                auto& verts = it->verts;
                draw_vert(verts.vert0);
                draw_vert(verts.vert1);
                draw_vert(verts.vert2);
                draw_vert(verts.vert3);
                drawn_quads++;
            }
        }
        
        GX_End();
    });

    water_disp_list.resize(get_display_list_size(water_vert_count));

    water_disp_list.write_into([&building_quads, &vf, &draw_vert, water_vert_count]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, water_vert_count);

        std::size_t drawn_quads = 0;
        for (auto it = building_quads.begin(); drawn_quads < vf.water_quad_count && it != vf.quad_it; ++it) {
            if (it->tp == chunk::quad::type::WATER) {
                auto& verts = it->verts;
                draw_vert(verts.vert0);
                draw_vert(verts.vert1);
                draw_vert(verts.vert2);
                draw_vert(verts.vert3);
                drawn_quads++;
            }
        }
        
        GX_End();
    });
}