sudo docker run -it --name lab \
	  --cpus=2 --memory="500m" --memory-swap="5g"\
	    -v "$PWD/merged:/mnt/merged" \
	      ubuntu:22.04 /bin/bash

