-------------------------------------------------------------------------------
Submitted Features
-------------------------------------------------------------------------------
In Part 1, I implemented:
    Bump mapped terrain
    Rim lighting to simulate atmosphere
    Nighttime lights on the dark side of the globe
    Specular mapping
    Moving clouds
	
In part 2: Ambient Occlusion with regular grid

In Part 3: Morphing in sphere and Vertex pulsing/Bulge


-------------------------------------------------------------------------------
Blog
-------------------------------------------------------------------------------
http://advancedglsl.blogspot.com/

-------------------------------------------------------------------------------
CIS565: Project 5: Advanced GLSL
-------------------------------------------------------------------------------
Fall 2012
-------------------------------------------------------------------------------
Due Monday 11/19/2012
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
NOTE:
-------------------------------------------------------------------------------
This project requires any graphics card with support for a modern OpenGL pipeline. Any AMD, NVIDIA, or Intel card from the past few years should work fine, and every machine in the SIG Lab and Moore 100 is capable of running this project.

Theis project's default basecode requires Visual Studio 2010 or newer. Of course, if you want to roll your own makefiles, feel free to do so.

-------------------------------------------------------------------------------
INTRODUCTION:
-------------------------------------------------------------------------------
In this project, you will further explore the world of GLSL in three parts. The first part of this project is a globe renderer capable of rendering a convincing model of planet Earth from space. In the second part of this project, you will explore and implement a handful of screen-space ambient occlusion techniques based entirely in the fragment shader. Finally, in the third and last part of this project, you will implement several interesting vertex shading operations of your choice from a given list of options.

-------------------------------------------------------------------------------
CONTENTS:
-------------------------------------------------------------------------------
The Project5 root directory contains the following subdirectories:
	
* part1/ contains the base code for the globe renderer part of the assignment.
* part1/Globe contains a Visual Studio 2010 project for the globe renderer
* part1/shared32 contains libraries that are required to build and run the globe renderer
* part2/ contains the base code for both the screen space ambient occlusion and vertex shading parts of the assignment.
* part2/565GLFrame contains a Visual Studio 2010 project that serves as a generic OpenGL framework

-------------------------------------------------------------------------------
PART 1 REQUIREMENTS:
-------------------------------------------------------------------------------
In Part 1, you are given code for:

* Reading and loading textures
* Rendering a sphere with textures mapped on
* Basic passthrough fragment and vertex shaders 
* A basic globe with Earth terrain color mapping
* Gamma correcting textures

You are required to implement:

* Bump mapped terrain
* Rim lighting to simulate atmosphere
* Nighttime lights on the dark side of the globe
* Specular mapping
* Moving clouds

You are also required to pick one open-ended effect to implement:

* Procedural water rendering and animation using noise 
* Shade base on altitude using the height map
* Cloud shadows via ray-tracing through the cloud map in the fragment shader
* Orbiting Moon with texture mapping and shadow casting onto Earth
* Your choice! Email Karl and Patrick to get approval first

Some examples of what your completed globe renderer will look like:

![Completed globe, day side](Project5-AdvancedGLSL/raw/master/readme_files/globe_day.png)

Figure 0. Completed globe renderer, daylight side.

![Completed globe, twilight](Project5-AdvancedGLSL/raw/master/readme_files/globe_twilight.png)

Figure 1. Completed globe renderer, twilight border.

![Completed globe, night side](Project5-AdvancedGLSL/raw/master/readme_files/globe_night.png)

Figure 2. Completed globe renderer, night side.

-------------------------------------------------------------------------------
PART 1 WALKTHROUGH:
-------------------------------------------------------------------------------

In part 1, we render a globe with night lights on the unlit side; a specular map so specular highlights only occur in the ocean; animated clouds; and bump mapping, which perturbs the surface normal to give the appears of mountains.

Open Globe\Globe\Globe.sln in Visual Studio and run it. You’ll see a globe with Phong lighting like the one in Figure 3. All changes you need to make will be in the fragment shader, fs.glsl.

![Initial globe](Project5-AdvancedGLSL/raw/master/readme_files/globe_initial.png)

Figure 3. Initial globe with diffuse and specular lighting.

**Night Lights**

The backside of the globe not facing the sun is completely black in the initial globe. Use the `diffuse` lighting component to detect if a fragment is on this side of the globe, and, if so, shade it with the color from the night light texture, `u_Night`. Do not abruptly switch from day to night; instead us the `GLSL mix` function to smoothly transition from day to night over a reasonable period. The result globe will look like Figure 4. Consider brightening the night lights by multiplying the value by two. 

The base code shows an example of how to gamma correct the nighttime texture:

`float gammaCorrect = 1/1.8;`

`vec4 nightColor = pow(texture2D(u_Night, v_Texcoord),1/2.2);`

Feel free to play with gamma correcting the night and day textures if you wish. Find values that you think look nice!

![Day/Night without specular mapping](Project5-AdvancedGLSL/raw/master/readme_files/globe_nospecmap.png)

Figure 4. Globe with night lights and day/night blending at dusk/dawn.

**Specular Map** 

Our day/night color still shows specular highlights on landmasses, which should only be diffuse lit. Only the ocean should receive specular highlights. Use `u_EarthSpec` to determine if a fragment is on ocean or land, and only include the specular component if it is in ocean.

![Day/Night with specular mapping](Project5-AdvancedGLSL/raw/master/readme_files/globe_specmap.png)

Figure 5. Globe with specular map. Compare to Figure 4. Here, the specular component is not used when shading the land.

**Clouds**

In day time, clouds should be diffuse lit. Use `u_Cloud` to determine the cloud color, and `u_CloudTrans` and `mix` to determine how much a daytime fragment is affected by the day diffuse map or cloud color. See Figure 6.

In night time, clouds should obscure city lights. Use `u_CloudTrans` and `mix` to blend between the city lights and solid black. See Figure 7.

Animate the clouds by offseting the `s` component of `v_Texcoord` by `u_time` when reading
`u_Cloud` and `u_CloudTrans`.

![Day with clouds](Project5-AdvancedGLSL/raw/master/readme_files/globe_daycloud.png)

Figure 6. Clouds with day time shading.

![Night with clouds](Project5-AdvancedGLSL/raw/master/readme_files/globe_nightcloud.png)

Figure 7. Clouds observing city nights on the dark side of the globe.

**Bump Mapping**

Add the appearance of mountains by perturbing the normal used for diffuse lighting the ground (not the clouds) by using the bump map texture, `u_Bump`. This texture is 1000x500, and is zero when the fragment is at sea-level, and one when the fragment is on the highest mountain. Read three texels from this texture: once using `v_Texcoord`; once one texel to the right; and once one texel above. Create a perturbed normal in tangent space:

`normalize(vec3(center - right, center - top, 0.2))`

Use `eastNorthUpToEyeCoordinates` to transform this normal to eye coordinates, normalize it, then use it for diffuse lighting the ground instead of the original normal.

![Globe with bump mapping](Project5-AdvancedGLSL/raw/master/readme_files/globe_bumpmap.png)

Figure 8. Bump mapping brings attention to mountains.

**Rim Lighting**

Rim lighting is a simple post-processed lighting effect we can apply to make the globe look as if it has an atmospheric layer catching light from the sun. Implementing rim lighting is simple; we being by finding the dot product of `v_Normal` and `v_Position`, and add 1 to the dot product. We call this value our rim factor. If the rim factor is greater than 0, then we add a blue color based on the rim factor to the current fragment color. You might use a color something like `vec4(rim/4, rim/2, rim/2, 1)`. If our rim factor is not greater than 0, then we leave the fragment color as is. Figures 0,1 and 2 show our finished globe with rim lighting.

For more information on rim lighting, read http://www.fundza.com/rman_shaders/surface/fake_rim/fake_rim1.html.

-------------------------------------------------------------------------------
PART 2 REQUIREMENTS:
-------------------------------------------------------------------------------
In Part 2, you are given code for:

* Loading triangle mesh OBJ files and rendering them as VBOs in OpenGL
* Passthrough vertex and fragment shaders
* Code for binding FBOs and textures to OpenGL for reading/writing in GLSL
* Partially implemented vertex and fragment shaders for screen space ambient occlusion
* Camera movement through keyboard and mouse

For Part 2, you will be working entirely in the file `ssao.frag`. `ssao.frag` contains stubs for three types of screen space ambient occlusion: regular sample based ssao, poisson sampling based ssao, and world space sampling based ssao. 

You are only required to implement the regular sample based approach.

However, if you are ambitious or want better looking results, instructions for the poisson and world space sampling based approaches are also included in this project README.

The famous "Sponza" test scene file is included with part 2, and by default the base code will load `sponza.obj` and render the scene with only diffuse lighting. With SSAO, Sponza should look something like this:

![Sponza, with SSAO](Project5-AdvancedGLSL/raw/master/readme_files/ao_sponza.png)

-------------------------------------------------------------------------------
PART 2 WALKTHROUGH:
-------------------------------------------------------------------------------

**Intro to SSAO**

In this part, we explore Screen Space Ambient Occlusion (SSAO), an ubiquitous technique in real-time graphics. First introduced in Crysis in 2007, SSAO approximates global illumination using basic scene information captured in normal, depth, and/or position buffers, yielding a decent visual appearance at a fraction of the cost. 

Ambient occlusion assumes that all light sources are ambient; that is, incoming in uniform strength from all directions. Think of the illumination of an overcast day. Ambient Occlusion measures how much ambient illumination is occluded, or blocked, by nearby geometry. To calculate ambient occlusion, rays are cast outwards into the scene from worldspace sample points and collided with nearby geometry. For static or semi-static objects (forexample, mildly deforming characters) ambient occlusion can be precomputed and included into real-time lighting calculations.

SSAO takes advantage of the fact that much of the information needed to calculate Ambient Occlusion is present in screen-space buffers of position, normals, and depth, that might be present in, for example, a deferred renderer. To create such buffers, we render the scene geometry with a shader program that outputs the desired attributes for each fragment. To estimate the ambient occlusion, instead of casting rays out into the scene to collide with scene geometry, we sample in the neighborhood of points from these stored screen-space
buffers.

**Base Code Notes**

The base code for this part allows for a number of different view modes, which are triggered by the numberpad on your keyboard:

* 1: Displays no occlusion and just diffuse lighting
* 2: Displays occlusion via regular grid samples (once implemented)
* 3. Displays occlusion via poisson screen space samples (once implemented)
* 4. Displays occlusion via world space samples (once implemented)
* 6. Displays depth
* 7. Displays normals
* 8. Displays position
* 9. Displays total occlusion subtracted from white

Where the camera looks in this base code is controlled using the mouse, and you can move around using the WASD keys.

**Occlusion Function**

The first task you must complete before you can move on to implementing your sampling schemes is to implement the function `float gatherOcclusion(vec3 pt_normal, vec3 pt_position, vec3 occluder_normal, vec3 occluder_position)`. This function estimates the occlusion of a point by a new sample occluder. This function has several desired qualities; one is occlusion should fall off with large distances to prevent samples from foreground objects from spuriously occluding background points.

Another quality is that points should not be occluded much by samples in the same or similar plane. This prevents false self-occlusion on a flat surface. Co-planarity can be determined by looking at the normals at the point and the sampled occluder.

The last quality is that occluding samples that are located overhead a point, with respect to its normal, are more important than samples that are located obliquely. This is because, in a basic diffuse lighting model, the luminance due to a light source is proportional to dot(Normal, Incident). Thus ambient light sources blocked out by an occluder overhead are more important.

Design a function that has that roughly fits these criteria and any more you can think of. Of course, you may change the function signature if you need additional information. You may wish to start this part to get something workable, then do some of the sampling tasks before coming back to tune it. Designing a function is easier with a good sampling scheme to view to the results.

Some good resources to utilize in implementing this section are listed below. Feel free to look at the code examples posted for inspiration, but DO NOT copy them.

* http://www.john-chapman.net/content.php?id=8
* http://www.gamerendering.com/2009/01/14/ssao/
* http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/a-simple-and-practical-approach-to-ssao-r2753
* http://developer.download.nvidia.com/whitepapers/2008/PCSS_Integration.pdf
* http://www.iquilezles.org/www/articles/ssao/ssao.htm

**Regular Grid Samples**

Armed with your occluding function, you can now estimate ambient occlusion by sampling a number of occluders around each fragment and average the results. The first sampling scheme we’ll try is using a regular grid in screen-space around each fragment. To keep performance and appearance roughly consistent with the next two parts, you should use 16 total samples in a 4x4 grid, and use the step size provided in ssao.frag. At each screenspace grid sample, access the position and normal buffers and call your occlusion function to estimate the occlusion on the fragment from that sample. Average the sampled occlusions to get your result.

**Poisson Disk Screen Space Samples**

This sampling scheme seeks to eliminate many of the coherent artifacts from the regular grid. It will use a two-pronged approach to do so. First, we will no longer sample on regularly, evenly spaced grid points. Instead, we will sample on an irregular Poisson disk, an irregular but well-spaced distribution. Second, will not use the exact same sampling mask at each fragment, but will instead rotate the mask based on a random angle per fragment.

We will again use 16 samples per fragment. The disk is provided as a 16-element array of vec2’s in ssao.frag called poissonDisk. For each fragment, grab a random 0-1 float from a random texture with getRandomScalar(), linearly scale it to a random angle between 0 and 2*PI,and rotate the disk points around the origin by that angle. Then sample around the screenspace coordinates of the fragment with offsets from rotated disk and average the computed occlusions. To keep the appearance consistent with the other sampling schemes, you should
scale the disk radius by the provided radius.

**World Space Samples**

In this approach, instead of sampling occluders from a screen-space disk, we will sample occluders on a view-space hemisphere around the fragment. This gives us accurate scaling of the effect with distance, better importance sampling (since we only sample points in front of the fragment), and a sampling that better represents the neighborhood of the fragment. As a reminder, View-space is the coordinate system after the model and view transformations, but before the perspective transformations, where points exists in 3-dimensions but up and forward are aligned to the camera.

As before, we will use a single set of 16 sample points in a Poisson Sphere. To break up artifacts, we will again randomly adjust the points per-fragment. In this case, we will get a random unit vector using getRandomNormal(), and reflect all the sample points across the plane that unit vector defines. This is done per-fragment.

Once we have a somewhat-random sphere of 16 points, some of points on that sphere may be “behind” the normal of the fragment, and couldn’t contribute to occlusion. We want to reflect them across the plane of the normal, so that we now have a hemisphere of points above the normal. To keep the appearance consistent with the other schemes, you should use scale the hemisphere by the provided radius.

Now that we have a proper hemisphere, our occlusion samples have to be taken in 0-1 screen space. Thus, we need to transform our view-space points into screen-space. This is similar to what happens in most vertex shaders and then later during clipping.

First, the points should then be transformed to perspective space by multiplying with the provided projection matrix u_Persp. After this step, you need to perform homogenous division and divide all the points by their w coordinate. The points are now in clipping space, which ranges from -1 to 1. Since screen-space ranges from 0 to 1, we need to perform a last affine remapping of clipping/2 + 0.5 to place the points in screen-space. We can now sample occluders and average as before.

-------------------------------------------------------------------------------
PART 3 REQUIREMENTS:
-------------------------------------------------------------------------------
In Part 3, you will utilize the same base code used in Part 2. You may choose to either work out of the same code base as you used in Part 2, or you may choose to make a separate copy of the Part 2 base code to work out of for Part 3.

In Part 3, you are required to load an interesting triangle mesh model of your choice (the standard cow, or the Stanford Dragon, for example) in place of the Sponza scene. Then, you are required to implement any two of the following four vertex shaders:

* Vertex Morphing
* Vertex Pulsing
* Key-frame interpolation
* Skinning

Add keyboard controls to switch between your vertex shaders, and for each feature you implement, create a new shader based on the given passthrough vertex shader.

-------------------------------------------------------------------------------
PART 3 WALKTHROUGH:
-------------------------------------------------------------------------------
Detailed information on how to implement these for shaders can be found here:

http://http.developer.nvidia.com/CgTutorial/cg_tutorial_chapter06.html

-------------------------------------------------------------------------------
BLOG
-------------------------------------------------------------------------------
As mentioned in class, all students should have student blogs detailing progress on projects. If you already have a blog, you can use it; otherwise, please create a blog using www.blogger.com or any other tool, such as www.wordpress.org. Blog posts on your project are due on the SAME DAY as the project, and should include:

* A brief description of the project and the specific features you implemented.
* A link to your github repo if the code is open source.
* At least one screenshot of your project running.
* A 30 second or longer video of your project running. To create the video, use http://www.microsoft.com/expression/products/Encoder4_Overview.aspx 

-------------------------------------------------------------------------------
THIRD PARTY CODE POLICY
-------------------------------------------------------------------------------
* Use of any third-party code must be approved by asking on Piazza.  If it is approved, all students are welcome to use it.  Generally, we approve use of third-party code that is not a core part of the project.  For example, for the ray tracer, we would approve using a third-party library for loading models, but would not approve copying and pasting a CUDA function for doing refraction.
* Third-party code must be credited in README.md.
* Using third-party code without its approval, including using another student's code, is an academic integrity violation, and will result in you receiving an F for the semester.

-------------------------------------------------------------------------------
SELF-GRADING
-------------------------------------------------------------------------------
* On the submission date, email your grade, on a scale of 0 to 100, to Karl, yiningli@seas.upenn.edu, with a one paragraph explanation.  Be concise and realistic.  Recall that we reserve 30 points as a sanity check to adjust your grade.  Your actual grade will be (0.7 * your grade) + (0.3 * our grade).  We hope to only use this in extreme cases when your grade does not realistically reflect your work - it is either too high or too low.  In most cases, we plan to give you the exact grade you suggest.
* Projects are not weighted evenly, e.g., Project 0 doesn't count as much as the path tracer.  We will determine the weighting at the end of the semester based on the size of each project.

-------------------------------------------------------------------------------
SUBMISSION
-------------------------------------------------------------------------------
As with the previous project, you should fork this project and work inside of your fork. Upon completion, commit your finished project back to your fork, and make a pull request to the master repository.
You should include a README.md file in the root directory detailing the following

* A brief description of the project and specific features you implemented
* At least one screenshot of your project running, and at least one screenshot of the final rendered output of your raytracer
* Instructions for building and running your project if they differ from the base code
* A link to your blog post detailing the project
* A list of all third-party code used