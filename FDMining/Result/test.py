tane = open("TANE_result.txt")
test = open("output.txt")
l1 = tane.readlines()
l2 = test.readlines()
for (a, b) in zip(l1, l2):
	if a != b:
		print(a[:-1], ', ', b[:-1])