#include "chunk_rendering.hpp"
#include "gfx.hpp"

using namespace game;

void game::init_chunk_drawing() {
	// set number of rasterized color channels
	GX_SetNumChans(1);

	//set number of textures to generate
	GX_SetNumTexGens(1);

	// setup texture coordinate generation
	// args: texcoord slot 0-7, matrix type, source to generate texture coordinates from, matrix to use
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	GX_SetTevOp(GX_TEVSTAGE0,GX_REPLACE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

	//

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	// GX_VTXFMT0 is for standard geometry
	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_U8, 2);
	// Since the fractional size of the fixed point number is 4, it is equivalent to 1 unit = 16 pixels
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 6);
}

void game::draw_chunks_standard(const math::matrix view, const camera& cam, chunk::map& chunks) {
	init_chunk_drawing();

	GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);

	GX_SetAlphaUpdate(GX_FALSE);
	GX_SetZCompLoc(GX_TRUE);
	GX_SetCullMode(GX_CULL_BACK);

	if (cam.update_view) {
		for (auto& [ pos, chunk ] : chunks) {
			chunk.tf.update_model_view(view);

			chunk.tf.load(GX_PNMTX3);
			chunk.standard_disp_list.call();
		}
	} else {
		for (auto& [ pos, chunk ] : chunks) {
			chunk.tf.load(GX_PNMTX3);
			chunk.standard_disp_list.call();
		}
	}
}

void game::draw_chunks_foliage(const chunk::map& chunks) {
	init_chunk_drawing();

	GX_SetAlphaCompare(GX_GEQUAL, 0x20, GX_AOP_AND, GX_ALWAYS, 0); // I don't know why this works but it does, see https://github.com/devwizard64/metapro/blob/bda8d24556ba160a339ade631469dffe2a1cf752/src/gdp/set_rm.gekko.c

	GX_SetAlphaUpdate(GX_TRUE);
	GX_SetZCompLoc(GX_FALSE);
	GX_SetCullMode(GX_CULL_NONE);
	for (const auto& [ pos, chunk ] : chunks) {
		chunk.tf.load(GX_PNMTX3);
		chunk.foliage_disp_list.call();
	}
}

void game::draw_chunks_water(const chunk::map& chunks) {
	init_chunk_drawing();

	GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);

	GX_SetAlphaUpdate(GX_TRUE);
	GX_SetZCompLoc(GX_TRUE);
	GX_SetCullMode(GX_CULL_BACK);
	for (const auto& [ pos, chunk ] : chunks) {
		chunk.tf.load(GX_PNMTX3);
		chunk.water_disp_list.call();
	}
}