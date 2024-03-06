#include <SDL2/SDL.h>
#include <omp.h>
#include <stdio.h>

#include <array>
#include <fstream>
#include <iterator>
#include <thread>
#include <vector>

#include "PerformanceTimer.h"

constexpr int ROWS = 900;
constexpr int COLUMNS = 1400;
constexpr int CELL_SIZE = 1;
constexpr int THREAD_COUNT = 4;
constexpr int RANDOM_SEED = 47;
constexpr int ROWS_PER_THREAD = ROWS / THREAD_COUNT;
constexpr int GENERATIONS = 100;

typedef std::array<bool, ROWS * COLUMNS> Grid;

void fillGridRandomly(Grid &grid);
void printGrid(Grid &grid, SDL_Renderer *renderer);
int countNeighbours(Grid &grid, int i);
void updateGrid(Grid &grid, Grid &newGrid, int startRow, int endRow);

int main(int argc, char *argv[]) {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  bool running = true;
  SDL_Event event;
  int generations = 0;

  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_CreateWindowAndRenderer(COLUMNS, ROWS, 0, &window, &renderer);
  SDL_RenderSetScale(renderer, CELL_SIZE, CELL_SIZE);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  Grid grid;
  Grid newGrid;
  fillGridRandomly(grid);

  {
    PerformanceTimer timer;

    while (running) {
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          running = false;
        }
      }

      if (generations >= GENERATIONS) {
        running = false;
      }
      printGrid(grid, renderer);

#pragma omp parallel num_threads(THREAD_COUNT)
      {
        int tid = omp_get_thread_num();
        int startRow = tid * ROWS_PER_THREAD;
        int endRow =
            (tid == THREAD_COUNT - 1) ? ROWS : (tid + 1) * ROWS_PER_THREAD;

        updateGrid(grid, newGrid, startRow, endRow);
      }

      for (int i = 0; i < grid.size(); i++) {
        grid[i] = newGrid[i];
      }

      generations += 1;
    }
  }

  return 0;
}

void fillGridRandomly(Grid &grid) {
  srand(RANDOM_SEED);

  for (int i = 0; i < ROWS * COLUMNS; i++) {
    grid[i] = rand() % 2;
  }
}

void printGrid(Grid &grid, SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  std::vector<SDL_Point> pointsToRender;
  for (int i = 0; i < grid.size(); i++) {
    if (grid[i]) {
      pointsToRender.push_back({i % COLUMNS, i / COLUMNS});
    }
  }

  SDL_RenderDrawPoints(renderer, &pointsToRender[0], pointsToRender.size());
  SDL_RenderPresent(renderer);
}

int countNeighbours(Grid &grid, int i) {
  int result = 0;
  int row = i / COLUMNS;
  int column = i % COLUMNS;

  // Check for elements on top
  if (row > 0) {
    if (column > 0) {
      result += grid[(row - 1) * COLUMNS + (column - 1)];  // Top left
    }
    result += grid[(row - 1) * COLUMNS + column];  // Top middle
    if (column < COLUMNS - 1) {
      result += grid[(row - 1) * COLUMNS + (column + 1)];  // Top right
    }
  }

  // Check for left and right
  if (column > 0) {
    result += grid[row * COLUMNS + (column - 1)];  // Left
  }
  if (column < COLUMNS - 1) {
    result += grid[row * COLUMNS + (column + 1)];  // Right
  }

  // Check for elements on bottom
  if (row < ROWS - 1) {
    if (column > 0) {
      result += grid[(row + 1) * COLUMNS + (column - 1)];  // Bottom left
    }
    result += grid[(row + 1) * COLUMNS + column];  // Bottom middle
    if (column < COLUMNS - 1) {
      result += grid[(row + 1) * COLUMNS + (column + 1)];  // Bottom right
    }
  }

  return result;
}

void updateGrid(Grid &grid, Grid &newGrid, int startRow, int endRow) {
  for (int i = startRow * COLUMNS; i < endRow * COLUMNS; i++) {
    int count = countNeighbours(grid, i);

    newGrid[i] = (grid[i] ? (count == 2 || count == 3) : count == 3);
  }
}