-------------------------------------------------------------------------------
CIS565: Project 5: Advanced GLSL
-------------------------------------------------------------------------------
Fall 2012
-------------------------------------------------------------------------------
Due Monday 11/19/2012
-------------------------------------------------------------------------------
In the final structured project for 565, I have implemented some interesting shader effects. First I have made a fragment shader for a textured globe with the following:


	Separate day and night textures smoothly blended with respect to the light sources position
	A cloud layer with transparency and animation
	Bump mapping
	Rim lighting

In addition to these features I have implemented parallax occlusion mapping in a separate (toggle-able) shader. This extra step allows me to simulate a globe with a much higher polygon count by "removing" a thin layer from the outer surface.

I have also implemented SSAO with a uniform sampler and a poisson disk sampler (again toggle-able).

Finally I have implemented two vertex shaders with interesting effects. The first is a melting effect where the model gradually sinks into a 2D puddle at the bottom of the screen. The second is a blip effect where the model is bulged out from the y-axis along the x and z axes in a small blip that moves up and down in time.

Blog: http://liamboone.blogspot.com/2012/11/project-5-more-shaders.html