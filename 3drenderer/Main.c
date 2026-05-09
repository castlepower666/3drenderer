#include "display.h"
#include "vector.h"
#include "mesh.h"

float fov_factor = 1024;
vec3_t camera_position = { 0,0,-5 };
vec3_t cube_rotation = { 0,0,0 };
int previous_frame_time = 0;

bool is_running = false;

void setup(void)
{
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );



}

void process_input(void)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type)
    {
    case SDL_QUIT:
        is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
        {
            is_running = false;
        }
        break;
    }


}


vec2_t project(vec3_t point)
{
    vec2_t projected_point = {
        point.x * fov_factor / point.z,//乘以fov_factor是为了放大投影后的点，使其更容易看到
        point.y * fov_factor / point.z
    };

    return projected_point;
}

void update(void)
{
    //while会阻塞程序，占满cpu；使用SDL_Delay可以让程序休眠一段时间，降低cpu占用率
    //while (!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));

    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    cube_rotation.x += 0.001;
    cube_rotation.y += 0.001;
    cube_rotation.z += 0.001;

    for (int i = 0; i < N_MESH_VERTICES; i++)
    {
        face_t mesh_face = mesh_faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh_vertices[mesh_face.a - 1];
        face_vertices[1] = mesh_vertices[mesh_face.b - 1];
        face_vertices[2] = mesh_vertices[mesh_face.c - 1];

        for (int j = 0; j < 3; j++)
        {
            vec3_t transformed_vertex = face_vertices[j];

            transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);

            vec2_t projected_vertex = project(transformed_vertex);
        }

    }


}

void render(void)
{
    draw_grid();

    //for (int i = 0; i < N_POINTS; i++)
    //{
    //    draw_rect(
    //        projected_points[i].x + (window_width / 2),//把cube放到屏幕中心
    //        projected_points[i].y + (window_height / 2),
    //        4,
    //        4,
    //        0xFFFFFF00
    //    );
    //}


    render_color_buffer();//颜色缓冲区渲染到渲染器上
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}

void destroy_window(void)
{
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    is_running = initialize_window();

    setup();

    while (is_running)
    {
        process_input();
        update();
        render();
    }

    return 0;
}