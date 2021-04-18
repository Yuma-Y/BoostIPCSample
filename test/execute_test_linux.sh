#/bin/sh

./server/test_server &

./client/test_client
wait
