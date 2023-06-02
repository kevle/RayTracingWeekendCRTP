## What is this repository? ##

This repository contains a simple ray tracer based on Peter Shirley's excellent "Ray Tracing in One Weekend" (RTOW) series: https://raytracing.github.io/

In addition to the features from the RTOW volume 1, it also features some rudimentary texture support and support for triangle rendering. 

For visualization of rendering results the fantastic [PixelGameEngine](https://github.com/OneLoneCoder/olcPixelGameEngine) is being used.

## Why is this repository? ##

Back when I started toying with this code in 2019, I had never used the [CRTP](https://en.cppreference.com/w/cpp/language/crtp) technique or [expression templates](https://en.cppreference.com/w/cpp/language/expression_template).

Another thing I wanted to try was the QML module of Qt for visualization. This has since been replaced by the much simpler [PixelGameEngine](https://github.com/OneLoneCoder/olcPixelGameEngine) to reduce binary dependencies.

Initially, I had also planned to use SYCL ( or failing that, CUDA ) to run the ray tracer on a GPU. Because of that, instead of using virtual interfaces variant is being used. For the GPU side, this [variant](https://github.com/jaredhoberock/variant) implementation was to be used.

So I was looking for a way to use all those techniques / tools in a single project that would also be kind of fun and produce something visually appealing.

## Vector arithmetic operation ##

Since lots of vector operations are used in the making of the ray tracer, I thought implementing some rudimentary expression template mechanisms to potentially let the compiler exploit SIMD parallelism in vector expressions might be instructive.

Here is one of the vector operations as used in vanilla RTOW:

    inline vec3 operator+(const vec3 &u, const vec3 &v) {
      return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
    }

Operations similarly defined to this one are concatenated and may defeat compiler optimization. Or so I thought at the time. I have never actually checked whether the generated assembly in fact improves when using the implementation in this repository.

What can be rendered?
-------------

Truth be told, not so much since most of the settings are hardcoded and only volume 1 of the RTOW series has been implemented. But here are some examples for your viewing pleasure.

##### Visualization of rendering process with the dragon from [Computer Graphics Laboratory Stanford University](http://graphics.stanford.edu/data/3Dscanrep/)

##### Balls

##### World texture