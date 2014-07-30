If 2 moves exist that are considered unimportant, simulate neither and instead have a catch-all node for no opponent move.

If an opponent move exists that is infeasable, i.e. taking a piece defended by 1 defender, or moving to a square that is in the line of attack of one of my pieces, or leaving an important piece undefended for me to take, but there exists a previous move that would make this move feasable, assume the move is feasable *before* deciding whether it is important.

What constitutes an important move
	Taking one of my pieces - limits what I can do during my following move. Only even trades or better. Queen takes pawn doesn't cut it.
	Check, drastically limits what I can do during my following move. Only without sacrifice. 
	Threatening one of my pieces - a complete threat, where there are more attackers than defenders, or a defender may be removed. This forces me to defend, usually, reducing what I can feasibly do during my following move.
	????? A move that drastically increases my opponents possible moves ?????
	????? A feasable (definition above) pawn push to the 3rd/6th rank or closer to queening the pawn ?????


Before simulating one of my moves (excluding the first of my moves), see if there are any moves my opponent could make that would render my move infeasable, or effect the outcome of my move. For example, if my move captures an enemy piece, but a previous opponent move could have defended it or attacked a piece of >= value. This is only if the above node is a catch-all opponent move node, and the opponent move is considered unimportant. If there are any such moves, simulate that move before simulating my move. If there is more than one such move, simulate each one in turn before my move and branch out from all of those.

Also note when one of my pieces is called upon to do two different things, i.e. render two different opponent moves infeasable on two different squares. i.e. my rook prevents the opponent from making a move to position A in row 1, and it prevents the opponent from making a move to position B in rank h, however if the opponent moves to position A and I take him, the move to position B is now feasible, and vice versa.
