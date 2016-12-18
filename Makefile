all : client.c serverA.c serverB.c serverC.c aws.c
	gcc -o client client.c -lsocket  -lnsl  -lresolv
	gcc -o server_A serverA.c -lsocket  -lnsl  -lresolv
	gcc -o server_B serverB.c -lsocket  -lnsl  -lresolv
	gcc -o server_C serverC.c -lsocket  -lnsl  -lresolv
	gcc -o aws_server aws.c -lsocket  -lnsl  -lresolv
	
run_A: serverA
serverA:
	@(./server_A)
	
run_B: serverB
serverB:
	@(./server_B)
	
run_C: serverC
serverC:
	@(./server_C)

run_aws: aws
aws:
	@(./aws_server)
	
clean:
	$(RM) client server_A server_B server_C aws_server
	
.PHONY: serverA serverB serverC aws clean
