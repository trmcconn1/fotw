# sum3 - print sums of numeric columns
# input: rows of integers and strings
# output: sums of numeric colums
# assumes every line has the same layout

NR==1 { nfld = NF
	for (i=1;i <= NF; i++)
		numcol[i] = isnum($i)
	}
	{ for (i=1; i <= NF; i++)
		if(numcol[i])
		 sum[i] += $i
	}
END	{ for(i = 1; i  <= nfld; i++) {
		if(numcol[i])
		  printf("%g",sum[i])
		printf(i < nfld ? " " : "\n" )
		}
	}
function isnum(n) { return n ~ /^[+-]?[0-9]+[.]?[0-9]*$|^[.][0-9]+$/ }
