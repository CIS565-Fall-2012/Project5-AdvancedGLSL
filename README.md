-------------------------------------------------------------------------------
CIS565: Project 5: Advanced GLSL
-------------------------------------------------------------------------------
Fall 2012
-------------------------------------------------------------------------------
Due Tuesday 11/20/2012
-------------------------------------------------------------------------------
BLOG Link: Not determined yet.
-------------------------------------------------------------------------------
A brief description
-------------------------------------------------------------------------------
The goal of this project is to explore the functionality of GLSL further in three parts.
The first part of this project is a globe renderer capable of rendering a convincing model of planet Earth from space.
The second part is a screen-space ambient occlusion technique based entirely in the fragment shader.
The third one is to implement two interesting vertex shadings.

-------------------------------------------------------------------------------
PART 1: Globe Renderer
-------------------------------------------------------------------------------
- Basic
* Bump mapped terrain
* Rim lighting to simulate atmosphere
* Nighttime lights on the dark side of the globe
* Specular mapping
* Moving clouds

- Additional 
* Shade base on altitude using the height map(keyboard '2')

![Shade base on altitude using the height map](Project5-AdvancedGLSL/raw/master/readme_files/altitude_shading.png)

Figure 0. Shade base on altitude using the height map.

-------------------------------------------------------------------------------
PART 2: Screen-Space Ambient Occlusion
-------------------------------------------------------------------------------
- Basic
* regular sample based approach

![regular sample based SSAO](Project5-AdvancedGLSL/raw/master/readme_files/ssao_sponza.png)

Figure 1. regular sample based SSAO.

-------------------------------------------------------------------------------
PART 3: Vertex Shading
-------------------------------------------------------------------------------
* Vertex Pulsing(keyboard '2')
* Vertex Morphing(keyboard '3')

![pulsing cow](Project5-AdvancedGLSL/raw/master/readme_files/pulsing_cow.png)

Figure 2. pulsing cow.

-------------------------------------------------------------------------------
How to build
-------------------------------------------------------------------------------
I developed the part1 on Visual Studio 2010.
The solution files are located in 
part1/Globe/Globe.sln
part2/565GLFrame/565GLFrame.sln
part3/565GLFrame/565GLFrame.sln

You should be able to build it without modification.