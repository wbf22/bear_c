#ifndef CSV_DB
#define CSV_DB

#include "Map.h"
#include "Set.h"
#include "List.h"
#include "String.h"

#include <sys/time.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>



const size_t BYTES_IN_GIGABYTE = 1073741824;
const char* TMP_DIRECTORY = "CSV_DB_TEMP_FILES";
const size_t WRITE_FREQUENCY_MS = 500;

typedef struct Row {
    
    char* key;
    List* cells;

} Row;


typedef struct Index {
    
    char* key;
    size_t last_accessed_ms;

} Index;

typedef struct Table {
    
    // primary keys to rows
    Map* keys_to_rows;

    // basically indicies
    Map* column_values_to_indices;

    List* columns;
    Map* columns_to_is_indexed;

    char* csv_path;

} Table;

typedef struct CsvDb {
    
    Map* table_name_to_table;
    size_t last_write_ms;

} CsvDb;


// UTIL FUNCTIONS

size_t current_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    size_t milliseconds = (size_t)(tv.tv_sec) * 1000 + (tv.tv_usec / 1000);

    return milliseconds;
}

Table* new_table(char* path) {
    Table* table = malloc(sizeof(Table));
    table->keys_to_rows = new_map();
    table->column_values_to_indices = new_map();
    table->columns = new_list();
    table->columns_to_is_indexed = new_map();
    table->csv_path = strdup(path);

    return table;
}

String** read_lines(char* path, size_t* num_lines) {

    // READ FILE
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Failed to open file");
        fprintf(stderr, "CsvDb couldn't open file! Exiting...");
        return NULL;
    }

    // make buffer
    size_t buffer_size = 2048; // or appropriate size
    char *buffer = malloc(buffer_size);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        fprintf(stderr, "CsvDb couldn't get more memory on the system! Exiting...");
        return NULL;
    }


    // read file contents into a stream
    String* ss = new_string();
    int done = 0;
    while(!done) {
        size_t bytesRead = fread(buffer, 1, buffer_size, file);
        if (bytesRead < buffer_size && feof(file)) {
            done = 1;
        } else if (ferror(file)) {
            perror("Error reading file");
            fclose(file);
            fprintf(stderr, "CsvDb failed reading file! Exiting...");
            return NULL;
        }

        append(ss, buffer);
    }
    free(buffer);

    // close file
    fclose(file);

    // split by newlines
    String** lines = split(ss, "\n", num_lines);
    free_string(ss);

    return lines;
}

String* read_line(FILE* file) {

    String* ss = new_string();
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            break;
        }
        append_c(ss, ch);
    }

    if (ss->len == 0) {
        free_string(ss);
        return NULL;
    }

    return ss;
}

int write_lines(char* path, List* lines_or_char_stars) {

    FILE *file = fopen(path, "w");  // Open in append mode
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // write lines
    for (int i = 0; i < lines_or_char_stars->len; ++i) {
        char* line = (char*) get(lines_or_char_stars, i);
        fprintf(file, line);
        fprintf(file, "\n");
    }

    // Close the file
    if (fclose(file) != 0) {
        perror("Error closing file");
        return 1;
    }


    return 0;
}

int write_to_file(char* path, char* str) {
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        perror("Failed to open file");
        fprintf(stderr, "CsvDb couldn't open file! Exiting...");
        return 1;
    }


    if (fputs(str, file) == EOF) {
        perror("Error writing to file");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fflush(file);

    fclose(file);

    return 0;
}

String* convert_rows_to_csv_text(List* rows) {
    String* ss = new_string();
    for (int i = 0; i < rows->len; ++i) {

        List* row = (Row*) get(rows, i);
        for (int j = 0; j < row->len; ++j) {
            char* cell = (char*) get(row, j);
            append(ss, cell);
            if (j < row->len-1) 
                append(ss, ",");
        }
        append(ss, "\n");
    }

    return ss;
}

Row* parse_row(String* str) {

    Row* row = malloc(sizeof(Row));
    row->cells = new_list();

    size_t num_splits;
    String** cells = split(str, ",", &num_splits);
    for (int c = 0; c < num_splits; ++c) {
        char* cell_str = free_string_str(cells[c]);
        push(row->cells, cell_str);
    }
    row->key = (char*) get(row->cells, 0);
    free(cells);

    return row;
}

String* row_to_str(Row* row) {
    String* str = new_string();
    for (int i = 0; i < row->cells->len; ++i) {
        char* cell = get(row->cells, i);
        append(str, cell);
        if (i < row->cells->len-1)
            append(str, ",");
    }

    return str;
}

void free_row(Row* row) {
    while (row->cells->len > 0) {
        char* cell = pop(row->cells);
        free(cell);
    }
    // free(row->key); row->key is also a cell and is freed in the loop 
    free_list(row->cells);
    free(row);
}

int compare_file_names(const void* a, const void* b) {
    const char* a_str = *(const char**)a;
    const char* b_str = *(const char**)b;
    return strcmp(a_str, b_str);
}



// USER FUNCTIONS

void free_database(CsvDb* db) {

    Element** tables = map_elements(db->table_name_to_table);
    for(int t = 0; t < db->table_name_to_table->len; ++t) {
        Element* ele = tables[t];
        Table* table = (Table*) erase(db->table_name_to_table, ele->key);

        // free rows
        Element** rows = map_elements(table->keys_to_rows);
        for (int r = 0; r < table->keys_to_rows->len; ++r) {
            Element* row_ele = rows[r];
            Row* row = (Row*) at(table->keys_to_rows, row_ele->key);
            free_row(row);
        }
        free(rows);
        free_map(table->keys_to_rows);


        // free indices
        Element** indices = map_elements(table->column_values_to_indices);
        for (int i = 0; i < table->column_values_to_indices->len; ++i) {
            Element* index_ele = indices[i];
            Index* index = at(table->column_values_to_indices, index_ele->key);
            free(index->key);
            free(index);
        }
        free(indices);
        free_map(table->column_values_to_indices);


        // free columns
        while (table->columns->len > 0) {
            char* column = pop(table->columns);
            free(column);
        }
        free_list(table->columns);


        // free columns_to_is_indexed
        Element** columns = map_elements(table->columns_to_is_indexed);
        for (int i = 0; i < table->columns_to_is_indexed->len; ++i) {
            Element* ele = columns[i];
            int* indexed = at(table->columns_to_is_indexed, ele->key);
            free(indexed);
        }
        free(columns);
        free_map(table->columns_to_is_indexed);


        // free table path
        free(table->csv_path);


        // free the table itself
        free(table);
    }
    free(tables);

    free_map(db->table_name_to_table);
    free(db);
}

CsvDb* new_database() {
    CsvDb* db = malloc(sizeof(CsvDb));
    db->table_name_to_table = new_map();

    return db;
}

/*
    Loads a csv into the database with these assumptions: 
    - The first column in the csv is assumed to be the primary key.
    - The top row is assumed to be column names.

    ## memory_threshold
    'memory_threshold' is the approximate amount of bytes caches for the table can take up.
    If set to 0 no caches will be maintained. If memory is not a constraint for you, you 
    might consider setting this value very high as this will allow select operations to 
    become lightning fast.

    ## write_frequency_ms
    'write_frequency_ms' is the frequency in milliseconds that data will be written to the csv
    files. If set to 0 then the table will write to the csv files for every write operation
    immediately. If you need data to be accurate immediatly this is the best choice, otherwise
    data can be lost in the event of a crash.

    If you increase 'write_frequency_ms' then writes can be batched together improving efficiency
    for write operations. 

    Another option for increasing efficiency is doing write operations in batches or transactions.
    This is a safer and more recommended option.
*/
void load_csv(CsvDb* db, char* path) {
    

    // PROCESS LINES
    Table* table = new_table(path);
    
    size_t num_lines;
    String** lines = read_lines(path, &num_lines);
    if (lines == NULL) {
        perror("Failed to open file");
        fprintf(stderr, "CsvDb couldn't open file! Exiting...");
        exit(EXIT_FAILURE);
    }
    for(size_t y = 0; y < num_lines; ++y) {
        String* line = lines[y];

        if (line->len > 0) {
            size_t columns;
            String** cells = split(line, "," , &columns);
            if (y == 0) {
                for (size_t x = 0; x < columns; ++x) {
                    String* cell = cells[x];
                    char* cell_str = free_string_str(cell);
                    int* indexed = malloc(sizeof(int));
                    *indexed = 0;
                    push(table->columns, cell_str);
                    insert(table->columns_to_is_indexed, cell_str, indexed, sizeof(int));
                }
            }
            else {
                Row* row = malloc(sizeof(Row));
                row->cells = new_list();
                for (size_t x = 0; x < columns; ++x) {
                    String* cell = cells[x];
                    char* cell_str = free_string_str(cell);
                    push(row->cells, cell_str);
                    if (x == 0) {
                        row->key = cell_str;
                    }
                }
                insert(table->keys_to_rows, row->key, row, sizeof(Row));
            }

            free(cells);
        }
        free_string(line);
    }
    free(lines);



    // SET IN DB
    
    // get table name
    String* table_name_ss = new_string();
    size_t start = strlen(path) - 1;
    while(start >= 0 && path[start] != '.') {
        --start;
    }
    --start;
    for(int i = start; i >= 0; --i) {
        char c = path[i];
        if (c != '/') {
            append_front_c(table_name_ss, c);
        }
        else {
            break;
        }
    }
    
    // set in db map
    insert(db->table_name_to_table, str(table_name_ss), table, sizeof(table));
    free_string(table_name_ss);
    
}

/*
    Marks a column to be indexed by in-memory caches. This index will not be complete if it requires
    more memory then is alloted to the table with it's memory_threshold.
*/
void index_column(CsvDb* db, char* column_name) {}

/*
    Runs all changes in a transaction.

    'table_name_to_rows' is a map of table names to a list of lists of char*
    representing each cell in the row. Here's an example of how to create one:

    ```
    List* row1 = new_list();
    push(row1, "user_3412351234");
    push(row1, "bob@gmail.com");

    List* row2 = new_list();
    push(row1, "user_9348102348");
    push(row1, "sarah@gmail.com");

    List* rows = new_list();
    push(rows, row1);
    push(rows, row2);

    Map* table_name_to_rows = new_map();
    char* table_name = "my_table";
    insert(table_name_to_rows, table_name, rows, sizeof(rows));
    
    transaction(db, table_name_to_rows);
    
    free_map(table_name_to_rows);
    free_list(rows);
    free_list(row1);
    free_list(row2);
    ```

    If you don't want to overwrite rows completely, then you might use the
    run() method below which let's you use postgres like syntax to do updates.
*/
void transaction(CsvDb* db, Map* table_name_to_rows) {

    /*
        write to transaction files
        - just edit changes
        
        then set in in memory tables

        update indices

        trigger table write if ready
    
    */

    // create tmp directory if missing
    struct stat st = {0};
    if (stat(TMP_DIRECTORY, &st) == -1) {
        // Directory does not exist; create it
        if (mkdir(TMP_DIRECTORY, 0755) == -1) {
            perror("Failed to create directory");
            exit(EXIT_FAILURE);
        }
    }


    // get timestamp for transaction file name
    size_t milliseconds = current_time_ms();
    char buffer[32];
    sprintf(buffer, "%zu", milliseconds);
    String* ss = new_string();
    append(ss, TMP_DIRECTORY);
    append(ss, "/");
    append(ss, buffer);
    append(ss, ".trans");

    // WRITE TO TRANSACTION FILE
    String* transaction_str = new_string();
    Element** tables = map_elements(table_name_to_rows);
    for (size_t i = 0; i < table_name_to_rows->len; ++i) {
        Element* ele = tables[i];
        char* table_name = ele->key;
        List* rows = (List*) ele->data;

        append(transaction_str, "<TABLE> ");
        append(transaction_str, table_name);
        append(transaction_str, "\n");

        String* row_str = convert_rows_to_csv_text(rows);
        append(transaction_str, str(row_str));
        free_string(row_str);
    }
    free(tables);
    if( write_to_file(str(ss), str(transaction_str)) ) {
        perror("Failed to write transaction to file");
        exit(EXIT_FAILURE);
    }
    free_string(ss);
    free_string(transaction_str);


    // SET IN IN-MEMORY TABLES 
    Element** elements = map_elements(table_name_to_rows);
    for (size_t i = 0; i < table_name_to_rows->len; ++i) {
        Element* ele = elements[i];
        char* table_name = ele->key;
        List* rows = (List*) ele->data;

        // create new table if needed
        Table* table = at(db->table_name_to_table, table_name);
        if (table == NULL) {
            String* path = new_string();
            append(path, table_name);
            append(path, ".csv");
            table = new_table(str(path));
            free_string(path);

            insert(db->table_name_to_table, strdup(table_name), table, sizeof(Table));
        }

        // insert rows
        for (int j = 0; j < rows->len; ++j) {
            List* row = (List*) get(rows, j);

            // copy row
            Row* row_cpy = malloc(sizeof(Row));
            char* row_key = (char*) get(row, 0);
            row_cpy->cells = new_list();
            for (int c = 0; c < row->len; ++c) {
                char* cell_cpy = strdup(get(row, c));
                push(row_cpy->cells, cell_cpy);
            }
            row_cpy->key = (char*) get(row_cpy->cells, 0);
            if (contains(table->keys_to_rows, row_key)) {
                Row* row = (Row*) erase(table->keys_to_rows, row_key);
                free_row(row);
            }
            insert(table->keys_to_rows, row_key, row_cpy, sizeof(Row));
        }

        // UPDATE INDICES
        for (int c = 0; c < table->columns->len; ++c) {
            char* column = get(table->columns, c);
            int* indexed = (int*) at(table->columns_to_is_indexed, column);
            if (*indexed) {
                for (int j = 0; j < rows->len; ++j) {
                    Row* row = (Row*) get(rows, j);
                    Set* indices = (Set*) at(table->column_values_to_indices, column);
                    if (indices == NULL) {
                        indices = new_set();
                        insert(table->column_values_to_indices, column, indices, sizeof(Set));
                    }
                    add(indices, row->key);
                }
            }
        }
    }
    free(elements);


    // TRIGGER TABLE WRITE IF NEEDED
    size_t time = current_time_ms();
    if (time - db->last_write_ms > WRITE_FREQUENCY_MS) {
        db->last_write_ms = time;

        // read transaction files
        List* transaction_files = new_list();
        DIR *dir = opendir(TMP_DIRECTORY);
        if (dir == NULL) {
            perror("Failed to open tmp directory");
            exit(EXIT_FAILURE);
        }
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            char* file = entry->d_name;
            if (strcmp(file, ".") == 0 || strcmp(file, "..") == 0)
                continue;
            String* transaction_file_path = new_string();
            append(transaction_file_path, TMP_DIRECTORY);
            append(transaction_file_path, "/");
            append(transaction_file_path, file);
            char* trans_file_path_str = free_string_str(transaction_file_path);
            push(transaction_files, trans_file_path_str);
        }
        closedir(dir);
        sort(transaction_files, compare_file_names);


        Map* transaction_tables_to_rows = new_map();
        for (int i = 0; i < transaction_files->len; ++i) {

            char* file = get(transaction_files, i);

            // collect rows from transaction files
            size_t num_lines;
            String** lines = read_lines(file, &num_lines);
            Map* keys_to_rows;
            char* table_name = NULL;
            for (int i = 0; i < num_lines; ++i) {
                String* line = lines[i];
                if (line->len > 0) {
                    if (starts_with(line, "<TABLE>")) {
                        String* table_name_ss = substr(line, 8, line->len);
                        if (table_name != NULL) free(table_name);
                        table_name = free_string_str(table_name_ss);

                        if (contains(transaction_tables_to_rows, table_name)) {
                            keys_to_rows = at(transaction_tables_to_rows, table_name);
                        }
                        else {
                            keys_to_rows = new_map();
                            insert(transaction_tables_to_rows, table_name, keys_to_rows, sizeof(Map));
                        }
                    }
                    else {
                        Row* row = parse_row(line);
                        insert(keys_to_rows, row->key, row, sizeof(row));
                    }
                }
                free_string(line);
            }
            free(table_name);
            free(lines);
        }




        // step through lines in real csvs writing to copy csvs and apply changes
        Map* table_name_to_temp_tables = new_map();
        Element** tables_in_db = map_elements(db->table_name_to_table);
        for(int i = 0; i < db->table_name_to_table->len; ++i) {
            Element* table_ele = tables_in_db[i];
            char* table_name = table_ele->key;
            Table* table = (Table*) table_ele->data;

            if (contains(transaction_tables_to_rows, table_name)) {
                FILE *file = fopen(table->csv_path, "r");
                if (file == NULL) {
                    perror("Failed to open file");
                    exit(EXIT_FAILURE);
                }

                String* new_table_path = new_string();
                append(new_table_path, TMP_DIRECTORY);
                append(new_table_path, "/");
                append(new_table_path, table_name);
                append(new_table_path, ".csv");
                insert(table_name_to_temp_tables, table_name, str_c(new_table_path), sizeof(char*));

                FILE* tmp_file = fopen(str(new_table_path), "a");
                if (tmp_file == NULL) {
                    fclose(file);
                    perror("Failed to make temp file writing to db");
                    exit(EXIT_FAILURE);
                }

                String* line = read_line(file);
                Map* transaction_keys_to_rows = at(transaction_tables_to_rows, table_name);
                while(line != NULL) {

                    Row* row = parse_row(line);
                    if (contains(transaction_keys_to_rows, row->key)) {
                        Row* trans_row = at(transaction_keys_to_rows, row->key);
                        String* row_str = row_to_str(trans_row);
                        fprintf(tmp_file, str(row_str));
                        free_string(row_str);
                        erase(transaction_keys_to_rows, trans_row->key);
                        free_row(trans_row);
                    }
                    else {
                        fprintf(tmp_file, str(line));
                    }
                    fprintf(tmp_file, "\n");
                    fflush(tmp_file);

                    free_string(line);
                    free_row(row);

                    line = read_line(file);
                }
                
                Element** left_over = map_elements(transaction_keys_to_rows);
                for(int e = 0; e < transaction_keys_to_rows->len; ++e) {
                    Element* ele = left_over[e];
                    Row* trans_row = (Row*) ele->data;
                    String* row_str = row_to_str(trans_row);
                    fprintf(tmp_file, str(row_str));
                    fprintf(tmp_file, "\n");
                }
                free(left_over);

                fclose(tmp_file);
                fclose(file);

                free_string(new_table_path);
            }
            
        }
        free(tables_in_db);


        // replace csv's with temp csv's
        Element** table_name_tmps = map_elements(table_name_to_temp_tables);
        for (int t = 0; t < table_name_to_temp_tables->len; ++t) {

            Element* ele = table_name_tmps[t];
            char* table_name = ele->key;
            char* tmp_path = (char*) ele->data;

            Table* table = at(db->table_name_to_table, table_name);
            
            if (rename(tmp_path, table->csv_path) != 0) {
                perror("Error csv file with tmp file");
                exit(EXIT_FAILURE);
            }

            free(tmp_path);
        }
        free(table_name_tmps);
        free_map(table_name_to_temp_tables);


        // delete transaction files
        while (transaction_files->len > 0) {
            char* transaction_file = pop(transaction_files);
            if (remove(transaction_file) != 0) {
                perror("Error deleting transaction file");
                exit(EXIT_FAILURE);
            }
            free(transaction_file);
        }
        free_list(transaction_files);


        // clean up transaction varaibles
        Element** transaction_tables = map_elements(transaction_tables_to_rows);
        for(int i = 0; i < transaction_tables_to_rows->len; ++i) {
            Element* transaction_table = transaction_tables[i];
            Map* key_to_rows = (Map*) transaction_table->data;

            Element** key_to_rows_elements = map_elements(key_to_rows);
            for(int j = 0; j < key_to_rows->len; ++j) {
                Element* ele = key_to_rows_elements[j];
                Row* row = (Row*) ele->data;
                free_row(row);
            }
            free(key_to_rows_elements);
            free_map(key_to_rows);
        }
        free(transaction_tables);
        free_map(transaction_tables_to_rows);
    }



}


/*
    Runs the provided postgres like statement(s) to update or 
    query a table.  

    All statments will be run in a single transaction.

    Returns a list of rows (or an empty list)
*/
List* run(CsvDb db, char* statement) {

}



#endif