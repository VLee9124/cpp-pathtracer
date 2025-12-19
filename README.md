# cpp-pathtracer

This is my code-along to Raytracing In One Weekend to learn more about raytracing and review C++.

Notes + Todo List:

*   Features Multithreaded renderer using C++ Threads
    - Multithreaded version using C++ threads on [main branch](https://github.com/VLee9124/cpp-pathtracer/tree/main)
    - Multithreaded version using OpenMP on [openmp branch](https://github.com/VLee9124/cpp-pathtracer/tree/openmp)
*   Code needs to be refactored
    - Create project structure
    - Refactor classes and remove non-threadsafe implementations since render class is multithreaded.
    - Create a CMAKE to handle messy compile flags for clang OpenMP.

Here is a sample render that took 30 minutes to render.

![Final Render](image.png)
