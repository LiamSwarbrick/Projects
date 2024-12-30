
## Thesis "Integrating Linearly-Transformed Cosine based Area Lights into a Clustered Forward Renderer" Progress Update

<img src="files/30dec-heatmapoff-suntemple2.png">
<img src="files/30dec-heatmap-suntemple2.png">

<!-- 2fps -->
<!-- <img src="30dec-suntemple-fps-comparison-offf.png">  -->
<!-- 16fps -->
<!-- <img src="30dec-suntemple-fps-comparison-on.png"> -->

<img src="files/30dec-heatmapoff-suntemple.png">
<img src="files/30dec-heatmap-suntemple.png">

This 2024 November I've begun my thesis project **"Integrating Linearly-Transformed Cosine based Area Lights into a Clustered Forward Renderer"**, I'm developing my own OpenGL 4.6 renderer for it in C and you can see my progress on my project so far below if I remember to update this page...

#### DONE: Custom C OpenGL 4.6 glTF Physically-Based Renderer (full PBR including dynamic lights, normal mapping, alpha-blending with depth sorting).
Renderer also implements emissive textures, occlusion maps and normal maps. Developed support for way too many glTF files, its became quite a complete renderer.

#### DONE: Clustered Shading for huge performance boost with huge numbers of dynamic point lights.

My pipeline now involves a GLSL compute shader to divide view space into voxel clusters, fed into another compute shader to assign lights to those clusters. Each fragment now only has to compute the lights in its cluster.

<!-- Put these results in 2 column table -->
![No clustered shading example, 1fps](files/30dec-testlosttemple-clusteroff.PNG)
![Clustered shading example, 16fps](files/30dec-testlosttemple-clusteron.PNG)

#### Ongoing: Area lights. Clustered Shading. Area light assignment algorithm. Order-Independant-Transparency.


Area lights are physically-based polygonal lights, and I'm a full renderer that includes a way to assign them to spatial clusters in order to integrate clustered shading which drastically reduces the large GPU throughput that comes from forward rendering lots of lights and thus render scenes with potentially thousands of these lights in real-time.

<img src="22dec-blendtest.PNG" width=50%>
<!-- <img src="files/13nov-added-ambient-0-halfres.PNG"> -->
<!-- <img src="files/28nov-progress-normal-mapping.PNG"> -->
<img src="files/7dec-point-light-initial-testing.PNG" width=48%>
<img src="files/28dec-cool.PNG">
<img src="files/19dec-100point_lights-shinyUntitled.png" width=50%>
<img src="files/18dec-screenshot.PNG" width=49%>
<!-- <img src="files/28nov-normal-mapping-working.PNG"> -->
<!-- <img src="files/28nov-nmapping-with-shinies.png" width = 48%> -->
<img src="files/11nov-basecolor-texture-not-properly-working-initially.PNG", width=49%>
<img src="files/13nov-helmet-pbr-halfres.PNG" width=50%>
Early version^


