

assert:{$[x;::;'`$y];}

test01:{[x] ([] j:til x)}

test02:{[num]
	([]
		b:num#01b;
		x:num#"x"$til 256;
		g:num?0Ng; / Random GUIDs
		h:"h"$til num;
		i:"i"$til num;
		j:til num;
		e:3.1e*til num;
		f:4.1f*til num;
		c:num#"abc";
		s:`$"symbol-",/:string til num; / Symbol
		p:.z.p+1000000000*til num; / Timestamp
		d:num#.z.d; / Date
		cc:"char-",/:string til num; / Character array
		xx:"x"$til each num?32; / Byte array
		ii:"i"$til each num?20; / Integer array
		jj:til each num?20; / Long array
		ee:"e"til each num?20.2 / Real array
		)
	}

test03:{[x] ([]h:1#x)}

test04:{[b;h;i;j;e;f;p;cc] ([] res:1#-1 -5 -6 -7 -8 -9 -12 10h~type each (b;h;i;j;e;f;p;cc))}

test05:{[x] ([] c:1#x)}

test06:{[x1;x2] ([] p1:1#x1; p2:1#x2)}

test07:{ ([] d:1#.z.d)}

test08:{[x] ([] g:1#x)}

test51:{[num] ([] s:`$"symbol-",/:string til num)}

test09:{[x] ([] tt:1#enlist x)}

test10:{[x] ([] xx:1#enlist x)}

test11:{[n] ([] jj:til each n?20)}

test50:{[n] ([] ii:"i"$til each n?20)}

test52:{[n] ([] ee:"e"$til each n?20)}

test53:{[n] ([] ff:"f"$til each n?20)}

test12:{[h] ([] i:1#h; j:1#h; e:1#h; f:1#h)}

test13:{[i] ([] j:1#i; e:1#i; f:1#i)}

test14:{[j] ([] e:1#j; f:1#j)}

test15:{[e] ([] f:1#e) }

test16:{1!([] j1:1 2 3; j2:1 2 3)}

test17:{([m] j:1#1)}

test18:{([] m:1#1)}

test19:{([] j:1#1)}

test20:{([] c:1#23:50)}

test21:{([] c:1#23:50)}

test22:{([] c:1#23:50)}

test23:{([] c:1#23:50)}

test24:{([] c:1#23:50)}

test25:{([] c:1#23:50)}

test26:{([] c:1#23:50)}

test27:{([] c:1#23:50)}

test28:{([] c:1#23:50)}

test29:{([] c:1#23:50)}

test30:{([] c:1#23:50)}

test31:{assert[0;"An exception (assertion) in the kdb+ code"]}

test32:{([] j:1#1)}

test33:{([] j:1#1)}

test34:{([] j:1#1)}

test35:{[n] ([] cc:n#enlist 1000#"x" ; xx:n#enlist "x"$til 256 ; g:n?0Ng)}

test36:{[n] ([] i:"i"$til n)}

show "Ready to run tests."

// Handy utility to renumber tests in the .SQL and .Q files. It used after additional
// tests are added (starting at the next available test number) throughout the
// code in the scripts. The output files are renamed so that they can be compared
// with diff, etc.

getfile:{[n] -1_raze "\000",/:read0 n}
setfile:{[n;d] n 0: "\000" vs d}
sufind:{[d] ss[d;"[tT]est",raze 2#enlist "[0123456789]"]+\:4 5}
allsuf:{[n] -2#'"0",/:string 1+til n}

renumber:{
	f:getfile `:pgtokdb_test.sql;
	i:sufind[f];
	d:distinct s:f[i];
	a:allsuf count d;
	f[i]:a[d?s];
	setfile[`:pgtokdb_test1.sql;f];

	f:getfile `:pgtokdb_test.q;
	i:sufind f;
	f[i]:a[d?f[i]];
	setfile[`:pgtokdb_test1.q;f];
	}




