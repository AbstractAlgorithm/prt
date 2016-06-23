# prt
precomputed radiance transport

### todo

- ~~terrain~~
    - ~~render~~
    - ~~tessellated LOD~~
    - ~~fix cracks~~
    - textured and nicely shaded
- cubemap
    - ~~render irradiance cubemap from a point~~
    - ~~debug view~~
    - cubemap to SH
        - cs - solid angles
        - cs - accumulates SH coefficient while sampling the cubemap
    - render to six textures
        - cs - process textures as cubemap faces
            - solid angles
            - accumulate SH coefficient
    - debug SH (eval)
    - irradiance volumes
- prt
    - model loader
    - raytracer and sampler
    - diffuse
        - direct
        - self-shadow
        - self-interreflection
    - debug view
    - specular
        - ?
    - volumetric
        - direct + self-shadow
    - neighborhood transfer (soft shadows)
        - direct + self-shadow
    - radiance transfer volumes

- dynamic
    - dynamic scenes
    - dynamic transfer functions
