#include "parametrs.h"
#include "stack_t.h"

int main() {
	stack_t stack;
	stack.sp = 0;

	double s = 0;
	double A = BEG, B = END;
	double fA = F(A), fB = F(B);
	double sAB = (fA+fB)*(B-A)/2;
	
	while (1) {
		double C  = (A+B)/2;
		double fC = F(C);

		double sAC  = (fA + fC) * (C-A)/2;
		double sCB  = (fC + fB) * (B-C)/2;
		double sACB = sAC + sCB;

		if (fabs(sAB - sACB) >= EPS * fabs(sACB)) {
			stack_data_t t = {A, C, fA, fC, sAC};
			push(&stack, &t);
			A = C, fA = fC, sAB = sCB;
		} else {
			s += sACB;
			if (!stack.sp) break;
			stack_data_t t;
			pop(&stack, &t);
			A = t.a, B = t.b, fA = t.fa, fB = t.fb, sAB = t.sab;
		}
	}
	printf("%lg\n", s);
	return 0;
}
