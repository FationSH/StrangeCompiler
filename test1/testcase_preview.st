program exams {
	declare a, b enddeclare
	function p1(in x, inout y) {
		declare c, d enddeclare
		function p11(in w, inout z) {
			declare e enddeclare
			function p21(in x) {
					e := x;
					z := w;
					e := p21(in a);
					return(e);
			}
			e := z;
			z := w;
			e := p21(in c);
			return(e);
		}
		b := 100;
		a := p11(in a, inout b);
		y := b+c;
		return(a);
	}
	a := p1(in a, inout b);
}
