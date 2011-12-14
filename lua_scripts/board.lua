rootnode = gr.node('white_tiles')

white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

white_tile_poly = gr.mesh('white_tile', '../models/tile.obj')
--white_tile_poly:set_material(white)
--white_tile_poly:scale(0.1, 0.1, 0.1)

xtrans = 1
ytrans = 1
tile_num = 1
for i = 8, 1, -1 do
	for j = 1,  4 do
		white_tile_instance = gr.node('tile'..tostring(tile_num))
		if i == 2 or i == 4 or i == 6 or i == 8 then
			modifier = 0
			white_tile_instance:translate(-5.05,0,0)
		else
			modifier = 1
		end
		xtrans = (j-modifier)*5.05*2
		ytrans = (i-1)*4.66
		rootnode:add_child(white_tile_instance)
		white_tile_instance:add_child(white_tile_poly)
		white_tile_instance:translate(xtrans,0,ytrans);
				--this next line just centres the pivot of the board
		white_tile_instance:translate(-5.05*4, 0, -4.66*4)
		white_tile_instance:rotate('y', 90)
		white_tile_instance:rotate('z', 90)
		tile_num = tile_num + 2
	end
end

rootnode:translate(0, 0.5, -80.0)
rootnode:rotate('x', 50)

return rootnode
