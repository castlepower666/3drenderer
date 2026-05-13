#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"


triangle_t* triangles_to_render = NULL;

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

    load_cube_mesh_data();
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

    triangles_to_render = NULL;

    //遍历每个面，计算每个面的顶点在屏幕上的位置，并存储到triangles_to_render数组中
    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++)
    {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        triangle_t projected_triangle;

        for (int j = 0; j < 3; j++)
        {
            vec3_t transformed_vertex = face_vertices[j];

            transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);

            transformed_vertex.z -= camera_position.z;

            vec2_t projected_vertex = project(transformed_vertex);

            projected_vertex.x += window_width / 2;
            projected_vertex.y += window_height / 2;

            projected_triangle.points[j] = projected_vertex;
        }

        array_push(triangles_to_render, projected_triangle);

    }


}

void render(void)
{
    draw_grid();

    int num_triangles = array_length(triangles_to_render);

    for (int i = 0; i < num_triangles; i++)
    {
        triangle_t triangle = triangles_to_render[i];

        draw_triangle(
            triangle.points[0].x,
            triangle.points[0].y,
            triangle.points[1].x,
            triangle.points[1].y,
            triangle.points[2].x,
            triangle.points[2].y,
            0xFFFFFF00
        );

    }

    array_free(triangles_to_render);

    render_color_buffer();//颜色缓冲区渲染到渲染器上
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}

void free_resources(void)
{
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
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

    destroy_window();
    free_resources();


    return 0;
}