test1:{[x] ([] j:til x)}

test2:{[x]
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

test3:{[x]
	([]
		j1:x#"abc";
		j2:"h"$til x;
		j3:"i"$til x;
		j4:til x
		)
	}

test4:{[b;h;i;j;e;f;p;cc]
	([]
		res:-1 -5 -6 -7 -8 -9 -12 10h=type each (b;h;i;j;e;f;p;cc)
		)
	}

test5:{[x] ([] c:2#x)}

test6:{[x1;x2] ([] p1:2#x1; p2:2#x2)}

test7:{ ([] d:.z.d+til 3)}
