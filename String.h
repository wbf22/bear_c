#ifndef STRING_STREAM
#define STRING_STREAM

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*
    String stream object that maintains an interal buffer
    allowing efficient and easy appending of strings.
*/
typedef struct String {
    char* buffer; // string buffer
    size_t buffer_size;

    size_t start;
    size_t end;
    size_t len;
} String;



static void stream_mem_error_exit_failing() {
    fprintf(stderr, "StringStream couldn't get more memory on the system! Exiting...");
    exit(EXIT_FAILURE);
}


String* new_string_s(size_t size) {
    String *p_stream = malloc(sizeof(String));
    if (p_stream == NULL) {
        stream_mem_error_exit_failing();
    }
    p_stream->buffer = malloc(size * sizeof(void*));
    if (p_stream->buffer == NULL) {
        free(p_stream);
        stream_mem_error_exit_failing();
    }
    p_stream->buffer_size = size;
    p_stream->start = 0;
    p_stream->end = 0;
    p_stream->len = 0;

    p_stream->buffer[0] = '\0'; // null terminate

    return p_stream;
}

String* new_string() {
    return new_string_s(10);
}



static int resize_string(String* stream) {

    size_t new_size = stream->buffer_size * 2;
    char* new_data = malloc(new_size * sizeof(char));
    if (new_data == NULL) {
        stream_mem_error_exit_failing();
    }

    if (stream->start > stream->end) {

        // copy start index to array end
        memcpy(
            new_data, 
            stream->buffer + stream->start, 
            (stream->buffer_size - stream->start) * sizeof(char)
        );
        // copy array start to end index
        memcpy(
            new_data + (stream->buffer_size - stream->start), 
            stream->buffer, 
            stream->end * sizeof(char)
        );
    }
    else {
        memcpy(
            new_data, 
            stream->buffer + stream->start, 
            (stream->end - stream->start) * sizeof(char)
        );
    }

    // freeing stream but not contents
    free(stream->buffer);
    stream->buffer = new_data;
    stream->buffer_size = new_size;
    stream->start = 0;
    stream->end = stream->len;

    return 0;
}



/*
    Appends a character to a stream, ocassionaly
    resizing an internal buffer if the string is getting
    too long.

    This is a O(1) amoritized operation
*/
void append(String* stream, char* str) {
    
    int str_len = strlen(str) + 1; // including null terminator
    while (stream->len + str_len > stream->buffer_size) {
        resize_string(stream);
    }

    memcpy(stream->buffer + stream->end, str, str_len * sizeof(char));
    stream->end += str_len - 1;

    stream->len += str_len - 1;
}

void append_c(String* stream, char c) {
    
    if (stream->len + 1 > stream->buffer_size) {
        resize_string(stream);
    }

    stream->buffer[stream->end] = c;
    stream->end += 1;
    stream->len += 1;
    stream->buffer[stream->end] = '\0';
}


/*
    Appends a character to the front of a stream, ocassionaly
    resizing an internal buffer if the string is getting
    too long.

    This method is less efficient than append, as it will
    cause an extra restructering of the internal array when
    str() is called. 

    This operation itself is a O(1) amoritized operation. But
    if called, this method will cause str() to O(n) instead of 
    O(1).
*/
void append_front(String* stream, char* str) {

    int str_len = strlen(str);
    while (stream->len + str_len + 1 > stream->buffer_size) {
        resize_string(stream);
    }

    if (stream->start > 0) {
        if (stream->start - str_len > 0) {
            memcpy(stream->buffer + (stream->start - str_len), str, str_len * sizeof(char)); 
            stream->start -= str_len;
        }
        else {
            int second_len = stream->start;
            int first_len = str_len - second_len;
            memcpy(stream->buffer, str + first_len, second_len * sizeof(char));
            memcpy(stream->buffer + (stream->buffer_size - first_len), str, first_len * sizeof(char));
            stream->start = stream->buffer_size - first_len;
        }
    }
    else {
        memcpy(stream->buffer + (stream->buffer_size - str_len), str, str_len * sizeof(char));
        stream->start = stream->buffer_size - str_len;
    }


    stream->len += str_len;
}

void append_front_c(String* stream, char c) {
    
    if (stream->len + 1 + 1 > stream->buffer_size) {
        resize_string(stream);
    }

    if (stream->start > 0) {
        stream->buffer[stream->start - 1] = c;
        stream->start -= 1;
    }
    else {
        stream->start = stream->buffer_size - 1;
        stream->buffer[stream->start] = c;
    }

    stream->len += 1;
}

/*
    Returns a the string of the stream. This actually points to the
    buffer of the array. So modifying the returned string will alter
    the stream (risky). If you plan to modify the returned string use
    str_c().

    There is no need to clean up this string if you call free_string().

    O(1) amoritized unless append_front() has been used. In that case
    it will be O(n).
*/
char* str(String* stream) {
    if (stream->start == 0) {
        return stream->buffer;
    }
    else if(stream->start > stream->end) {
        resize_string(stream);
        return stream->buffer;
    }
    else {
        return stream->buffer + stream->start;
    }

    return NULL;
} 


/*
    Converts the stream to a string and populates the provided char array
    with that string. 'dest' must be larger enough to contain the resulting
    string.

    O(n) operation since the string is copied to 'dest'.
*/
void str_c(String* stream, char* dest) {
    char* str = stream;
    strcpy(dest, str);
}



static int convert_index_string(String* stream, int index) {


    while (index < 0) {
        index += stream->len;
    }

    int in_bounds = index < stream->len;
    if (!in_bounds) {
        fprintf(stderr, "Index ");
        fprintf(stderr, "%d", index);
        fprintf(stderr, " out of bounds for len ");
        fprintf(stderr, "%d", stream->len);
        exit(EXIT_FAILURE);
    }

    return (stream->start + index) % stream->buffer_size;
}

/*
    Get's a char at an index. O(1)
*/
char char_at(String* stream, int index) {
    int real_index = convert_index_string(stream, index);
    return stream->buffer[real_index];
}

/*
    Sets a char at an index. O(1)
*/
void set_char(String* stream, int index, char c) {
    int real_index = convert_index_string(stream, index);
    stream->buffer[real_index] = c;
}

/*
    Creates a sub string of the stream. O(n) operation.
*/
String* substr(String* stream, int start, int end) {
    int real_start = convert_index_string(stream, start);
    int real_end = convert_index_string(stream, end);
    int new_len = 0;
    if (real_start > real_end) {
        new_len = stream->buffer_size - real_start + real_end;
    }
    else {
        new_len = real_end - real_start;
    }


    String* new_string = new_string_s(new_len * 2);

    if (real_start > real_end) {
        // copy start index to array end
        memcpy(
            new_string->buffer, 
            stream->buffer + real_start, 
            (stream->buffer_size - real_start) * sizeof(char)
        );
        // copy array start to end index
        memcpy(
            new_string->buffer + (stream->buffer_size - real_start), 
            stream->buffer, 
            real_end * sizeof(char)
        );
    }
    else {
        memcpy(
            new_string->buffer, 
            stream->buffer + real_start, 
            (real_end - real_start) * sizeof(char)
        );
    }

    new_string->start = 0;
    new_string->end = new_len;
    new_string->len = new_len;
    new_string->buffer[new_string->end] = '\0';

    return new_string;
}

/*
    Free's the stream's meta data but doesn't free the stream's
    internal buffer, returning that instead for use. 

    The resulting string does need to be freed after you are done
    with it.
*/
char* free_string_str(String* stream) {
    char* str_res = str(stream);
    free(stream);
    return str_res;
}

void free_string(String* stream) {
    // Free data
    free(stream->buffer);
    // Free the struct itself
    free(stream);
}

String* replace(String* ss, char* str, char* replacement) {
    int str_len = strlen(str);
    int rep_len = strlen(replacement);

    String* new_ss = new_string();
    for (int i = 0; i < ss->len; ++i) {

        // handle matches
        char c = char_at(ss, i);
        if (c == str[0]) {
            int is_match = 1;
            for(int j = 0; j < str_len; ++j) {
                c = char_at(ss, i+j);
                if (c != str[j]) {
                    is_match = 0;
                    break;
                }
            }
            if (is_match) {
                for(int j = 0; j < rep_len; ++j) {

                    append_c(new_ss, replacement[j]);
                }
                i += str_len - 1;
            }
        }
        // otherwise just append
        else {
            append_c(new_ss, c);
        }
    }

    free_string(ss);

    return new_ss;
}

String** split(String* ss, char* delim, size_t* num_splits) {

    // count splits
    *num_splits = 0;
    int delim_len = strlen(delim);
    for (int i = 0; i < ss->len; ++i) {

        // find matches
        char c = char_at(ss, i);
        if (c == delim[0]) {
            int is_match = 1;
            for(int j = 0; j < delim_len; ++j) {
                c = char_at(ss, i+j);
                if (c != delim[j]) {
                    is_match = 0;
                    break;
                }
            }
            if (is_match) {
                ++*num_splits;
            }
        }
    }
    ++*num_splits;

    // split string
    String** splits = malloc(*num_splits * sizeof(String));
    int s_index = 0;
    String* buffer = new_string();
    for (int i = 0; i < ss->len; ++i) {

        // find matches
        char c = char_at(ss, i);
        if (c == delim[0]) {
            int is_match = 1;
            for(int j = 0; j < delim_len; ++j) {
                c = char_at(ss, i+j);
                if (c != delim[j]) {
                    is_match = 0;
                    break;
                }
            }
            if (is_match) {
                splits[s_index] = buffer;
                buffer = new_string();
                ++s_index;
                i += delim_len - 1;
            }
            else {
                append_c(buffer, c);
            }
        }
        // else append to current buffer
        else {
            append_c(buffer, c);
        }
    }

    splits[s_index] = buffer;

    return splits;
}

int equals(String* ss, char* str) {
    for (int i = 0; i < ss->len; ++i) {
        if (char_at(ss, i) != str[i]) {
            return 0;
        }
        else if (str[i] == '\0') {
            return 0;
        }
    }

    return 1;
}




#endif