# Wave Simulation

actual size is 240x240 in 480x480 bordless window

- To change size of window you need to change w and h in SDL_CreateWindow in init()
- To change size of simulation you need to change width and height in main()

# Simulation versions

as you can see, repo have two different files with no co-linkage,
that's it because there are two different Render-mechanics used inside each file
- in PixelRender every frame drawed by pixel directly to renderer
- in TextureRender every frame copied by pixel to texture and then texture is copied to renderer

# Requirements

to use simulation you need to install and link in project SDL2 library