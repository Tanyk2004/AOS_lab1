B+ file example
for i in {1..3}; do
	  dd if=file-1g of=/dev/null bs=4M iflag=fullblock status=progress
done
fincore --bytes --output=RES,PAGES,SIZE,FILE file-1g
