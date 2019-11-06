assert:{$[x;::;'`$y];}

test01:{[x] ([] j:til x)}

test02:{[x]
	([]
		b:x#01b;
		h:"h"$til x;
		i:"i"$til x;
		j:til x;
		e:3.1e*til x;
		f:4.1f*til x;
		p:.z.p+1000000000*til x;
		c:x#"abc";
		cc:"string",/:string til x
		)
	}

test03:{[x] ([]h:2#x)}

test04:{[b;h;i;j;e;f;p;cc]
	([]
		res:-1 -5 -6 -7 -8 -9 -12 10h=type each (b;h;i;j;e;f;p;cc)
		)
	}

test05:{[x] ([] c:2#x)}

test06:{[x1;x2] ([] p1:2#x1; p2:2#x2)}

test07:{ ([] d:.z.d+til 2)}

test08:{[x] ([] g:2#x)}

test09:{[x] ([] tt:2#enlist x)}

test10:{[x] ([] xx:2#enlist x)}

test40:{[h]
	([]
		i:1#h;
		j:1#h;
		e:1#h;
		f:1#h
		)
	}

test41:{[i]
	([]
		j:1#i;
		e:1#i;
		f:1#i
		)
	}

test42:{[j]
	([]
		e:1#j;
		f:1#j
		)
	}

test43:{[e]
	([]
		f:1#e
		)
	}

test11:{1!([] j1:1 2 3; j2:1 2 3)}

test12:{([m] j:1#1)}

test13:{([] j:1#1)}

test14:{([] j:1#1)}

test15:{([] c:1#23:50)}

test16:{([] c:1#23:50)}

test17:{([] c:1#23:50)}

test18:{([] c:1#23:50)}

test19:{([] c:1#23:50)}

test20:{([] c:1#23:50)}

test21:{([] c:1#23:50)}

test22:{([] c:1#23:50)}

test23:{([] c:1#23:50)}

test24:{([] c:1#23:50)}

test25:{([] c:1#23:50)}

test28:{assert[0;"An exception (assertion) in the kdb+ code"]}

test29:{([] j:1#1)}

test30:{([] j:1#1)}

test31:{([] j:1#1)}

show "Ready to run tests."