rootnode = gr.node('puddle')

puddle_poly = gr.mesh('puddle_poly', 'puddle.obj')
puddle_scale = 0.65
puddle_poly:scale(puddle_scale,puddle_scale,puddle_scale)

puddle_instance = gr.node('puddle')
ytrans =1*4.66
rootnode:add_child(puddle_instance)
puddle_instance:add_child(puddle_poly)
--this next line just centres the pivot of the board
puddle_instance:translate(-5.05, 0.5, ytrans)
puddle_instance:rotate('y', 90)
--puddle_instance:rotate('z', -90)

rootnode:translate(0, 0.5, -80.0)
rootnode:rotate('x', 50)

return rootnode
