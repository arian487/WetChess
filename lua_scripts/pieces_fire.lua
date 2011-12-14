

rootnode = gr.node('fire_pieces')

scale_factor = 0.7

-- Drawing our body
pawn_scale = 0.8
pawn_poly = gr.mesh('pawn_poly', '../models/pawn.obj')
pawn_poly:scale(pawn_scale,pawn_scale,pawn_scale)
--pawn_poly:scale(0.5,0.5,0.5)

king_scale = 0.7
king_poly = gr.mesh('king_poly', '../models/king.obj')
king_poly:scale(king_scale,king_scale,king_scale)

bishop_poly = gr.mesh('bishop_poly', '../models/bishop.obj')
bishop_poly:scale(scale_factor,scale_factor,scale_factor)

queen_scale = 0.7
queen_poly = gr.mesh('queen_poly', '../models/queen.obj')
queen_poly:scale(queen_scale,queen_scale,queen_scale)

rook_poly = gr.mesh('rook_poly', '../models/rook.obj')
rook_poly:scale(scale_factor,scale_factor,scale_factor)

knight_scale = 0.4
knight_poly = gr.mesh('knight_poly','../models/knight.obj')
knight_poly:scale(knight_scale, knight_scale, knight_scale)

xtrans = 1;
ytrans = 1;

--draw the pawns
pawn_num = 1;
for i = 1, 8 do
	pawn_instance = gr.node('pawn'..tostring(pawn_num))
	xtrans = (i-1)*5.05
	ytrans =-3*4.66
	rootnode:add_child(pawn_instance)
	pawn_instance:add_child(pawn_poly)
	pawn_instance:translate(xtrans,0,ytrans);
	--this next line just centres the pivot of the board
	pawn_instance:translate(-5.05*4, 4.5, 0)
	pawn_instance:rotate('y', 90)
	pawn_instance:rotate('z', -90)
	pawn_num = pawn_num + 1
end

--draw the rooks
rook_num = 1;
for i = 1, 2 do
	rook_instance = gr.node('rook'..tostring(rook_num))
	xtrans = (i-1)*5.05*7
	ytrans =-4*4.66
	rootnode:add_child(rook_instance)
	rook_instance:add_child(rook_poly)
	rook_instance:translate(xtrans,0,ytrans);
	--this next line just centres the pivot of the board
	rook_instance:translate(-5.05*4, 4, 0)
	rook_instance:rotate('y', 90)
	rook_instance:rotate('z', -90)
	rook_num = rook_num + 1
end

--draw the bishops
bishop_num = 1;
for i = 1, 2 do
	bishop_instance = gr.node('bishop'..tostring(bishop_num))
	xtrans = (i-1)*5.05*3
	ytrans =-4*4.66
	rootnode:add_child(bishop_instance)
	bishop_instance:add_child(bishop_poly)
	bishop_instance:translate(xtrans,0,ytrans);
	--this next line just centres the pivot of the board
	bishop_instance:translate(-5.05*2, 4, 0)
	bishop_instance:rotate('y', 90)
	bishop_instance:rotate('z', -90)
	bishop_num = bishop_num + 1
end

--draw the knights
knight_num = 1;
for i = 1, 2 do
	knight_instance = gr.node('knight'..tostring(knight_num))
	xtrans = (i-1)*5.05*5
	ytrans =-4*4.66
	rootnode:add_child(knight_instance)
	knight_instance:add_child(knight_poly)
	knight_instance:translate(xtrans,0,ytrans);
	--this next line just centres the pivot of the board
	knight_instance:translate(-5.05*3, 5, 0)
--	knight_instance:rotate('y', 90)
--	knight_instance:rotate('z', -90)
	knight_num = knight_num + 1
end

--Draw the queen
queen_instance = gr.node('queen')
ytrans =-4*4.66
rootnode:add_child(queen_instance)
queen_instance:add_child(queen_poly)
--this next line just centres the pivot of the board
queen_instance:translate(-5.05, 4, ytrans)
queen_instance:rotate('y', 90)
queen_instance:rotate('z', -90)

--draw the king
king_instance = gr.node('king')
ytrans =-4*4.66
rootnode:add_child(king_instance)
king_instance:add_child(king_poly)
--this next line just centres the pivot of the board
king_instance:translate(0, 4, ytrans)
king_instance:rotate('y', 90)
king_instance:rotate('z', -90)

rootnode:translate(0, 0.5, -80.0)
rootnode:rotate('x', 50)



return rootnode
