#include "display.h"
#include "vector.h"

#define N_POINTS (9*9*9)
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];
float fov_factor = 120;

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

    int point_count = 0;
    for (float x = -1; x <= 1; x += 0.25)
    {
        for (float y = -1; y <= 1; y += 0.25)
        {
            for (float z = -1; z <= 1; z += 0.25)
            {
                vec3_t new_point = { x,y,z };
                cube_points[point_count++] = new_point;
            }
        }
    }

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

//正交投影
vec2_t project(vec3_t point)
{
    vec2_t projected_point = {
        point.x * fov_factor,//乘以fov_factor是为了放大投影后的点，使其更容易看到
        point.y * fov_factor
    };

    return projected_point;
}

void update(void)
{
    for (int i = 0; i < N_POINTS; i++)
    {
        projected_points[i] = project(cube_points[i]);
    }
}

void render(void)
{
    draw_grid();

    for (int i = 0; i < N_POINTS; i++)
    {
        draw_rect(
            projected_points[i].x + (window_width / 2),//把cube放到屏幕中心
            projected_points[i].y + (window_height / 2),
            4,
            4,
            0xFFFFFF00
        );
    }


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