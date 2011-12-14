rootnode = gr.node('black_tiles')

black = gr.material({0, 0, 0}, {0.1, 0.1, 0.1}, 10)

black_tile_poly = gr.mesh('black_tile', '../models/tile.obj')
--black_tile_poly:set_material(black)
--black_tile_poly:scale(0.1, 0.1, 0.1)
xtrans = 1;
ytrans = 1;
tile_num = 0
for i = 8, 1, -1 do
	for j = 1,  4 do
		black_tile_instance = gr.node('tile'..tostring(tile_num))
		if i == 1 or i == 3 or i == 5 or i == 7 then
			modifier = 0
			black_tile_instance:translate(-5.05,0,0)
		else
			modifier = 1
		end
		xtrans = (j-modifier)*5.05*2
		ytrans = (i-1)*4.66
		rootnode:add_child(black_tile_instance)
		black_tile_instance:add_child(black_tile_poly)
		black_tile_instance:translate(xtrans,0,ytrans);
		--this next line just centres the pivot of the board
		black_tile_instance:translate(-5.05*4, 0, -4.66*4)
		black_tile_instance:rotate('y', 90)
		black_tile_instance:rotate('z', 90)
		tile_num = tile_num + 2
	end
end


rootnode:translate(0, 0.5, -80.0)
rootnode:rotate('x', 50)

return rootnode
