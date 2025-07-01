#ifndef CSV_DB
#define CSV_DB

#include "Map.h"
#include "Set.h"
#include "List.h"
#include "String.h"



const size_t BYTES_IN_GIGABYTE = 1073741824;

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

    size_t memory_threshold;
    size_t write_frequency_ms;

} Table;


typedef struct CsvDb {
    
    Map* table_name_to_table;

} CsvDb;


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
            while (row->cells->len > 0) {
                char* cell = pop(row->cells);
                free(cell);
            }
            // free(row->key); row->key is also a cell and is freed in the loop 
            free_list(row->cells);
            free(row);
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
void load_csv(CsvDb* db, char* path, size_t memory_threshold, size_t write_frequency_ms) {
    
    // READ FILE
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Failed to open file");
        fprintf(stderr, "CsvDb couldn't open file! Exiting...");
        exit(EXIT_FAILURE);
    }

    // make buffer
    size_t buffer_size = 2048; // or appropriate size
    char *buffer = malloc(buffer_size);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        fprintf(stderr, "CsvDb couldn't get more memory on the system! Exiting...");
        exit(EXIT_FAILURE);
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
            exit(EXIT_FAILURE);
        }

        append(ss, buffer);
    }
    free(buffer);


    // PROCESS LINES
    Table* table = malloc(sizeof(Table));
    table->keys_to_rows = new_map();
    table->column_values_to_indices = new_map();
    table->columns = new_list();
    table->memory_threshold = memory_threshold;
    table->write_frequency_ms = write_frequency_ms;
    
    size_t num_lines;
    String** lines = split(ss, "\n", &num_lines);
    free_string(ss);
    for(size_t y = 0; y < num_lines; ++y) {
        String* line = lines[y];

        if (line->len > 0) {
            size_t columns;
            String** cells = split(line, "," , &columns);
            if (y == 0) {
                for (size_t x = 0; x < columns; ++x) {
                    String* cell = cells[x];
                    char* cell_str = free_string_str(cell);
                    push(table->columns, cell_str);
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
                insert(table->keys_to_rows, row->key, row, sizeof(row));
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
void index_column(CsvDb db, char* column_name) {}



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
void transaction(CsvDb db, Map* table_name_to_rows) {}

/*
    Adds rows to the table in a single transaction or batch.

    'rows' is a list of lists of char* representing each cell in the row
*/
void add_rows(CsvDb db, char* table, char* key, List* rows) {}

/*
    Adds a row to the table. 

    'cells' is a list of char* representing each cell in the row
*/
void add_row(CsvDb db, char* table, char* key, List* cells) {
    
}



void remove_row(CsvDb db, char* key) {

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