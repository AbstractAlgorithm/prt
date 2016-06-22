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
    - generate prt for a point
    - debug view
    - radiance transfer volumes
- app
    - debug view with prt and irradiance
    - shade with prt and irradiance
    - interpolate between the local prts

