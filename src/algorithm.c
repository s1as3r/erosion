// clang-format off
#include "fbm_raylib.h"
#include "erosion_raylib.h"

#include <stdlib.h>
// clang-format on

#define INITIAL_DIM_X 256
#define INITIAL_DIM_Y 256

typedef enum { FBM = 0, Erosion } AlgorithmType;

#define ALGO_DISPATCH(type, fbm_expr, erosion_expr)                            \
  do {                                                                         \
    switch ((type)) {                                                          \
    case FBM: {                                                                \
      fbm_expr;                                                                \
    } break;                                                                   \
    case Erosion: {                                                            \
      erosion_expr;                                                            \
    } break;                                                                   \
    }                                                                          \
  } while (0)

typedef struct {
  u32 dim_x;
  u32 dim_y;
  Image image;
  Texture texture;
  Mesh mesh;
  Model model;

  union {
    FBMState fbm_state;
    ErosionState erosion_state;
  };
  AlgorithmType type;
} AlgorithmState;

void algo_init(AlgorithmState *state) {
  elog("[%s] initializing", state->type == Erosion ? "EROSION" : "FBM");
  state->dim_x = INITIAL_DIM_X;
  state->dim_y = INITIAL_DIM_Y;
  state->image = (Image){.height = INITIAL_DIM_Y,
                         .width = INITIAL_DIM_X,
                         .mipmaps = 1,
                         .format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE};

  state->image.data = malloc(INITIAL_DIM_X * INITIAL_DIM_Y * sizeof(u8));

  ALGO_DISPATCH(state->type,
                fbm_init(&state->fbm_state, state->dim_x, state->dim_y,
                         (u8 *)state->image.data),
                erosion_init(&state->erosion_state, state->dim_x, state->dim_y,
                             (u8 *)state->image.data));

  state->texture = LoadTextureFromImage(state->image);
  state->mesh = GenMeshHeightmap(state->image, (Vector3){16, 8, 16});
  state->model = LoadModelFromMesh(state->mesh);
  state->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = state->texture;
}

void _reload_raylib_stuff(AlgorithmState *state) {
  // elog("[%s] reloading", state->type == Erosion ? "EROSION" : "FBM");
  UnloadTexture(state->texture);
  UnloadModel(state->model);

  ALGO_DISPATCH(state->type,
                fbm_gen_data(&state->fbm_state, (u32)state->image.width,
                             (u32)state->image.height, state->image.data),
                erosion_gen_data(&state->erosion_state, state->dim_x,
                                 state->dim_y, (u8 *)state->image.data));

  state->texture = LoadTextureFromImage(state->image);
  state->mesh = GenMeshHeightmap(state->image, (Vector3){16, 8, 16});
  state->model = LoadModelFromMesh(state->mesh);
  state->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = state->texture;
}

void algo_update_state(AlgorithmState *state) {
  bool needs_reload = false;
  ALGO_DISPATCH(state->type,
                (needs_reload = fbm_update_state(&state->fbm_state)),
                (needs_reload = erosion_update_state(&state->erosion_state)));

  if (needs_reload) {
    _reload_raylib_stuff(state);
  }
}

void algo_draw_ui(AlgorithmState *state, f32 slider_x_offset) {
  ALGO_DISPATCH(state->type, fbm_draw_ui(&state->fbm_state, slider_x_offset),
                erosion_draw_ui(&state->erosion_state, slider_x_offset));
}

void algo_cleanup(AlgorithmState *state) {
  elog("[%s] cleanup", state->type == Erosion ? "EROSION" : "FBM");
  UnloadTexture(state->texture);
  UnloadModel(state->model);
  free(state->image.data);

  ALGO_DISPATCH(state->type, fbm_cleanup(&state->fbm_state),
                erosion_cleanup(&state->erosion_state));
}

#undef ALGO_DISPATCH
