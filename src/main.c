// clang-format off
#include "rand.c"
#include "fbm.c"

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
  Camera camera = {0};
  camera.position = initial_camera_position;
  camera.target = initial_camera_target;
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  const u32 dim_x = 256;
  const u32 dim_y = 256;
  Image image = {.height = dim_y,
                 .width = dim_x,
                 .mipmaps = 1,
                 .format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE};

  image.data = malloc(dim_x * dim_y * sizeof(u8));

  f32 octaves_f = 5.0f;
  FBMParams params = {
      .octaves = (u32)octaves_f, .lacunarity = 2.0f, .gain = 0.4f};
  FBMParams prev_params = params;

  gen_fbm((u8 *)image.data, dim_x, dim_y, params);

  Texture2D texture = LoadTextureFromImage(image);
  Mesh mesh = GenMeshHeightmap(image, (Vector3){16, 8, 16});
  Model model = LoadModelFromMesh(mesh);
  model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

  const f32 slider_x_offset = 10 + (f32)GetTextWidth("lacunarity: ");
  Vector3 map_position = {-8.0f, 0.0f, -8.0f};

  SetTargetFPS(60);

  bool change_seed = false;
  bool params_changed = false;

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

    params.octaves = (u32)octaves_f;
    params_changed = (prev_params.octaves != params.octaves) ||
                     (prev_params.gain != params.gain) ||
                     (prev_params.lacunarity != params.lacunarity);

    if (change_seed) {
      g_seed = pcg32_randomf();
    }

    if (change_seed || params_changed) {
      prev_params = params;
      UnloadTexture(texture);
      UnloadModel(model);

      gen_fbm((u8 *)image.data, dim_x, dim_y, params);
      texture = LoadTextureFromImage(image);
      mesh = GenMeshHeightmap(image, (Vector3){16, 8, 16});
      model = LoadModelFromMesh(mesh);
      model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    }

    BeginDrawing();
    {
      ClearBackground(RAYWHITE);

      GuiSlider(
          (Rectangle){
              .x = slider_x_offset, .y = 10, .height = 30, .width = 200},
          "octaves: ", TextFormat("%d", (i32)octaves_f), &octaves_f, 1, 10);

      GuiSlider(
          (Rectangle){
              .x = slider_x_offset, .y = 40, .height = 30, .width = 200},
          "lacunarity: ", TextFormat("%f", params.lacunarity),
          &params.lacunarity, 1, 5);

      GuiSlider(
          (Rectangle){
              .x = slider_x_offset, .y = 70, .height = 30, .width = 200},
          "gain: ", TextFormat("%f", params.gain), &params.gain, 0, 1);

      change_seed = GuiButton(
          (Rectangle){
              .x = slider_x_offset, .y = 100, .height = 30, .width = 200},
          "Change Seed");
      reset_camera = GuiButton(
          (Rectangle){
              .x = slider_x_offset, .y = 160, .height = 30, .width = 200},
          "Reste Camera");

      camera_dropdown_clicked = GuiDropdownBox(
          (Rectangle){
              .x = slider_x_offset, .y = 130, .height = 30, .width = 200},
          "Free;Orbital", (int *)&camera_model_selection,
          camera_dropdown_active);

      BeginMode3D(camera);
      {
        DrawModel(model, map_position, 1.0f, RED);
        DrawGrid(20, 1.0f);
      }
      EndMode3D();

      DrawTexture(texture, screen_width - texture.width - 20, 20, WHITE);
      DrawRectangleLines(screen_width - texture.width - 20, 20, texture.width,
                         texture.height, GREEN);
    }
    EndDrawing();
  }
  UnloadTexture(texture);
  UnloadModel(model);
  free(image.data);

  CloseWindow();
  return 0;
}
