#ifndef MAP
#define MAP


typedef struct Element {
    void* key;
    size_t key_size;
    void* data;
} Element;


typedef struct Map {
    Element** data; // pointer to array of pointers
    size_t data_size;
    size_t len;
} Map;


const size_t PRIMES[] = {
    127,
    257,
    509,
    1021,
    2053,
    4099,
    9311,
    18253,
    37633,
    65713,
    115249,
    193939,
    505447,
    1062599,
    2017963,
    4393139,
    6972593,
    13466917,
    30402457,
    57885161,
    99990001,
    370248451,
    492366587,
    715827883,
    6643838879,
    8589935681,
    32212254719,
    51539607551, // 412 GB of 8 byte elements
    
    // just for fun probably
    80630964769,
    119327070011,
    228204732751,
    1171432692373,
    1398341745571,
    9809862296159,
    15285151248481,
    304250263527209,
    1746860020068409,
    10657331232548839,
    790738119649411319,
    2305843009213693951 // 18,446 PB of 8 byte elements, more than largest super computers have in ram
};

static void map_mem_error_exit_failing() {
    fprintf(stderr, "Map couldn't get more memory on the system! Exiting...");
    exit(EXIT_FAILURE);
}

Map* new_map() {
    size_t size = PRIMES[0];

    Map *map = malloc(sizeof(Map));
    if (map == NULL) {
        map_mem_error_exit_failing();
    }
    map->data = malloc(size * sizeof(void*));
    if (map->data == NULL) {
        free(map);
        map_mem_error_exit_failing();
    }
    map->data_size = size;
    for (int i = 0; i < size; ++i) {
        map->data[i] = NULL;
    }
    map->len = 0;

    return map;
}


void make_key(void* object, size_t object_size, char* output, size_t output_size) {
    unsigned char* bytes = (unsigned char*)object;
    unsigned long hash = 5381; // Initialize with a prime number (DJB2's initial value)

    // Process each byte
    const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    for (size_t i = 0; i < output_size; i++) {
        char byte = bytes[i % object_size];
        hash = (hash << 5) + hash + byte;
        int index = hash % 36;
        output[i] = charset[index];
    }
}

size_t hash(void* key, size_t key_size) {
    unsigned char* bytes = (unsigned char*)key;
    size_t hash = 5381;

    for (size_t i = 0; i < key_size; i++)
        hash = ((hash << 5) + hash) + bytes[i];

    return (size_t)hash;
}

size_t hash2(const char *key, size_t key_size) {
    unsigned char* bytes = (unsigned char*)key;
    size_t hash = 5381;

    for (size_t i = 0; i < key_size; i++)
        hash = (hash * 131) + bytes[i];

    // not odd or zero
    if (hash % 2 == 0) {
        hash += 1;
    }

    return (size_t)hash;
}

size_t probe(Map* map, void* key, size_t key_size) {
    size_t index = hash(key, key_size);
    size_t first_index = index;

    int failures = 1;
    int open_or_match = map->data[index] == NULL;
    if (map->data[index] != NULL && map->data[index]->key_size == key_size) {
        open_or_match = open_or_match || memcmp(map->data[index]->key, key, key_size * sizeof(unsigned char)) == 0;
    }
    while (!open_or_match) {
        index = (first_index + failures * hash2(key, key_size)) % map->data_size;
        ++failures;

        int open_or_match = map->data[index] == NULL;
        if (map->data[index] != NULL && map->data[index]->key_size == key_size) {
            open_or_match = open_or_match || memcmp(map->data[index]->key, key, key_size * sizeof(unsigned char)) == 0;
        }
    }

    return index;
}


void resize(Map* map) {

}


void struct_insert(Map* map, void* key, size_t key_size, void* data, size_t data_size) {
    size_t index = probe(map, key, key_size);

    void* copy = malloc(data_size);
    if (copy == NULL) {
        map_mem_error_exit_failing();
    }
    memcpy(copy, data, data_size);

    map->data[index] = copy;

    ++map->len;


    // resize if neeeded
    if (map->data_size * 0.7 < map->len) {
        resize(map);
    }
}

void int_insert(Map* map, int key, void* data, size_t data_size) {
    struct_insert(map, &key, sizeof(int), data, data_size);
}

void insert(Map* map, char* key, void* data, size_t data_size) {
    struct_insert(map, &key, strlen(key) + 1, data, data_size);
}


void* struct_get(Map* map, void* key, size_t key_size) {
    size_t index = probe(map, key, key_size);
    if(map->data[index] == NULL) {
        return NULL;
    }

    return map->data[index]->data;
}

void* int_get(Map* map, int key) {
    return struct_get(map, &key, sizeof(key));
}

void* get(Map* map, void* key, size_t key_size) {
    return struct_get(map, &key, strlen(key) + 1);
}


#endif

