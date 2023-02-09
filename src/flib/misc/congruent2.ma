# maple notebook to for estimate needed in numerical integration for
# the confluent hypergeometric of the 2nd kind

# We want an upper bound on the fourth derivative of this function
f(t) := exp(-x*t)*t^A*(1+t)^B:
f1(t) := diff(f(t),t):
f2(t) := diff(f1(t),t):
f3(t) := diff(f2(t),t):
f4(t) := diff(f3(t),t):
# factor(f4(t)):  # Shows this looks like exp(-xt)t^(A-4)(1+t)^B-4 times
# a big ugly polynomial in A B and t. 
poly = simplify(f4(t)*exp(x*t)*t^(4-A)*(1+t)^(4-B));
