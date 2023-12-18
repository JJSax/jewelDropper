

int wrapNum(int n, int mn, int mx) {
	return mn + (n - mn) % (mx - mn);
}
void wrapNum(int *n, int mn, int mx) {
	*n = mn + (*n - mn) % (mx - mn);
}