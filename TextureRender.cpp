#include <iostream>
#include <SDL2/SDL.h>
#include <vector>

using namespace std;

SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;
SDL_Surface* surf = NULL;
SDL_Texture *Tile = NULL;

bool init() {
    bool ok = true;
    
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        ok = false;
        cout << "Can't init SDL: " << SDL_GetError() << endl;
    }
    
    win = SDL_CreateWindow("roundrend", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 480, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
    if (win == NULL) {
        ok = false;
        cout << "Can't init window: " << SDL_GetError() << endl;
    }
    
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL) {
        ok = false;
        cout << "Can't init renderer: " << SDL_GetError() << endl;
    }

    int mnwidth, mnheight;
    SDL_GetWindowSize(win, &mnwidth, &mnheight);
    Tile = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_STREAMING, mnwidth, mnheight);
    if (Tile == NULL) {
        ok = false;
        cout << "Can't init Texture: " << SDL_GetError() << endl;
    }

    return ok;
}

void quit() {
    SDL_DestroyWindow(win);
    win = NULL;

    SDL_DestroyRenderer(ren);
    ren = NULL;

    SDL_DestroyTexture(Tile);
    Tile = NULL;

    SDL_Quit();
}

template <class R>
R champ(R l, R mino, R maxo) {
    return (min(max(l,mino),maxo));
}


int main() {
    if (!init()) {
        quit();
        return 1;
    }
    
    int mnwidth, mnheight;
    SDL_GetWindowSize(win, &mnwidth, &mnheight);
    int width = mnwidth;
    int height = mnheight;
    int wdiff = mnwidth/width;
    int hdiff = mnheight/height;
    int frame = 0;
    vector<vector<double>> pixelmassa = {}, waveVelocitya = {}, waveHeighta = {};
    for (int x = 0; x <= width; x++) {
        pixelmassa.push_back({});
        waveVelocitya.push_back({});
        waveHeighta.push_back({});
        for (int y = 0; y <= height; y ++) {
            pixelmassa[x].push_back(1);
            waveVelocitya[x].push_back(0);
            waveHeighta[x].push_back(0);
        }
    }
    Uint64 NOW = SDL_GetTicks64(), CT = NOW, THEN = NOW, DELTA, DESDELTA = 1000/60;
    int second = 1000;
    int fps = 0, fpsc = 0;

    SDL_Rect rec{720, 300, 480, 480};

    SDL_Event event;

    bool running = true;

    while (running) {
    // SDL_GetWindowSize(win, &mnwidth, &mnheight);
    // int wdiff = mnwidth/width;
    // int hdiff = mnheight/height;

        NOW = SDL_GetTicks64();
        DELTA = NOW - THEN;
        THEN = NOW;
        if (DELTA < DESDELTA) {
            SDL_Delay(DESDELTA-DELTA);
        }
        fpsc++;
        if (NOW-CT >= second) {
            
            CT = NOW;
            fps = fpsc;
            fpsc = 0;
        }
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit();
                return 0;
            }
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                quit();
                return 0;
            }
        }
        if (frame < 100) {
            waveHeighta[(int)(width*0.5)][(int)(height*0.5)] = frame;
            frame++;
        }
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y ++) { 
                waveHeighta[x][y] += waveVelocitya[x][y];
            }
        }

        for (int x = 1; x < width; x++) {
            for (int y = 1; y < height; y ++) { 
                double force = waveHeighta[x+1][y] + waveHeighta[x-1][y] + waveHeighta[x][y+1] + waveHeighta[x][y-1]; 
                waveVelocitya[x][y] += (force / 4 - waveHeighta[x][y]) * pixelmassa[x][y];
            }
        }




        SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(ren);

        unsigned char* bytes = nullptr;
        int pitch = 0;
        SDL_LockTextureToSurface(Tile, NULL, &surf);
        SDL_FillRect(surf, NULL, SDL_MapRGBA(surf->format, 0, 0, 0, 0));
        SDL_UnlockTexture(Tile);
        SDL_LockTexture(Tile, nullptr, reinterpret_cast<void**>(&bytes), &pitch);
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) { 
                if ((waveHeighta[x][y] < 0)) {
                    unsigned char rgba[4] = {0xFF, (unsigned char)champ((int)(waveHeighta[x][y]*(-85))+30, 0, 255), (unsigned char)champ((int)(waveHeighta[x][y]*(-88))+30, 0, 255), (unsigned char)champ((int)((waveHeighta[x][y])*(-23)), 0, 255)};
                    
                    memcpy(&bytes[((y * width*hdiff*hdiff+hdiff) + (x*wdiff+wdiff))*sizeof(rgba)], rgba, sizeof(rgba));
                }
            }
        }
        SDL_UnlockTexture(Tile);
        SDL_RenderCopy(ren, Tile, NULL, NULL);
        SDL_RenderPresent(ren);
        cout << fps << " frames per second\n";
    }

    quit();
    return 0;
}
