test_dbms:
	gcc -c db_service.c load_store/load_store.c file_locking/file_lock.c  schema/schema_func.c
	gcc -c ../data_structs/map/map.c ../data_structs/pair/pair.c ../data_structs/vector/vector.c ../data_structs/map_vector/map_vector.c
	gcc *.o -o test_dbms

clean:
	rm *.o test_dbms