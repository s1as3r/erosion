// clang-format off
#include "rand.c"
#include "fbm.c"
#include "fbm_raylib.c"
#include "erosion.c"
#include "erosion_raylib.c"
#include "algorithm.c"

#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>
// clang-format on

void custom_trace_log(i32 msg_type, const char *text, va_list args) {
  if (msg_type == LOG_EROSION) {
    vprintf(text, args);
    printf("\n");
  }
}

i32 main(void) {
  const i32 screen_width = 1200;
  const i32 screen_height = 800;
  SetTraceLogCallback(custom_trace_log);
  InitWindow(screen_width, screen_height, "erosion");

  Vector3 initial_camera_position = {18.0f, 21.0f, 18.0f};
  Vector3 initial_camera_target = {0.0f, 0.0f, 0.0f};
  Vector3 map_position = {-8.0f, 0.0f, -8.0f};

  Camera camera = {0};
  camera.position = initial_camera_position;
  camera.target = initial_camera_target;
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  AlgorithmType algo_selection = Erosion;
  AlgorithmType prev_algo_selection = Erosion;
  AlgorithmState algo_state = {.type = algo_selection};
  algo_init(&algo_state);

  SetTargetFPS(60);

  i32 camera_model_selection = 1;
  i32 camera_mode;

  const i32 font_size = GuiGetStyle(DEFAULT, TEXT_SIZE);
  const f32 slider_x_offset =
      15.0f + (f32)MeasureText("drop lifetime: ", font_size);

  const i32 texture_draw_width = 256;
  const f32 texture_draw_scale =
      (f32)texture_draw_width / (f32)algo_state.texture.width;

  bool algo_dropdown_clicked = false;
  bool algo_dropdown_active = false;

  bool reset_camera = false;
  bool camera_dropdown_active = false;
  bool camera_dropdown_clicked = false;
  while (!WindowShouldClose()) {
    if (prev_algo_selection != algo_selection) {
      algo_cleanup(&algo_state);
      algo_state.type = algo_selection;
      algo_init(&algo_state);

      prev_algo_selection = algo_selection;
    }

    if (camera_dropdown_clicked) {
      camera_dropdown_active = !camera_dropdown_active;
    }

    if (algo_dropdown_clicked) {
      algo_dropdown_active = !algo_dropdown_active;
    }

    if (reset_camera) {
      camera.position = initial_camera_position;
      camera.target = initial_camera_target;
    }

    camera_mode = camera_model_selection + 1;
    if (camera_mode == 3) {
      camera_mode = CAMERA_CUSTOM;
    }
    UpdateCamera(&camera, camera_mode);

    algo_update_state(&algo_state);

    BeginDrawing();
    {
      ClearBackground(RAYWHITE);

      BeginMode3D(camera);
      {
        DrawModel(algo_state.model, map_position, 1.0f, RED);
        DrawGrid(20, 1.0f);
      }
      EndMode3D();

      DrawTextureEx(algo_state.texture,
                    (Vector2){screen_width - texture_draw_width - 20, 20}, 0.0f,
                    texture_draw_scale, WHITE);
      DrawRectangleLines(
          screen_width - texture_draw_width - 20, 20, texture_draw_width,
          (i32)((f32)algo_state.texture.height * texture_draw_scale), GREEN);

      algo_dropdown_clicked = GuiDropdownBox(
          (Rectangle){.x = ((f32)screen_width / 2.0f) - 150.0f - 1.0f,
                      .y = 10.0f,
                      .height = 30,
                      .width = 100},
          "fbm;erosion", (int *)&algo_selection, algo_dropdown_active);

      camera_dropdown_clicked =
          GuiDropdownBox((Rectangle){.x = ((f32)screen_width / 2.0f) - 50.0f,
                                     .y = 10.0f,
                                     .height = 30,
                                     .width = 100},
                         "Free;Orbital;Static", (int *)&camera_model_selection,
                         camera_dropdown_active);
      reset_camera =
          GuiButton((Rectangle){.x = ((f32)screen_width / 2.0f) + 50.0f + 1.0f,
                                .y = 10.0f,
                                .height = 30.0f,
                                .width = 100},
                    "Reset Camera");

      algo_draw_ui(&algo_state, slider_x_offset);
    }
    EndDrawing();
  }
  algo_cleanup(&algo_state);

  CloseWindow();
  return 0;
}
