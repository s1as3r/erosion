// clang-format off
#include "fbm.c"
#include "rand.c"
#include "fbm_raylib.c"

#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>
// clang-format on

i32 main(void) {
  const i32 screen_width = 1200;
  const i32 screen_height = 800;
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

  FBMState fbm_state;
  fbm_init(&fbm_state);

  SetTargetFPS(60);

  i32 camera_model_selection = 1;
  i32 camera_mode;

  bool reset_camera = false;
  bool camera_dropdown_active = false;
  bool camera_dropdown_clicked = false;
  while (!WindowShouldClose()) {
    if (camera_dropdown_clicked) {
      camera_dropdown_active = !camera_dropdown_active;
    }
    if (reset_camera) {
      camera.position = initial_camera_position;
      camera.target = initial_camera_target;
    }
    camera_mode = camera_model_selection + 1;
    UpdateCamera(&camera, camera_mode);

    fbm_update_state(&fbm_state);

    BeginDrawing();
    {
      ClearBackground(RAYWHITE);

      camera_dropdown_clicked =
          GuiDropdownBox((Rectangle){.x = (screen_width / 2.0f) - 101.0f,
                                     .y = 10.0f,
                                     .height = 30,
                                     .width = 100},
                         "Free;Orbital", (int *)&camera_model_selection,
                         camera_dropdown_active);
      reset_camera = GuiButton((Rectangle){.x = (screen_width / 2.0f) + 1.0f,
                                           .y = 10.0f,
                                           .height = 30.0f,
                                           .width = 100},
                               "Reset Camera");

      fbm_draw_ui(&fbm_state);

      BeginMode3D(camera);
      {
        DrawModel(fbm_state.model, map_position, 1.0f, RED);
        DrawGrid(20, 1.0f);
      }
      EndMode3D();

      DrawTexture(fbm_state.texture,
                  screen_width - fbm_state.texture.width - 20, 20, WHITE);
      DrawRectangleLines(screen_width - fbm_state.texture.width - 20, 20,
                         fbm_state.texture.width, fbm_state.texture.height,
                         GREEN);
    }
    EndDrawing();
  }
  fbm_cleanup(&fbm_state);

  CloseWindow();
  return 0;
}
