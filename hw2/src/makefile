main : main.o iimg.o oimg.o transform.o
	cc -o main main.o iimg.o oimg.o transform.o -lm

debug : main.c imglib.h iimg.c iimg.h oimg.c oimg.h transform.c transform.h
	cc -g main.c iimg.c oimg.c transform.c -o main -lm

main.o : main.c imglib.h
	cc -c main.c
iimg.o : iimg.c iimg.h
	cc -c iimg.c
oimg.o : oimg.c oimg.h
	cc -c oimg.c
transform.o : transform.c transform.h
	cc -c transform.c -lm

clean : 
	rm main main.o iimg.o oimg.o transform.o