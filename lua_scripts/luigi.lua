rootnode = gr.node('luigi')

luigi_poly = gr.mesh('luigi_poly', '../models/luigi textured obj.obj')

--Draw the luigi
luigi_instance = gr.node('luigi')
ytrans =3*4.66
rootnode:add_child(luigi_instance)
luigi_instance:add_child(luigi_poly)
--this next line just centres the pivot of the board
luigi_instance:rotate('y', 90)
luigi_instance:rotate('z', -90)

rootnode:translate(0, 0.5, -80.0)
rootnode:rotate('x', 50)


return rootnode
